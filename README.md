# FM DELAY 0.11.0 — audited listening beta

Original stereo AU/VST3 delay for Apple Silicon and Intel Macs, macOS 11+ (universal build). Includes a standalone audition app. The giant red button and black metal panel follow Donnie's supplied design reference.

## Update safely

Save your song, quit and reopen the DAW before testing this version. A confirmed older editor teardown defect could crash a host when its window closed. Replacing files does not replace a binary already loaded in a running DAW. The update fixes attachment lifetime/destruction order; it passed sanitized and native-host close tests. See QUALITY-AUDIT.md for evidence and limits.

Copy FM DELAY.vst3 into ~/Library/Audio/Plug-Ins/VST3 and FM DELAY.component into ~/Library/Audio/Plug-Ins/Components. These builds are locally ad-hoc signed, not notarized. The task's installed AU/VST3 update replaces the previous copies; old backups stay outside scanned plugin folders.

## Start here

Use a stereo track. Select Starting Point or a source preset. New instances use quarter-note Sync. Every factory preset loads note Sync and follows host tempo, including slap/doubling recipes. Saved millisecond sessions remain unchanged until you select/Reload a preset. Mix around 20–35% is an insert starting point. On a send/aux return use 100% Mix: that is delay-only, with no immediate dry input. In idle Throw Mode no new audio enters until capture is started.

Click SYNC for notes or MS for milliseconds. The Time knob follows the selected timing mode. The note browser has Straight, Triplet and Dotted groups, retaining all 24 choices. The readout shows note-derived milliseconds and host tempo, with 120 BPM fallback. Double-click the millisecond readout to type an exact value.

## Character

- **Clean:** uncolored delayed copy when HP/LP are open and Diffusion/Ducking are off. With filters engaged, original two-pole Butterworth HP/LP shape the repeats; filter endpoints blend smoothly to bypass. Analog amount is intentionally disabled. Choose this to check timing and clean repeats.
- **Tape:** soft asymmetric saturation, moderate bandwidth and subtle wow.
- **Analog:** stronger symmetric saturation, a darker BBD-inspired bandwidth and more modulation. This retains the old Vintage choice index for session compatibility.
- **Dark:** subdued, low-pass repeats with little movement.

Colored processing uses 4x FIR interpolation/decimation to reduce nonlinear aliasing. Its 32-sample filter latency is compensated in the colored delay tap at normal music sample rates. The dry path is not delayed. Analog adjusts the amount of coloration in Tape/Analog/Dark. HP/LP remain independent user filters.

## Ducking

Ducking reduces the wet return while the source plays and lets repeats emerge in gaps. The knob controls up to 18 dB of reduction. **DUCK THRESHOLD** determines when it engages; the red DUCK readout shows actual gain reduction.

For a lead vocal, begin with a moderate Ducking amount and adjust Threshold until the meter shows roughly 3–6 dB reduction during phrases. Reduce Amount if echoes are too recessed; raise Threshold if quiet breaths trigger it. Detection is stereo linked, with RMS sensing, soft engagement, 8 ms gain attack, 40 ms hold and 220 ms release. It does not lower the dry track or suppress the feedback loop. There is no external sidechain input.

## Throw, width and diffusion

With Throw Mode off, click the big button to toggle effect bypass. With Throw Mode on, click once to capture and again to stop capture; existing echoes continue. Mouse release and closing the editor do not stop a latched capture. Preset loads and session recall reset capture for safety. Capture is not quantized to the grid; note length controls repeat spacing.

Ping Pong uses a left-first wet feed and cross-feedback for actual L/R/L/R repeats. Hard-panned input no longer feeds both delay lanes on the first repeat. Highly anti-correlated stereo uses a smooth side-fold fallback instead of cancelling. The dry stereo signal stays intact. Turning Ping on also seeds an existing centered tail into alternating repeats. Width zero is mono; Width 1.00 or higher gives full left/right separation. Values above 1.00 no longer create opposite-channel phase ghosts in Ping mode. Width retains its usual range outside Ping mode. Diffusion spreads repeats into a denser texture. Its slider and editable number box use Off, then whole-number steps 1–100. Existing preset/session amounts remain stored in their original range; moving the slider selects the nearest whole-number step. Starting Point has Diffusion off. The 0.10 diffuser uses short, bounded-feedback scattering delays; small amounts make shorter smears instead of the old long resonant ringing. Fractional interpolation gently damps high frequencies in diffuse tails; Diffusion is a texture control, not a spectrally transparent effect.

## Presets and window

37 factory programs: the existing instrument library plus FM's Fav Delay, four Adlib Vocal presets, Radio Delay, and Starting Point. Use the category browser and previous/next arrows to audition. Reload restores a selected factory recipe; Edited means its parameter values differ. Your DAW session saves edited values and selection. New ducking is stronger and presets were retuned for it; use Reload to get the current recipe. PRESETS.md lists the intended uses.

Choose 60–150% in the size menu or drag the bottom-right corner. Artwork, controls and mouse targets scale together. Size is saved with the session. Popups remain compact and do not enlarge with window zoom.

## New presets

FM's Fav Delay follows Donnie’s October 29 screenshot: half-note Sync, approximately 68% Feedback, 70% Mix, 50% Analog, Ducking off, HP 370 Hz, LP 9.5 kHz, Width 1.00, Diffusion 100, Dark, Ping on, Throw off and -39.385 dBFS threshold. This supersedes the earlier literal-max recipe. Hidden knob values were estimated with permission; existing sessions retain their saved edits. Select FM's Fav and click Reload to hear the new recipe.

Adlib Vocal: Pocket Replies, Triplet Sauce, Hype Slap, Adlib Throw Tail. Radio: Radio Delay, a band-limited eighth-triplet echo. Factory entries were appended so existing saved program indices remain intact.

## Validation and practical limits

See QUALITY-AUDIT.md. Checks cover character bandwidth and Clean transparency, oversampling alias rejection, ducking threshold/depth/recovery and dry preservation, timing, all 37 factory states, full wet, stereo/anti-phase input, diffusion, feedback stability, UI scaling and saved-state compatibility. AddressSanitizer reproduced the old close-window use-after-free; the corrected editor passed 250 stress cycles. The actual VST3 passed 30 native window cycles with concurrent audio in an isolated host.

These checks are not a subjective listening evaluation of your recordings or a Fender Studio Pro session certification. The benchmark is an isolated CPU measurement. Mono buses and external sidechain are not supported yet. The universal Mac package contains arm64 and x86_64; Intel tests use Rosetta and physical Intel hardware remains unverified. Windows x64 build tools are included, but a Windows binary has not yet been built or tested. See COMPATIBILITY.md and WINDOWS-START-HERE.txt. Very short colored delays at unusually low sample rates are limited by oversampling filter latency. Saturation oversampling reduces aliasing; it does not make all nonlinear settings alias-free.

## Build

CMake 3.22+, macOS C++17 compiler, JUCE 8.0.6:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j 4
ctest --test-dir build --output-on-failure
./build/quality_benchmark
```

CMake fetches JUCE unless -DJUCE_PATH=/absolute/path/to/JUCE is supplied. Binaries appear under build/FMDelay_artefacts/Release. EditorLifecycle.cpp, PluginCheck.cpp and HostCheck.cpp are additional integration harnesses; the first two link to plugin shared code, while HostCheck builds with JUCE_PLUGINHOST_VST3=1 and JUCE_MODAL_LOOPS_PERMITTED=1. Sanitizer verification requires all shared-code/JUCE objects to be instrumented, not only the harness. JUCE is a separate dependency with its own license.

Workflow reference: Waves H-Delay user manual, https://assets.wavescdn.com/pdf/plugins/h-delay-hybrid-delay.pdf. Build reference: JUCE 8.0.6 CMake API, https://github.com/juce-framework/JUCE/blob/8.0.6/docs/CMake%20API.md. No Waves code or assets are used; no claim of superior sound has been established.

## Platform builds

For a universal Mac build, configure with `-DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"`. Run `python3 scripts/test-mac-architectures.py build` to execute both architecture slices; Rosetta is required for Intel tests on Apple Silicon.

On Windows x64, follow WINDOWS-START-HERE.txt or run `scripts/build-windows.ps1 -RunWindowTest` after installing Visual Studio 2022 C++ tools. `FM_BUILD_HOST_CHECK=ON` builds the isolated VST3 host on either platform. The Windows kit is source, not an installable Windows plugin until a successful Windows build/test run.

## New clean vocal starting point

Lead Vocal / Studio Sixteenth uses the clean-filter audition direction: host-synced sixteenth notes, Feedback10%, Mix17.5%, HP350Hz, LP2.5kHz, no extra color, ducking or diffusion, and centered stereo. At105BPM the repeat is142.9ms. The listening comparison was matched in average level; use the DAW fader to compare fairly. Existing Clean sessions retain their parameter values but the engaged HP/LP now have steeper slopes, so their sound intentionally changes. FM's Fav retains the separate half-note screenshot recipe.

Lead Vocal / Full Vocal Echo adds body (HP160Hz), smoother upper repeats (LP2.2kHz), Mix20.5% and Feedback18% for a stronger, gently longer sixteenth-note alternative. All37 factory recipes load note Sync.
