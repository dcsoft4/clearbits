# ClearBits Music Player

![ClearBits Screenshot](https://dcsoft.com/Content/Images/Products/ClearBits/ClearBitsSS.png)

## Welcome
The ClearBits Music Player (ClearBits.exe) is a small C++/MFC app that demonstrates the ClearBits audio enhancement by playing a list of WAVE / MP3 files while allowing the Audio Buffer Size algorithm to be changed on-the-fly.

ClearBits is a technique of using randomly sized audio buffers to improve the perceptual sound quality of audio playback.

Please see the [home page](https://dcsoft.com/products/clearbits) for full information

&nbsp;

## Objectivist testing with an ABX app

There has always been an Objectivist vs Subjectivist debate on audio quality assessment. I have always been a Subjectivist, trusting my own hearing as opposed to measurements.  Oftentimes, administering the ABX test results in fatigue, stress, focus, and mood changes so that alone in my mind invalidates the test.

But, ClearBits lends itself extraordinarily well to ABX testing because it is simple to engage at will, and there are few (if any) external distractions.

Therefore, it makes sense to create an ABX app for ClearBits.


### Sample UI

Here is a potential UI for the ABX app.

![ABX UI](./doc/abx_ui.png)

An ABX test presents the user with three audio samples: a known A, a known B, and an unknown X that is secretly either A or B. The listener's job is not to judge which they prefer, but to correctly identify whether X matches A or B. This is repeated across multiple trials (typically 10) to build statistical confidence.

Key UI Regions
Playback controls — Three clearly labeled buttons: Play A, Play B, and Play X. The user can switch between them freely as many times as needed before deciding.
?

Waveform / progress bar — Shows the current playback position, keeping both samples time-synced so the listener can compare the same musical passage.
?

Trial tracker — Dot indicators showing current trial number (e.g., "Trial 3 of 10") so the user knows their progress through the test session.
?

Decision buttons — Two prominent buttons labeled X = A and X = B to submit the answer for the current trial.
?

Running statistics — A live summary panel showing correct answers, incorrect answers, remaining trials, and a confidence percentage (derived from binomial probability).
