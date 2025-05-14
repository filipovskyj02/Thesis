
FROM debian:bookworm-slim AS build
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
        build-essential cmake libboost-dev libssl-dev curl ca-certificates \
        libjemalloc-dev && \
    rm -rf /var/lib/apt/lists/*
ENV LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libjemalloc.so.2  

# librdkafka 2.4
RUN curl -L -o /tmp/librdkafka.tgz \
        https://github.com/confluentinc/librdkafka/archive/refs/tags/v2.4.0.tar.gz && \
    tar -C /tmp -xzf /tmp/librdkafka.tgz && \
    cd /tmp/librdkafka-2.4.0 && ./configure --prefix=/usr && \
    make -j$(nproc) && make install

WORKDIR /src
COPY . .
RUN cmake -S . -B build && cmake --build build -j$(nproc)


FROM debian:bookworm-slim
COPY --from=build /usr /usr
COPY --from=build /src/build/src/MatchingEngine /usr/local/bin/MatchingEngine
ENV LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libjemalloc.so.2
ENTRYPOINT ["MatchingEngine"]
