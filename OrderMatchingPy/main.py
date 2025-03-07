from Order import Order
from OrderType import OrderType
from Orderbook import OrderBook
from Side import Side
import time


def main():
    order_book = OrderBook()
    filename = "../data-gen/outputs/1000000-2000000-0.670000-0.950000-02-03-2025 16-02-34.txt"

    try:
        with open(filename, 'r') as file:
            first_line = file.readline().strip().split()
            initial_orders_count = int(first_line[0])
            orders_count = int(first_line[1])

            for _ in range(initial_orders_count):
                parts = file.readline().strip().split()
                order_id = int(parts[0])
                side = Side[parts[1]]
                order_type = OrderType[parts[2]]
                price = float(parts[3])
                volume = int(parts[4])

                order = Order(side, order_type, price, volume)
                order_book.place_order(order)

            start_time = time.time()

            for _ in range(orders_count):
                parts = file.readline().strip().split()
                order_id = int(parts[0])
                side = Side[parts[1]]
                order_type = OrderType[parts[2]]
                price = float(parts[3])
                volume = int(parts[4])

                if order_type == OrderType.CANCEL:
                    order_book.cancel_order_lazy(order_id)
                else:
                    if order_type == OrderType.MARKET:
                        order = Order(side, order_type, volume=volume)
                    else:
                        order = Order(side, order_type, price, volume)
                    order_book.place_order(order)

            end_time = time.time()
            print(f"{int((end_time - start_time) * 1000)}ms")

    except FileNotFoundError:
        print(f"File not found: {filename}")
    except Exception as e:
        print(f"Error: {e}")

    order_book.count_filled_count()


if __name__ == "__main__":
    main()