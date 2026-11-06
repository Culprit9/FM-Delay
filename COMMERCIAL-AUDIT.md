# FM Delay commercial and DSP audit

UPDATE: The owner confirmed free JUCE Starter eligibility. The new local
candidate pins JUCE 8.0.12 with its MIT VST3 SDK and explicitly disables ASIO.
See FREE-BUILD.md for the current no-purchase path. References below to JUCE
8.0.6 and its older SDK describe the audited upstream baseline. No new candidate
build has passed yet. The baseline Windows workflow was subsequently confirmed
successful: https://github.com/Culprit9/FM-Delay/actions/runs/40982027586

Audited 2026-11-05, upstream Culprit9/FM-Delay main at
5edfbfea867d5c26d2a3abd9c20e0e8be872e6a8. Changes are local on
audit/proprietary-delay; no GitHub push, installed-plugin replacement or GUI edits.

## Recommendation and release blockers

Retain the existing self-contained C++ engine and exact JUCE GUI, obtain the
appropriate JUCE commercial entitlement, and resolve the bundled VST3 SDK license
before proprietary distribution. This is the least disruptive route. If all
framework code must be permissive, a separate native VST3 wrapper/editor port is
required; it cannot be presented as an exact GUI-preserving drop-in change.
Do that as a separate project with visual and saved-session parity tests.

This audit is source inspection, not proof of authorship or legal clearance.
No root source license or third-party notices file was present. Confirm ownership
of the existing code and supplied artwork reference, then choose explicit terms
for your original work. A public GitHub repository does not itself establish
permission to reuse its contents or make future published source confidential.
Assets/README.md records generated artwork derived from a supplied screenshot;
that record does not establish rights to the reference or any depicted branding.

## Dependency evidence

* CMake fetches JUCE tag 8.0.6, or accepts an unversioned JUCE_PATH override.
  Its modules are AGPLv3/commercial dual licensed, not MIT:
  https://github.com/juce-framework/JUCE/blob/8.0.6/LICENSE.md
* That specific tag bundles an older VST3 SDK under proprietary Steinberg/GPLv3
  terms; its text requires a signed agreement before publishing under the
  proprietary route. Do not substitute today's MIT claim for that version:
  https://github.com/juce-framework/JUCE/blob/8.0.6/modules/juce_audio_processors/format_types/VST3_SDK/LICENSE.txt
* Current upstream VST3 SDK LICENSE.txt is MIT. Suitable in principle for a new
  proprietary wrapper, retaining notices. Pin and audit the actual SDK commit,
  submodules and any editor library before adoption; it is not used by this patch:
  https://github.com/steinbergmedia/vst3sdk/blob/master/LICENSE.txt
* Signalsmith Audio dsp LICENSE.txt is MIT, copyright Geraint Luff / Signalsmith
  Audio Ltd. Retain its copyright and permission notice if incorporated:
  https://github.com/Signalsmith-Audio/dsp/blob/main/LICENSE.txt
  Not currently a dependency. No benefit established from replacing the working
  ring buffer merely to add a permissive library.
* JUCE lists additional codec, graphics, AU and other dependencies. Generate a
  component inventory and ship required notices from the exact resolved build;
  its root license's dependency list is a starting point, not a complete binary SBOM.
* No IV-X, Waves or Signalsmith includes/dependencies were found in Source/Tests
  or CMake. No external delay code was consulted or copied for this patch.
  Text inspection cannot prove historical provenance. Only two upstream commits
  exist, including a bulk source import; earlier authorship is not established.

## Signal path

Stereo input -> finite-value guard -> smoothed capture/bypass gate -> stereo or
left-first ping-pong send -> two 32-second ring buffers. Four-point cubic taps
crossfade over 20 ms for time changes. Clean repeats use smoothed two-pole
Butterworth LP/HP, blending to open endpoints. Colored taps compensate 32 samples
for the 4x FIR saturation path, with wow, user one-pole filters, character low-pass,
biased tanh saturation, DC removal and a second low-pass. Three short scattering
stages add diffusion. Cross-feedback uses a soft knee above .8 and .94 maximum
feedback; ring writes are clamped to +/-8. Wet body/makeup, width and stereo-linked
ducking are outside the feedback loop. Dry remains undelayed.

The existing design already supplies progressive filtering and nonlinear warmth.
It is an independently inspectable implementation, not a verified H-Delay model.
At minimum delay the compensated colored tap clamps, so timing differs from
Clean. Cubic interpolation may overshoot; limiting is a safeguard rather than a
formal proof of stability under every input. All-pass/diffusion and saturation
settings need renewed long-tail and automation measurements before release.

## Controls and compatibility

Parameter IDs/version 1 and existing presets were preserved:

| Control | Stored range and mapping |
|---|---|
| Time | 1–4000 ms, JUCE skew .35; Sync repurposes knob to 24 sorted note choices |
| Sync/division | 24 legacy indices retained; BPM clamped 30–400, fallback 120; up to 24 s at 30 BPM |
| Feedback | 0–.94, linear; engine smoothing 20 ms |
| Mix | 0–1, dry 1-mix, wet sin(pi*mix/2); not a constant-power crossfade |
| Analog | 0–1, character-dependent drive/filter/wow; disabled in Clean |
| Duck | 0–1 -> up to 18 dB wet reduction; threshold -60–0 dBFS |
| HP / LP | 20–2000 / 500–20000 Hz, skew .4; open endpoints bypass filters |
| Width | 0–2; Ping limits effective width to 1 |
| Diffusion | stored 0–.7; GUI Off and 1–100 via .007 steps |
| Character | Clean/Tape/Analog/Dark, indices 0–3 |
| Big button | effect bypass or latched Throw capture; existing tails retained in Throw |

Manual time and synced division intentionally use different mappings; preserve
those saved IDs. Most coefficients and gains are smoothed (5 or 20 ms); time is
dual-tap crossfaded. Character changes still warrant audio testing. A smoothed
control is not by itself proof of inaudible artifacts. No knob ranges, labels,
attachment code, layout, drawing, artwork or presets changed in this patch.

## Changes and validation

Fixed a source-level block-size dependency: if a new time target arrived during
an active crossfade, the next transition previously waited for the next process
call after fade completion. It now starts at the next sample boundary. Added
TimeAutomation regression with identical parameter event positions and host
chunks of 1/37/512/2048 samples. This changes only transition scheduling.

Added FM_BUILD_PLUGIN=OFF so original DSP tests can configure without fetching
JUCE or compiling its framework. Plugin builds remain enabled by default.

Available CTest suites: DSP, Presets, Regressions, Voicing, WetDiffusion,
CharacterDucking, PingPong, Texture, ToneFilters, plus new TimeAutomation.
QualityBenchmark builds but is not registered as a CTest. VocalRender,
EditorLifecycle and PluginCheck are not default CMake targets. HostCheck is
optional; DirectAUCheck is Apple-only with host checking enabled.

This session attempted CMake configuration, but cmake is absent. No cl/g++/clang++
was found on PATH; searches of Program Files found no clang++ or CMake.
Consequently no C++ compilation, CTest run, benchmark, plugin binary, listening
comparison or DAW validation was completed. Historical QUALITY-AUDIT.md claims
were not independently rerun. Source diff checks and GUI file identity were
checked separately. New regression and engine change remain uncompiled.

With a C++17 compiler and CMake 3.22+:

```
cmake -S . -B build-dsp -DFM_BUILD_PLUGIN=OFF
cmake --build build-dsp --config Release
ctest --test-dir build-dsp -C Release --output-on-failure
```

## Build targets and next release gate

CMake supports VST3 and Standalone, plus AU on macOS (minimum 11). Only stereo
buses, no MIDI, external sidechain or mono bus support. Windows script selects
VS2022 x64 and host tests. GitHub workflow runs on windows-2022; no macOS CI is
defined. Source README has stale Windows status statements; do not treat those
or existing Mac validation narratives as evidence about this local build.

Before shipping: resolve JUCE and actual SDK rights; pin all resolved dependencies,
collect notices, establish code/artwork ownership, then run both clean and changed
DSP suites, ASan lifecycle tests, VST3 validator and real Windows/macOS DAW tests.
Validate saved sessions, automation, bypass/tails, long maximum-feedback operation,
44.1/48/88.2/96/192 kHz and varied buffer sizes. Sign Windows binaries and sign/
notarize macOS builds as appropriate. Compare level-matched wet-only renders from
the same dry vocal/guitar against a legally obtained reference with documented
settings. No dry/reference audio is present here, so subjective warmth remains
an audition task. Do not claim commercial release readiness from this patch.
