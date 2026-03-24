# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

ClearBits Music Player is a Windows desktop application that demonstrates the "ClearBits" audio enhancement technique — using randomly sized audio buffers to improve perceptual sound quality. Users can play WAVE/MP3 files and switch buffer-sizing algorithms on-the-fly for comparison.

The MFC version is the original.  It produces ClearBits.exe.

The Qt/QtQuick (QML) version of the modern replacement.  A ClearBitsQt.vcprojx called ClearBitsQt, which shares the same audio pipeline and buffer-sizing code but replaces the MFC UI with a Qt-based one.

`ClearBits.sln` contains both these .vcxproj files.

## Build

Build the solution (which has projects for both the MFC app and Qt app) with .\Build\buildcpp.ps1, fix errors as needed until success.

Output: 
* MFC:  `Release/ClearBits.exe` or `Debug/ClearBits.exe`
* Qt:  `ClearBitsQt\build\Release\ClearBitsQt.exe` or `ClearBitsQt\build\Debug\ClearBitsQt.exe`

There is no automated test suite or linter. The app itself is the demonstration/test vehicle.

## MFC Architecture

The app is a single MFC dialog (`CClearBitsDlg`) launched by `CClearBitsApp`.

**Audio pipeline:**
1. `McWaveReader` (`MP3/`) — reads WAVE, MP3, and RMP files; handles ACM-based MP3 decoding
2. `CSampleBuffer` (`SAMPBUF.h`) — wraps a single waveOut buffer (`WAVEHDR`)
3. `CClearBitsDlg` drives playback by calling `GetNextBufSize_*()` to pick the next buffer size, then filling and submitting buffers to the Windows waveOut API

**Buffer-sizing algorithms** (all in `ClearBitsDlg.cpp`):
- `GetNextBufSize_Fixed()` — constant size
- `GetNextBufSize_VC()` — `rand()` from CRT
- `GetNextBufSize_CAPI()` — Windows Crypto API
- `GetNextBufSize_RandomOrg()` — fetches entropy from random.org
- `GetNextBufSize_WMP()` — Windows Media Player style
- `GetNextBufSize_SmallBuf()` — small buffer algorithm

**Supporting modules:**
- `Options.cpp/.h` — registry-based settings (`HKEY_CURRENT_USER`)
- `MP3/id3_int28` — ID3 tag parsing
- `Dundas/` — third-party MFC helper controls (`OXStatic`, `OXInteger`, string ops)
- `Layout/` — dynamic window layout
 
## Key Dependencies

Windows system libraries (no external packages):
- `winmm.lib` — waveOut audio playback
- `Msacm32.lib` — Audio Compression Manager (MP3 decoding)
- `vfw32.lib` — Video for Windows
- Static MFC linkage (v145 toolset)


### Completed tasks

### Next tasks


## Proposed Future Work

The README describes a planned **ABX testing UI** — a separate app (or mode) that presents three audio samples (A, B, X) across multiple trials and computes binomial confidence statistics. See `readme.md` and `doc/abx_ui.png` for the UI mockup.
