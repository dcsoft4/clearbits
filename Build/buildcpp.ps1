# Build script for Visual Studio 2022
#
# Usage: powershell.exe -NoProfile -ExecutionPolicy Bypass -File buildcpp.ps1 [Release | Debug]
#
# PowerShell invocation parameters explained:
#   -NoProfile          Skips loading user's profile scripts ($PROFILE) for faster,
#                       predictable startup without user customizations or side effects
#   -ExecutionPolicy Bypass   Overrides system script execution policy for this session
#                             only, allowing the script to run regardless of system
#                             restrictions (Restricted, AllSigned, RemoteSigned, etc.)
#
# Parameter: Build configuration (Release or Debug), defaults to Release
param([string]$Config = "Release")

# Save the script's directory for relative paths
$ScriptDir = $PSScriptRoot

# Set up Visual Studio 2022 developer environment (only if not already initialized)
#   -Arch amd64             Target 64-bit architecture (valid: x86, amd64, arm, arm64)
#   -SkipAutomaticLocation  Prevents changing current directory to VS install folder;
#                           we need to stay in the project directory to find the .sln
if (-not $env:VSCMD_VER) {
    & "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\Launch-VsDevShell.ps1" -Arch amd64 -SkipAutomaticLocation

    if ($LASTEXITCODE -ne 0) {
        Write-Error "Failed to setup Visual Studio environment"
        exit 1
    }
}

if ($LASTEXITCODE -ne 0) {
    Write-Error "Failed to setup Visual Studio environment"
    exit 1
}

# Build Target
msbuild ClearBits.sln /p:Configuration=$Config
if ($LASTEXITCODE -ne 0) {
    Write-Error "BUILD FAILED"
    exit 1
}
