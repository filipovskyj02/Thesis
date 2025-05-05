package com.example.notification.entities.notification;

import com.example.notification.entities.interfaces.NotificationEvent;

// FILLED,orderId,userId,ticker,execPrice,totalVol,timestampMs
public class OrderFilled implements NotificationEvent {
    private final String orderId;
    private final long userId;
    private final String ticker;
    private final double execPrice;
    private final long totalVol;
    private final long timestampMs;

    public OrderFilled(String orderId,
                       long userId,
                       String ticker,
                       double execPrice,
                       long totalVol,
                       long timestampMs) {
        this.orderId = orderId;
        this.userId = userId;
        this.ticker = ticker;
        this.execPrice = execPrice;
        this.totalVol = totalVol;
        this.timestampMs = timestampMs;
    }

    @Override public String getOrderId()    { return orderId; }
    @Override public long   getUserId()     { return userId; }
    @Override public String getTicker()     { return ticker; }
    public double getExecPrice()            { return execPrice; }
    public long   getTotalVol()             { return totalVol; }
    @Override public long   getTimestampMs() { return timestampMs; }
}
