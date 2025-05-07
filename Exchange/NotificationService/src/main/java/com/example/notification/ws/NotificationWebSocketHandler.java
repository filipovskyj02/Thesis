package com.example.notification.ws;

import com.example.notification.entities.interfaces.NotificationEvent;
import com.fasterxml.jackson.databind.ObjectMapper;
import org.springframework.stereotype.Component;
import org.springframework.web.socket.CloseStatus;
import org.springframework.web.socket.TextMessage;
import org.springframework.web.socket.WebSocketSession;
import org.springframework.web.socket.handler.TextWebSocketHandler;

import java.util.Arrays;
import java.util.Collections;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;

@Component
public class NotificationWebSocketHandler extends TextWebSocketHandler {
    private final ObjectMapper mapper = new ObjectMapper();

    // userId → sessions
    private final Map<Long, Set<WebSocketSession>> sessions = new ConcurrentHashMap<>();

    // called by your KafkaConsumerService
    public void pushEvent(NotificationEvent evt) {
        String json;
        try {
            json = mapper.writeValueAsString(evt);
        } catch (Exception e) {
            return;
        }
        Set<WebSocketSession> userSessions = sessions.get(evt.getUserId());
        if (userSessions != null) {
            userSessions.forEach(sess -> {
                if (sess.isOpen()) {
                    try {
                        sess.sendMessage(new TextMessage(json));
                    } catch (Exception ignored) {}
                }
            });
        }
    }

    @Override
    public void afterConnectionEstablished(WebSocketSession session) {
        // expect ?userId=123 on the URL
        String query = session.getUri().getQuery();
        long userId = Arrays.stream(query.split("&"))
                .map(p -> p.split("=",2))
                .filter(kv -> kv[0].equals("userId"))
                .map(kv -> Long.parseLong(kv[1]))
                .findFirst().orElse(-1L);

        sessions
                .computeIfAbsent(userId, id -> Collections.newSetFromMap(new ConcurrentHashMap<>()))
                .add(session);
    }

    @Override
    public void afterConnectionClosed(WebSocketSession session, CloseStatus status) {
        sessions.values().forEach(set -> set.remove(session));
    }
}
