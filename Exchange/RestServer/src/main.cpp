#include <iostream>
#include "kafka/OrderSender.h"
#include <utility>
#include "crow.h"

#include <string>

#include "service/OrderService.h"


int main() {
    crow::SimpleApp app;

    OrderSender sender("orders");
    OrderService service(sender);

    CROW_ROUTE(app, "/order/limit").methods("POST"_method)
        ([&service](const crow::request& req) {
            return service.processLimit(req);
        });

    CROW_ROUTE(app, "/order/market").methods("POST"_method)
        ([&service](const crow::request& req) {
            return service.processMarket(req);
        });

    CROW_ROUTE(app, "/order/cancel").methods("POST"_method)
        ([&service](const crow::request& req) {
            return service.processCancel(req);
        });

    app.bindaddr("0.0.0.0")
        .port(8080)
        .multithreaded()
        .run();
}

