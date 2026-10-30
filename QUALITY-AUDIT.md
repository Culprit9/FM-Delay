# FM DELAY 0.8.0 — quality and crash audit

October 27, 2026. This is an engineering audit and listening candidate, not a claim that subjective sound quality or a particular Fender Studio Pro song has been certified.

## Confirmed close-window defect

The Time slider attachment was declared before its Slider target. Reverse member destruction could delete the Slider before its attachment removed its listener. The isolated AddressSanitizer reproducer failed with `heap-use-after-free` in `juce::Slider::removeListener`, reached from `SliderParameterAttachment::~SliderParameterAttachment` and `Panel::~Panel`.

Fixed both member ordering and explicit teardown: stop the timer, dismiss owned popups, disconnect attachments while their controls still exist, clear callbacks, then release look-and-feel references. Closing the editor does not change latched Throw audio state.

The corrected code passed 250 sanitized editor create/close cycles, including MS and Sync, resizing, popup teardown, queued parameter updates from another thread, and concurrent processing. The actual VST3 also passed 30 native editor open/resize/close cycles in an isolated JUCE host with audio running and a state roundtrip. These are stronger checks than the earlier offscreen smoke test. No recent Fender crash stack was available to prove this is its only possible shutdown cause. The user's song and DAW session were left untouched.

## Audio findings and corrections

- Clean previously used colored processing. It is now a genuinely uncolored delayed copy when HP/LP are open and Diffusion/Ducking are off; Analog is intentionally disabled in Clean.
- Character choices are **Clean, Tape, Analog, Dark**. Legacy Vintage remains index 2 and is now labeled Analog, preserving saved choice identity. Tape uses soft asymmetric saturation and subtle wow; Analog has stronger symmetric color, narrower bandwidth and more movement; Dark emphasizes subdued, low-pass repeats.
- Nonlinear processing now uses 4x FIR interpolation/decimation rather than base-rate tanh alone. The colored tap compensates the FIR's 32-sample latency at normal music sample rates. Dry audio remains zero latency.
- Wet makeup/body compensation stays outside the feedback path. Feedback peaks have a bounded soft safeguard. Width and filter/control transitions are smoothed. Non-finite input/settings are guarded.
- Ducking was a weak level-dependent multiplier. It is now a stereo-linked RMS detector with adjustable threshold, a soft engagement knee, up to 18 dB depth, 8 ms gain attack, 40 ms hold and 220 ms release. Only the wet return is reduced; feedback tails remain intact. The interface shows actual gain reduction. Default and factory amounts were retuned for the stronger detector.
- Starting Point now has Diffusion off. Non-FX presets have less diffusion so basic repeats are clearer and less phase-smeared. Diffusion remains available deliberately for textured echoes.
- Note and preset menus use short submenus and do not enlarge with editor zoom. All 24 note choices and their saved IDs remain intact.
- Tail reporting now follows the selected delay and feedback instead of assuming 120 seconds for every setting.

## Measurements

At 48 kHz with the documented test fixtures:

| Check | Result |
|---|---|
| Clean, open filters, stationary integer delay | Maximum sample error 0 |
| 250 Hz / 8 kHz RMS, Clean / Tape / Analog / Dark | 1.00 / 3.27 / 7.93 / 25.47 |
| Oversampling alias test | Passed >26 dB rejection requirement for the defined folded 6 kHz harmonic from a driven 14 kHz input |
| Duck amount 0 / 50 / 100%, sufficiently above threshold | 0 / 9 / 18 dB gain reduction |
| Below-threshold input | No gain reduction |
| Phrase ends | Gain reduction releases below 0.1 dB in the defined recovery test |
| Dry path with ducking | Unchanged |
| Presets | All 29 parameter/state roundtrips; audio exercised at three rates and three tempos |
| Wet endpoints / stereo / diffusion | Dry rejection, audible anti-phase returns, tone energy and impulse spreading checks passed |
| UI | Saved MS compatibility, note-sync defaults, threshold wiring, menu IDs, resize hit targets and saved size checked |

Isolated processing benchmark at 96 kHz, 32–1024-sample blocks: worst average processing time used about 7% of the block deadline on this machine. This does not predict CPU load in a busy DAW song.

Installed AU validation specifically confirmed version 0.8.0 and passed. The installed VST3 signature verified.

## Remaining validation

No real dry vocal/guitar recording was provided for subjective comparison. A short dry source and the disliked processed example would let the next pass address audible problems directly. A saved Fender Studio Pro test song should still confirm native close/reopen behavior after the user restarts into this build. Mono buses and external sidechain are not implemented. Extremely short colored delays at unusually low sample rates can be limited by the oversampling filter latency. Four-times oversampling reduces aliasing; it does not make every nonlinear setting alias-free.

## October 28, 2026 — 0.9.0 Ping Pong and preset follow-up

Found that the previous mid/side input injection sent hard-panned sources into both delay lanes, preventing true alternating repeats. Width expansion also made an opposite-polarity opposite-channel ghost. Corrected left-first injection with smooth anti-correlation fallback, cross-feedback, mode-transition tail seeding and Ping width limited to actual hard L/R separation. Tests verify five repeats, their feedback ratios and opposite-channel rejection for centered, left-only, right-only and anti-phase inputs at 44.1/48/96 kHz and Width 1/1.35/2. Switching on Ping during an existing centered tail and Width-zero mono were checked.

Added six programs, preserving all prior indices: literal-max FM's Fav Delay (explicitly confirmed by the user), four Adlib Vocal recipes and Radio Delay. The library totals 35 programs. Favorite intentionally uses 100% wet Mix, 4 seconds free Time, 94% Feedback, maximum filters/color/width/diffusion/duck controls, with bypass off and continuous capture. Its maximum threshold of 0 dBFS intentionally means normal-level input does not trigger ducking.

0.9.0 validation: all seven audio suites passed; all 35 program/state recalls and literal-max endpoints passed; 250 memory-sanitized editor cycles and 30 native VST3 cycles with concurrent audio passed. Installed AU validation specifically confirmed 0.9.0; VST3 signature verified. Fender Studio Pro itself was not manipulated.

## October 29, 2026 — 0.10.0 texture correction from user bounces

Donnie supplied FM DELAY and H DELAY bounces of the same verse (both stereo 48 kHz/24-bit, 30.19225 s), disliked FM's digital texture, and asked for H-Delay-like musical sound with better results. This is an approval target, not a verified superiority claim. Files were measured, not subjectively auditioned by the agent. FM peak/RMS: -17.57/-32.41 dBFS, no clipped samples; H: -15.75/-31.23 dBFS. Alignment offset 0 samples, cross-correlation .9854. H's RMS is 1.18 dB higher. Whole-bounce band energies are very similar. These mixed bounces do not isolate either wet path, establish equivalent settings, or prove the origin of the disliked sound. Dry vocal and settings were requested; neither was available during this correction. Do not process the already-delayed bounce as a substitute for dry-source evaluation.

Confirmed independent DSP defect: the old Diffusion formula moved all-pass poles toward unity at small amounts. At .0075, roughly 2.4% of isolated-echo energy remained more than 200 ms after the echo; .015 also rang. Larger amounts could settle sooner than smaller amounts. This affected several subtle vocal recipes. The old code fails the new Texture regression. It is a plausible contributor, not an established diagnosis of the supplied bounce.

Replaced it with three short scattering delays with fixed .5 feedback and amount-controlled lengths. Sub-sample delays solve the implicit current-write contribution, providing a continuous zero-amount identity instead of an abrupt switch. Fractional linear interpolation deliberately damps upper frequencies; this diffuser is not an energy-flat all-pass effect. Body is retained; tests enforce low-frequency preservation, bounded damping and no resonant gain. Updated the old high-frequency-flatness test to reflect this explicit design change rather than hiding the changed response. Across 44.1/48/96 kHz and amounts .0001 to .7, isolated echoes settle below -70 dB relative energy after 200 ms. Automation and host-block-size invariance tests cover the new path.

Also replaced the feedback safeguard's hard clamp at unity with a continuous-slope soft knee above .8. This affects high internal levels; the supplied file's low sample peaks do not establish whether that safeguard was active. Original dry audio remains outside this feedback protection. All 35 recipes and literal-max Favorite controls retained. Existing sessions retain parameters, but diffusion and overloaded-feedback sound intentionally change; previous binaries are backed up.

0.10.0 checks passed: all eight CTest suites (78.60 s), all 35 factory state/parameter recalls and literal-max endpoints, tempo/control/Throw/resizing integration checks, 250 fully instrumented AddressSanitizer editor close cycles, and 30 native VST3 editor open/resize/close cycles with 2,500 audio blocks plus state roundtrip. Isolated 96 kHz benchmark worst mean deadline use 6.79%; this is not a busy Fender song CPU guarantee. Installed AU/VST3 0.10.0; 0.9.0 retained outside scanned folders. Actual Fender session untouched. Subjective approval and source-isolated H-Delay comparison remain outstanding.
Apple auval specifically reported Component Version 0.10.0 (0xA00) and AU VALIDATION SUCCEEDED. Installed VST3 strict deep signature verification passed.

## October 29, 2026 — universal Mac and Windows build preparation

Universal 0.10.0 AU/VST3/standalone bundles now contain arm64 and x86_64. Discovered and corrected a deployment-target bug: CMake initialized an empty cache value before the previous non-forcing assignment, so older binaries actually carried minimum macOS 15 despite README claims of 11. The default now sets 11 only when no target was supplied. Verified actual Mach-O LC_BUILD_VERSION minos 11.0 in both slices of all three final bundles. This establishes the compiled minimum, not runtime testing on macOS 11 itself.

Final macOS-11-targeted DSP builds passed all eight suites natively (80.07 s) and in x86_64 through Rosetta. Both architecture control harnesses passed all 35 recalls, timing, latching, resize and state checks. The final universal VST3 passed 30 native editor/audio/state cycles in each architecture (1,406 arm64 and 1,172 x86_64 audio blocks). Cubase 15's scanner recognized final VST3 classes in both architectures. This is not a full Cubase session certification. Direct process-local AU factory creation, stereo format/callback, 200 audio blocks, finite/audible output and disposal passed in both architectures. Apple Silicon auval passed on the installed final universal AU. Intel auval through Rosetta returned no discoverable components and failed to find FM DELAY; it did not validate the Intel AU. Its independent direct-load check passed. Physical Intel hardware, older macOS releases, and full sessions across major DAWs remain unverified. Existing user DAW sessions were not touched.

Installed final universal AU/VST3, retaining both earlier arm-only and preliminary universal bundles outside scan folders. Final bundle signatures verified. No DSP or preset settings changed in this platform pass; it retains 0.10.0 sound corrections. Restart the DAW to load the replacement.

Windows preparation: AU restricted to Apple in CMake; VST3/standalone targets remain for Windows, MSVC UTF-8 and static-runtime options added, optional isolated VST3 host target integrated, double-click build launcher/PowerShell builder and manual GitHub Actions workflow included. The builder compiles, runs eight suites plus optional actual VST3 lifecycle/audio/state checks, and packages outputs. No Windows build environment is connected to this Mac; no Windows executable, VST3 binary or Windows test result exists yet. Donnie confirmed access to a Windows PC; the Windows source/build kit must be run there. Nothing uploaded to GitHub, no cloud workflow dispatched. See WINDOWS-START-HERE.txt and COMPATIBILITY.md.

## October 29, 2026 — 0.10.1 Diffusion steps and screenshot Favorite

Diffusion UI now snaps to Off, then integers 1–100 (internal audio range remains 0–.7). Added editable value box, integer formatting and text-to-value mapping. Existing saved/preset fractional amounts remain stored; moving the control selects whole-number steps. Underlying DSP is unchanged. Tests exercise every step 1–100, correct parameter mapping and typed values, plus Off, in both Mac architectures.

Donnie supplied Screenshot 2026-10-29 at 11.24.36 AM and approved matching hidden knob numbers approximately. FM's Fav (index 29 retained) now uses half-note Sync, Feedback .68, Mix .70, Analog .50, Ducking 0, HP 370 Hz, LP 9500 Hz, Width 1, Diffusion .7 (display 100), Dark, Ping on, Throw off, Threshold -39.385 dBFS. Stored free Time 1143 ms is unused while synced. Half note at 105 BPM is 1142.9 ms and follows the host thereafter. This explicitly supersedes the earlier literal-max recipe. Hidden knob values are estimates, not exact recovered settings. Existing session parameters remain until Reload.

Updated all-35 preset audio checks passed at three sample rates/tempos: finite output, test-signal headroom and decay; peak .293927 and worst late/early energy .0133284. Control/state/preset/tempo/Throw/resize tests passed arm64 and x86_64, including all Diffusion integers. Final VST3 passed 30 native window cycles with audio and state roundtrip (1,470 blocks). Universal bundles retain both slices with compiled minimum macOS 11; strict signatures checked. Installed 0.10.1 AU/VST3 and kept prior universal 0.10.0 outside scanned folders. Restart Fender, select FM's Fav Delay and Reload to load the changed recipe.
Apple AU validator specifically confirmed Component Version 0.10.1 (0xA01) and AU VALIDATION SUCCEEDED after registry refresh. This is Apple Silicon validation; the previously documented Intel auval discovery limitation remains.

## October 29, 2026 — 0.10.2 all factory timings in notes

At Donnie's request every one of the 35 factory programs now loads Sync. Converted the seven free-time recipes to nearby note values at reference120 BPM: Velvet Slap/Backbeat Slap/Hype Slap dotted32nds; Pocket Double sixteenth-triplet; Nashville Slap sixteenth; Felt Room/Radio Delay eighth-triplet. They follow host tempo, so the old fixed millisecond sound is intentionally tempo dependent. Program indices, non-time parameters and FM's screenshot Favorite preserved. Explicit saved MS sessions remain unchanged until choosing/Reloading a factory preset; manual MS remains available.

All35 preset audio cases passed at three rates/tempos, finite output, test-signal headroom and decay: peak .293927, worst late/early ratio .0133284. Both architecture integration checks assert Sync on for every factory program and verify full parameter/state recalls, timing, control wiring and Diffusion1–100. Actual final VST3 passed30 native window/audio cycles (2,580 blocks) and state roundtrip. Universal slices/minimum macOS11/signatures verified. Installed0.10.2 AU/VST3 with0.10.1 backup outside scan folders. Restart Fender and Reload to apply new factory values in an existing instance. No experimental voicing/filter change included in this timing release.

The H-Delay screenshot supplied this turn shows1/16 Host at105 BPM, modulation depth0, AnalogOff. This corrects the earlier assumption that the target requires tape color/wow. Exact hidden Mix/Feedback/filter numbers are not read from the image. Separate offline Clean filter prototype uses original topology-preserving two-pole Butterworth filters, no saturation/modulation, approximatelyHP350/LP2500/Feedback.10. The vendor manual does not establish its internal filter slope; no claim of reproducing H's exact design is made. Seven prior DSP suites and rapid filter-endpoint sweeps at three rates passed for the experimental implementation. Its level-matched vocal audition and reproducible patch/helper/settings live in outputs/FM-Delay-Vocal-Clean-Reference. This remains a listening audition, not installed or approved voicing. H-Delay remains the user's preferred reference until he says otherwise.
Apple AU validation specifically reported Component Version0.10.2 (0xA02) and AU VALIDATION SUCCEEDED; installed VST3 strict deep signature verification passed.

## October 29, 2026 — 0.11.0 clean vocal direction and fuller alternative

Donnie said the clean-filter real-vocal audition sounds closer to his H-Delay reference. Asked what remains, he replied "all of the above" to installing it, fuller/smoother repeats, and repeat-strength/decay adjustment. Integrated the clean direction and provided a reference-style recipe plus a fuller/stronger alternative. This is approval to progress that direction, not a claim he has declared the plugin finished or superior to H-Delay.

Clean now uses original topology-preserving two-pole Butterworth HP/LP, with5ms coefficient smoothing and smooth blends to bypass at open endpoints. Open-filter Clean remains sample-transparent. No saturation/modulation was introduced in Clean; Tape/Analog/Dark retain their existing character paths. Existing saved Clean parameters remain, but engaged-filter slopes and therefore sound intentionally change;0.10.2 is retained as backup. No parameter IDs/ranges changed.

Appended two Lead Vocal programs while preserving indices0–34: Studio Sixteenth(35), Clean/1/16 Sync/Mix17.5%/Feedback10%/HP350/LP2500; Full Vocal Echo(36), Clean/1/16 Sync/Mix20.5%/Feedback18%/HP160/LP2200. Both centered, with no additional modulation, ducking or diffusion. All37 factory programs use notes. The browser now groups appended entries under the existing category instead of duplicating Lead Vocal submenus. FM's Fav remains the separate half-note screenshot recipe.

Actual engine vocal renders: level-matched Studio Sixteenth differs from the endorsed Clean audition by relative squared error1.079e-7. Full Vocal Echo has about2.18dB greater wet RMS before level matching on this vocal, with gentler upper bandwidth and a longer feedback trail. These measurements establish controlled changes, not universal sound superiority. No clipped samples in either matched vocal output. outputs/FM-Delay-0.11.0-Vocal-Auditions contains the actual new recipes and reference at matched average levels.

Nine native audio suites passed in77.91s, including all37 programs at three rates/tempos, exact Clean-open behavior, filter cutoff/rolloff, automated endpoint stability, stereo coherence, characters/ducking/Throw/feedback/diffusion. Eight quick DSP/filter suites also passed x86_64 through Rosetta. Both architecture control harnesses passed every factory/state recall, Sync and Diffusion1–100, the new program settings and one-group browser IDs. Actual VST3 passed30 window/audio/state cycles per new program in each architecture (120 total cycles). A fully instrumented current-source sanitizer fixture passed250 editor cycles. Direct AU factory/audio/disposal passed both architectures; Apple Silicon auval specifically confirmed0.11.0 and AU VALIDATION SUCCEEDED. Intel auval's registry limitation and physical Intel/Windows/full-DAW certification limits remain.

Universal AU/VST3/standalone slices verified arm64/x86_64 and actual minimum macOS11.0, strict signatures checked. Installed0.11.0 AU/VST3, preserving0.10.2 outside scan folders. Isolated96kHz worst mean deadline use5.54%, not a DAW-session CPU guarantee. User Fender session was not closed or edited. Save/restart Fender and select Lead Vocal / Studio Sixteenth or Full Vocal Echo to hear the new direction.
