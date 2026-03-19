# ClearBits Music Player

![ClearBits Screenshot](Doc/ClearBitsSS.png)

## Welcome
ClearBits is a technique that varies audio buffer sizes during playback in order to explore whether such changes may affect the perceived sound quality of audio.

The ClearBits Music Player (ClearBits.exe) is a Windows-based audio player app that allows enabling and disabling of the ClearBits technique while playing the user's choice of WAVE / MP3 audio files and to easily seek within them.  It is intended to allow users to evaluate the ClearBits effect for for themselves using familiar and repeatable audio sources.

Please see the [home page](https://dcsoft.com/products/clearbits) for full information.

&nbsp;

## Next Step:  Add ABX functionality into the ClearBits Music Player

Listeners who perceive a difference in casual listening may not always demonstrate statistically significant results in ABX testing.  ClearBits is particularly well suited to ABX-style listening because the effect can be engaged or disengaged during continuous playback of the same audio stream.

This avoids the need to synchronize multiple audio streams and ensures time and level alignment when switching between conditions, reducing the listener's reliance on short-term auditory memory.  At the same time, it allows the listener to control pacing, repeat passages, and explore both short and longer listening intervals.

To ensure that the transition itself does not reveal which mode is active, switching conditions briefly mute playback, reposition to a recent (or optionally user-selected fixed) point within the same passage, and resume playback, allowing repeated comparison of identical musical context under different conditions.

Because the new buffer behavior takes effect almost immediately, the listener can evaluate differences without requiring long settling periods.

Overall, ClearBits provides a simple and practical way to explore whether differences are perceptible under controlled yet natural listening conditions.  For this reason, adding ABX functionality allows listeners to evaluate whether perceived differences can be detected reliably under controlled comparison.


### Sample UI

Here is a potential UI for the ABX extension.

![ABX UI](Doc/abx_ui.png)

An ABX test presents the user with three audio samples: a known A, a known B, and an unknown X that is secretly either A or B. The listener's job is not to judge which they prefer, but to correctly identify whether X matches A or B. This is repeated across multiple trials (typically 10) to build statistical confidence.

*Key UI Regions*

* Playback controls — Three clearly labeled buttons: Play A, Play B, and Play X. The user can switch between them freely as many times as needed before deciding.

* Waveform / progress bar — Shows the current playback position, keeping both samples time-synced so the listener can compare the same musical passage.

* Trial tracker — Dot indicators showing current trial number (e.g., "Trial 3 of 10") so the user knows their progress through the test session.

* Decision buttons — Two prominent buttons labeled X = A and X = B to submit the answer for the current trial.

* Running statistics — A live summary panel showing correct answers, incorrect answers, remaining trials, and a confidence percentage (derived from binomial probability).
