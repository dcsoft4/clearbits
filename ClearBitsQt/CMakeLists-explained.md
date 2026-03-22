# CMakeLists.txt Explained

CMakeLists.txt replaces all the compiler/linker settings you'd normally see in a vcxproj.
Here's how each piece maps:

---

**`find_package(Qt6 REQUIRED COMPONENTS Quick)`**
> Locates the Qt6 installation and imports its CMake config files. Those config files define
> everything Qt needs — include dirs, lib paths, preprocessor defines, compile flags.
> You never write those manually.

---

**`qt_standard_project_setup(REQUIRES 6.8)`**
> Sets global defaults: C++17, Unicode, UTF-8 source encoding, AUTOMOC on (auto-runs Qt's
> `moc` code generator on headers). Equivalent to a dozen compiler settings you'd set by
> hand in a vcxproj.

---

**`qt_add_executable(ClearBitsQt main.cpp)`**
> Declares the target and its source files. CMake infers Debug/Release configs, output
> filename, object file locations, etc.

---

**`qt_add_qml_module(ClearBitsQt URI ClearBitsQt QML_FILES Main.qml)`**
> Registers the QML module. CMake generates the `rcc`, `qmlcachegen`, and
> type-registration steps automatically — these would be custom build steps in a vcxproj.

---

**`set_target_properties(... WIN32_EXECUTABLE TRUE)`**
> Sets the linker subsystem to `/SUBSYSTEM:WINDOWS` (no console window) — same as the
> `<SubSystem>Windows</SubSystem>` setting in a vcxproj linker section.

---

**`target_link_libraries(ClearBitsQt PRIVATE Qt6::Quick)`**
> Links Qt6Quick.lib plus all its transitive dependencies (Qt6Core, Qt6Gui, Qt6Qml, etc.).
> CMake resolves the full chain — in a vcxproj you'd list each lib manually under
> Additional Dependencies.

---

## How It All Works

CMake uses the Qt6 package config files (in `C:\Qt\6.10.2\msvc2022_64\lib\cmake\Qt6`) to
know everything about Qt's layout. You declare *what* you want at a high level; CMake
translates it into the actual compiler/linker invocations at configure time.

That's why the VS project properties appear blank — the real settings live in the
generated `build\Release\build.ninja` file.
