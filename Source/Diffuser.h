#pragma once
#include <vector>
#include <algorithm>
#include <cmath>
namespace fm {
// A short scattering delay with bounded feedback. Diffusion changes density by
// changing delay length, never by moving a feedback pole toward the unit circle.
// Linear interpolation intentionally damps high frequencies in the diffuse tail.
class Diffuser {
 std::vector<float> buffer;
 size_t position=0;
 float maximumDelay=1;
public:
 void prepare(double rate,float milliseconds){maximumDelay=float(rate*.001*milliseconds);buffer.assign(size_t(std::ceil(maximumDelay))+2,0);position=0;}
 float process(float x,float amount){
  constexpr float g=.5f;
  float distance=std::clamp(amount,0.f,1.f)*maximumDelay;
  auto past=[&](size_t age){return buffer[(position+buffer.size()-age)%buffer.size()];};
  float y;
  if(distance<1){
   // Solve the sub-sample delay's implicit current-write contribution exactly.
   // At zero distance this reduces to y=x, with no off/on discontinuity.
   y=((g+1-distance)*x+distance*past(1))/(1+g*(1-distance));
  }else{
   size_t age=size_t(distance);float fraction=distance-float(age);
   float delayed=past(age)*(1-fraction)+past(age+1)*fraction;
   y=g*x+delayed;
  }
  buffer[position]=x-g*y;
  position=(position+1)%buffer.size();return y;
 }
};
}
