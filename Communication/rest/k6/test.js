import http from 'k6/http';

export const options = {
  iterations: 100000,
};

// The default exported function is gonna be picked up by k6 as the entry point for the test script. It will be executed repeatedly in "iterations" for the whole duration of the test.
export default function () {
    const url = "http://localhost:8080/submit_order";
    const payload = JSON.stringify({
        id: __ITER,
        timestamp: Date.now(),
        side: (__ITER % 2 === 0) ? "BUY" : "SELL",
        order_type: (__ITER % 3 === 0) ? "MARKET" : "LIMIT",
        price: 100.0 + (__ITER % 10),
        volume: (__ITER % 5) + 1
      });
  
    const params = {
      headers: {
        'Content-Type': 'application/json',
      },
    };
  
    http.post(url, payload, params);
  }