package com.example.notification.service;

import com.example.notification.entities.interfaces.NotificationEvent;
import com.example.notification.entities.parsers.NotificationEventParser;
import com.example.notification.store.NotificationStore;
import org.springframework.kafka.annotation.KafkaListener;
import org.springframework.stereotype.Service;

@Service
public class KafkaConsumerService {
    private final NotificationStore store;
    private final NotificationEventParser parser;

    public KafkaConsumerService(NotificationStore store, NotificationEventParser parser) {
        this.store = store;
        this.parser = parser;
    }

    @KafkaListener(topics = "${app.kafka.topic.notifications}")
    public void consume(String csvLine) {
        System.out.println(csvLine);
        NotificationEvent e = parser.parse(csvLine.trim());
        store.push(e);
    }
}
