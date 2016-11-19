# remote-light-control
An ESP8266 firmware to demonstrate MQTT pubsub mechanism.
LED and relay subscribe to MQTT topic, DHT22 sensor puslishes to another topic.
MQTT broker's ip address is hardcoded. (Mosquitto was used)

Once broker is started, the devices can be controller like this:
```Shell
mosquitto_pub -t command1 -m "{\"on\": true, \"type\": \"led\", \"port\": \"1\"}"
mosquitto_pub -t command1 -m "{\"on\": true, \"type\": \"switch\", \"port\": \"4\"}"
mosquitto_pub -t command1 -m "{\"on\": true, \"type\": \"switch\", \"port\": \"5\"}"
```
To subscribe to DHT22's updates:
```Shell
mosquitto_sub -t "sensor"
```
