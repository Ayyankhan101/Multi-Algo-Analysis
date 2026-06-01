#!/bin/bash
# Multi-Algo-Analysis — unified build & run helper (Linux / macOS)
# Usage: ./scripts/run.sh [command] [options]
#
# Commands:
#   build           Build the C++ binary (default if no command given)
#   tui             Build + launch the TUI dashboard
#   cli [args...]   Build + run the CLI binary with forwarded args
#   sweep <algo>    Run complexity sweep for one algorithm
#   compare         Run all-algorithm comparison sweep
#   test            Build + run the test suite
#   clean           Remove build artefacts

set -e

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/build/resource_monitor_app"
JOBS=$(nproc 2>/dev/null || sysctl -n hw.logicalcpu 2>/dev/null || echo 2)

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

log_info()    { echo -e "${BLUE}[INFO]${NC} $1"; }
log_success() { echo -e "${GREEN}[OK]${NC} $1"; }
log_error()   { echo -e "${RED}[ERROR]${NC} $1"; exit 1; }

usage() {
    echo "Usage: $0 [build|tui|cli|sweep|compare|test|clean] [options]"
    echo ""
    echo "  build           Compile C++ binary"
    echo "  tui             Launch interactive TUI dashboard"
    echo "  cli [args]      Run CLI binary (e.g.: cli --algorithm merge_sort --runs 10)"
    echo "  sweep <algo>    Complexity sweep for <algo>"
    echo "  compare         Benchmark all algorithms side-by-side"
    echo "  test            Run test suite"
    echo "  clean           Delete build/"
    echo ""
    echo "Algorithms: binary_search linear_search interpolation_search"
    echo "            merge_sort quick_sort heap_sort shell_sort"
    echo "            insertion_sort selection_sort bubble_sort"
}

do_build() {
    log_info "Configuring with CMake..."
    cmake -B "$ROOT/build" -DCMAKE_BUILD_TYPE=Release -Wno-dev -S "$ROOT" > /dev/null
    log_info "Building with $JOBS jobs..."
    cmake --build "$ROOT/build" -j"$JOBS"
    log_success "Binary: $BIN"
}

ensure_built() {
    [ -x "$BIN" ] || do_build
}

CMD="${1:-build}"
shift || true

case "$CMD" in
    build)
        do_build
        ;;

    tui)
        ensure_built
        if [ ! -d "$ROOT/tui/node_modules" ]; then
            log_info "Installing TUI dependencies..."
            ( cd "$ROOT/tui" && npm install --silent )
        fi
        if [ ! -d "$ROOT/tui/dist" ]; then
            log_info "Building TUI..."
            ( cd "$ROOT/tui" && npm run build --silent )
        fi
        log_info "Launching TUI..."
        ( cd "$ROOT/tui" && npm start )
        ;;

    cli)
        ensure_built
        log_info "Running: $BIN $*"
        "$BIN" "$@"
        ;;

    sweep)
        [ -n "$1" ] || log_error "Usage: $0 sweep <algorithm>"
        ensure_built
        log_info "Sweep: $1"
        "$BIN" --algorithm "$1" --sweep
        ;;

    compare)
        ensure_built
        log_info "Comparing all algorithms..."
        "$BIN" --compare
        ;;

    test)
        do_build
        log_info "Running tests..."
        ( cd "$ROOT/build" && ctest --verbose )
        ;;

    clean)
        log_info "Removing build/..."
        rm -rf "$ROOT/build"
        log_success "Cleaned"
        ;;

    help|-h|--help)
        usage
        ;;

    *)
        log_error "Unknown command: $CMD. Run '$0 help' for usage."
        ;;
esac
