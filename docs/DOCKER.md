# Docker Deployment Guide

## Overview
Multi-Algo-Analysis can be run in Docker for easy deployment and reproducibility.

## Prerequisites
- Docker and Docker Compose installed
- At least 2GB of free disk space

## Quick Start

### 1. Run TUI Dashboard
```bash
docker compose --profile tui up
```

### 2. Run Algorithm via CLI
```bash
# Run binary search with JSON output
docker compose --profile cli run multi-algo-binary

# Run with custom parameters
docker compose --profile cli run --rm multi-algo-binary \
  /app/resource_monitor_app --algorithm linear_search --json --data-size 500000 --runs 3
```

### 3. Development Mode
```bash
docker compose --profile dev up
```

## Building Images

### Build all images
```bash
docker compose build
```

### Build specific image
```bash
docker compose build multi-algo-app
```

## Running Commands in Container

### Run C++ binary directly
```bash
docker compose run --rm multi-algo-app \
  /app/resource_monitor_app --algorithm binary_search --json
```

### Run TUI
```bash
docker compose run --rm multi-algo-app \
  sh -c "cd /app/tui && npm start"
```

### Access bash in container
```bash
docker compose run --rm multi-algo-app bash
```

## Data Persistence

Data is automatically persisted via volumes to these directories:
- `./database/` - SQLite databases
- `./csv/` - CSV exports
- `./png/` - GNUplot visualizations
- `./exports/` - Exported reports

## Configuration

### Environment Variables
- `NODE_ENV` - Node.js environment (production/development)

### Custom Parameters
Pass CLI flags to the binary:
- `--algorithm <name>` - Select algorithm
- `--data-size <n>` - Number of elements
- `--runs <n>` - Number of iterations
- `--targets <list>` - Comma-separated targets
- `--json` - JSON output
- `--stream` - Real-time metrics streaming

## Production Deployment

### Build optimized image
```bash
docker build -t multi-algo-analysis:latest .
```

### Run in detached mode
```bash
docker run -d \
  --name multi-algo \
  -v $(pwd)/database:/app/database \
  -v $(pwd)/csv:/app/csv \
  -v $(pwd)/png:/app/png \
  multi-algo-analysis:latest
```

### Health Check
The container includes a health check that verifies the binary is functional:
```bash
docker inspect --format='{{.State.Health.Status}}' multi-algo
```

## Troubleshooting

### View logs
```bash
docker compose logs -f
```

### Clean up
```bash
# Stop all containers
docker compose down

# Remove volumes (deletes data!)
docker compose down -v

# Remove images
docker compose rm
```

### Rebuild from scratch
```bash
docker compose down -v --rmi all
docker compose build
docker compose --profile tui up
```

## Architecture

The Docker image uses multi-stage builds:
1. **cpp-builder**: Compiles C++ binary with all dependencies
2. **tui-builder**: Builds TypeScript TUI with npm
3. **runtime**: Minimal runtime image with only necessary libraries

This keeps the final image size small while maintaining full functionality.
