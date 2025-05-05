package com.example.notification.entities.notification;

import com.example.notification.entities.interfaces.NotificationEvent;

// REJECTED,orderId,userId,ticker,reasonCode,timestampMs
public class OrderRejected implements NotificationEvent {
    private final String orderId;
    private final long userId;
    private final String ticker;
    private final String reason;
    private final long timestampMs;

    public OrderRejected(String orderId,
                         long userId,
                         String ticker,
                         String reason,
                         long timestampMs) {
        this.orderId = orderId;
        this.userId = userId;
        this.ticker = ticker;
        this.reason = reason;
        this.timestampMs = timestampMs;
    }

    @Override public String getOrderId()     { return orderId; }
    @Override public long   getUserId()      { return userId; }
    @Override public String getTicker()      { return ticker; }
    public String getReason()                { return reason; }
    @Override public long   getTimestampMs()  { return timestampMs; }
}
