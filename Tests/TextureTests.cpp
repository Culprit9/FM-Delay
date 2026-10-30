#include "../Source/DelayEngine.h"
#include <iostream>
#include <stdexcept>
void check(bool x,const char* m){if(!x)throw std::runtime_error(m);}
int main(){try{
 // Regression: tiny diffusion used to store ~2.4% of an impulse beyond 200 ms.
 // Low diffusion must be a short smear, never a long ringing resonator.
 for(double sr:{44100.,48000.,96000.})for(float amount:{.0001f,.001f,.0075f,.015f,.03f,.075f,.35f,.7f}){
  fm::DelayEngine e;e.prepare(sr);fm::Settings s;s.mode=0;s.mix=1;s.timeMs=50;s.feedback=s.duck=0;s.hp=20;s.lp=20000;s.diffusion=amount;
  std::vector<float> l(size_t(sr*.6)),r(l.size());l[0]=r[0]=.5f;e.process(l.data(),r.data(),int(l.size()),s);
  double total=0,late=0;for(size_t i=0;i<l.size();++i){check(std::isfinite(l[i]),"finite diffusion output");total+=l[i]*l[i];if(i>sr*.25)late+=l[i]*l[i];}
  check(total>.01,"diffusion must not mute an impulse");check(late<std::max(1e-15,total*1e-7),"diffuser must settle 200ms after isolated echo at every amount");
 }
 // Static settings must not change with host block size, including ducker and ping.
 for(int mode=0;mode<4;++mode){std::vector<float> result;for(int block:{1,37,512}){
  fm::DelayEngine e;e.prepare(48000);fm::Settings s;s.mode=mode;s.timeMs=23.7f;s.diffusion=.015f;s.ping=true;
  std::vector<float> l(48000),r(l.size());for(size_t i=0;i<l.size();++i){l[i]=.2f*std::sin(i*.09);r[i]=.13f*std::sin(i*.11);}
  for(int i=0;i<int(l.size());i+=block)e.process(l.data()+i,r.data()+i,std::min(block,int(l.size())-i),s);
  if(result.empty())result=l;else{float error=0;for(size_t i=0;i<l.size();++i)error=std::max(error,std::abs(result[i]-l[i]));check(error<1e-6f,"host block size must not change static processing");}
 }}
 // Sweep the diffuser on a sustained low tone: bounded output and click-free changes.
 {fm::DelayEngine e;e.prepare(48000);fm::Settings s;s.mode=0;s.mix=1;s.feedback=.4f;s.duck=0;s.timeMs=70;s.hp=20;s.lp=20000;float previous=0,maxStep=0;for(int b=0;b<1000;++b){s.diffusion=.35f*(1+std::sin(b*.025f));float l[64],r[64];for(int i=0;i<64;++i)l[i]=r[i]=.1f*std::sin((b*64+i)*6.283185307179586*150/48000);e.process(l,r,64,s);for(float x:l){check(std::isfinite(x)&&std::abs(x)<.5f,"automated diffusion must stay bounded");if(b>100)maxStep=std::max(maxStep,std::abs(x-previous));previous=x;}}check(maxStep<.015f,"diffusion automation must not click");std::cout<<"Diffusion automation max sample step: "<<maxStep<<'\n';}
 std::cout<<"PASS: short non-ringing diffusion, block-size invariance, bounded smooth automation\n";
 }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
