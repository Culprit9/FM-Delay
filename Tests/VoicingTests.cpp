#include "../Source/DelayEngine.h"
#include <iostream>
#include <stdexcept>
void check(bool ok,const char* m){if(!ok)throw std::runtime_error(m);}
int main(){try{
 // A quarter-turn Mix should give a clearly audible first repeat without touching dry gain.
 {fm::DelayEngine e;e.prepare(48000);fm::Settings s;s.timeMs=100;s.mode=0;s.analog=s.feedback=s.duck=s.diffusion=0;s.hp=20;s.lp=20000;s.mix=.25f;std::vector<float> l(10000),r(l.size());l[0]=r[0]=1;e.process(l.data(),r.data(),int(l.size()),s);float repeat=0;for(int i=4798;i<4810;++i)repeat=std::max(repeat,std::abs(l[i]));check(repeat>.30f,"25 percent Mix should have stronger repeat presence");check(std::abs(l[0]-.75f)<1e-6,"new wet curve must retain existing dry gain");std::cout<<"First-repeat peak at 25% Mix: "<<repeat<<'\n';}
 // Measure low-mid body against upper-mid bite with identical-amplitude tones.
 {fm::DelayEngine e;e.prepare(48000);fm::Settings s;s.timeMs=100;s.mode=1;s.analog=.45f;s.feedback=s.duck=s.diffusion=0;s.hp=60;s.lp=5000;s.mix=1;std::vector<float> l(96000),r(l.size());for(size_t i=0;i<l.size();++i){l[i]=.1f*std::sin(i*6.2831853*250/48000);r[i]=.1f*std::sin(i*6.2831853*6000/48000);}e.process(l.data(),r.data(),int(l.size()),s);double lo=0,hi=0;for(size_t i=24000;i<l.size();++i){lo+=l[i]*l[i];hi+=r[i]*r[i];}double ratio=std::sqrt(lo/hi);std::cout<<"250 Hz / 6 kHz RMS ratio: "<<ratio<<'\n';check(ratio>2.5,"warm tape should favor body over crispy upper mids");}
 // Ducking should still work, without burying the repeats during a phrase.
 {double energy[2]={};for(int n=0;n<2;++n){fm::DelayEngine e;e.prepare(48000);fm::Settings s;s.timeMs=100;s.mode=0;s.analog=s.feedback=s.diffusion=0;s.duck=n?.6f:0;s.hp=20;s.lp=20000;s.mix=1;std::vector<float> l(48000),r(l.size());for(size_t i=0;i<l.size();++i)l[i]=r[i]=.4f*std::sin(i*6.2831853*500/48000);e.process(l.data(),r.data(),int(l.size()),s);for(size_t i=24000;i<l.size();++i)energy[n]+=l[i]*l[i];}double ratio=std::sqrt(energy[1]/energy[0]);std::cout<<"Ducked / unducked wet RMS: "<<ratio<<'\n';check(ratio>.23&&ratio<.35,"above-threshold 60 percent duck should give about 10.8dB reduction");}
 std::cout<<"PASS: stronger wet presence, retained dry gain, fuller tape spectrum, gentler ducking\n";
 }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
