#include "../Source/DelayEngine.h"
#include "../Source/Timing.h"
#include <iostream>
#include <random>
#include <stdexcept>
void check(bool v,const char* m){if(!v)throw std::runtime_error(m);}
int main(){try{
 for(int i=0;i<24;++i)check(std::abs(fm::syncedMilliseconds(120,i)-500*fm::noteBeats[i])<.01f,"note conversion");
 for(float ms:{7.8125f,31.25f,83.3333f,250.f,1500.f}){fm::DelayEngine e;e.prepare(48000);fm::Settings s;s.timeMs=ms;s.mode=0;s.analog=s.diffusion=s.duck=s.feedback=0;s.hp=20;s.lp=20000;s.mix=1;std::vector<float> l(100000),r(100000);l[0]=r[0]=1;e.process(l.data(),r.data(),int(l.size()),s);auto at=std::max_element(l.begin(),l.end());check(std::abs(std::distance(l.begin(),at)-48*ms)<3,"first-block time must use chosen value");}
 {fm::DelayEngine e;e.prepare(48000);fm::Settings s;s.throwMode=true;s.mix=1;s.feedback=.5f;s.timeMs=100;s.duck=s.diffusion=s.analog=0;s.mode=0;std::vector<float> l(12000),r(12000);l[0]=r[0]=1;e.process(l.data(),r.data(),12000,s);check(*std::max_element(l.begin(),l.end())==0,"idle throw must not leak audio on startup");
 s.trigger=true;std::fill(l.begin(),l.end(),.2f);std::fill(r.begin(),r.end(),.2f);e.process(l.data(),r.data(),317,s);s.trigger=false;std::fill(l.begin(),l.end(),0);std::fill(r.begin(),r.end(),0);e.process(l.data(),r.data(),12000,s);check(*std::max_element(l.begin(),l.end())>.01f,"arbitrary-length throw must retain echo after release");}
 {fm::DelayEngine e;e.prepare(48000);fm::Settings s;s.bypass=true;s.mix=1;float l[9],r[9];std::fill_n(l,9,.25f);std::fill_n(r,9,-.25f);e.process(l,r,9,s);check(l[0]==.25f&&r[0]==-.25f,"bypass must preserve dry signal");}
for(double sr:{44100.,48000.,96000.}){fm::DelayEngine e;e.prepare(sr);fm::Settings s;s.timeMs=375;s.mode=0;s.hp=20;s.lp=20000;s.analog=0;s.diffusion=0;s.duck=0;s.mix=1;s.feedback=0;
 std::vector<float> l(size_t(sr*2)),r(l.size());e.process(l.data(),r.data(),int(sr*.5),s);std::fill(l.begin(),l.end(),0);std::fill(r.begin(),r.end(),0);l[0]=r[0]=1;e.process(l.data(),r.data(),int(l.size()),s);auto peak=std::max_element(l.begin(),l.end());check(std::abs(std::distance(l.begin(),peak)-sr*.375)<3,"impulse timing");check(*peak>.4,"impulse gain");
 for(int mode=0;mode<4;++mode){e.prepare(sr);s.mode=mode;s.feedback=.94;s.ping=true;s.analog=1;s.diffusion=.7;s.width=2;s.throwMode=true;std::mt19937 gen(42);std::uniform_real_distribution<float> noise(-1,1);for(int block=0;block<1000;++block){s.trigger=block%19<2;s.timeMs=block%37==0?1:375;s.mix=float(block%100)/100;for(int i=0;i<256;++i)l[i]=r[i]=block<200?noise(gen):0;e.process(l.data(),r.data(),256,s);for(int i=0;i<256;++i)check(std::isfinite(l[i])&&std::isfinite(r[i])&&std::abs(l[i])<10&&std::abs(r[i])<10,"feedback stability");}}}
 std::cout<<"PASS: impulse timing at 44.1/48/96 kHz; all modes; automated time/mix/throw; maximum feedback stability\n";
 }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
