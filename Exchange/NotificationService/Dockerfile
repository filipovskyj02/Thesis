FROM openjdk:21-jdk-slim AS builder

RUN apt-get update && \
    apt-get install -y --no-install-recommends maven ca-certificates && \
    rm -rf /var/lib/apt/lists/*


WORKDIR /app

COPY pom.xml .
RUN mvn dependency:go-offline -B

COPY src ./src
RUN mvn package -DskipTests -B

FROM openjdk:21-jdk-slim

WORKDIR /app

COPY --from=builder /app/target/*.jar notification-service.jar

EXPOSE 8090

ENTRYPOINT ["java", "-jar", "notification-service.jar"]
