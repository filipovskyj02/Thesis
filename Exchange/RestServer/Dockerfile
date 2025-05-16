FROM alpine:latest

RUN apk add --no-cache \
      build-base \
      curl \
      cmake \
      boost-dev \
      bash \
      openssl-dev \
      zlib-dev \
      ca-certificates \
      asio-dev \
      git \
      wget && \
    update-ca-certificates  # update the CA bundle 

RUN mkdir /tmp/librdkafka && \
    cd /tmp/librdkafka && \
    curl -L -o v2.4.0.tar.gz https://github.com/confluentinc/librdkafka/archive/refs/tags/v2.4.0.tar.gz && \
    tar xzf v2.4.0.tar.gz && \
    cd librdkafka-2.4.0 && \
    ./configure --prefix=/usr && \
    make -j$(nproc) && \
    make install && \
    cd / && rm -rf /tmp/librdkafka

RUN git clone --depth 1 --branch v1.2.0 \
      https://github.com/CrowCpp/Crow.git /opt/crow

ENV CPATH=/opt/crow/include:$CPATH

WORKDIR /opt/rest-server
COPY . .
RUN mkdir build && cd build && \
    cmake .. && make -j"$(nproc)"

EXPOSE 8080
ENTRYPOINT ["/opt/rest-server/build/src/RestServer"]
