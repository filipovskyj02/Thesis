import java.util.*;

import static java.lang.Math.min;

public class OrderBook {
    private PriorityQueue<Order> bids;
    private PriorityQueue<Order> asks;
    private List<Order> orders;
    private long orderCount = 0;

    public OrderBook() {
        this.bids = new PriorityQueue<>(Comparator.comparingDouble(Order::getPrice).reversed()); // Max-heap for buys
        this.asks = new PriorityQueue<>(Comparator.comparingDouble(Order::getPrice)); // Min-heap for sells
        this.orders = new ArrayList<>();
    }

    public void placeOrder(Order order) {
        storeOrder(order);
        if (order.getSide() == Side.BUY) {
            executeBuy(order);
        }
        else if (order.getSide() == Side.SELL) {
            executeSell(order);
        }
        else {
            throw new IllegalArgumentException("Invalid order side: " + order.getSide());
        }

    }

    public void cancelOrderLazy(long orderId) {
        getOrder(orderId).setCancelled(true);
    }

    public void countFilledCount() {
        long count = 0;
        long totalVolume = 0;
        for (Order order : orders) {
            if (order.getFilledVolume() == order.getOriginalVolume()) {
                totalVolume += order.getFilledVolume();
                count++;
            }
        }
        System.out.println("Total filled volume: " + totalVolume + " by " + count + " orders");
    }

    public Order getOrder(long id) {
        return (id >= 0 && id < orderCount) ? orders.get((int) id) : null;
    }

    private void storeOrder(Order order) {
        orders.add(order);
        orderCount++;
    }

    private void executeBuy(Order order) {
        while(!asks.isEmpty() && order.getRemainingVolume() > 0) {
            while(!asks.isEmpty() && asks.peek().isCancelled()) {
                asks.poll();
            }
            if (asks.isEmpty()) {
                break;
            }
            Order topAsk = asks.peek();
            if (order.getOrderType() == OrderType.LIMIT && order.getPrice() < topAsk.getPrice()) {
                break;
            }
            long matchedVolume = min(order.getRemainingVolume(), topAsk.getRemainingVolume());
            order.setFilledVolume(order.getFilledVolume() + matchedVolume);
            topAsk.setFilledVolume(topAsk.getFilledVolume() + matchedVolume);
            //System.out.println(topAsk.getPrice());
            if (topAsk.getRemainingVolume() == 0) {
                asks.poll();
            }
        }
        if (order.getOrderType() == OrderType.LIMIT && order.getRemainingVolume() > 0) {
            bids.add(order);
        }
    }

    private void executeSell(Order order) {
        while(!bids.isEmpty() && order.getRemainingVolume() > 0) {
            while (!bids.isEmpty() && bids.peek().isCancelled()) {
                bids.poll();
            }
            if (bids.isEmpty()) {
                break;
            }
            Order topBid = bids.peek();
            if (order.getOrderType() == OrderType.LIMIT && order.getPrice() > topBid.getPrice()) {
                break;
            }
            long matchedVolume = min(order.getRemainingVolume(), topBid.getRemainingVolume());
            order.setFilledVolume(order.getFilledVolume() + matchedVolume);
            topBid.setFilledVolume(topBid.getFilledVolume() + matchedVolume);
            //System.out.println(topBid.getPrice());
            if (topBid.getRemainingVolume() == 0) {
                bids.poll();
            }
        }
        if (order.getOrderType() == OrderType.LIMIT && order.getRemainingVolume() > 0) {
            asks.add(order);
        }
    }
}