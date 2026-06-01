# Multi-Algo-Analysis — unified build & run helper (Windows / PowerShell)
# Usage: .\scripts\run.ps1 [command] [options]
#
# Commands:
#   build           Build the C++ binary (default)
#   tui             Build + launch TUI dashboard
#   cli [args...]   Build + run CLI binary
#   sweep <algo>    Complexity sweep for one algorithm
#   compare         Benchmark all algorithms
#   test            Build + run test suite
#   clean           Remove build\

#Requires -Version 5.1
$ErrorActionPreference = "Stop"

$Root  = Split-Path $PSScriptRoot -Parent
$Bin   = "$Root\build\resource_monitor_app.exe"
$Jobs  = $env:NUMBER_OF_PROCESSORS

function Write-Info  { param($m) Write-Host "[INFO]  $m" -ForegroundColor Cyan   }
function Write-OK    { param($m) Write-Host "[OK]    $m" -ForegroundColor Green  }
function Write-Err   { param($m) Write-Host "[ERROR] $m" -ForegroundColor Red; exit 1 }

function Show-Usage {
    Write-Host ""
    Write-Host "Usage: .\scripts\run.ps1 [build|tui|cli|sweep|compare|test|clean] [options]"
    Write-Host ""
    Write-Host "  build           Compile C++ binary"
    Write-Host "  tui             Launch interactive TUI dashboard"
    Write-Host "  cli [args]      Run CLI (e.g.: cli --algorithm merge_sort --runs 10)"
    Write-Host "  sweep <algo>    Complexity sweep for <algo>"
    Write-Host "  compare         Benchmark all algorithms side-by-side"
    Write-Host "  test            Run test suite"
    Write-Host "  clean           Delete build\"
    Write-Host ""
    Write-Host "Algorithms: binary_search linear_search interpolation_search"
    Write-Host "            merge_sort quick_sort heap_sort shell_sort"
    Write-Host "            insertion_sort selection_sort bubble_sort"
    Write-Host ""
}

function Invoke-Build {
    Write-Info "Configuring CMake..."
    cmake -B "$Root\build" -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -Wno-dev -S "$Root"
    Write-Info "Building with $Jobs jobs..."
    cmake --build "$Root\build" -j $Jobs
    Write-OK "Binary: $Bin"
}

function Assert-Built {
    if (-not (Test-Path $Bin)) { Invoke-Build }
}

$Cmd  = if ($args.Count -gt 0) { $args[0] } else { "build" }
$Rest = if ($args.Count -gt 1) { $args[1..($args.Count-1)] } else { @() }

switch ($Cmd) {
    "build" {
        Invoke-Build
    }

    "tui" {
        Assert-Built
        Push-Location "$Root\tui"
        if (-not (Test-Path "node_modules")) {
            Write-Info "Installing TUI deps..."
            npm install --silent
        }
        if (-not (Test-Path "dist")) {
            Write-Info "Building TUI..."
            npm run build --silent
        }
        Write-Info "Launching TUI..."
        npm start
        Pop-Location
    }

    "cli" {
        Assert-Built
        Write-Info "Running: $Bin $Rest"
        & $Bin @Rest
    }

    "sweep" {
        if ($Rest.Count -eq 0) { Write-Err "Usage: run.ps1 sweep <algorithm>" }
        Assert-Built
        Write-Info "Sweep: $($Rest[0])"
        & $Bin --algorithm $Rest[0] --sweep
    }

    "compare" {
        Assert-Built
        Write-Info "Comparing all algorithms..."
        & $Bin --compare
    }

    "test" {
        Invoke-Build
        Write-Info "Running tests..."
        Push-Location "$Root\build"
        ctest --verbose
        Pop-Location
    }

    "clean" {
        Write-Info "Removing build\..."
        Remove-Item -Recurse -Force "$Root\build" -ErrorAction SilentlyContinue
        Write-OK "Cleaned"
    }

    { $_ -in "help","-h","--help" } {
        Show-Usage
    }

    default {
        Write-Err "Unknown command: $Cmd. Run '.\scripts\run.ps1 help' for usage."
    }
}
