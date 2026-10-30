#pragma once
#include <array>
#include "DelayEngine.h"
#include "Timing.h"
namespace fm {
struct Preset {const char* category;const char* name;const char* tip;float time,feedback,mix,analog,duck,hp,lp,width,diffusion;int mode,note;bool sync,ping,throwMode;float duckThreshold=-36;};
inline constexpr std::array<Preset,37> presets={{
{"Init","Starting Point","Balanced tape echo. Choose a source category to get started.",375,.42f,.30f,.45f,0.15f,60,5000,1,0,1,4,true,false,false},
{"Lead Vocal","Front & Center","Subtle quarter-note repeats; ducking leaves room for the lyric.",375,.28f,.18f,.22f,0.36f,80,4200,1,0.015f,1,4,true,false,false},
{"Lead Vocal","Velvet Slap","A warm dotted-32nd slap for body; follows host tempo.",105,.12f,.16f,.40f,0.15f,90,3600,1,0.0075f,1,13,true,false,false},
{"Lead Vocal","After Hours","Dotted-eighth echoes open up between phrases.",375,.39f,.23f,.30f,0.396f,110,3800,1.15f,0.03f,1,3,true,true,false},
{"Lead Vocal","Last Word Throw","Click to capture a phrase ending; click again to release a dark quarter-note trail.",375,.52f,.29f,.38f,0.21f,140,3000,1.15f,0.045f,3,4,true,true,true},
{"BG Vocal","Halo Stack","Low-mix eighth-note taps tuck a vocal stack behind the lead.",250,.22f,.15f,.20f,0.27f,140,4800,1.25f,0.035f,0,2,true,true,false},
{"BG Vocal","Silk & Shadows","Dark quarter-note bloom with softened, diffuse repeats.",500,.37f,.22f,.40f,0.252f,160,2800,1.25f,0.075f,3,4,true,true,false},
{"BG Vocal","Choir Lift","Long half-note motion for sustained harmonies; restrained low end.",1000,.40f,.24f,.22f,0.3f,180,4500,1.35f,0.085f,1,6,true,true,false},
{"BG Vocal","Pocket Double","A tight sixteenth-triplet double; follows host tempo.",80,.08f,.13f,.16f,0.12f,120,5000,1,0.02f,0,14,true,false,false},
{"Snare","Backbeat Slap","A dark dotted-32nd slap keeps the hit forward.",90,.10f,.14f,.42f,0.072f,250,3600,1,0.0125f,2,13,true,false,false},
{"Snare","Ghost Notes","Quiet sixteenth-note taps create movement around the backbeat.",125,.25f,.13f,.24f,0.12f,350,4200,1.10f,0.02f,1,0,true,true,false},
{"Snare","Dub Rim","Filtered dotted-eighth repeats, a little grit, no low-frequency clutter.",375,.44f,.21f,.55f,0.144f,450,2800,1.20f,0.03f,2,3,true,true,false},
{"Snare","Fill Throw","Click to capture a snare fill; click again to let the eighth-note spill decay.",250,.47f,.25f,.32f,0.108f,300,3800,1.2f,0.045f,1,2,true,true,true},
{"Guitar","Nashville Slap","A focused sixteenth-note tape slap for clean picking and rhythm parts.",115,.14f,.18f,.38f,0.09f,70,4200,1,0.01f,1,0,true,false,false},
{"Guitar","Dotted Drive","Dotted-eighth repeats for rhythmic picking; play to the host tempo.",375,.36f,.27f,.24f,0.108f,80,4600,1.15f,0.0125f,1,3,true,true,false},
{"Guitar","Solo Spotlight","Quarter-note sustain that ducks under new notes.",500,.35f,.23f,.32f,0.3f,90,3800,1.1f,0.03f,1,4,true,true,false},
{"Guitar","Midnight Swell","Dark half-note trails turn sparse chords into a soft backdrop.",1000,.50f,.27f,.40f,0.21f,140,2600,1.3f,0.1f,3,6,true,true,false},
{"Piano","Felt Room","An understated eighth-triplet echo adds space without clouding chords.",160,.16f,.13f,.15f,0.228f,100,4800,1,0.03f,1,1,true,false,false},
{"Piano","Glass Steps","Clean eighth-note repeats for rhythmic piano and arpeggios.",250,.29f,.19f,.10f,0.21f,120,6400,1.15f,0.015f,0,2,true,true,false},
{"Piano","Evening Waltz","Dotted-quarter motion gives sparse chords a rolling three-eighth feel.",750,.34f,.21f,.24f,0.27f,140,4200,1.2f,0.05f,1,5,true,true,false},
{"Piano","Soft Horizon","Diffuse half-note echoes for ballads and open arrangements.",1000,.44f,.24f,.30f,0.3f,160,3200,1.3f,0.095f,3,6,true,true,false},
{"Synth","Clockwork","Clean sixteenth-note taps add motion to a dry synth part.",125,.28f,.20f,.08f,0.09f,70,7000,1.1f,0.01f,0,0,true,true,false},
{"Synth","Neon Bounce","Bright dotted-eighth ping-pong, softened just enough for repeated notes.",375,.42f,.27f,.20f,0.15f,90,5500,1.3f,0.03f,1,3,true,true,false},
{"Synth","Analog Orbit","A darker quarter-note loop with tape movement and diffuse edges.",500,.47f,.25f,.58f,0.18f,160,3600,1.25f,0.075f,2,4,true,true,false},
{"Synth","Cloud Nine","Long half-note ambience for pads and sustained notes.",1000,.55f,.28f,.28f,0.288f,180,3500,1.35f,0.12f,3,6,true,true,false},
{"FX","Telephone Trail","Band-limited quarter-note repeats for transitions and ear candy.",500,.48f,.30f,.50f,0.09f,1100,2200,1.2f,.15f,2,4,true,true,false},
{"FX","Triplet Sparks","Short eighth-triplet bounces turn a single hit into rhythmic decoration.",167,.40f,.26f,.16f,0.072f,220,4800,1.3f,.10f,0,1,true,true,false},
{"FX","Dub Throw","Click to start a long, dark dotted-quarter trail; click again to let it decay.",750,.64f,.34f,.62f,0.12f,180,2400,1.25f,.32f,3,5,true,true,true},
{"FX","Transition Cloud","Click to capture an accent; click again for diffuse half-note echoes into the next section.",1000,.58f,.32f,.42f,0.192f,220,3000,1.35f,.52f,3,6,true,true,true},
{"Favorites","FM's Fav Delay","FM’s screenshot favorite: dark half-note ping-pong, full diffusion, continuous capture.",1143,.68f,.70f,.50f,0,370,9500,1,.7f,3,6,true,true,false,-39.385f},
{"Adlib Vocal","Pocket Replies","Warm dotted-eighth replies between adlibs; steady, clear left/right motion.",375,.36f,.25f,.45f,.25f,140,4200,1,.02f,1,3,true,true,false},
{"Adlib Vocal","Triplet Sauce","Darker eighth-triplet bounces for rhythmic adlibs and answer phrases.",167,.45f,.30f,.6f,.18f,180,3800,1.15f,.08f,2,1,true,true,false},
{"Adlib Vocal","Hype Slap","A close dotted-32nd tape slap adds size without a long wash.",95,.18f,.22f,.5f,.18f,120,4800,1.25f,0,1,13,true,false,false},
{"Adlib Vocal","Adlib Throw Tail","Click to capture a shout or phrase; click again for a warm quarter-note trail.",500,.58f,.32f,.6f,.2f,180,3200,1,.09f,1,4,true,true,true},
{"Radio","Radio Delay","Band-limited eighth-triplet radio repeats. Mix 100% on a return for radio-only echoes.",165,.32f,.28f,.6f,.15f,650,3100,1,.025f,2,1,true,false,false},
{"Lead Vocal","Studio Sixteenth","Clean, centered sixteenth-note repeats based on the vocal reference audition.",142.85714f,.10f,.175f,0,0,350,2500,1,0,0,0,true,false,false},
{"Lead Vocal","Full Vocal Echo","Fuller low mids, stronger centered repeats and a gently longer sixteenth-note trail.",142.85714f,.18f,.205f,0,0,160,2200,1,0,0,0,true,false,false}
}};
inline Settings presetSettings(const Preset& p,float bpm=120){Settings s;s.timeMs=p.sync?60000.f/bpm*noteBeats[p.note]:p.time;s.feedback=p.feedback;s.mix=p.mix;s.analog=p.analog;s.duck=p.duck;s.hp=p.hp;s.lp=p.lp;s.width=p.width;s.diffusion=p.diffusion;s.duckThreshold=p.duckThreshold;s.mode=p.mode;s.ping=p.ping;s.throwMode=p.throwMode;return s;}
}
