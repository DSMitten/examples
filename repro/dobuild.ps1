$sourcePath = Split-Path $PSCommandPath -Parent
$buildPath = Join-Path $sourcePath build
$generateLogPath = Join-Path $buildPath 'generate.log'
$buildLogPath = Join-Path $buildPath 'build.log'

if (Test-Path $buildPath) {
    Remove-Item -Recurse -Force -Path $buildPath
}

if (-not (Test-Path $buildPath)) {
    New-Item -ItemType Directory -Path $buildPath | Out-Null
}

cmake --preset repro -S $sourcePath -DCMAKE_CXX_COMPILER:STRING='C:/Program Files/LLVM/bin/clang++.exe' | Out-File $generateLogPath
cmake --build --preset repro --verbose | Out-File $buildLogPath