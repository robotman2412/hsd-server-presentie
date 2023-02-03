# HSD presentie server
A simple "server" that responds to queries on MQTT, providing user data for NFC tag reads.

## Prerequisites
```
sudo apt install git build-essential gcc make cmake cmake-gui cmake-curses-gui libssl-dev libpaho-mqtt-dev libpaho-mqttpp-dev
```

If `libpaho-mqtt-dev` and `libpaho-mqttpp-dev` can't be found, you can clone the repository and run:

```./make-paho-from-source.sh```
