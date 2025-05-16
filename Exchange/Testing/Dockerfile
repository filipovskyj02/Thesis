FROM debian:bookworm-slim

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    libboost-all-dev \
    curl \
    ca-certificates \
    gnupg \
    python3 \
    python3-requests \
    python3-websocket \
    python3-pip \
    nano \
    && rm -rf /var/lib/apt/lists/*

RUN curl -sL https://github.com/grafana/k6/releases/download/v0.49.0/k6-v0.49.0-linux-amd64.tar.gz | \
    tar -xz -C /usr/local/bin --strip-components=1

WORKDIR /tests

COPY socket/throughput /tests/socket/throughput
COPY socket/latency /tests/socket/latency

COPY rest/throughput /tests/rest/throughput
COPY rest/latency /tests/rest/latency

RUN g++ -O2 -std=c++17 -o /tests/socket/latency/latency-socket /tests/socket/latency/latency-socket.cpp -lboost_system -lpthread && \
    g++ -O2 -std=c++17 -o /tests/socket/throughput/client /tests/socket/throughput/client.cpp -lboost_system -lpthread