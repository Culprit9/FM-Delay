# FM DELAY platform validation — October 29, 2026

These are measured build/test results, not a claim of compatibility with every DAW.

| Target | Package | Validation status |
|---|---|---|
| macOS 11+ Apple Silicon | Universal AU, VST3, standalone | Build contains arm64; native tests run on Apple Silicon |
| macOS 11+ Intel | Same universal package | Build contains x86_64; tests run through Rosetta on Apple Silicon; physical Intel Mac still needs testing |
| Windows x64 | Source/build kit only | Windows-specific build settings, launcher and optional manual CI prepared; no Windows binary built or executed here |
| Windows ARM64 native | None | Not built/tested |
| Pro Tools AAX | None | Not built/tested; AU/VST3 cannot replace AAX |
| VST2 | None | Not included |

## Host coverage

- Cubase 15 Mac: its installed VST3 scanner successfully recognized the universal candidate's effect/controller classes. This is a scan check, not playback/editor certification inside Cubase.
- Isolated JUCE VST3 host: actual universal bundle checked separately in arm64 and x86_64 processes for audio, state, and repeated native window open/resize/close.
- Apple AU validator: Apple Silicon validation passed. Intel validation through Rosetta could not discover any registered components (including FM DELAY), so Intel auval remains unverified. A separate process-local Intel AU factory/load/render/disposal check passed without changing the system registry.
- Fender Studio Pro: user has run earlier versions. No user song/session was modified for this platform work. Current universal build still needs a normal session smoke test.
- Ableton Live, Logic Pro, REAPER, FL Studio, Bitwig, other hosts: no full in-DAW session certification performed in this pass. VST3/AU support makes them potential hosts, not verified results.

## Windows build

Read WINDOWS-START-HERE.txt. The source project now includes AU only on Apple platforms; Windows builds VST3 and standalone. MSVC uses UTF-8 source and a static C++ runtime. `scripts/build-windows.ps1 -RunWindowTest` compiles, runs eight audio suites, loads the actual VST3 in an isolated host, checks state/window closing, and packages outputs. Its Windows execution remains outstanding.

The optional `.github/workflows/windows-build.yml` workflow must be placed in a repository rooted at this source folder. It is manual-only and has not been uploaded or dispatched.

## Session smoke test for each DAW/platform

Use an empty song and a known dry recording. Confirm discovery, stereo playback, tempo changes, notes/MS readout, all four characters, all 35 preset recalls, 100% wet on an aux, ducking meter/recovery, ping-pong, latched Throw/tail, resize, repeated window closing, save/reopen, and offline bounce. Test multiple instances and the DAW's normal low buffer. Record exact host/OS/CPU and any crash report. Existing audio quality approval against the user's H-Delay reference remains a separate outstanding task.

The deployment target is explicitly set to macOS 11 and the actual Mach-O minimum must be checked in both slices. Earlier builds left CMake’s initialized-empty target unchanged and therefore actually required macOS 15, despite the older README saying 11; the universal package corrects that build bug. Running tests on a modern Mac does not test macOS 11 itself.

Mac packages are locally ad-hoc signed, not notarized; Windows packages from this kit are not Authenticode signed. Architecture tests through Rosetta do not establish physical Intel hardware performance or compatibility with every macOS version since 11.
