# -------- Base layer: Debian slim + k6 + Python -------------
FROM debian:bookworm-slim

# 1) Core utilities
RUN apt-get update && apt-get install -y --no-install-recommends \
        curl ca-certificates gnupg python3 python3-requests python3-websocket python3-pip nano && \
    rm -rf /var/lib/apt/lists/*

# 2) k6 static binary (latest release) ------------------------
RUN curl -sL https://github.com/grafana/k6/releases/download/v0.49.0/k6-v0.49.0-linux-amd64.tar.gz | \
    tar -xz -C /usr/local/bin --strip-components=1

# 3) Copy *both* test suites and install latency deps ---------
WORKDIR /tests
COPY throughput  /tests/throughput
COPY latency     /tests/latency


