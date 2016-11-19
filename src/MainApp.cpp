#include "MainApp.h"
#include <stdio.h>
#include <algorithm>
// #include "Utils/TimeUtils.h"
#include "Feature/FeatureController.h"
#include "Feature/SwitchFeatureController.h"
#include "Feature/LedFeatureController.h"
// #include "Feature/DHT22FeatureController.h"
// #include "Feature/MotionSensorFeatureController.h"

#define DEVICE_UNIQUE_ID "ESP8266_#1"

#define NETWORK_NAME "UPC012786"
#define NETWORK_PASSWORD "SAAADVGQ"
#define SERVER_HOST "192.168.0.18"
#define SERVER_PORT 8124
#define TOPIC_REGISTER "register"
#define TOPIC_SENSOR "sensor"

MainApp::MainApp() : _deviceConfig(DEVICE_UNIQUE_ID, NETWORK_NAME, NETWORK_PASSWORD),
                     _serializationProvider(),
                     _messageBus(SERVER_HOST, SERVER_PORT, &_serializationProvider, this, _deviceConfig.uniqueId)
{
  _messageBus.Subscribe(_deviceConfig.uniqueId);
  _features.push_back(new SwitchFeatureController(4, this, 4, false));
  _features.push_back(new SwitchFeatureController(5, this, 5, false));
  _features.push_back(new LedFeatureController(1, this, 13));
  // _features.push_back(new DHT22FeatureContorller(6, this, 12, TOPIC_SENSOR));
  // _features.push_back(new MotionSensorFeatureController(7, this, 14, TOPIC_SENSOR));
}

MainApp::~MainApp()
{
  std::for_each(_features.begin(), _features.end(), [](FeatureController* feature) {delete feature;});
  _features.clear();
}

void MainApp::Init()
{
  Serial.begin(115200);
  SetupWifi();
  OnStart();
}

void MainApp::Loop()
{
  _messageBus.Loop();
  OnLoop();
}

void MainApp::SetupWifi()
{
  Serial.printf("\n[MainApp] Connecting to %s\n", _deviceConfig.networkName);
  WiFi.begin(_deviceConfig.networkName, _deviceConfig.networkPassword);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\n[MainApp] WiFi connected\n");
  Serial.print("[MainApp] IP: ");
  Serial.println(WiFi.localIP());
}

void MainApp::Handle(const char* topic, JsonObject& message)
{
  if (strcmp(_deviceConfig.uniqueId, topic) == 0)
    OnCommand(message);
  else
    OnMessage(message, topic);
}

void MainApp::OnStart()
{
  Serial.println("[MainApp] Started.");
}

void MainApp::OnStop()
{
  Serial.println("[MainApp] Stopped.");
}

void MainApp::OnLoop()
{
  if (!_sentDeviceDescription && SendDeviceDescription())
    _sentDeviceDescription = true;

  std::for_each(_features.begin(), _features.end(), [](FeatureController* feature) {feature->Loop();});
}

bool MainApp::SendDeviceDescription()
{
  Serial.println("[MainApp] Sending DeviceDescription...");
  JsonObject& description = _serializationProvider.CreateMessage();
  description["deviceId"] = _deviceConfig.uniqueId;
  JsonArray& featureDescriptions = description.createNestedArray("features");

  std::for_each(_features.begin(), _features.end(), [&featureDescriptions](FeatureController* feature) {feature->PopulateDescription(featureDescriptions);});
  return _messageBus.Publish(TOPIC_REGISTER, description);
}

void MainApp::OnCommand(JsonObject& command)
{
  Serial.println("[MainApp] OnCommand start");
  std::for_each(_features.begin(), _features.end(), [&command](FeatureController* feature){feature->TryHandle(command);});
  Serial.println("[MainApp] OnCommand end");
}

void MainApp::OnMessage(JsonObject& command, const char* topic)
{
  Serial.println("MainApp: OnMessage start");
  std::for_each(_features.begin(), _features.end(), [&command, topic](FeatureController* feature){feature->TryHandle(command, topic);});
  Serial.println("MainApp: OnMessage end");
}
