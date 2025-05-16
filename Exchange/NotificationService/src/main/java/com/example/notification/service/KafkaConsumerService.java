package com.example.notification.service;

import com.example.notification.entities.interfaces.NotificationEvent;
import com.example.notification.entities.parsers.NotificationEventParser;
import com.example.notification.ws.NotificationWebSocketHandler;
import org.springframework.kafka.annotation.KafkaListener;
import org.springframework.stereotype.Service;

@Service
public class KafkaConsumerService {
    private final NotificationEventParser parser;
    private final NotificationWebSocketHandler wsHandler;

    public KafkaConsumerService(NotificationEventParser parser, NotificationWebSocketHandler wsHandler) {
        this.parser = parser;
        this.wsHandler = wsHandler;
    }

    @KafkaListener(topics = "${app.kafka.topic.notifications}")
    public void consume(String csvLine) {
        //System.out.println(csvLine);
        NotificationEvent e = parser.parse(csvLine.trim());
        wsHandler.pushEvent(e);
    }
}
