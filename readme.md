# ClearBits Music Player

![ClearBits Screenshot](Doc/ClearBitsSS.png)

## Welcome
The ClearBits Music Player (ClearBits.exe) is a small C++/MFC app that demonstrates the ClearBits audio enhancement by playing a list of WAVE / MP3 files while allowing the Audio Buffer Size algorithm to be changed on-the-fly.

ClearBits is a technique of using randomly sized audio buffers to potentially improve the perceptual sound quality of audio playback.

Please see the [home page](https://dcsoft.com/products/clearbits) for full information

&nbsp;

## Next Step:  ABX Evaluation of ClearBits

ClearBits is particularly well suited to ABX-style listening because the effect can be engaged or disengaged during continuous playback of the same audio stream.  In many traditional ABX setups, listeners compare separately rendered audio samples and must rely on short-term auditory memory while switching between sources. This often requires careful synchronization and rapid switching to remain effective.

With ClearBits, the listener can switch between conditions (e.g., fixed vs randomized buffer sizes) while the same track is playing. Because both conditions operate on the same underlying audio stream, playback remains inherently level-matched and time-aligned, avoiding common confounding factors such as small volume differences or mismatched playback positions.

When switching, playback is briefly muted, repositioned to a fixed recent point within the same passage, and resumed. This allows repeated comparison of identical musical context under different conditions.

Because the new buffer behavior takes effect almost immediately, the listener can evaluate differences without requiring long settling periods.

To minimize non-audio cues, switching uses a consistent, brief mute and resume sequence so that the transition itself does not reveal which mode is active. The rewind interval and timing behavior are fixed, ensuring that comparisons remain consistent across trials.

This approach reduces reliance on auditory memory and avoids the need to align multiple playback streams. At the same time, it allows the listener to control pacing, repeat passages, and explore both short and longer listening intervals.

Overall, ClearBits provides a simple and practical way to explore whether differences are perceptible under controlled yet natural listening conditions.  Therefore, it makes sense to create an ABX app for ClearBits.


### Sample UI

Here is a potential UI for the ABX app.

![ABX UI](Doc/abx_ui.png)

An ABX test presents the user with three audio samples: a known A, a known B, and an unknown X that is secretly either A or B. The listener's job is not to judge which they prefer, but to correctly identify whether X matches A or B. This is repeated across multiple trials (typically 10) to build statistical confidence.

*Key UI Regions*

* Playback controls — Three clearly labeled buttons: Play A, Play B, and Play X. The user can switch between them freely as many times as needed before deciding.

* Waveform / progress bar — Shows the current playback position, keeping both samples time-synced so the listener can compare the same musical passage.

* Trial tracker — Dot indicators showing current trial number (e.g., "Trial 3 of 10") so the user knows their progress through the test session.

* Decision buttons — Two prominent buttons labeled X = A and X = B to submit the answer for the current trial.

* Running statistics — A live summary panel showing correct answers, incorrect answers, remaining trials, and a confidence percentage (derived from binomial probability).
