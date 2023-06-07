#include "Arduino.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <Firebase_ESP_Client.h>
#ifndef ConnecT_h
#define ConnecT_h

class ConnecT
{
  public:
    void setWiFi_AP(char* wifi_ssid, char* wifi_password);
    void setWiFi_STA(char* wifi_ssid, char* wifi_password);
    void setWiFi_wokwi();
    void setFirebase(char* api_key, char* database_url, char* user_email, char* user_password);
    void setFiresense(char* basePath, char* deviceID, int timeZone, int lastSeenInterval,
                      int logInterval, int conditionProcessInterval, long dataRetainingPeriod);
    FirebaseJson* getJSON(String path);
    void addSensor(float* sensor_value);
    void sendFloat(String path, float data);
    void setWebServer(int port);
    void addGETtoWeb(String path, void (*function)());
    void addPUTtoWeb(String path, void (*function)());
    void load_FsConfig();
    WebServer* getServerPointer();
  private:
    WebServer* _serverPointer;
};

#endif