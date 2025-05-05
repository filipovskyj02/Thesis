package com.example.notification.entities.notification;

import com.example.notification.entities.interfaces.NotificationEvent;

// PARTIAL,orderId,userId,ticker,execPrice,filledVol,cumFilled,remaining,timestampMs
public class OrderPartialFill implements NotificationEvent {
    private final String orderId;
    private final long userId;
    private final String ticker;
    private final double execPrice;
    private final long filledVol;
    private final long cumFilled;
    private final long remaining;
    private final long timestampMs;

    public OrderPartialFill(String orderId,
                            long userId,
                            String ticker,
                            double execPrice,
                            long filledVol,
                            long cumFilled,
                            long remaining,
                            long timestampMs) {
        this.orderId = orderId;
        this.userId = userId;
        this.ticker = ticker;
        this.execPrice = execPrice;
        this.filledVol = filledVol;
        this.cumFilled = cumFilled;
        this.remaining = remaining;
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

    public double getExecPrice() {
        return execPrice;
    }

    public long getFilledVol() {
        return filledVol;
    }

    public long getCumFilled() {
        return cumFilled;
    }

    public long getRemaining() {
        return remaining;
    }

    @Override
    public long getTimestampMs() {
        return timestampMs;
    }
}