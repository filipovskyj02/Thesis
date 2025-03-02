import heapq
from typing import Optional

from Order import Order
from OrderType import OrderType
from Side import Side


class OrderBook:
    def __init__(self):
        self.bids = []
        self.asks = []
        self.orders = []
        self.order_count = 0

    def place_order(self, order: Order):
        self.store_order(order)
        if order.get_side() == Side.BUY:
            self.execute_buy(order)
        elif order.get_side() == Side.SELL:
            self.execute_sell(order)
        else:
            raise ValueError(f"Invalid order side: {order.get_side()}")

    def cancel_order_lazy(self, order_id: int):
        order = self.get_order(order_id)
        if order:
            order.set_cancelled(True)

    def count_filled_count(self):
        count = 0
        total_volume = 0
        for order in self.orders:
            if order.get_filled_volume() == order.get_original_volume():
                total_volume += order.get_filled_volume()
                count += 1
        print(f"Total filled volume: {total_volume} by {count} orders")

    def get_order(self, order_id: int) -> Optional[Order]:
        if 0 <= order_id < self.order_count:
            return self.orders[order_id]
        return None

    def store_order(self, order: Order):
        self.orders.append(order)
        self.order_count += 1

    def execute_buy(self, order: Order):
        while self.asks and order.get_remaining_volume() > 0:
            while self.asks and self.asks[0].is_cancelled:
                heapq.heappop(self.asks)

            if not self.asks:
                break

            top_ask = self.asks[0]

            if (order.get_order_type() == OrderType.LIMIT and
                    order.get_price() < top_ask.get_price()):
                break

            matched_volume = min(order.get_remaining_volume(), top_ask.get_remaining_volume())
            order.set_filled_volume(order.get_filled_volume() + matched_volume)
            top_ask.set_filled_volume(top_ask.get_filled_volume() + matched_volume)

            if top_ask.get_remaining_volume() == 0:
                heapq.heappop(self.asks)

        if order.get_order_type() == OrderType.LIMIT and order.get_remaining_volume() > 0:
            order_for_heap = order
            order_for_heap.price = -order.get_price()  # Negate for max-heap https://stackoverflow.com/questions/2501457/what-do-i-use-for-a-max-heap-implementation-in-python
            heapq.heappush(self.bids, order_for_heap)

    def execute_sell(self, order: Order):
        while self.bids and order.get_remaining_volume() > 0:
            while self.bids and self.bids[0].is_cancelled:
                heapq.heappop(self.bids)

            if not self.bids:
                break

            top_bid = self.bids[0]

            if (order.get_order_type() == OrderType.LIMIT and
                    order.get_price() > -top_bid.get_price()):
                break

            matched_volume = min(order.get_remaining_volume(), top_bid.get_remaining_volume())
            order.set_filled_volume(order.get_filled_volume() + matched_volume)
            top_bid.set_filled_volume(top_bid.get_filled_volume() + matched_volume)

            if top_bid.get_remaining_volume() == 0:
                heapq.heappop(self.bids)

        if order.get_order_type() == OrderType.LIMIT and order.get_remaining_volume() > 0:
            heapq.heappush(self.asks, order)
