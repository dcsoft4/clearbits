# ClearBits Qt — Resume Notes

## Project overview

Two related projects:

| Path | Purpose |
|---|---|
| `D:\Src\DCSoft\ClearBitsDesign\` | Qt Design Studio (QDS 4.8 / Qt 6.8) project — UI design only |
| `D:\Src\DCSoft\clearbits\` | Original MFC player + `ClearBitsQt\` runtime Qt/C++ project |

The QDS project is used to visually design the UI. The runtime project `ClearBitsQt` is where the C++ backend will live and where the final app is built.

## What has been done

### ClearBitsDesign (QDS project)
- `Screen01.ui.qml` — fully laid out with all MFC controls ported to QML:
  - Header row: title text + format text
  - Controls row: algorithm ComboBox + Prev / Play / Next buttons
  - Seek row: `<<` `<` progress text `>` `>>`
  - Playlist: `ListView` with `ListModel`, fills remaining height, border
  - Footer row: Load / Clear / Delete / Move link texts + Shuffle `CheckBox`
- Theme: Universal / System (follows OS dark/light mode) via `qtquickcontrols2.conf`
- All `Text` items use `palette.windowText` / `palette.link` for theme-correct colors
- Footer link `Text` items have `MouseArea { cursorShape: Qt.PointingHandCursor }` children
- `App.qml` has `minimumWidth: 640 / minimumHeight: 240`

### ClearBitsQt (runtime project)
- `Screen01.qml` — copy of the QDS UI (identical content, no QDS-only restrictions)
- `Main.qml` — Window hosting `Screen01 { anchors.fill: parent }`
- `ClearBitsDesign/` module folder copied in (Constants singleton, etc.)
- `qtquickcontrols2.conf` copied in (Universal / System theme)
- `CMakeLists.txt` updated:
  - Added `Qt6::QuickControls2`
  - Added `Screen01.qml` to `QML_FILES`
  - Added `ClearBitsDesign` as a second `qt_add_qml_module`
  - `qtquickcontrols2.conf` added as a resource

## What needs to be done next

### Build verification
- Rebuild `ClearBitsQt` in Visual Studio (the CMakeLists.txt was changed — CMake reconfigure required)
- Run `build/Release/ClearBitsQt.exe` and verify UI appears correctly

### Backend wiring (the main work remaining)
The MFC `CClearBitsDlg` (in `MFC/ClearBitsDlg.h`) is the reference. Features to port:

| Feature | MFC class/method | QML control to wire |
|---|---|---|
| Playlist model | `m_lbWaveFiles` / `AddToPlayList()` | `playlistModel` (ListModel in Screen01) |
| Play/Pause | `OnButtonPlayPause()` / `Play()` / `Pause()` | `playPauseButton` |
| Stop | `Stop()` | (add Stop button or keyboard shortcut) |
| Prev / Next | `PrevFile()` / `NextFile()` | `prevButton` / `nextButton` |
| Seek | `OnSeekBack/Forward Fast/Slow()` | seek buttons |
| Progress display | `IDC_STATIC_PROGRESS` | `progressText` |
| Algorithm selector | `m_nAlgo` / `SetAlgo()` / `GetNextBufSize_*()` | `algoCombo` |
| Shuffle toggle | `m_bRandomPlay` | `shuffleCheck` |
| Load files | `OnButtonLoad()` | `loadLink` MouseArea `onClicked` |
| Clear playlist | `OnButtonClear()` | `clearLink` MouseArea `onClicked` |
| Delete from disk | `DeleteFileFromDisk()` | `deleteLink` (currently hidden) |
| Move file | `OnButtonMove()` | `moveLink` (currently hidden) |
| Audio engine | `McWaveReader` + `waveOut` API | Replace with `Qt Multimedia` |

### Architecture pattern to use
- Create C++ `QObject`-derived classes (e.g. `AudioEngine`, `PlaylistModel`)
- Expose to QML via `qmlRegisterType()` or `QML_ELEMENT` macro
- Add `.cpp`/`.h` files to `qt_add_executable()` in `CMakeLists.txt`
- Use `Q_PROPERTY` + signals for reactive bindings to QML controls
- Wire `onClicked` handlers in a companion `Screen01Behavior.qml` or directly in `Main.qml`

### Key MFC audio files to reference
- `MFC/ClearBitsDlg.cpp` — main dialog logic
- `MP3/MCWaveReader.h/.cpp` — WAV/MP3 reader (replace with Qt Multimedia)
- `SAMPBUF.h` — sample buffer (`CSampleBuffer`)
- `Options.h/.cpp` — settings persistence
- Buffer algorithms: `GetNextBufSize_Fixed/VC/CAPI/RandomOrg()` in `ClearBitsDlg.cpp`

## Key file paths

```
clearbits\
  MFC\ClearBitsDlg.h          — MFC dialog header (feature reference)
  MFC\ClearBitsDlg.cpp        — MFC dialog implementation
  MP3\MCWaveReader.h/.cpp     — Audio reader
  SAMPBUF.h                   — Sample buffer
  ClearBitsQt\
    CMakeLists.txt            — Build config (recently updated)
    Main.qml                  — Window root
    Screen01.qml              — Full UI (keep in sync with QDS Screen01.ui.qml)
    ClearBitsDesign\          — QDS module (Constants singleton etc.)
    qtquickcontrols2.conf     — Universal/System theme
    main.cpp                  — Entry point (QGuiApplication + QQmlEngine)
```
