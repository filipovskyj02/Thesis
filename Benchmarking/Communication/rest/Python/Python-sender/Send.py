import requests
import time
from concurrent.futures import ThreadPoolExecutor, as_completed

URL = "http://localhost:8080/submit_order"
TOTAL_ORDERS = 10000
MAX_WORKERS = 3 

pre_serialized_order = (
    '{"id": 123, "timestamp": 1743877066453455, "side": "BUY", '
    '"order_type": "LIMIT", "price": 123.45, "volume": 10}'
)

def wait_for_server():
    print("Waiting for server to become ready...")
    deadline = time.time() + 10
    while time.time() < deadline:
        try:
            r = requests.post(URL, data="{}", headers={"Content-Type": "application/json"})
            if r.status_code in {200, 400}:
                print("Server is ready.")
                return True
        except Exception:
            pass
        time.sleep(0.2)
    print("Server not responding after timeout.")
    return False

def send_order(order_index):
    try:
        r = requests.post(URL, data=pre_serialized_order, headers={"Content-Type": "application/json"})
        return r.status_code
    except Exception as e:
        print(f"Order {order_index} exception: {e}")
        return None

if not wait_for_server():
    exit(1)

start = time.perf_counter()

with ThreadPoolExecutor(max_workers=MAX_WORKERS) as executor:
    futures = [executor.submit(send_order, i) for i in range(TOTAL_ORDERS)]
    for future in as_completed(futures):
        status = future.result()
        if status != 200:
            print(f"Request failed with status: {status}")

end = time.perf_counter()

duration = end - start
print(f"Sent {TOTAL_ORDERS} orders in {duration:.3f} seconds")
print(f"Throughput: {TOTAL_ORDERS / duration:.2f} orders/sec")
