#ifndef _JsonSerializationProvider_h
#define _JsonSerializationProvider_h

#include <Arduino.h>
#include <ArduinoJson.h>
#define JSON_BUFFER_SIZE 512

class JsonSerializationProvider
{
private:
  StaticJsonBuffer<JSON_BUFFER_SIZE> _jsonBuffer;
  void ResetBuffer();

public:
  JsonObject& CreateMessage();
  JsonObject& CreateObject();
  JsonObject& Deserialize(String& payload);
  String Serialize(JsonObject& m);
};
#endif
