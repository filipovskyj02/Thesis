package com.example.notification.entities.notification;

import com.example.notification.entities.interfaces.NotificationEvent;

// CANCELED,orderId,userId,ticker,canceledRem,timestampMs
public class OrderCanceled implements NotificationEvent {
    private final String orderId;
    private final long userId;
    private final String ticker;
    private final long canceledRem;
    private final long timestampMs;

    public OrderCanceled(String orderId,
                         long userId,
                         String ticker,
                         long canceledRem,
                         long timestampMs) {
        this.orderId = orderId;
        this.userId = userId;
        this.ticker = ticker;
        this.canceledRem = canceledRem;
        this.timestampMs = timestampMs;
    }

    @Override public String getOrderId()     { return orderId; }
    @Override public long   getUserId()      { return userId; }
    @Override public String getTicker()      { return ticker; }
    public long   getCanceledRem()           { return canceledRem; }
    @Override public long   getTimestampMs()  { return timestampMs; }
}
