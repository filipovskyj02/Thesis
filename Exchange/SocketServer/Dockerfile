FROM alpine:latest

RUN apk add --no-cache \
      build-base \
      cmake \
      boost-dev \
      curl \
      bash



# librdkafka
RUN mkdir /tmp/librdkafka && \
    cd /tmp/librdkafka && \
    curl -L -o v2.4.0.tar.gz https://github.com/confluentinc/librdkafka/archive/refs/tags/v2.4.0.tar.gz && \
    tar xzf v2.4.0.tar.gz && \
    cd librdkafka-2.4.0 && \
    ./configure --prefix=/usr && make -j$(nproc) && make install

WORKDIR /opt/socket-server
COPY . .
RUN mkdir build && cd build && \
    cmake .. && make -j$(nproc)

EXPOSE 9000
ENTRYPOINT ["/opt/socket-server/build/src/SocketServer"]

