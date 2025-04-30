import java.io.*;


public class Main {
    public static void main(String[] args) {
        OrderBook orderBook = new OrderBook();
        String filename = "../../DataGen/outputs/30000-500000-0.200000-0-02-03-2025 17-00-43.txt";

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

                Order order = new Order(side, type, price, volume);
                orderBook.placeOrder(order);
            }

            long startTime = System.currentTimeMillis();

            for (int i = 0; i < ordersCount; i++) {
                String[] parts = reader.readLine().split(" ");
                long orderId = Long.parseLong(parts[0]);
                Side side = Side.valueOf(parts[1]);
                OrderType type = OrderType.valueOf(parts[2]);
                double price = Double.parseDouble(parts[3]);
                long volume = Long.parseLong(parts[4]);


                if (type.equals(OrderType.CANCEL)) {
                    orderBook.cancelOrderLazy(orderId);
                } else {
                    if (type == OrderType.MARKET) {
                        Order order = new Order(side, type, volume);
                        orderBook.placeOrder(order);
                    } else {
                        Order order = new Order(side, type, price, volume);
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