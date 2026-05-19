#!/usr/bin/env bash
set -e
cd "$(dirname "$0")"
cmake -B build .
cmake --build build
echo "Build complete: build/resource_monitor_app"
