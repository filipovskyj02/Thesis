import java.util.Date;

public class Order implements Comparable<Order> {
    private long orderId;
    private Side side;
    private OrderType orderType;
    private long volume;
    private Date datetime;
    private boolean isCancelled;
    private long filledVolume;
    private double price;

    long getOrderId() {
        return orderId;
    }
    OrderType getOrderType() {
        return orderType;
    }
    Side getSide() {
        return side;
    }
    long getOriginalVolume() {
        return volume;
    }
    Date getDatetime() {
        return datetime;
    }
    boolean isCancelled() {
        return isCancelled;
    }
    void setCancelled(boolean cancelled) {
        isCancelled = cancelled;
    }
    long getFilledVolume() {
        return filledVolume;
    }
    void setFilledVolume(long filledVolume) {
        this.filledVolume = filledVolume;
    }
    long getRemainingVolume() {
        return volume - filledVolume;
    }
    double getPrice() {
        return price;
    }


    Order(OrderType orderType, Side side, double price, long volume) {
        this.orderId = getCurrentIdSequence();
        this.orderType = orderType;
        this.side = side;
        this.price = price;
        this.volume = volume;
        this.filledVolume = 0;
        this.datetime = new Date();
        this.isCancelled = false;
    }

    Order(OrderType orderType, Side side, long volume) {
        this.orderId = getCurrentIdSequence();
        this.orderType = orderType;
        this.side = side;
        this.price = 0;
        this.volume = volume;
        this.filledVolume = 0;
        this.datetime = new Date();
        this.isCancelled = false;
    }
    long getCurrentIdSequence() {
        return currentIdSequence++;
    }
    static long currentIdSequence = 1;

    @Override
    public int compareTo(Order o) {
        return Double.compare(o.price, price);
    }
}
