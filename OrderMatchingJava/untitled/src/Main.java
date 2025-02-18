import java.io.*;
import java.util.*;
import java.util.concurrent.TimeUnit;

public class Main {
    public static void main(String[] args) {
        OrderBook orderBook = new OrderBook();
        String filename = "30000-1000000-21-47-16-02-2025.txt";

        try (BufferedReader reader = new BufferedReader(new FileReader(filename))) {
            String[] firstLine = reader.readLine().split(" ");
            int initialOrdersCount = Integer.parseInt(firstLine[0]);
            int ordersCount = Integer.parseInt(firstLine[1]);

            // Read initial orders
            for (int i = 0; i < initialOrdersCount; i++) {
                String[] parts = reader.readLine().split(" ");
                long orderId = Long.parseLong(parts[0]);
                Side side = Side.valueOf(parts[1]);
                OrderType type = OrderType.valueOf(parts[2]);
                double price = Double.parseDouble(parts[3]);
                long volume = Long.parseLong(parts[4]);

                Order order = new Order(type, side, price, volume);
                orderBook.placeOrder(order);
            }

            long startTime = System.currentTimeMillis();

            // Process incoming orders
            for (int i = 0; i < ordersCount; i++) {
                String[] parts = reader.readLine().split(" ");
                long orderId = Long.parseLong(parts[0]);
                String sideString = parts[1];

                if (sideString.equals("CANCEL")) {
                    orderBook.cancelOrderLazy(orderId);
                } else {
                    Side side = Side.valueOf(sideString);
                    OrderType type = OrderType.valueOf(parts[2]);

                    if (type == OrderType.MARKET) {
                        long volume = Long.parseLong(parts[3]);
                        Order order = new Order(type, side, volume);
                        orderBook.placeOrder(order);
                    } else {
                        double price = Double.parseDouble(parts[3]);
                        long volume = Long.parseLong(parts[4]);
                        Order order = new Order(type, side, price, volume);
                        orderBook.placeOrder(order);
                    }
                }

            }

            long endTime = System.currentTimeMillis();
            System.out.println((endTime - startTime) + "ms");

        } catch (IOException e) {
            e.printStackTrace();
        }
        orderBook.countFilledCount();
    }
}