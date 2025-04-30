#!/bin/bash
# Usage: ./spam.sh <number_of_requests> [<url>]
# Example: ./spam.sh 10000 http://localhost:8080/hello

if [ $# -lt 1 ]; then
    echo "Usage: $0 <number_of_requests> [<url>]"
    exit 1
fi

NUM_REQUESTS=$1
# Default URL if none provided
URL=${2:-http://localhost:8080/hello}

echo "Spamming $URL with $NUM_REQUESTS requests..."

start=$(date +%s.%N)

# Fire off curl requests in the background
for i in $(seq 1 $NUM_REQUESTS); do
    curl -s "$URL" > /dev/null &
done

# Wait for all background processes to finish
wait

end=$(date +%s.%N)
runtime=$(echo "$end - $start" | bc -l)
echo "Sent $NUM_REQUESTS requests in $runtime seconds"

throughput=$(echo "$NUM_REQUESTS / $runtime" | bc -l)
echo "Throughput: $throughput requests/sec"
