<div align="center">

# 🌡️ IoT Temperature Monitor

**Real-time temperature sensing, local LCD display, and live web dashboard via MQTT**

[![Arduino](https://img.shields.io/badge/Arduino-Uno-00979D?style=for-the-badge&logo=arduino&logoColor=white)](https://www.arduino.cc/)
[![Python](https://img.shields.io/badge/Python-3.8+-3776AB?style=for-the-badge&logo=python&logoColor=white)](https://www.python.org/)
[![Flask](https://img.shields.io/badge/Flask-SocketIO-000000?style=for-the-badge&logo=flask&logoColor=white)](https://flask-socketio.readthedocs.io/)
[![MQTT](https://img.shields.io/badge/MQTT-HiveMQ-660066?style=for-the-badge&logo=mqtt&logoColor=white)](https://www.hivemq.com/)
[![License](https://img.shields.io/badge/License-MIT-22c55e?style=for-the-badge)](LICENSE)

*made with ♥ by Don Durkheim*

</div>

---

## Overview

An end-to-end IoT pipeline: a DHT11 sensor on an Arduino Uno reads temperature every 2 seconds, shows it on a 16×2 I²C LCD, streams it over USB serial to a Python client, which publishes to MQTT. A Flask + SocketIO dashboard on a VPS subscribes and pushes readings live to a browser chart.

---

## System Architecture

```mermaid
flowchart LR
    subgraph hw["🔧 Hardware"]
        DHT11["DHT11\nSensor"]
        UNO["Arduino Uno"]
        LCD["16×2 I²C LCD\n0x27"]
    end

    subgraph pc["💻 PC Client"]
        SERIAL["Serial Reader\n9600 baud"]
        PARSER["TEMP: Parser"]
        PUB["paho-mqtt Publisher"]
    end

    subgraph cloud["☁️ Cloud"]
        BROKER["HiveMQ\nbroker.hivemq.com:1883"]
        TOPIC["student/sensor/temperature\n/dondurkheim"]
    end

    subgraph dashboard["🖥️ VPS Dashboard"]
        FLASK["Flask + SocketIO\nport 24500"]
        UI["Browser\nLive Chart"]
    end

    DHT11 -->|"GPIO 2 / 2s"| UNO
    UNO -->|"I²C SDA/SCL"| LCD
    UNO -->|"USB Serial\nTEMP:xx.xx"| SERIAL
    SERIAL --> PARSER --> PUB
    PUB -->|"MQTT TCP 1883"| BROKER
    BROKER --> TOPIC
    TOPIC -->|"MQTT subscribe"| FLASK
    FLASK -->|"WebSocket"| UI
```

---

## Data Flow

```mermaid
sequenceDiagram
    participant DHT11
    participant Arduino
    participant LCD
    participant PC as PC Client
    participant Broker as HiveMQ
    participant Flask
    participant Browser

    loop Every 2 seconds
        Arduino->>DHT11: readTemperature()
        DHT11-->>Arduino: float °C
        Arduino->>LCD: row 1 → "Temp: xx.xx C"
        Arduino->>PC: Serial "TEMP:xx.xx"
        PC->>Broker: publish(topic, value)
        Broker->>Flask: on_message callback
        Flask->>Browser: emit("temperature_update")
        Browser->>Browser: update chart + cards
    end

    loop Continuously
        Arduino->>LCD: scroll candidate name (row 0)
    end
```

---

## Project Structure

```
.
├── arduino/
│   └── temp_display.ino       # Sensor + LCD + serial output
├── pc_client/
│   └── pc_client.py           # Serial reader + MQTT publisher
├── dashboard/
│   ├── app.py                 # Flask + SocketIO + MQTT subscriber
│   └── templates/
│       └── index.html         # Live dashboard UI
└── README.md
```

---

## Hardware

| Component | Detail |
|-----------|--------|
| Microcontroller | Arduino Uno |
| Sensor | DHT11 — data on **GPIO 2** |
| Display | 16×2 LCD, I²C address `0x27` |
| Bus | USB Serial @ 9600 baud |

### Wiring

```
DHT11  DATA → Arduino D2      LCD SDA → Arduino A4
DHT11  VCC  → 5V              LCD SCL → Arduino A5
DHT11  GND  → GND             LCD VCC → 5V
                               LCD GND → GND
```

---

## Getting Started

### 1 — Arduino

Install via Arduino Library Manager: `DHT sensor library`, `LiquidCrystal_I2C`, `Wire`.

Flash `arduino/temp_display.ino`. Update the name if needed:

```cpp
String candidateName = "Cyubahiro Don Durkheim";
```

### 2 — PC Client

```bash
pip install pyserial paho-mqtt
python pc_client/pc_client.py
```

Auto-detects the Arduino port. To pin a specific port:

```python
COM_PORT = "/dev/ttyACM0"  # Linux
COM_PORT = "COM3"           # Windows
```

### 3 — Dashboard (VPS)

```bash
pip install flask flask-socketio eventlet paho-mqtt
python dashboard/app.py
```

Keep it running after disconnect:

```bash
nohup python3 dashboard/app.py > dashboard.log 2>&1 &
```

Then open **`http://157.173.101.159:24500`** in a browser.

---

## Configuration

| Variable | Default | Description |
|----------|---------|-------------|
| `COM_PORT` | `None` | Serial port — `None` = auto-detect |
| `BAUD_RATE` | `9600` | Must match Arduino sketch |
| `MQTT_BROKER` | `broker.hivemq.com` | HiveMQ public broker |
| `MQTT_PORT` | `1883` | Standard MQTT port |
| `MQTT_TOPIC` | `student/sensor/temperature/dondurkheim` | Unique per-student topic |
| Dashboard port | `24500` | Flask app port on VPS |

---

## Serial Protocol

```
TEMP:25.60
```

Arduino emits one line per reading. The PC client filters for `TEMP:` prefix, strips it, and publishes the numeric value to MQTT. Any other serial output is printed as debug.

---

## LCD Layout

```
┌────────────────┐
│ Cyubahiro Don  │  ← candidate name (scrolls every 300ms if > 16 chars)
│ Temp: 25.60 C  │  ← live temperature updated every 2s
└────────────────┘
```

---

## License

MIT © Cyubahiro Don Durkheim
