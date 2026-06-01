#!/bin/bash
# Multi-Algo-Analysis setup — Linux & macOS
# Usage: chmod +x scripts/setup.sh && ./scripts/setup.sh
# Linux: sudo ./scripts/setup.sh    (needs root for package managers)
# macOS: ./scripts/setup.sh         (uses Homebrew, no sudo needed)

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

log_info()    { echo -e "${BLUE}[INFO]${NC} $1"; }
log_success() { echo -e "${GREEN}[SUCCESS]${NC} $1"; }
log_warn()    { echo -e "${YELLOW}[WARN]${NC} $1"; }
log_error()   { echo -e "${RED}[ERROR]${NC} $1"; }

detect_os() {
    case "$(uname -s)" in
        Darwin) OS="macos" ;;
        Linux)  OS="linux" ;;
        *)      log_error "Unsupported OS: $(uname -s)"; exit 1 ;;
    esac
    log_info "Detected OS: $OS"
}

# ── macOS ────────────────────────────────────────────────────────────────────

install_macos() {
    if ! command -v brew &>/dev/null; then
        log_info "Homebrew not found — installing..."
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    fi

    log_info "Updating Homebrew..."
    brew update

    log_info "Installing build tools..."
    brew install cmake make

    log_info "Installing SQLite3..."
    brew install sqlite3

    log_info "Installing GNUplot..."
    brew install gnuplot

    log_info "Installing Node.js..."
    brew install node

    log_info "Installing GoogleTest..."
    brew install googletest || log_warn "googletest formula unavailable — tests will be skipped"

    log_info "Installing Python deps for analysis scripts..."
    pip3 install --quiet numpy scipy 2>/dev/null || \
        log_warn "pip3 unavailable — install numpy/scipy manually if you need the analysis scripts"
}

# ── Linux ────────────────────────────────────────────────────────────────────

detect_linux_pkg_manager() {
    if   command -v apt-get &>/dev/null; then PKG="apt"
    elif command -v dnf     &>/dev/null; then PKG="dnf"
    elif command -v yum     &>/dev/null; then PKG="yum"
    elif command -v pacman  &>/dev/null; then PKG="pacman"
    else log_error "No supported package manager found"; exit 1
    fi
    log_info "Package manager: $PKG"
}

install_linux() {
    if [ "$EUID" -ne 0 ]; then
        log_error "Linux install requires root. Run: sudo $0"
        exit 1
    fi

    detect_linux_pkg_manager

    case "$PKG" in
        apt)
            apt-get update -q
            apt-get install -y g++ build-essential cmake make \
                libsqlite3-dev sqlite3 gnuplot nodejs npm \
                libgtest-dev python3-pip
            # Build gtest static libs if not pre-built
            local gtest_src=""
            [ -d /usr/src/googletest/googletest ] && gtest_src="/usr/src/googletest/googletest"
            [ -d /usr/src/gtest ]                 && gtest_src="/usr/src/gtest"
            if [ -n "$gtest_src" ] && [ ! -f /usr/lib/libgtest.a ]; then
                log_info "Building GTest from source..."
                ( cd "$gtest_src" && cmake . -B _build -DCMAKE_BUILD_TYPE=Release > /dev/null \
                  && cmake --build _build > /dev/null \
                  && cp _build/lib/*.a /usr/lib/ 2>/dev/null || true )
            fi
            pip3 install --quiet numpy scipy 2>/dev/null || true
            ;;
        dnf|yum)
            $PKG check-update || true
            $PKG install -y gcc-c++ cmake make sqlite-devel sqlite \
                gnuplot gtest-devel nodejs npm python3-pip
            pip3 install --quiet numpy scipy 2>/dev/null || true
            ;;
        pacman)
            pacman -Sy --noconfirm --needed \
                base-devel cmake sqlite gnuplot gtest nodejs npm python-pip
            pip install --quiet numpy scipy 2>/dev/null || true
            ;;
    esac
}

# ── Common ───────────────────────────────────────────────────────────────────

create_directories() {
    mkdir -p build database csv png
    log_success "Directories ready: build/ database/ csv/ png/"
}

verify_tools() {
    local ok=true
    for tool in cmake make sqlite3; do
        if command -v "$tool" &>/dev/null; then
            log_success "$tool: $(${tool} --version 2>&1 | head -1)"
        else
            log_error "$tool not found"
            ok=false
        fi
    done
    command -v gnuplot &>/dev/null \
        && log_success "gnuplot: $(gnuplot --version)" \
        || log_warn "gnuplot not found — PNG plots disabled"
    command -v node &>/dev/null \
        && log_success "node: $(node --version)" \
        || log_warn "Node.js not found — TUI disabled"
    [ "$ok" = false ] && { log_error "Critical deps missing"; exit 1; }
}

build_project() {
    log_info "Building C++ binary..."
    cmake -B build -DCMAKE_BUILD_TYPE=Release -Wno-dev > /dev/null
    cmake --build build -j"$(nproc 2>/dev/null || sysctl -n hw.logicalcpu 2>/dev/null || echo 2)"
    log_success "C++ build done → build/resource_monitor_app"

    if [ -f tui/package.json ]; then
        log_info "Building TUI..."
        ( cd tui && npm install --silent && npm run build --silent )
        log_success "TUI build done"
    fi
}

print_next_steps() {
    echo ""
    log_success "Setup complete!"
    echo ""
    echo -e "  ${YELLOW}TUI:${NC}       cd tui && npm start"
    echo -e "  ${YELLOW}CLI:${NC}       ./build/resource_monitor_app --algorithm binary_search"
    echo -e "  ${YELLOW}Sweep:${NC}     ./build/resource_monitor_app --algorithm merge_sort --sweep"
    echo -e "  ${YELLOW}Compare:${NC}   ./build/resource_monitor_app --compare"
    echo -e "  ${YELLOW}Tests:${NC}     cd build && ctest --verbose"
    echo ""
    echo -e "  Or use the unified run script: ${BLUE}./scripts/run.sh${NC}"
    echo ""
}

main() {
    echo ""
    echo -e "${BLUE}══════════════════════════════════════${NC}"
    echo -e "${BLUE}  Multi-Algo-Analysis Setup${NC}"
    echo -e "${BLUE}══════════════════════════════════════${NC}"
    echo ""
    detect_os
    case "$OS" in
        macos) install_macos ;;
        linux) install_linux ;;
    esac
    create_directories
    verify_tools

    echo ""
    read -rp "Build the project now? [y/N] " resp
    [[ "$resp" =~ ^[Yy]$ ]] && build_project

    print_next_steps
}

main "$@"
