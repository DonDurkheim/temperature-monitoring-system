import eventlet
eventlet.monkey_patch()

from flask import Flask, render_template
from flask_socketio import SocketIO
import paho.mqtt.client as mqtt

app = Flask(__name__)
socketio = SocketIO(app, cors_allowed_origins="*", async_mode="eventlet")

MQTT_BROKER = "broker.hivemq.com"
MQTT_PORT = 1883
MQTT_TOPIC = "student/sensor/temperature/dondurkheim"

latest_temp = {"value": 0}

def on_connect(client, userdata, flags, rc, properties=None):
    print("Connected to MQTT broker")
    client.subscribe(MQTT_TOPIC)

def on_message(client, userdata, msg):
    global latest_temp
    try:
        temp = float(msg.payload.decode())
        latest_temp["value"] = temp
        print("Temp received:", temp)
        socketio.emit("temperature_update", {"temp": temp})
    except Exception as e:
        print("Parse error:", e)

mqtt_client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
mqtt_client.on_connect = on_connect
mqtt_client.on_message = on_message
mqtt_client.connect(MQTT_BROKER, MQTT_PORT, 60)
mqtt_client.loop_start()

@app.route("/")
def index():
    return render_template("index.html")

if __name__ == "__main__":
    socketio.run(app, host="0.0.0.0", port=9265)
