function Get-RootDir {
    return Resolve-Path "$PSScriptRoot/../.."
}

function Get-BuildDir(
    [ConfigObject]$ConfigObject
) {
    $buildSubDir = "build/$ConfigObject"
    return Join-Path (Get-RootDir) $buildSubDir
}

function Get-DevDir
{
    return Join-Path (Get-RootDir) 'dev'
}

function Get-MakeDir
{
    return Join-Path (Get-DevDir) 'make'
}

function Get-ScriptsDir
{
    return Join-Path (Get-DevDir) 'scripts'
}

function Get-SrcDir
{
    return Join-Path (Get-DevDir) 'src'
}

function Get-ExternalDir
{
    return Join-Path (Get-RootDir) 'external'
}

function Get-NugetDir
{
    return Join-Path (Get-ExternalDir) 'nuget'
}

function Get-VcpkgDir
{
    return Join-Path (Get-ExternalDir) 'vcpkg'
}
