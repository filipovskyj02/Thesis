#include <iostream>
#include <thread>

#include "Order.h"
#include "OrderBook.h"

// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or
// click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
int main() {
    auto orderBook = std::make_unique<OrderBook>();

    long long total = 0;
    OrderId orderId = 0;
    const int POPULATED_SIZE = 30000;
    for (int i = 0; i < POPULATED_SIZE; i++) {
        auto order = std::make_shared<Order>(MARKET, BUY, 94 - rand() % 90, rand() % 100);
        auto order2 = std::make_shared<Order>(MARKET, SELL, 220 + rand() % 90, rand() % 100);
        orderBook->placeOrder(order);
        orderBook->placeOrder(order2);
        orderId+= 2;
    }

    auto start = std::chrono::steady_clock::now();

    while (true) {
        total++;
        if (rand() % 10 == 0) {
            orderBook->cancelOrder(orderBook->getOrder(rand() % orderId));
        }
        else {
            auto order = std::make_shared<Order>(MARKET, Side(rand() % 2), 95 + rand() % 110, rand() % 100);
            orderBook->placeOrder(order);
            orderId++;
        }

        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start);
        if (elapsed.count() >= 1) {
            break;

        }
    }

    std::cout << "Loop exited after 1 second.\n " << (orderBook->orderCount - (POPULATED_SIZE * 2))<< " canceled: " << orderBook->canceledOrderCount  << " canceledFull " << orderBook->canceledFullfiledOrderCount<< std::endl;
    return 0;
}

// TIP See CLion help at <a
// href="https://www.jetbrains.com/help/clion/">jetbrains.com/help/clion/</a>.
//  Also, you can try interactive lessons for CLion by selecting
//  'Help | Learn IDE Features' from the main menu.