# cmake-build.ps1 — CMake configure+build helper for ClearBitsQt
#
# Called by ClearBitsQt.vcxproj (NMakeBuildCommandLine / NMakeCleanCommandLine /
# NMakeReBuildCommandLine) when building ClearBitsQt from ClearBits.sln in Visual Studio
# or via Build\buildcpp.ps1.  Not intended to be run directly.
#
# How it works:
#   For "build" (the default): locates the Qt installation by scanning PATH for an entry
#   under C:\Qt that contains "msvc2022_64" (e.g. C:\Qt\6.11.0\msvc2022_64\bin, which must
#   be added manually to PATH — this same entry also allows the debug Qt DLLs to be found
#   when running the debug executable).  It strips the trailing \bin to get the Qt prefix,
#   then runs
#   "cmake -G NMake Makefiles" to configure the build tree (creating it if needed), and
#   finally "cmake --build" to compile.  NMake Makefiles is used instead of Ninja to avoid
#   a CMake 3.30+/Ninja incompatibility with $Config in rule names.
#
#   For "clean" and "rebuild": skips Qt detection and just calls "cmake --build" with
#   --target clean or --clean-first, since the build tree is already configured.
#
# Usage: cmake-build.ps1 -Config <Debug|Release> -BuildDir <path> [-Target <build|clean|rebuild>]

param(
    [string]$Config = "Release",
    [string]$BuildDir,
    [string]$Target = "build"   # build | clean | rebuild
)

$CMake = "C:\Qt\Tools\CMake_64\bin\cmake.exe"

# Clean and rebuild don't need Qt detection — the build dir already has a configured cache.
if ($Target -eq "clean") {
    & $CMake --build $BuildDir --target clean
    exit $LASTEXITCODE
}

if ($Target -eq "rebuild") {
    & $CMake --build $BuildDir --clean-first
    exit $LASTEXITCODE
}

# Locate Qt by finding the first PATH entry under C:\Qt that contains msvc2022_64.
# Qt installer adds e.g. C:\Qt\6.11.0\msvc2022_64\bin to PATH.
$QtBinDir = ($env:PATH -split ';') |
    Where-Object { $_ -like 'C:\Qt*' -and $_ -match 'msvc2022_64' } |
    Select-Object -First 1

if (-not $QtBinDir) {
    Write-Error "Qt not found in PATH. Add C:\Qt\<version>\msvc2022_64\bin to your PATH."
    exit 1
}

$QtPrefix = Split-Path $QtBinDir -Parent
if (-not (Test-Path $QtPrefix)) {
    Write-Error "Qt prefix '$QtPrefix' (from PATH entry '$QtBinDir') does not exist. Fix your PATH."
    exit 1
}
Write-Host "Using Qt at $QtPrefix"

# Use NMake Makefiles — available in any VS developer environment and avoids a
# CMake 3.30+/Ninja incompatibility where $Config appears in rule names.
& $CMake -S "$PSScriptRoot" -B $BuildDir -G "NMake Makefiles" `
    -DCMAKE_BUILD_TYPE=$Config `
    -DCMAKE_PREFIX_PATH=$QtPrefix
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

& $CMake --build $BuildDir
exit $LASTEXITCODE
