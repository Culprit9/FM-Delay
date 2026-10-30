#pragma once
#include <array>
#include <vector>
#include <cmath>
#include <algorithm>
#include "AnalogColour.h"
#include "ToneFilter.h"
#include "Diffuser.h"
namespace fm {
struct Settings {float timeMs=375,feedback=.42f,mix=.3f,analog=.45f,duck=.15f,duckThreshold=-36,hp=60,lp=5000,width=1,diffusion=0;int mode=1;bool ping=false,throwMode=false,trigger=false,bypass=false;};
class DelayEngine {
 double sr=48000;std::array<std::vector<float>,2> ring;size_t pos=0;bool initialised=false;
 float delay=18000,nextDelay=18000,timeFade=1,mix=.3f,feedback=.42f,gate=1,active=1,analog=.45f,width=1,diffusion=0,colourWeight=1;
 float wowDepth=0,bodyAmount=0,phase=0,slowPhase=0,power=0,duckDb=0,duckAmount=0,hold=0,drive=1.5f,bias=0,blend=.5f;
 struct MonoFold{float mid=0,side=0,choice=0;bool ready=false;};MonoFold inputFold,seedFold;float pingBlend=0;bool lastPing=false;int pingSeedRemaining=0;
 float userLow=1,userHigh=1,colourLow=.5f,cleanHPWeight=0,cleanLPWeight=0;
 std::array<float,2> inputLow{},inputHigh{},inputPrevious{},toneOne{},toneTwo{},dcInput{},dcOutput{},body{};
 std::array<AnalogColour,2> colour;std::array<ToneFilter,2> cleanHP,cleanLP;
 std::array<std::array<Diffuser,3>,2> diffuse;
 static float slew(float x,float target,float a){return target+a*(x-target);}
 static float finite(float x,float fallback){return std::isfinite(x)?x:fallback;}
 static float folded(float l,float r,MonoFold& state,float detector,float smoothing){float m=(l+r)*.5f,d=(l-r)*.5f;state.mid=slew(state.mid,m*m,detector);state.side=slew(state.side,d*d,detector);float target=state.side>4*state.mid+1e-12f?1.f:state.side<2*state.mid+1e-12f?0.f:state.choice;if(!state.ready&&std::max(std::abs(l),std::abs(r))>1e-6f){state.choice=target;state.ready=true;}else state.choice=slew(state.choice,target,smoothing);return 1.41421356237f*(m*(1-state.choice)+d*state.choice);}
 float read(int c,float distance)const{double p=double(pos)-distance;while(p<0)p+=ring[c].size();auto i=(size_t)p;float t=float(p-i);auto at=[&](long k){return ring[c][(k+ring[c].size())%ring[c].size()];};float a=at((long)i-1),b=at(i),cc=at(i+1),d=at(i+2);return b+.5f*t*(cc-a+t*(2*a-5*b+4*cc-d+t*(3*(b-cc)+d-a)));}
 float filtered(float x,int c,bool clean,bool hpOpen,bool lpOpen){if(clean){float lo=cleanLP[c].process(x,false);float v=x+(lo-x)*cleanLPWeight;float hi=cleanHP[c].process(v,true);return v+(hi-v)*cleanHPWeight;}auto& low=inputLow[c];auto& high=inputHigh[c];auto& previous=inputPrevious[c];low+=userLow*(x-low);float v=lpOpen?x:low;high=userHigh*(high+v-previous);previous=v;return hpOpen?v:high;}
public:
 void prepare(double rate){sr=rate;for(auto& r:ring)r.assign(size_t(sr*32)+8,0);const float ms[2][3]={{3.7f,5.1f,7.3f},{4.1f,5.9f,8.3f}};for(int c=0;c<2;++c){colour[c].prepare();cleanHP[c].reset();cleanLP[c].reset();for(int n=0;n<3;++n)diffuse[c][n].prepare(sr,ms[c][n]);}pos=0;initialised=false;inputFold={};seedFold={};pingSeedRemaining=0;lastPing=false;power=duckDb=hold=phase=slowPhase=0;inputLow={};inputHigh={};inputPrevious={};toneOne={};toneTwo={};dcInput={};dcOutput={};body={};}
 float getDuckReductionDb()const{return duckDb;}
 void process(float* left,float* right,int frames,const Settings& raw){Settings s=raw;
 s.timeMs=std::clamp(finite(s.timeMs,375),.1f,32000.f);s.mix=std::clamp(finite(s.mix,.3f),0.f,1.f);s.feedback=std::clamp(finite(s.feedback,.42f),0.f,.94f);s.analog=std::clamp(finite(s.analog,.45f),0.f,1.f);s.duck=std::clamp(finite(s.duck,0),0.f,1.f);s.duckThreshold=std::clamp(finite(s.duckThreshold,-36),-60.f,0.f);s.hp=std::clamp(finite(s.hp,60),20.f,2000.f);s.lp=std::clamp(finite(s.lp,5000),500.f,20000.f);s.width=std::clamp(finite(s.width,1),0.f,2.f);s.diffusion=std::clamp(finite(s.diffusion,0),0.f,.7f);s.mode=std::clamp(s.mode,0,3);
 const float target=std::clamp(s.timeMs*.001f*float(sr),4.f,float(ring[0].size()-5));
 const float smooth=std::exp(-1.f/float(sr*.02)),fast=std::exp(-1.f/float(sr*.005));
 const float detector=std::exp(-1.f/float(sr*.010)),attack=std::exp(-1.f/float(sr*.008)),release=std::exp(-1.f/float(sr*.220));
 const float hpTarget=std::exp(-6.2831853f*s.hp/float(sr));const float lpTarget=1-std::exp(-6.2831853f*std::min(s.lp,float(sr*.45))/float(sr));
 const float cutoff=s.mode==1?7000-3000*s.analog:s.mode==2?4600-2600*s.analog:2400-1400*s.analog;
 const float toneTarget=1-std::exp(-6.2831853f*std::min(cutoff,float(sr*.45))/float(sr));
 const float driveTarget=s.mode==1?1.3f+2.2f*s.analog:s.mode==2?1.8f+3.2f*s.analog:1.2f+1.8f*s.analog;
 const float biasTarget=s.mode==1?.06f*s.analog:s.mode==3?.025f*s.analog:0;
 const float blendTarget=s.mode==1?.35f+.65f*s.analog:s.mode==2?.5f+.5f*s.analog:.3f+.5f*s.analog;
 const float depthTarget=s.mode==1?.000015f+s.analog*.00006f:s.mode==2?.00003f+s.analog*.00013f:0;const float bodyTarget=s.analog*(s.mode==2?.3f:.15f);
 const float bodyCoefficient=1-std::exp(-6.2831853f*350/float(sr)),dcCoefficient=std::exp(-6.2831853f*15/float(sr));
 const bool hpOpen=s.hp<=20.01f,lpOpen=s.lp>=19999;for(int c=0;c<2;++c){cleanHP[c].cutoff(sr,s.hp);cleanLP[c].cutoff(sr,s.lp);}
 if(!initialised){cleanHPWeight=hpOpen?0:1;cleanLPWeight=lpOpen?0:1;delay=nextDelay=target;timeFade=1;mix=s.mix;feedback=s.feedback;gate=(!s.bypass&&(!s.throwMode||s.trigger))?1:0;active=s.bypass?0:1;analog=s.analog;width=s.width;pingBlend=s.ping?1:0;lastPing=s.ping;diffusion=s.diffusion;duckAmount=s.duck;colourWeight=s.mode==0?0:1;userLow=lpTarget;userHigh=hpTarget;colourLow=toneTarget;drive=driveTarget;bias=biasTarget;blend=blendTarget;wowDepth=depthTarget;bodyAmount=bodyTarget;initialised=true;}
 if(s.ping!=lastPing){pingSeedRemaining=s.ping?int(std::max(delay,nextDelay)):0;seedFold={};lastPing=s.ping;}
 if(timeFade>=1&&std::abs(target-delay)>.01f){nextDelay=target;timeFade=0;}
 for(int i=0;i<frames;++i){cleanHPWeight=slew(cleanHPWeight,hpOpen?0:1,fast);cleanLPWeight=slew(cleanLPWeight,lpOpen?0:1,fast);if(std::abs(cleanHPWeight-(hpOpen?0:1))<.00005f)cleanHPWeight=hpOpen?0:1;if(std::abs(cleanLPWeight-(lpOpen?0:1))<.00005f)cleanLPWeight=lpOpen?0:1;float in[2]={finite(left[i],0),finite(right[i],0)},wet[2];
 pingBlend=slew(pingBlend,s.ping?1:0,fast);if(std::abs(pingBlend-(s.ping?1:0))<.00005f)pingBlend=s.ping?1:0;
 mix=slew(mix,s.mix,smooth);if(std::abs(mix-s.mix)<.00005f)mix=s.mix;feedback=slew(feedback,s.feedback,smooth);analog=slew(analog,s.analog,smooth);width=slew(width,s.width,smooth);diffusion=slew(diffusion,s.diffusion,smooth);duckAmount=slew(duckAmount,s.duck,smooth);colourWeight=slew(colourWeight,s.mode==0?0:1,smooth);if(std::abs(colourWeight-(s.mode==0?0:1))<.00005f)colourWeight=s.mode==0?0:1;
 gate=slew(gate,(!s.bypass&&(!s.throwMode||s.trigger))?1:0,fast);active=slew(active,s.bypass?0:1,fast);if(std::abs(active-(s.bypass?0:1))<.00005f)active=s.bypass?0:1;
 userLow=slew(userLow,lpTarget,fast);userHigh=slew(userHigh,hpTarget,fast);colourLow=slew(colourLow,toneTarget,smooth);drive=slew(drive,driveTarget,smooth);bias=slew(bias,biasTarget,smooth);blend=slew(blend,blendTarget,smooth);
 power=slew(power,std::max(in[0]*in[0],in[1]*in[1]),detector);float level=10*std::log10(std::max(power,1e-12f));float knee=std::clamp((level-s.duckThreshold+6)/18,0.f,1.f);knee=knee*knee*(3-2*knee);float requested=18*duckAmount*knee;
 if(requested>duckDb){hold=float(sr*.04);duckDb=slew(duckDb,requested,attack);}else if(hold>0){hold-=1;}else duckDb=slew(duckDb,requested,release);
 if(s.duck==0&&duckDb<.001f)duckDb=0;const float duckGain=std::pow(10.f,-duckDb/20);
 phase+=6.2831853f*.43f/float(sr);slowPhase+=6.2831853f*.137f/float(sr);if(phase>6.2831853f)phase-=6.2831853f;if(slowPhase>6.2831853f)slowPhase-=6.2831853f;
 wowDepth=slew(wowDepth,depthTarget,smooth);bodyAmount=slew(bodyAmount,bodyTarget,smooth);
 float referenceGain=(1-blend)+blend*std::tanh(drive*.25f)/(drive*.25f);float makeup=1/std::max(.5f,referenceGain);
 for(int c=0;c<2;++c){float clean=read(c,delay);if(timeFade<1)clean=clean*(1-timeFade)+read(c,nextDelay)*timeFade;clean=filtered(clean,c,true,hpOpen,lpOpen);
 float x=0;if(colourWeight>0){float mod=(.7f*std::sin(phase+c*.7f)+.3f*std::sin(slowPhase+c*1.1f))*wowDepth*float(sr);
 x=read(c,std::clamp(delay-32+mod,4.f,float(ring[c].size()-5)));if(timeFade<1)x=x*(1-timeFade)+read(c,std::clamp(nextDelay-32+mod,4.f,float(ring[c].size()-5)))*timeFade;
 x=filtered(x,c,false,hpOpen,lpOpen);toneOne[c]+=colourLow*(x-toneOne[c]);x=colour[c].process(toneOne[c],drive,bias,blend);float dc=x-dcInput[c]+dcCoefficient*dcOutput[c];dcInput[c]=x;dcOutput[c]=dc;toneTwo[c]+=colourLow*(dc-toneTwo[c]);x=toneTwo[c];}
 float value=clean*(1-colourWeight)+x*colourWeight;
 // Bounded scattering: low settings create short smears, not high-Q ringing.
 float amount=std::clamp(diffusion/.7f,0.f,1.f);for(auto& stage:diffuse[c])value=stage.process(value,amount);wet[c]=value;}
 // Makeup is outside the nonlinear feedback path. Limit feedback peaks without clipping dry audio.
 // True left-first ping-pong: fold the wet send, inject only left, cross-feed repeats.
 // Highly anti-correlated stereo uses a smooth side-fold fallback instead of cancelling.
 float monoSend=folded(in[0],in[1],inputFold,detector,smooth);
 if(pingSeedRemaining>0){float seed=folded(wet[0],wet[1],seedFold,detector,smooth);wet[0]=wet[0]*(1-pingBlend)+seed*pingBlend;wet[1]*=1-pingBlend;--pingSeedRemaining;}
 for(int c=0;c<2;++c){float send=in[c]*(1-pingBlend)+(c==0?monoSend:0)*pingBlend;float returned=wet[c]*(1-pingBlend)+wet[1-c]*pingBlend;float magnitude=std::abs(returned);float safeReturn=magnitude<=.8f?returned:std::copysign(.8f+.2f*std::tanh((magnitude-.8f)/.2f),returned);ring[c][pos]=std::clamp(send*gate+feedback*safeReturn,-8.f,8.f);}

 float voiced[2];for(int c=0;c<2;++c){body[c]+=bodyCoefficient*(wet[c]-body[c]);voiced[c]=(wet[c]+body[c]*bodyAmount*colourWeight)*(1+(makeup-1)*colourWeight);}
 float mid=(voiced[0]+voiced[1])*.5f,side=(voiced[0]-voiced[1])*.5f*(width*(1-pingBlend)+std::min(width,1.f)*pingBlend),wetMix=std::sin(mix*1.57079632679f);
 left[i]=in[0]*(1-mix*active)+(mid+side)*wetMix*active*duckGain;right[i]=in[1]*(1-mix*active)+(mid-side)*wetMix*active*duckGain;
 if(timeFade<1){timeFade=std::min(1.f,timeFade+1.f/float(sr*.02));if(timeFade>=1)delay=nextDelay;}pos=(pos+1)%ring[0].size();}
 }
};
}
