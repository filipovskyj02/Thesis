package com.example.notification.controllers;

import com.example.notification.store.NotificationStore;
import com.example.notification.entities.interfaces.NotificationEvent;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import java.util.List;

@RestController
@RequestMapping("/notifications")
public class NotificationController {
    private final NotificationStore store;

    public NotificationController(NotificationStore store) {
        this.store = store;
    }

    @GetMapping
    public List<NotificationEvent> getNotifications(@RequestParam long userId) {
        return store.popAll(userId);
    }
}
