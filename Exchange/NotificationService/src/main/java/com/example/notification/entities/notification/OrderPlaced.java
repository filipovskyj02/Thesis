package com.example.notification.entities.notification;

import com.example.notification.entities.interfaces.NotificationEvent;

public class OrderPlaced implements NotificationEvent {
    private final String orderId;
    private final long userId;
    private final String ticker;
    private final double limitPrice;
    private final long originalVolume;
    private final long timestampMs;

    public OrderPlaced(String orderId,
                       long userId,
                       String ticker,
                       double limitPrice,
                       long originalVolume,
                       long timestampMs) {
        this.orderId = orderId;
        this.userId = userId;
        this.ticker = ticker;
        this.limitPrice = limitPrice;
        this.originalVolume = originalVolume;
        this.timestampMs = timestampMs;
    }

    @Override
    public String getOrderId() {
        return orderId;
    }

    @Override
    public long getUserId() {
        return userId;
    }

    @Override
    public String getTicker() {
        return ticker;
    }

    public double getLimitPrice() {
        return limitPrice;
    }

    public long getOriginalVolume() {
        return originalVolume;
    }

    @Override
    public long getTimestampMs() {
        return timestampMs;
    }
}