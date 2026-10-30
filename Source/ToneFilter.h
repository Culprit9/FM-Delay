#pragma once
#include <cmath>
#include <algorithm>
namespace fm {
// Topology-preserving, non-resonant two-pole Butterworth filter.
class ToneFilter {
 float g=0,target=0,memory1=0,memory2=0,smoothing=0;bool ready=false;
public:
 void reset(){g=target=memory1=memory2=0;ready=false;}
 void cutoff(double rate,float frequency){target=std::tan(3.14159265358979323846*std::clamp(double(frequency),10.,rate*.45)/rate);smoothing=std::exp(-1.f/float(rate*.005));if(!ready){g=target;ready=true;}}
 float process(float x,bool high){g=target+smoothing*(g-target);constexpr float k=1.41421356237f;float a1=1/(1+g*(g+k)),a2=g*a1,a3=g*a2,v3=x-memory2,v1=a1*memory1+a2*v3,v2=memory2+a2*memory1+a3*v3;memory1=2*v1-memory1;memory2=2*v2-memory2;return high?x-k*v1-v2:v2;}
};
}
