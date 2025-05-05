package com.example.notification.entities.interfaces;

public interface NotificationEvent {
    String getOrderId();
    long getUserId();
    String getTicker();
    long getTimestampMs();
}
