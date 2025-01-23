$rootDir = [System.IO.Path]::GetDirectoryName($MyInvocation.MyCommand.Path)
$externalPath = Join-Path $rootDir "external"
$vcpkgRepoPath = Join-Path $externalPath "vcpkg"


if (-not (Test-Path -PathType Container $vcpkgRepoPath) -and (Test-Path -PathType Container (Join-Path $vcpkgRepoPath ".git"))) {
    Write-Host "Vcpkg has not been cloned into $vcpkgRepoPath. Please run ./dev/scripts/ensureVcpkg.ps1"
    return
}
Write-Verbose 'Deleting downloaded, intermediate, and built vcpkg files'
