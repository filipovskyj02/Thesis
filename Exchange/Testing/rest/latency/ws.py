import threading
import time
import json
import requests
import socket
import struct
from websocket import create_connection

# ─── Configuration ─────────────────────────────────────────────────────────────
USER_ID      = 123
HTTP_URL     = "http://localhost:8080/order/limit"
WS_URL       = "ws://localhost:8090/ws-notifications?userId=123"
MCAST_GRP    = "239.255.0.1"
MCAST_PORT   = 30001

# ─── Synchronization events & state ─────────────────────────────────────────────
ws_open       = threading.Event()
mcast_ready   = threading.Event()
msg_received  = threading.Event()

start_time      = None
ws_latency      = None
mcast_latency   = None

# ─── Multicast listener ─────────────────────────────────────────────────────────
def listen_multicast():
    global mcast_latency
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind(('', MCAST_PORT))  # listen on all interfaces
    
    # join the multicast group on all interfaces
    mreq = struct.pack('4s4s',
                       socket.inet_aton(MCAST_GRP),
                       socket.inet_aton('0.0.0.0'))
    sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

    mcast_ready.set()
    # wait for the first packet
    data, _ = sock.recvfrom(2048)
    elapsed = time.time() - start_time
    mcast_latency = elapsed
    print(f"▶ Multicast received: {data.decode().strip()}")
    sock.close()
    msg_received.set() if ws_latency is not None else None

# ─── WebSocket listener ─────────────────────────────────────────────────────────
def listen_ws():
    global ws_latency
    ws = create_connection(WS_URL)
    ws_open.set()
    while True:
        raw = ws.recv()
        elapsed = time.time() - start_time
        ws_latency = elapsed
        print(f"▶ WebSocket received: {raw}")
        ws.close()
        msg_received.set() if mcast_latency is not None else None
        break

# ─── Main test logic ────────────────────────────────────────────────────────────
if __name__ == "__main__":
    # Start listeners
    t_mcast = threading.Thread(target=listen_multicast, daemon=True)
    t_ws    = threading.Thread(target=listen_ws, daemon=True)
    t_mcast.start()
    t_ws.start()

    # Wait until both are ready
    if not mcast_ready.wait(timeout=5):
        raise RuntimeError("Multicast listener failed to join group")
    if not ws_open.wait(timeout=5):
        raise RuntimeError("WebSocket failed to open")

    # Now fire off one order
    payload = {
        "userId": USER_ID,
        "ticker": "AAPL",
        "side": 0,       # 0 = BUY
        "price": 150.0,
        "volume": 5
    }
    start_time = time.time()
    resp = requests.post(HTTP_URL, json=payload)
    resp.raise_for_status()
    order_id = resp.text.strip()
    print(f"Sent orderId: {order_id}")
    
    # Wait for both notifications (or timeout)
    if not msg_received.wait(timeout=5):
        print("Did not receive both notifications in time")
    else:
        print(f"Multicast latency: {mcast_latency*1000:.2f} ms")
        print(f"WebSocket latency: {ws_latency*1000:.2f} ms")
