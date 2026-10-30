#include "../Source/DelayEngine.h"
#include <iostream>
#include <stdexcept>
void check(bool ok,const char* msg){if(!ok)throw std::runtime_error(msg);}
int main(){try{
 // Full wet must produce silence before the selected delay, then an audible repeat.
 for(double sr:{44100.,48000.,96000.})for(bool ping:{false,true})for(bool anti:{false,true}){fm::DelayEngine e;e.prepare(sr);fm::Settings s;s.mix=1;s.timeMs=100;s.feedback=s.duck=s.analog=s.diffusion=0;s.mode=0;s.hp=20;s.lp=20000;s.ping=ping;std::vector<float> l(int(sr*.25)),r(l.size());l[0]=.4f;r[0]=anti?-.4f:.4f;e.process(l.data(),r.data(),int(l.size()),s);double early=0,wet=0;for(int i=0;i<sr*.09;++i)early+=l[i]*l[i]+r[i]*r[i];for(int i=int(sr*.095);i<sr*.15;++i)wet+=l[i]*l[i]+r[i]*r[i];check(early==0,"full wet must not leak dry input");check(wet>.005,"full wet stereo and anti-phase input must not disappear");}
 // At unity Mix, normal tape/duck settings must preserve a usable first-repeat level.
 {fm::DelayEngine e;e.prepare(48000);fm::Settings s;s.mix=1;s.timeMs=100;s.feedback=0;s.diffusion=0;std::vector<float> l(24000),r(l.size());for(int i=0;i<12000;++i)l[i]=r[i]=.2f*std::sin(i*6.2831853*300/48000);e.process(l.data(),r.data(),int(l.size()),s);double energy=0;for(int i=6000;i<11000;++i)energy+=l[i]*l[i];double ratio=std::sqrt(energy/5000)/(.2/std::sqrt(2.));check(ratio>.7&&ratio<1.6,"full-wet tape level must not be buried");std::cout<<"Full-wet tape/input RMS="<<ratio<<'\n';}
 // The bounded diffuser preserves vocal body; interpolation gently damps highs.
 // It must not produce a resonant peak or bury the return at any tested setting.
 for(float hz:{100.f,300.f,1000.f,4000.f,8000.f}){double energy[3]={};int n=0;for(float amount:{0.f,.35f,.7f}){fm::DelayEngine e;e.prepare(48000);fm::Settings s;s.mix=1;s.timeMs=50;s.feedback=s.duck=s.analog=0;s.diffusion=amount;s.mode=0;s.hp=20;s.lp=20000;std::vector<float> l(96000),r(l.size());for(size_t i=0;i<l.size();++i)l[i]=r[i]=.1f*std::sin(i*6.2831853*hz/48000);e.process(l.data(),r.data(),int(l.size()),s);for(size_t i=48000;i<l.size();++i){check(std::isfinite(l[i]),"diffusion finite");energy[n]+=l[i]*l[i];}++n;}for(int i=1;i<3;++i)check(energy[i]/energy[0]>(hz<=1000?.95:.12)&&energy[i]/energy[0]<1.10,"diffusion must preserve body with bounded high-frequency damping, no resonant boost");}
 // Diffusion must actually spread impulse energy into more samples.
 {double late[2]={};for(int n=0;n<2;++n){fm::DelayEngine e;e.prepare(48000);fm::Settings s;s.mix=1;s.timeMs=50;s.feedback=s.duck=s.analog=0;s.diffusion=n?.7f:0;s.mode=0;s.hp=20;s.lp=20000;std::vector<float> l(20000),r(l.size());l[0]=r[0]=.5f;e.process(l.data(),r.data(),int(l.size()),s);for(int i=2700;i<10000;++i)late[n]+=l[i]*l[i];}check(late[1]>late[0]*10,"diffusion must increase echo density");}
 std::cout<<"PASS: full-wet dry rejection and audible returns; stereo anti-phase retention; diffusion body preservation, bounded high-frequency damping and impulse spreading\n";
 }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
