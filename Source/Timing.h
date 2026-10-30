#pragma once
#include <array>
namespace fm {
// Preserve the original eight indices for saved sessions.
inline constexpr std::array<const char*,24> noteNames={"1/16","1/8 T","1/8","1/8 D","1/4","1/4 D","1/2","1 BAR","1/64","1/64 T","1/64 D","1/32","1/32 T","1/32 D","1/16 T","1/16 D","1/4 T","1/2 T","1/2 D","1 BAR T","1 BAR D","2 BARS","2 BARS T","2 BARS D"};
inline constexpr std::array<float,24> noteBeats={.25f,1.f/3,.5f,.75f,1,1.5f,2,4,.0625f,1.f/24,.09375f,.125f,1.f/12,.1875f,1.f/6,.375f,2.f/3,4.f/3,3,8.f/3,6,8,16.f/3,12};
inline constexpr std::array<int,24> noteOrder={9,8,12,10,11,14,13,0,1,15,2,16,3,4,17,5,6,19,18,7,22,20,21,23};
inline int notePosition(int index){for(int i=0;i<24;++i)if(noteOrder[i]==index)return i;return 13;}
inline float syncedMilliseconds(float bpm,int index){return 60000.f/bpm*noteBeats[static_cast<unsigned>(index)<noteBeats.size()?index:4];}
}
