# This script wraps cmake commands to make them easier to run
[CmdletBinding()]
param(
    [string] $Config,
    [switch] $Clean,
    [switch] $Generate,
    [switch] $Build
)

if ((-not $Clean) -and (-not $Generate) -and (-not $Build)) {
    throw "No command specified. Usage: run [-Clean] [-Generate] [-Build]"
}

$sourcePath = (Split-Path $PSCommandPath -Parent)
$buildPath = (Join-Path -Path $sourcePath -ChildPath 'build')

try{
    Push-Location $sourcePath

    if (-not [System.String]::IsNullOrEmpty($Config)) {
        $preset = $Config
    } else {
        if ($IsWindows) {
            # Build tools are x64 even when cross-compiling for x64 or arm64
            $preset = 'win-x64-debug'
        } elseif ($IsMacOS) {
            if ([System.Runtime.InteropServices.RuntimeInformation]::OSArchitecture -eq [System.Runtime.InteropServices.Architecture]::X64) {
                $preset = 'mac-x64-debug'
            } elseif ([System.Runtime.InteropServices.RuntimeInformation]::OSArchitecture -eq [System.Runtime.InteropServices.Architecture]::Arm64) {
                $preset = 'mac-arm64-debug'
            }
        } elseif ($IsLinux) {
            $preset = 'lnx-x64-debug'
        }    
    }

    if ([System.String]::IsNullOrEmpty($preset)) {
        throw "Unable to configure - no preset identified"
    }

    $buildPresetPath = Join-Path $buildPath $preset

    # -----------------------------------------------------------------------------
    # Clean
    # -----------------------------------------------------------------------------
    if ($Clean) {
        Write-Host "Deleting '$buildPath'"
        if ([System.IO.Directory]::Exists($buildPath)) {
            Remove-Item -Recurse -Force -Path $buildPath
        }
        npm run clean
    }

    # -----------------------------------------------------------------------------
    # Generate
    # -----------------------------------------------------------------------------
    if ($Generate) {
        Write-Host "Generating makefiles for the project"

        $logPath = Join-Path $buildPresetPath '_generate.log'
        if (-not [System.IO.Directory]::Exists($buildPresetPath)) {
            New-Item -ItemType Directory -Path $buildPresetPath | Out-Null
        }
        
        $sourceDir = (Split-Path $PSCommandPath -Parent)

        $generateArgs = '--preset', $preset, '-S', $sourceDir, '-Wno-dev', '--log-level=VERBOSE'

        Write-Host "cmake $([String]::Join(' ', $generateArgs)) 2>&1 | Tee-Object -FilePath $logPath"
        cmake @generateArgs 2>&1 | Tee-Object -FilePath $logPath
        if ($LastExitCode -ne 0) {
            Write-Host "Generate step failed - log file is at $logPath" -ForegroundColor Red
            return -1;
        }
    }

    # -----------------------------------------------------------------------------
    # Build
    # -----------------------------------------------------------------------------
    if ($Build) {
        Write-Host "Building the project"
        $buildLogPath = Join-Path $buildPresetPath '_build.log'
        Write-Host "cmake --build --preset $preset | Tee-Object -FilePath $buildLogPath"
        cmake --build --preset $preset 2>&1 | Tee-Object -FilePath $buildLogPath
        if ($LastExitCode -ne 0) {
            Write-Host "Build failed - log file is at $buildLogPath" -ForegroundColor Red
            return -1;
        }

        Write-Host "Installing the project"
        $installLogPath = Join-Path $buildPresetPath '_install.log'
        Write-Host "cmake --install $buildPresetPath | Tee-Object -FilePath $installLogPath"
        cmake --install $buildPresetPath 2>&1 | Tee-Object -FilePath $installLogPath
        if ($LastExitCode -ne 0) {
            Write-Host "Install failed - log file is at $installLogPath" -ForegroundColor Red
            return -1;
        }
    }
} finally {
    Pop-Location
}