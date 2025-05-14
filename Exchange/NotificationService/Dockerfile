# ─── Build Stage ────────────────────────────────────────────────────────────────
FROM openjdk:21-jdk-slim AS builder

# Install Maven (lightweight) and update certificates
RUN apt-get update && \
    apt-get install -y --no-install-recommends maven ca-certificates && \
    rm -rf /var/lib/apt/lists/*


WORKDIR /app

# Pre-fetch dependencies
COPY pom.xml .
RUN mvn dependency:go-offline -B

# Build the application
COPY src ./src
RUN mvn package -DskipTests -B

# ─── Runtime Stage ──────────────────────────────────────────────────────────────
FROM openjdk:21-jdk-slim

WORKDIR /app

# Copy only the JAR from the builder
COPY --from=builder /app/target/*.jar notification-service.jar

EXPOSE 8090

ENTRYPOINT ["java", "-jar", "notification-service.jar"]
