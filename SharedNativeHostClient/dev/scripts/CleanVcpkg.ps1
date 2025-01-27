. $PSScriptRoot/Paths.ps1

$vcpkgDir = Get-VcpkgDir

if (-not (Test-Path -PathType Container $vcpkgDir) -and (Test-Path -PathType Container (Join-Path $vcpkgDir ".git"))) {
    Write-Host "Vcpkg has not been cloned into $vcpkgDir. Please run ./dev/scripts/RepoChecks.ps1"
    return
}

Write-Host -ForegroundColor Cyan 'Deleting downloaded, intermediate, and built vcpkg files'
$vcpkgStatusLines = & git -C $vcpkgDir status --ignored --porcelain
$ignoreRegex = '!!\s+(?<path>[^/]+(?:/[^/]+)*)(?<dirsuffix>/?)'

foreach ($line in $vcpkgStatusLines) {
    if (-not ($line -match $ignoreRegex)) {
        continue;
    }

    $path = Join-Path -Path $vcpkgDir -ChildPath $matches['path']
    $dirsuffix = $matches['dirsuffix']
    if ($dirsuffix) {
        Write-Host "Deleting  dir: $path"
        Remove-Item -Path $path -Recurse -Force
    } else {
        Write-Host "Deleting file: $path"
        Remove-Item -Path $path -Force
    }
}

if ($IsMacOS -or $IsLinux) {
    $vcpkgArchivePath = Join-Path $env:HOME '.cache' 'vcpkg' 'archives'
}

if (($null -ne $vcpkgArchivePath) -and (Test-Path $vcpkgArchivePath)) {
    Write-Host -ForegroundColor Cyan 'Deleting vcpkg archive files'
    Write-Host "Deleting dir: $vcpkgArchivePath"
    Remove-Item -Path $vcpkgArchivePath -Recurse -Force
}
