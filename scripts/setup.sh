#!/bin/bash

#############################################################################
# Multi-Algo-Analysis Setup Script
# 
# This script automatically installs all dependencies and configures the
# development environment for the Multi-Algo-Analysis project.
#
# Usage:
#   chmod +x scripts/setup.sh
#   ./scripts/setup.sh
#
# Supported Distributions:
#   - Ubuntu/Debian (apt)
#   - Fedora/RHEL/CentOS (dnf/yum)
#   - Arch Linux (pacman)
#   - Kali Linux (apt)
#############################################################################

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if running as root
check_root() {
    if [ "$EUID" -ne 0 ]; then 
        log_error "Please run this script with sudo or as root"
        exit 1
    fi
}

# Detect package manager
detect_package_manager() {
    if command -v apt-get &> /dev/null; then
        PKG_MANAGER="apt"
        PKG_CMD="apt-get"
        PKG_INSTALL="apt-get install -y"
        PKG_UPDATE="apt-get update"
    elif command -v dnf &> /dev/null; then
        PKG_MANAGER="dnf"
        PKG_CMD="dnf"
        PKG_INSTALL="dnf install -y"
        PKG_UPDATE="dnf check-update || true"
    elif command -v yum &> /dev/null; then
        PKG_MANAGER="yum"
        PKG_CMD="yum"
        PKG_INSTALL="yum install -y"
        PKG_UPDATE="yum check-update || true"
    elif command -v pacman &> /dev/null; then
        PKG_MANAGER="pacman"
        PKG_CMD="pacman"
        PKG_INSTALL="pacman -S --noconfirm --needed"
        PKG_UPDATE="pacman -Sy"
    else
        log_error "Unsupported package manager. Please install dependencies manually."
        exit 1
    fi
    
    log_info "Detected package manager: $PKG_MANAGER"
}

# Detect Linux distribution
detect_distro() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        DISTRO=$ID
        DISTRO_VERSION=$VERSION_ID
        log_info "Detected distribution: $DISTRO $DISTRO_VERSION"
    else
        log_warn "Could not detect distribution, attempting to detect package manager only"
    fi
}

# Install packages for Debian/Ubuntu-based systems
install_apt() {
    log_info "Updating package lists..."
    $PKG_UPDATE
    
    log_info "Installing C++ compiler and build tools..."
    $PKG_INSTALL g++ build-essential
    
    log_info "Installing CMake build system..."
    $PKG_INSTALL cmake
    
    log_info "Installing GNU Make..."
    $PKG_INSTALL make
    
    log_info "Installing SQLite3 development libraries..."
    $PKG_INSTALL libsqlite3-dev sqlite3
    
    log_info "Installing Google Test framework..."
    $PKG_INSTALL libgtest-dev
    
    log_info "Installing GNUplot for visualization..."
    $PKG_INSTALL gnuplot

    log_info "Installing Node.js and npm for TUI..."
    $PKG_INSTALL nodejs npm
    
    # Build gtest from source if needed (some Debian-based systems require this)
    if [ -d /usr/src/gtest ] || [ -d /usr/src/googletest/googletest ]; then
        local gtest_lib="/usr/lib/libgtest.a"
        if [ ! -f "$gtest_lib" ]; then
            log_info "Building Google Test from source..."
            local gtest_src=""
            if [ -d /usr/src/googletest/googletest ]; then
                gtest_src="/usr/src/googletest/googletest"
            elif [ -d /usr/src/gtest ]; then
                gtest_src="/usr/src/gtest"
            fi
            
            if [ -n "$gtest_src" ]; then
                cd "$gtest_src"
                cmake CMakeLists.txt > /dev/null 2>&1
                make > /dev/null 2>&1
                if [ -d lib ]; then
                    cp lib/*.a /usr/lib/ 2>/dev/null || true
                fi
                cd - > /dev/null
                log_success "Google Test built successfully"
            fi
        fi
    fi
}

# Install packages for Fedora/RHEL/CentOS-based systems
install_dnf_yum() {
    log_info "Updating package lists..."
    $PKG_UPDATE
    
    log_info "Installing C++ compiler and build tools..."
    $PKG_INSTALL gcc-c++
    
    log_info "Installing CMake build system..."
    $PKG_INSTALL cmake
    
    log_info "Installing GNU Make..."
    $PKG_INSTALL make
    
    log_info "Installing SQLite3 development libraries..."
    $PKG_INSTALL sqlite-devel sqlite
    
    log_info "Installing Google Test framework..."
    $PKG_INSTALL gtest-devel gtest
    
    log_info "Installing GNUplot for visualization..."
    $PKG_INSTALL gnuplot

    log_info "Installing Node.js and npm for TUI..."
    $PKG_INSTALL nodejs npm
}

# Install packages for Arch Linux
install_pacman() {
    log_info "Updating package lists..."
    $PKG_UPDATE
    
    log_info "Installing C++ compiler and build tools..."
    $PKG_INSTALL base-devel
    
    log_info "Installing CMake build system..."
    $PKG_INSTALL cmake
    
    log_info "Installing SQLite3 development libraries..."
    $PKG_INSTALL sqlite
    
    log_info "Installing Google Test framework..."
    $PKG_INSTALL gtest
    
    log_info "Installing GNUplot for visualization..."
    $PKG_INSTALL gnuplot

    log_info "Installing Node.js and npm for TUI..."
    $PKG_INSTALL nodejs npm
}

# Install dependencies based on detected distribution
install_dependencies() {
    log_info "Installing all required dependencies..."
    
    case $PKG_MANAGER in
        apt)
            install_apt
            ;;
        dnf|yum)
            install_dnf_yum
            ;;
        pacman)
            install_pacman
            ;;
        *)
            log_error "Unsupported package manager"
            exit 1
            ;;
    esac
}

# Create required directories
create_directories() {
    log_info "Creating required directories..."
    
    mkdir -p build
    mkdir -p database
    mkdir -p csv
    mkdir -p png
    
    log_success "Directories created: build/, database/, csv/, png/"
}

# Verify installations
verify_installations() {
    log_info "Verifying installations..."
    
    local success=true
    
    # Check g++
    if command -v g++ &> /dev/null; then
        log_success "g++ installed: $(g++ --version | head -n 1)"
    else
        log_error "g++ not found"
        success=false
    fi
    
    # Check cmake
    if command -v cmake &> /dev/null; then
        log_success "cmake installed: $(cmake --version | head -n 1)"
    else
        log_warn "cmake not found (optional, Make is sufficient)"
    fi
    
    # Check make
    if command -v make &> /dev/null; then
        log_success "make installed: $(make --version | head -n 1)"
    else
        log_error "make not found"
        success=false
    fi
    
    # Check sqlite3
    if command -v sqlite3 &> /dev/null; then
        log_success "sqlite3 installed: $(sqlite3 --version)"
    else
        log_error "sqlite3 not found"
        success=false
    fi
    
    # Check gnuplot
    if command -v gnuplot &> /dev/null; then
        log_success "gnuplot installed: $(gnuplot --version)"
    else
        log_warn "gnuplot not found (plotting features will be disabled)"
    fi
    
    if [ "$success" = false ]; then
        log_error "Some critical dependencies failed to install"
        exit 1
    fi
}

# Build the project
build_project() {
    log_info "Building the C++ project..."
    
    # Try Make first (simpler)
    if [ -f Makefile ]; then
        log_info "Using Makefile build system..."
        make clean
        make
        log_success "C++ build successful!"
    else
        log_error "Makefile not found"
        exit 1
    fi

    log_info "Building the TUI..."
    
    if [ -d tui ] && [ -f tui/package.json ]; then
        cd tui
        npm install
        npm run build
        cd ..
        log_success "TUI build successful!"
    else
        log_warn "TUI directory not found, skipping TUI build"
    fi
}

# Display final instructions
display_next_steps() {
    echo ""
    log_success "Setup complete! The project is ready to use."
    echo ""
    log_info "Quick start - C++ Binary:"
    echo -e "  ${YELLOW}Build:${NC}      make && make run"
    echo -e "  ${YELLOW}Run:${NC}        ./resource_monitor_app"
    echo -e "  ${YELLOW}Clean:${NC}      make clean"
    echo ""
    log_info "Quick start - Terminal UI:"
    echo -e "  ${YELLOW}Run TUI:${NC}    cd tui && npm start"
    echo -e "  ${YELLOW}Dev Mode:${NC}   cd tui && npm run dev"
    echo ""
    log_info "Output files will be saved to:"
    echo -e "  ${BLUE}Database:${NC}   database/resource_metrics.db"
    echo -e "  ${BLUE}CSV data:${NC}    csv/binary_search_*.csv"
    echo -e "  ${BLUE}Plots:${NC}       png/*.png"
    echo ""
}

# Main execution
main() {
    echo ""
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}  Multi-Algo-Analysis Setup Script${NC}"
    echo -e "${BLUE}========================================${NC}"
    echo ""
    
    check_root
    detect_distro
    detect_package_manager
    install_dependencies
    create_directories
    verify_installations
    
    echo ""
    log_info "Would you like to build the project now? (y/n)"
    read -r response
    if [[ "$response" =~ ^[Yy]$ ]]; then
        build_project
    fi
    
    display_next_steps
}

# Run main function
main "$@"
