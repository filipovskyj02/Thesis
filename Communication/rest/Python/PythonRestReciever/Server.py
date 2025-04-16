from flask import Flask, request, jsonify

app = Flask(__name__)

@app.route('/submit_order', methods=['POST'])
def submit_order():
    try:
        order = request.get_json(force=True)
        print(f"Order ID: {order.get('id')} | Side: {order.get('side')}")
        return "OK", 200
    except Exception as e:
        return f"Invalid request: {e}", 400

if __name__ == "__main__":
    print("Listening on http://localhost:8080/submit")
    app.run(host='0.0.0.0', port=8080, threaded=False)

