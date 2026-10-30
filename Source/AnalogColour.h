#pragma once
#include <array>
#include <cmath>
namespace fm {
// Four-times, linear-phase FIR interpolation/decimation. 32 base-rate samples latency.
class AnalogColour {
 static constexpr int taps=129, history=33;
 std::array<float,taps> coefficients{};std::array<float,history*2> input{};std::array<float,taps*2> output{};int ip=0,op=0;
public:
 void prepare(){constexpr double pi=3.14159265358979323846;double sum=0;for(int i=0;i<taps;++i){double t=i-64.;double sinc=t==0?.225:std::sin(2*pi*.1125*t)/(pi*t);double a=2*pi*i/(taps-1);double window=.35875-.48829*std::cos(a)+.14128*std::cos(2*a)-.01168*std::cos(3*a);coefficients[i]=float(sinc*window);sum+=coefficients[i];}for(auto& c:coefficients)c/=float(sum);input={};output={};ip=op=0;}
 float process(float x,float drive,float bias,float blend){input[ip]=input[ip+history]=x;const float* past=input.data()+ip+history;float result=0;float zero=std::tanh(drive*bias);for(int phase=0;phase<4;++phase){float up=0;for(int k=0;phase+4*k<taps;++k)up+=coefficients[phase+4*k]*past[-k]*4;float shaped=(std::tanh(drive*(up+bias))-zero)/drive;float v=up+(shaped-up)*blend;output[op]=output[op+taps]=v;if(phase==0){const float* high=output.data()+op+taps;for(int i=0;i<taps;++i)result+=coefficients[i]*high[-i];}op=(op+1)%taps;}ip=(ip+1)%history;return result;}
};
}
