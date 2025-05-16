import http from 'k6/http';
import { check } from 'k6';
import { randomItem, randomIntBetween } from 'https://jslib.k6.io/k6-utils/1.2.0/index.js';

export const options = {
  vus: 10,
  iterations: 20000,
};

const baseUrl = 'http://localhost:8080';
const endpoints = {
  limit:  '/order/limit',
  market: '/order/market',
  cancel: '/order/cancel',
};

const tickers = ['AAPL','GOOG'];

// Each VU gets its own order history
let orderHistory = [];

export default function () {
  const params = { headers: { 'Content-Type': 'application/json' } };
  const r = Math.random();

  // 50% limit, 30% market, 20% cancel
  if (r < 0.5) {
    // LIMIT
    const ticker = randomItem(tickers);
    const payload = JSON.stringify({
      userId:  randomIntBetween(1,1000),
      ticker:  ticker,
      side:    Math.random() < 0.5 ? 0 : 1,
      price:   100 + Math.random() * 50,
      volume:  randomIntBetween(1,10),
    });

    const res = http.post(`${baseUrl}${endpoints.limit}`, payload, params);
    check(res, { 'limit→200': r => r.status === 200 });

    if (res.status === 200) {
      const orderId = res.body.trim();
      orderHistory.push({ orderId, ticker });
     
    }


  } else if (r < 0.8) {
    // MARKET
    const ticker = randomItem(tickers);
    const payload = JSON.stringify({
      userId: 123,
      ticker: ticker,
      side:   Math.random() < 0.5 ? 0 : 1,
      volume: randomIntBetween(1,10),
    });

    const res = http.post(`${baseUrl}${endpoints.market}`, payload, params);
    check(res, { 'market→200': r => r.status === 200 });


  } else {
    // CANCEL
    if (orderHistory.length === 0) {
      // nothing to cancel yet—skip
      return;
    }
    // pick and remove a random past order
    const idx = randomIntBetween(0, orderHistory.length - 1);
    const { orderId, ticker } = orderHistory[idx];
    orderHistory.splice(idx, 1);

    const payload = JSON.stringify({
      userId:         123,
      ticker:         ticker,
      cancelTargetId: orderId,
    });

    const res = http.post(`${baseUrl}${endpoints.cancel}`, payload, params);
    check(res, { 'cancel→200': r => r.status === 200 });
  }
}
