# This script performs essential tasks for setting up the local repo to build
# It assumes ./dev/scripts/mcheck.ps1 from webview2-host-framework repo has already completed successfully

[CmdletBinding()]
param()

. $PSScriptRoot/Paths.ps1

Write-Host -ForegroundColor Blue 'Performing checks for setting up the local repo to build.'
Write-Host -ForegroundColor Yellow 'These do not duplicate the checks performed by ./dev/scripts/mcheck.ps1 in webview2-host-framework. Be sure to run those checks first.'

# -----------------------------------------------------------------------------
# CPU Architecture checks
# -----------------------------------------------------------------------------
function IsX64 {
    return [System.Runtime.InteropServices.RuntimeInformation]::OSArchitecture -eq [System.Runtime.InteropServices.Architecture]::X64
}

function IsArm64 {
    return [System.Runtime.InteropServices.RuntimeInformation]::OSArchitecture -eq [System.Runtime.InteropServices.Architecture]::Arm64
}

# -----------------------------------------------------------------------------
# Vcpkg check
# -----------------------------------------------------------------------------
function EnsureVcpkg {
    $vcpkgDir = Get-VcpkgDir
    $externalDir = Get-ExternalDir
    if ((Test-Path -PathType Container $vcpkgDir) -and (Test-Path -PathType Container (Join-Path $vcpkgDir '.git'))) {
        Write-Host -ForegroundColor Green "Vcpkg has been cloned into $vcpkgDir"
    } else {
        Write-Host "Cloning vcpkg into $vcpkgDir"
        & git -C $externalDir clone https://github.com/microsoft/vcpkg 2>&1    
    }
}

# -----------------------------------------------------------------------------
# Nuget dir check
# -----------------------------------------------------------------------------
function EnsureNugetDir {
    $nugetDir = Get-NugetDir
    if (Test-Path -PathType Container $nugetDir) {
        Write-Verbose "$nugetDir exists"
    } else {
        Write-Host "Creating directory $nugetDir"
        New-Item -Path $nugetDir -ItemType Directory -ErrorAction Stop | Out-Null
    }
}

# -----------------------------------------------------------------------------
# Azure command-line tool (MacOS only)
# -----------------------------------------------------------------------------
function EnsureAzureCli {
    $azcmd = Get-Command az -ErrorAction Ignore
    if ($null -ne $azcmd) {
        Write-Host -ForegroundColor Green 'azure-cli formula is installed'
    } else {
        Write-Host 'Installing azure-cli formula'
        brew install azure-cli
        if ($LastExitCode -ne 0) {
            throw 'Failed to install azure-cli formula'
        }
    }

    $extensions = az extension list --query [*].name | ConvertFrom-Json
    if ($extensions -contains 'azure-devops') {
        Write-Host -ForegroundColor Green 'azure-cli extension "azure-devops" is installed'
    } else {
        Write-Host 'Installing azure-cli extension "azure-devops"'
        az extension add --name azure-devops
        if ($LastExitCode -ne 0) {
            throw 'Failed to install azure-devops extension'
        }
    }
}

# -----------------------------------------------------------------------------
# WebView2 SDK check
# -----------------------------------------------------------------------------
function EnsureWebView2SdkMac {
    $nugetDir = Get-NugetDir
    $webView2SdkDir = Join-Path $nugetDir 'MSWebView2'
    $webView2AppDir = Join-Path $webView2SdkDir 'Microsoft Edge WebView2.app'
    $webView2FrameworkDir = Join-Path $webView2SdkDir 'MSWebView2.framework'

    if ((Test-Path -PathType Container $webView2AppDir) -and (Test-Path -PathType Container $webView2FrameworkDir)) {
        Write-Host -ForegroundColor Green "WebView2 SDK has been downloaded to $webView2SdkDir"
    } else {
        # Create sdk directories if they don't exist
        EnsureNugetDir

        if (-not (Test-Path -PathType Container $webView2SdkDir)) {
            Write-Host "Creating directory $webView2SdkDir"
            New-Item -Path $webView2SdkDir -ItemType Directory -ErrorAction Stop | Out-Null
        }

        # Delete any directories in the destination paths
        # If one of these already exists, we could end up with mismatched sdk directories, so delete them and populate fresh ones
        if (Test-Path -PathType Container $webView2AppDir) {
            Remove-Item -Path $webView2AppDir -Recurse -ErrorAction Stop
        }

        if (Test-Path -PathType Container $webView2FrameworkDir) {
            Remove-Item -Path $webView2FrameworkDir -Recurse -ErrorAction Stop
        }

        # Check to see if az is logged in
        az account show --output none 2>$null
        if (0 -ne $LastExitCode) {
            Write-Host 'You must log in with the az tool to download the MSWebView2 SDK'
            Write-Host 'Log in interactively and, when prompted, choose the default Azure subscription'
            az login
            if (0 -ne $LastExitCode) {
                throw "Failed to log into azure cli tool. Try running 'az login' by hand."
            }
        }

        # Download the sdk artifact
        Write-Host "Installing MSWebView2 with Azure CLI"
        $webview2Version = "0.2957.124"
        $artifactName = "MSWebView2.$webView2Version"
        $tempPath = "/tmp/$artifactName"
        if (-not (Test-Path -PathType Container $tempPath)) {
            az artifacts universal download --organization "https://dev.azure.com/microsoft/" --project "90b2a23c-cab8-4e7c-90e7-a977f32c1f5d" --scope project --feed "webview2-mac-stable" --name "webview2_sdk" --version $webview2Version --path $tempPath
            if (0 -ne $LastExitCode) {
                throw "Failed to download MSWebView2 azure artifact"
            }
        }

        # Expand the downloaded zip files into the destination directories
        if (IsArm64) {
            $arch = "arm64"
        } elseif (IsX64) {
            $arch = "x64"
        }

        $appZipPath = Join-Path $tempPath $arch 'MicrosoftEdgeWebView2App.zip'
        $frameworkZipPath = Join-Path $tempPath $arch 'MicrosoftWebView2Framework.zip'

        # Note: We call unzip command instead of Expand-Archive because Expand-Archive won't convert text files to symlinks
        Write-Host "Expanding $appZipPath to $webView2AppDir"
        unzip -q $appZipPath -d $webView2SdkDir
        if (0 -ne $LastExitCode) {
            throw "Failed to expand $appZipPath to $webView2AppDir"
        }

        Write-Host "Expanding $frameworkZipPath to $webView2FrameworkDir"
        unzip -q $frameworkZipPath -d $webView2SdkDir
        if (0 -ne $LastExitCode) {
            throw "Failed to expand $frameworkZipPath to $webView2FrameworkDir"
        }
    }
}

# -----------------------------------------------------------------------------
# Check execution
# -----------------------------------------------------------------------------
EnsureVcpkg

if ($IsMacOS) {
    # Note: On Windows, the cmake script will use nuget to get the webview2 package
    # On MacOS, we have to do this using Azure CLI - it isn't part of the build
    EnsureAzureCli
    EnsureWebView2SdkMac
}
