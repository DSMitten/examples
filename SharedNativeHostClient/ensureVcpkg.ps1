# This script clones the vcpkg repository down to the local machine if it hasn't already

$rootDir = [System.IO.Path]::GetDirectoryName($MyInvocation.MyCommand.Path)
$externalPath = Join-Path $rootDir "external"
$vcpkgRepoPath = Join-Path $externalPath "vcpkg"

if ((Test-Path -PathType Container $vcpkgRepoPath) -and (Test-Path -PathType Container (Join-Path $vcpkgRepoPath ".git"))) {
    Write-Host "Vcpkg has been cloned into $vcpkgRepoPath"
    return
}

Write-Host "Cloning vcpkg into $vcpkgRepoPath"
& git -C $externalPath clone https://github.com/microsoft/vcpkg 2>&1
