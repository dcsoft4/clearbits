# Script to sign installer files using signtool
#
# Usage: powershell.exe -NoProfile -ExecutionPolicy Bypass -File sign_installer.ps1 <filepath> <filename>
#
# PowerShell invocation parameters explained:
#   -NoProfile          Skips loading user's profile scripts ($PROFILE) for faster,
#                       predictable startup without user customizations or side effects
#   -ExecutionPolicy Bypass   Overrides system script execution policy for this session
#                             only, allowing the script to run regardless of system
#                             restrictions (Restricted, AllSigned, RemoteSigned, etc.)
#
# Parameters:
#   FilePath  - Full path to the file to sign
#   FileName  - Filename only (for signtool which requires file in same folder)
param(
    [Parameter(Mandatory=$true)][string]$FilePath,
    [Parameter(Mandatory=$true)][string]$FileName
)

$ScriptDir = $PSScriptRoot

# signtool seems limited - it only accepts a filename (without path) and assumes
# it is in the same folder as the exe. Copy file to ThirdParty folder first.
Copy-Item $FilePath . -Force

# Try primary timestamp server
.\signtool.exe sign /a /n "DC" /fd sha256 /tr http://timestamp.sectigo.com/rfc3161 /td sha256 /v $FileName
$SignExit = $LASTEXITCODE

if ($SignExit -ne 0) {
    Write-Host "First attempt failed, trying backup timestamp server..."
    .\signtool.exe sign /a /n "DC" /fd sha256 /tr http://timestamp.digicert.com /td sha256 /v $FileName
    $SignExit = $LASTEXITCODE
}

if ($SignExit -eq 0) {
    Copy-Item ".\$FileName" $FilePath -Force
    Remove-Item ".\$FileName" -Force
} else {
    Write-Host "Failed to sign $FilePath"
    Remove-Item ".\$FileName" -Force -ErrorAction SilentlyContinue
    exit 1
}
