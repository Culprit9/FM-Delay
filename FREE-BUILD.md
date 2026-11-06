# FM Delay: no-purchase build candidate

The owner confirmed on 2026-11-05 that applicable revenue/funding is below the
JUCE Starter limit. Use the free Starter commercial licence subject to its terms;
this is not an AGPL release of FM Delay. No purchase or subscription was made.
The Starter limit is eligibility-based, not an unlimited lifetime exemption.
https://juce.com/legal/juce-8-licence/

## This candidate

* Same source GUI, red-button panel, artwork, presets and parameter IDs.
* Existing self-contained DSP, with the queued time-transition scheduling fix.
* JUCE 8.0.12 pinned to 29396c22c93392d6738e021b83196283d6e4d850.
* Its bundled VST3 SDK is MIT licensed; the 8.0.6 bundled-SDK finding in the
  original audit describes the baseline, not this new candidate.
* Explicit JUCE_ASIO=0 for plugin/standalone and host-check targets. The new
  JUCE release includes separately licensed ASIO files; those are not needed.
  The VST3 uses the DAW's device driver; the standalone uses Windows native audio.
* Tests can configure independently of JUCE. Added a three-platform DSP workflow
  and pull-request Windows build checks. These workflows have not run on this
  candidate. No remote branch was pushed and no metered job was started.

JUCE changelog:
https://github.com/juce-framework/JUCE/blob/8.0.12/CHANGE_LIST.md
Actual SDK licence:
https://github.com/juce-framework/JUCE/blob/8.0.12/modules/juce_audio_processors_headless/format_types/VST3_SDK/LICENSE.txt

## Build and test

Use free CMake 3.22+, Git and an eligible free Microsoft C++ toolchain on Windows.
The existing script expects Visual Studio 2022 C++ tools:

```
powershell -File scripts/build-windows.ps1 -RunWindowTest
```

That compiles VST3/Standalone, runs CTest and three native window checks, then
packages the results. Do not replace an installed plugin until those checks pass.
No compiler/CMake is installed in the current session, so this source candidate
has NOT been compiled or certified. Git network access also failed here.

For engine-only testing on a computer with a C++17 compiler:

```
cmake -S . -B build-dsp -DFM_BUILD_PLUGIN=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build-dsp --config Release
ctest --test-dir build-dsp -C Release --output-on-failure
```

Your original main commit did pass Windows GitHub Actions on October 29 local
time (October 30 UTC). The build/test/package step and artifact upload succeeded:
https://github.com/Culprit9/FM-Delay/actions/runs/40982027586
That validates the older baseline only. It does not validate the JUCE upgrade,
new test or changed engine. A framework update can affect text/rendering even
when the GUI source and image are identical: compare screenshots at all scales.

## Release without purchases

Start with a tested Windows VST3. Paid copy protection, a paid DSP library and an
Apple account are not needed to develop that target. No new DSP dependency was
added. Do not purchase Waves or copy IV-X code to build this engine.

The current source package is a development candidate, not a finished commercial
distribution. Still required: successful new builds/tests, wet-only listening
comparisons, GUI parity checks, notices for actual linked dependencies, and
code/artwork ownership confirmation. The JUCE upgrade does not make every
third-party component MIT licensed.

Mac builds remain possible with existing Mac hardware/tooling, but this no-cost
path does not include paid Developer ID membership or Apple notarization. Do not
advertise unsigned Mac builds as providing normal notarized installation.

If Starter eligibility ends, stop and reassess before further distribution:
either acquire the appropriate licence or complete a permissive framework port.
No-cost proprietary use of JUCE cannot be promised beyond its eligibility terms.
