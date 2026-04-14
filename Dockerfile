# Multi-stage build for Multi-Algo-Analysis
# Stage 1: Build C++ backend
FROM ubuntu:22.04 AS cpp-builder

RUN apt-get update && apt-get install -y \
    g++ \
    make \
    cmake \
    libsqlite3-dev \
    gnuplot-nox \
    git \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN make clean && make

# Stage 2: Build TUI frontend
FROM node:18-alpine AS tui-builder

WORKDIR /app/tui
COPY tui/package.json tui/package-lock.json* ./
RUN npm ci
COPY tui/ .
RUN npm run build

# Stage 3: Runtime image
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    libsqlite3-0 \
    gnuplot-nox \
    nodejs \
    npm \
    && rm -rf /var/lib/apt/lists/*

# Create app directory
WORKDIR /app

# Copy C++ binary
COPY --from=cpp-builder /app/resource_monitor_app .

# Copy TUI
COPY --from=tui-builder /app/tui ./tui
COPY --from=tui-builder /app/tui/node_modules ./tui/node_modules

# Create necessary directories
RUN mkdir -p database csv png exports

# Copy project files (docs, scripts, etc.)
COPY README.md .
COPY docs/ ./docs/

# Set environment variables
ENV NODE_ENV=production

# Expose port (if web dashboard is added later)
EXPOSE 3000

# Health check
HEALTHCHECK --interval=30s --timeout=10s --start-period=5s --retries=3 \
    CMD ["/app/resource_monitor_app", "--list"]

# Default command: Run TUI
CMD ["sh", "-c", "cd /app/tui && npm start"]
