# Multi-Algo-Analysis — Windows Setup Script
# Run in PowerShell (Admin) from the project root:
#   Set-ExecutionPolicy Bypass -Scope Process -Force
#   .\scripts\setup.ps1
#
# Requires: Windows 10 1709+ (winget) or Chocolatey pre-installed.

#Requires -Version 5.1

$ErrorActionPreference = "Stop"

function Write-Info    { param($msg) Write-Host "[INFO]    $msg" -ForegroundColor Cyan }
function Write-OK      { param($msg) Write-Host "[OK]      $msg" -ForegroundColor Green }
function Write-Warn    { param($msg) Write-Host "[WARN]    $msg" -ForegroundColor Yellow }
function Write-Err     { param($msg) Write-Host "[ERROR]   $msg" -ForegroundColor Red; exit 1 }

# ── Detect package manager ────────────────────────────────────────────────────

function Get-PackageManager {
    if (Get-Command winget -ErrorAction SilentlyContinue) { return "winget" }
    if (Get-Command choco  -ErrorAction SilentlyContinue) { return "choco"  }
    return $null
}

function Install-WithWinget { param($id, $label)
    Write-Info "Installing $label via winget..."
    winget install --id $id --accept-source-agreements --accept-package-agreements -e --silent
}

function Install-WithChoco { param($pkg, $label)
    Write-Info "Installing $label via chocolatey..."
    choco install $pkg -y --no-progress
}

# ── Installers ────────────────────────────────────────────────────────────────

function Install-Dependencies {
    $pm = Get-PackageManager
    if (-not $pm) {
        Write-Err "Neither winget nor Chocolatey found.`nInstall Chocolatey: https://chocolatey.org/install"
    }
    Write-Info "Using package manager: $pm"

    $packages = @(
        @{ winget = "Kitware.CMake";              choco = "cmake";       label = "CMake"       },
        @{ winget = "MSYS2.MSYS2";                choco = "msys2";       label = "MSYS2/GCC"   },
        @{ winget = "SQLite.SQLite";              choco = "sqlite";      label = "SQLite3"     },
        @{ winget = "OpenJS.NodeJS.LTS";          choco = "nodejs-lts";  label = "Node.js LTS" },
        @{ winget = "GnuWin32.GnuPlot";           choco = "gnuplot";     label = "GNUplot"     },
        @{ winget = "Python.Python.3.12";         choco = "python";      label = "Python 3"    }
    )

    foreach ($pkg in $packages) {
        try {
            if ($pm -eq "winget") { Install-WithWinget $pkg.winget $pkg.label }
            else                   { Install-WithChoco  $pkg.choco  $pkg.label }
            Write-OK "$($pkg.label) installed"
        } catch {
            Write-Warn "$($pkg.label) install failed: $_"
        }
    }

    # GoogleTest — build from source via vcpkg if available, else warn
    if (Get-Command vcpkg -ErrorAction SilentlyContinue) {
        Write-Info "Installing GoogleTest via vcpkg..."
        vcpkg install gtest:x64-windows 2>&1 | Out-Null
        Write-OK "GoogleTest installed via vcpkg"
    } else {
        Write-Warn "vcpkg not found — GoogleTest skipped (tests will be disabled). Install vcpkg for full test support."
    }

    # Python packages for analysis scripts
    if (Get-Command pip -ErrorAction SilentlyContinue) {
        Write-Info "Installing Python analysis deps..."
        pip install --quiet numpy scipy 2>&1 | Out-Null
        Write-OK "numpy, scipy installed"
    }
}

# ── Directories ───────────────────────────────────────────────────────────────

function New-Directories {
    foreach ($dir in "build","database","csv","png") {
        if (-not (Test-Path $dir)) { New-Item -ItemType Directory $dir | Out-Null }
    }
    Write-OK "Directories ready: build\ database\ csv\ png\"
}

# ── Verify ────────────────────────────────────────────────────────────────────

function Test-Tools {
    $ok = $true
    foreach ($tool in "cmake","node","sqlite3") {
        if (Get-Command $tool -ErrorAction SilentlyContinue) {
            Write-OK "$tool found"
        } else {
            Write-Warn "$tool not found in PATH (may need to restart shell)"
            $ok = $false
        }
    }
    if (-not $ok) {
        Write-Warn "Some tools not in PATH. Restart PowerShell and re-run if build fails."
    }
}

# ── Build ─────────────────────────────────────────────────────────────────────

function Build-Project {
    Write-Info "Configuring CMake (MinGW Makefiles)..."
    cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -Wno-dev -S .
    Write-Info "Building..."
    cmake --build build -j $env:NUMBER_OF_PROCESSORS
    Write-OK "Binary: build\resource_monitor_app.exe"

    if (Test-Path "tui\package.json") {
        Write-Info "Building TUI..."
        Push-Location tui
        npm install --silent
        npm run build --silent
        Pop-Location
        Write-OK "TUI built"
    }
}

# ── Main ──────────────────────────────────────────────────────────────────────

Write-Host ""
Write-Host "══════════════════════════════════════" -ForegroundColor Cyan
Write-Host "  Multi-Algo-Analysis Windows Setup"   -ForegroundColor Cyan
Write-Host "══════════════════════════════════════" -ForegroundColor Cyan
Write-Host ""

Install-Dependencies
New-Directories
Test-Tools

Write-Host ""
$resp = Read-Host "Build the project now? [y/N]"
if ($resp -match "^[Yy]") { Build-Project }

Write-Host ""
Write-OK "Setup complete!"
Write-Host ""
Write-Host "  TUI:     cd tui; npm start"                                            -ForegroundColor Yellow
Write-Host "  CLI:     .\build\resource_monitor_app.exe --algorithm binary_search"  -ForegroundColor Yellow
Write-Host "  Sweep:   .\build\resource_monitor_app.exe --algorithm merge_sort --sweep" -ForegroundColor Yellow
Write-Host "  Compare: .\build\resource_monitor_app.exe --compare"                  -ForegroundColor Yellow
Write-Host "  Tests:   cd build; ctest --verbose"                                   -ForegroundColor Yellow
Write-Host ""
Write-Host "  Or use the unified run script: .\scripts\run.ps1" -ForegroundColor Cyan
Write-Host ""
