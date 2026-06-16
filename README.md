<div align="center">

# 🌡️ IoT Temperature Monitor

**Real-time temperature sensing, local display, and MQTT telemetry over the internet.**

[![Arduino](https://img.shields.io/badge/Arduino-Uno-00979D?style=for-the-badge&logo=arduino&logoColor=white)](https://www.arduino.cc/)
[![Python](https://img.shields.io/badge/Python-3.8+-3776AB?style=for-the-badge&logo=python&logoColor=white)](https://www.python.org/)
[![MQTT](https://img.shields.io/badge/MQTT-HiveMQ-660066?style=for-the-badge&logo=mqtt&logoColor=white)](https://www.hivemq.com/)
[![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20Windows-lightgrey?style=for-the-badge)]()

</div>

---

## Overview

An end-to-end embedded IoT system that reads ambient temperature from a DHT11 sensor, displays the candidate name and live temperature on a 16×2 I²C LCD, streams readings over USB Serial to a PC client, and publishes them to an MQTT broker for remote consumption.

---

## System Architecture

```mermaid
flowchart LR
    subgraph Hardware["🔧 Hardware Layer"]
        DHT11["DHT11\nSensor"]
        UNO["Arduino Uno"]
        LCD["16×2 I²C LCD\n0x27"]
    end

    subgraph PC["💻 PC Client (Python)"]
        SERIAL["Serial Reader\n9600 baud"]
        PARSER["TEMP: Parser"]
        PUB["paho-mqtt\nPublisher"]
    end

    subgraph Cloud["☁️ Cloud / Broker"]
        BROKER["HiveMQ\nbroker.hivemq.com:1883"]
        TOPIC["Topic:\nstudent/sensor/temperature"]
    end

    subgraph Consumers["📡 Subscribers"]
        SUB["Any MQTT\nSubscriber"]
    end

    DHT11 -->|"GPIO 2 / every 2s"| UNO
    UNO -->|"I²C (SDA/SCL)"| LCD
    UNO -->|"USB Serial\nTEMP:xx.xx"| SERIAL
    SERIAL --> PARSER
    PARSER --> PUB
    PUB -->|"TCP Port 1883"| BROKER
    BROKER --> TOPIC
    TOPIC --> SUB
```

---

## Data Flow

```mermaid
sequenceDiagram
    participant DHT11
    participant Arduino
    participant LCD
    participant PC as PC Client
    participant Broker as MQTT Broker
    participant Sub as Subscriber

    loop Every 2 seconds
        Arduino->>DHT11: readTemperature()
        DHT11-->>Arduino: float (°C)
        Arduino->>LCD: setCursor(0,1) + print "Temp: xx.xx C"
        Arduino->>PC: Serial "TEMP:xx.xx\n"
    end

    loop Continuously
        Arduino->>LCD: scroll candidate name (row 0)
    end

    PC->>Broker: connect(broker.hivemq.com, 1883)
    loop On each TEMP: line
        PC->>PC: parse temp_value
        PC->>Broker: publish("student/sensor/temperature", temp_value)
        Broker->>Sub: forward message
    end
```

---

## Hardware

| Component | Detail |
|-----------|--------|
| Microcontroller | Arduino Uno |
| Temperature Sensor | DHT11 — data pin **GPIO 2** |
| Display | 16×2 LCD, I²C address `0x27` |
| Communication | USB Serial @ 9600 baud |

### Wiring

```
DHT11  DATA  →  Arduino D2
LCD    SDA   →  Arduino A4
LCD    SCL   →  Arduino A5
LCD    VCC   →  5V
LCD    GND   →  GND
DHT11  VCC   →  3.3V or 5V
DHT11  GND   →  GND
```

---

## Project Structure

```
.
├── arduino/
│   └── temp_display.ino   # Arduino sketch (sensor + LCD + serial output)
├── pc_client/
│   └── pc_client.py       # Python client (serial reader + MQTT publisher)
└── README.md
```

---

## Getting Started

### Arduino

1. Install the following libraries via Arduino Library Manager:
   - `DHT sensor library` by Adafruit
   - `LiquidCrystal_I2C` by Frank de Brabander
   - `Wire` (bundled with Arduino IDE)

2. Open `arduino/temp_display.ino` and update the candidate name if needed:
   ```cpp
   String candidateName = "Cyubahiro Don Durkheim";
   ```

3. Flash to your Arduino Uno.

### PC Client

**Requirements:** Python 3.8+

```bash
pip install pyserial paho-mqtt
```

**Run:**

```bash
python pc_client/pc_client.py
```

The script auto-detects the Arduino port. To pin a specific port, set `COM_PORT` in the script:

```python
COM_PORT = "/dev/ttyACM0"   # Linux
COM_PORT = "COM3"            # Windows
```

---

## Configuration

| Variable | Default | Description |
|----------|---------|-------------|
| `COM_PORT` | `None` | Serial port — `None` enables auto-detect |
| `BAUD_RATE` | `9600` | Must match Arduino sketch |
| `MQTT_BROKER` | `broker.hivemq.com` | Public HiveMQ broker |
| `MQTT_PORT` | `1883` | Standard MQTT port |
| `MQTT_TOPIC` | `student/sensor/temperature` | Topic to publish readings |

---

## Serial Protocol

The Arduino emits a single line per reading over USB Serial:

```
TEMP:25.60
```

The PC client filters for lines starting with `TEMP:`, strips the prefix, and publishes the numeric value to MQTT. Any other serial output is printed as debug info.

---

## LCD Layout

```
┌────────────────┐
│ Cyubahiro Don  │  ← candidate name (scrolls if > 16 chars)
│ Temp: 25.60 C  │  ← live temperature
└────────────────┘
```

---

## License

MIT © Cyubahiro Don Durkheim
