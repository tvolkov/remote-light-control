#include "MqttMessageBus.h"
#include <ESP8266HTTPClient.h>
#include "../Utils/TimeUtils.h"
#include <algorithm>

MqttMessageBus* Singleton;

MqttMessageBus::MqttMessageBus(const char* serverHost, int serverPort, JsonSerializationProvider* serializationPrivider, MessageHandler* handler, const char* deviceId) : _mqttClient(_espClient)
{
  _serverHost = serverHost;
  _serverPort = serverPort;
  _serializationProvider = serializationPrivider;
  _deviceId = deviceId;
  SetHandler(handler);
  _mqttClient.setServer(_serverHost, _serverPort);
  _mqttClient.setCallback(MqttMessageCallback);
  Singleton = this;
}

MqttMessageBus::~MqttMessageBus()
{
}

bool MqttMessageBus::Publish(const char* topic, JsonObject& message)
{
  String payload = _serializationProvider->Serialize(message);
  return PublishInternal(topic, payload.c_str(), false);
}

bool MqttMessageBus::PublishInternal(const char* topic, const char* payload, bool retain)
{
  Serial.printf("MQTT publish to topic: '%s', message: '%s'\n", topic, payload);
  return _mqttClient.publish(topic, payload, retain);
}

void MqttMessageBus::Subscribe(const char* topic)
{
  if (_topics.find(topic) != _topics.end())
    return;

  _topics.insert(topic);

  if (_mqttClient.connected())
    _mqttClient.subscribe(topic);
}

void MqttMessageBus::Unsubscribe(const char *topic)
{
  if (_topics.erase(topic) > 0 && _mqttClient.connected())
    _mqttClient.unsubscribe(topic);
}

#define OFFLINE_MESSAGE "{ \"online\": false}"
#define ONLINE_MESSAGE "{ \"online\": true}"

void MqttMessageBus::ReconnectMqtt()
{
  while (!_mqttClient.connected())
  {
    String clientId = String(_deviceId) + "-" + String(random(0xffff), HEX);
    Serial.printf("MQTT Attempting MQTT Connection (clientid %s)...\n", clientId.c_str());

    String statusTopic = String(_deviceId) + "/status";
    if (_mqttClient.connect(clientId.c_str(), statusTopic.c_str(), MQTTQOS0, true, OFFLINE_MESSAGE))
    {
      Serial.println("MQTT Connected");

      std::for_each(_topics.begin(), _topics.end(), [this](const char* topic) {
        Serial.printf("MQTT Subscribing to topic: %s\n", topic);
        _mqttClient.subscribe(topic);
      });
      PublishInternal(statusTopic.c_str(), ONLINE_MESSAGE, true);
    }
    else
    {
      Serial.print("[MQTT] Failed to connect, rc=");
      Serial.print(_mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void MqttMessageBus::Loop()
{
  if (!_mqttClient.connected())
    ReconnectMqtt();
  _mqttClient.loop();
}

void MqttMessageBus::MqttMessageCallback(char* topic, byte* payload, unsigned int length)
{
  if (Singleton)
    Singleton->MessageCallback(topic, payload, length);
}

void MqttMessageBus::MessageCallback(char* topic, byte* payload, unsigned int length)
{
  char payloadChars[512];
  if (length >= 512)
  {
    Serial.printf("MQTT Buffer too small to read message from toic '%s'\n", topic);
    return;
  }

  memcpy(payloadChars, payload, length);
  payloadChars[length] = 0;
  Serial.printf("MQTT Received on topic: '%s', message: '%s'\n", topic, payloadChars);
  String payloadString = payloadChars;
  JsonObject& message = _serializationProvider->Deserialize(payloadString);
  if (!message.success())
  {
    Serial.printf("MQTT Could not deserialize JSON message from topic '%s'\n", topic);
    return;
  }

  if (handler)
    handler->Handle(topic, message);
}
