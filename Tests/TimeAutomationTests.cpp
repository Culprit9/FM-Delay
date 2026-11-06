#include "../Source/DelayEngine.h"
#include <iostream>

// Identical parameter events, subdivided into different host block sizes.
int main() {
 std::vector<float> reference;
 for (int block : {1, 37, 512, 2048}) {
  fm::DelayEngine engine; engine.prepare(48000);
  fm::Settings s; s.mode=0; s.mix=1; s.feedback=.6f;
  s.hp=20; s.lp=20000; s.duck=0;
  std::vector<float> l(16000), r(l.size());
  for (size_t i=0;i<l.size();++i) l[i]=r[i]=.2f*std::sin(i*.03f);
  const int events[]={0,4000,4200,16000};
  const float times[]={40,70,25};
  for(int event=0;event<3;++event) {
   s.timeMs=times[event];
   for(int i=events[event];i<events[event+1];i+=block)
    engine.process(l.data()+i,r.data()+i,std::min(block,events[event+1]-i),s);
  }
  if(reference.empty()) reference=l;
  else for(size_t i=0;i<l.size();++i)
   if(!std::isfinite(l[i]) || std::abs(l[i]-reference[i])>1e-6f) {
    std::cerr<<"Time automation depends on block size at "<<i<<'\n'; return 1;
   }
 }
 std::cout<<"PASS: queued delay transitions independent of host block size\n";
}
