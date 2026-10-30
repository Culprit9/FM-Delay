#include "../Source/DelayEngine.h"
#include <iostream>
#include <stdexcept>
void check(bool b,const char* m){if(!b)throw std::runtime_error(m);}
int main(){try{
 for(double rate:{44100.,48000.,96000.})for(bool high:{false,true}){float response[3];int j=0;for(float hz:{500.f,1000.f,2000.f}){fm::ToneFilter f;f.cutoff(rate,1000);double e=0;int frames=int(rate);for(int i=0;i<frames;++i){float x=.1f*std::sin(i*6.283185307179586*hz/rate);float y=f.process(x,high);if(i>frames/2)e+=y*y;}response[j++]=std::sqrt(e/(frames/2)/.005);}check(response[1]>.69&&response[1]<.72,"cutoff should be Butterworth -3dB");check(high?response[0]<.26&&response[2]>.96:response[0]>.96&&response[2]<.26,"two-pole rolloff must preserve passband and reject stopband");}
 for(double rate:{44100.,48000.,96000.}){fm::DelayEngine e;e.prepare(rate);fm::Settings s;s.mode=0;s.mix=1;s.duck=s.diffusion=0;s.feedback=.75f;s.timeMs=33;float l[128],r[128];for(int b=0;b<1000;++b){s.hp=b%2?20:2000;s.lp=b%3?500:20000;for(int i=0;i<128;++i)l[i]=r[i]=.1f*std::sin((b*128+i)*.11);e.process(l,r,128,s);for(int i=0;i<128;++i)check(std::isfinite(l[i])&&std::abs(l[i])<2&&l[i]==r[i],"filter sweeps must remain bounded and centered");}}
 std::cout<<"PASS: two-pole response, cutoff gain, endpoint automation, three rates and stereo coherence\n";
 }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
