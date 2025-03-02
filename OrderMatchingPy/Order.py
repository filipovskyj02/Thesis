from datetime import datetime

from OrderType import OrderType
from Side import Side


class Order:
    current_id_sequence = 0

    def __init__(self, side: Side, order_type: OrderType, price: float = 0, volume: int = 0):
        self.order_id = self.get_current_id_sequence()
        self.side = side
        self.order_type = order_type
        self.price = price
        self.volume = volume
        self.filled_volume = 0
        self.datetime = datetime.now()
        self.is_cancelled = False

    @classmethod
    def get_current_id_sequence(cls):
        id_sequence = cls.current_id_sequence
        cls.current_id_sequence += 1
        return id_sequence

    def get_order_id(self):
        return self.order_id

    def get_order_type(self):
        return self.order_type

    def get_side(self):
        return self.side

    def get_original_volume(self):
        return self.volume

    def get_datetime(self):
        return self.datetime

    def is_cancelled(self):
        return self.is_cancelled

    def set_cancelled(self, cancelled: bool):
        self.is_cancelled = cancelled

    def get_filled_volume(self):
        return self.filled_volume

    def set_filled_volume(self, filled_volume: int):
        self.filled_volume = filled_volume

    def get_remaining_volume(self):
        return self.volume - self.filled_volume

    def get_price(self):
        return self.price

    def __lt__(self, other):
        # Required for Python's PriorityQueue comparison
        return self.price < other.price
