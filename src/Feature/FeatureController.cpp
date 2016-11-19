#include "FeatureController.h"

FeatureController::FeatureController(int port, const char* type, DeviceContext* context)
{
  _port = port;
  _type = type;
  _context = context;
}

FeatureController::~FeatureController()
{

}

void FeatureController::PopulateDescription(ArduinoJson::JsonArray &featureDescriptions)
{
  PopulateDescription(featureDescriptions, _type);
}

void FeatureController::PopulateDescription(JsonArray& featureDescriptions, const char* type)
{
  JsonObject& featureDescription = _context->GetSerializationProvider().CreateObject();
  featureDescription["port"] = _port;
  featureDescription["type"] = type;
  featureDescriptions.add(featureDescription);
}

void FeatureController::Loop()
{
}

bool FeatureController::TryHandle(ArduinoJson::JsonObject &message, const char *topic)
{
  return false;
}

bool FeatureController::TryHandle(ArduinoJson::JsonObject &command)
{
  if (CanHandle(command))
  {
    Handle(command);
    return true;
  }
  return false;
}

bool FeatureController::CanHandle(ArduinoJson::JsonObject &command)
{
  const char* commandType = command["type"].asString();
  const int commandPort = command["port"].as<int>();
  return _port == commandPort && strcmp(_type, commandType) == 0;
}

void FeatureController::Handle(JsonObject& command)
{

}
