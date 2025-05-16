# Exchange System

This directory contains the final implementation of a high-performance stock exchange system.

## Requirements

- Docker Compose **v2** or newer  
  *(You can verify with `docker compose version` — note the space between `docker` and `compose`, which is used in v2+.)*

## Running the System

To build and start the system, run:

```bash
docker compose up --build
```

This will build and start all services defined in `docker-compose.yml`, including:

- Core matching engine  
- REST and TCP gateways  
- Kafka and topic initialization  
- Notification service  
- Testing container  

The system becomes fully operational once you see the following message from the matching engine:

```
matching-engine-1 | [2025-05-16 08:20:31.532849] NOTICE KafkaConsumer[de06024f-ece3c65e] subscribed, topics[orders]
```

If the core engine fails to initialize, restart it with:

```bash
docker restart exchange-matching-engine-1
```

## Available Endpoints

Once the system is running, the following endpoints are available:

- **REST API**: [http://localhost:8080](http://localhost:8080)  
- **TCP Server**: `localhost:9000`  
- **WebSocket Notifications**: `ws://localhost:8090/ws-notifications?userId=<userId>`

## Testing

A dedicated testing container is provided with pre-configured latency and throughput tests for both REST and TCP sockets.

While Docker runs on the host network, you can run tests outside the container if your system has the required dependencies (Python libraries, `k6`, and a C++ compiler) installed. The testing container includes all dependencies and precompiled binaries.

To access the testing container:

```bash
docker exec -it exchange-test-1 bash
```

### REST Tests

Navigate to the `Testing/rest` directory:

- **Throughput Test**:  
  Go to `rest/throughput` and run:
  ```bash
  k6 run allTypestest.js
  ```

- **Latency Test**:  
  Go to `rest/latency` and run:
  ```bash
  python3 ws.py
  ```
  *(This test exercises the entire end-to-end pipeline, including notifications and multicast reception. It is the recommended test for evaluating the system as a whole.)*

### TCP Socket Tests

Navigate to the `Testing/socket` directory:

- **Throughput Test**:  
  Go to `socket/throughput` and either compile the test client:
  ```bash
  g++ client.cpp
  ```
  or, if you're in the testing container, simply run the precompiled executable.

- **Latency Test**:  
  Follow the same steps as the throughput test.

### Custom Tests

You're welcome to create and run your own tests targeting the REST or TCP interfaces. Custom test implementation is left up to you.