package com.example.notification.store;

import com.example.notification.entities.interfaces.NotificationEvent;
import org.springframework.stereotype.Component;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Queue;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.ConcurrentMap;

@Component
public class NotificationStore {
    private final ConcurrentMap<Long, Queue<NotificationEvent>> map = new ConcurrentHashMap<>();

    public void push(NotificationEvent e) {
        map.computeIfAbsent(e.getUserId(), id -> new ConcurrentLinkedQueue<>())
                .add(e);
    }

    public List<NotificationEvent> popAll(long userId) {
        Queue<NotificationEvent> q = map.remove(userId);
        if (q == null) return Collections.emptyList();
        List<NotificationEvent> list = new ArrayList<>(q);
        q.clear();
        return list;
    }
}