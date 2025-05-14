import http from 'k6/http';

export const options = {
  iterations: 1000000,
  vus: 10,
};

function randomTicker() {
  const tickers = ["AAPL", "GOOG"];
  return tickers[Math.floor(Math.random() * tickers.length)];
}

export default function () {
  const side = (__ITER % 2 === 0) ? 0 : 1;  // 0=BUY, 1=SELL
  const volume = (__ITER % 5) + 1;
  const ticker = randomTicker();

  let payload, url;

  url = "http://localhost:8080/order/limit";
      payload = JSON.stringify({
        userId: 123,
        ticker: ticker,
        side: side,
        price: 123.45 + (__ITER % 10),
        volume: volume
      });

 

  const params = {
    headers: { 'Content-Type': 'application/json' }
  };

  http.post(url, payload, params);
}
