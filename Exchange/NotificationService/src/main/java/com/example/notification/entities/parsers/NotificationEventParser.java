package com.example.notification.entities.parsers;

import com.example.notification.entities.interfaces.NotificationEvent;
import com.example.notification.entities.notification.*;
import org.springframework.stereotype.Component;

@Component
public class NotificationEventParser {

    public NotificationEvent parse(String line) {

        // split into exactly the parts your C++ toString created
        String[] p = line.split(",", -1);
        switch (p[0]) {
            case "PLACED":
                // PLACED,orderId,userId,ticker,limitPrice,originalVolume,timestampMs
                return new OrderPlaced(
                        p[1],
                        Long.parseLong(p[2]),
                        p[3],
                        Double.parseDouble(p[4]),
                        Long.parseLong(p[5]),
                        Long.parseLong(p[6])
                );
            case "PARTIAL":
                // PARTIAL,orderId,userId,ticker,execPrice,filledVol,cumFilled,remaining,timestampMs
                return new OrderPartialFill(
                        p[1],
                        Long.parseLong(p[2]),
                        p[3],
                        Double.parseDouble(p[4]),
                        Long.parseLong(p[5]),
                        Long.parseLong(p[6]),
                        Long.parseLong(p[7]),
                        Long.parseLong(p[8])
                );
            case "FILLED":
                // FILLED,orderId,userId,ticker,execPrice,totalVol,timestampMs
                return new OrderFilled(
                        p[1],
                        Long.parseLong(p[2]),
                        p[3],
                        Double.parseDouble(p[4]),
                        Long.parseLong(p[5]),
                        Long.parseLong(p[6])
                );
            case "CANCELED":
                // CANCELED,orderId,userId,ticker,canceledRem,timestampMs
                return new OrderCanceled(
                        p[1],
                        Long.parseLong(p[2]),
                        p[3],
                        Long.parseLong(p[4]),
                        Long.parseLong(p[5])
                );
            case "REJECTED":
                // REJECTED,orderId,userId,ticker,reasonCode,timestampMs
                return new OrderRejected(
                        p[1],
                        Long.parseLong(p[2]),
                        p[3],
                        p[4],
                        Long.parseLong(p[5])
                );
            default:
                throw new IllegalArgumentException("Unsupported event type: " + p[0]);
        }
    }
}
