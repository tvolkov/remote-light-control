#define MQTT_MAX_PACKET_SIZE 512

#include <Arduino.h>
#include "MainApp.h"

MainApp mainApp;

void setup()
{
  mainApp.Init();
}

void loop()
{
  mainApp.Loop();
}
// #include <ESP8266WiFi.h>
// #include <ESP8266HTTPClient.h>
//
// #define STATUS_LED 13
//
// const char* ssid = "UPC012786";
// const char* password = "SAAADVGQ";
// const char* server_host = "192.168.0.18";
// const char* server_port = "8124";

// void connectWifi()
// {
//   Serial.println();
//   Serial.print("Connecting to ");
//   Serial.println(ssid);
//
//   WiFi.begin(ssid, password);
//   while (WiFi.status() != WL_CONNECTED)
//   {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("");
//   Serial.println("WiFi connected");
//   Serial.print("IP: ");
//   Serial.println(WiFi.localIP());
// }
//
// void setup()
// {
//   Serial.begin(115200);
//   pinMode(STATUS_LED, OUTPUT);
//   digitalWrite(STATUS_LED, LOW);
//   connectWifi();
// }
//
// bool tryGetDeviceState(String& payload)
// {
//   bool success = false;
//   String url = String("http://") + server_host + "/api/device" + device_id + "/state";
//   Serial.print("[HTTP] Connecting to ")
//   Serial.print(url);
//
//   HttpClient http;
//   http.begin(url);
//
//   int httpCode = http.GET();
//   if (httpCode > 0)
//   {
//     Serial.printf("[HTTP] GET ... code: %d\n", httpCode);
//     if (httpCode == HTTP_CODE_OK)
//     {
//       payload = http.getString();
//       Serial.println("[HTTP] Response:");
//       Serial.println(payload);
//       success = true;
//     }
//   }
//   else
//   {
//     Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
//   }
//
//   http.end();
//   Serial.println("[HTTP] Closing connection");
//   Serial.println();
//   return success;
// }
//
// void loop()
// {
//   if (WiFi.status() != WL_CONNECTED)
//   {
//     digitalWrite(STATUS_LED, LOW);
//     Serial.println("WiFi not connected");
//     delay(1000);
//     return;
//   }
//
//   String payload;
//   if (tryGetDeviceState(payload))
//   {
//     digitalWrite(STATUS_LED, payload == "1" ? HIGH : LOW);
//   }
//   delay(2000);
// }
