#include "Arduino.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#ifndef ConnecT_h
#define ConnecT_h

class ConnecT
{
  public:
    void setWiFi_AP(char* wifi_ssid, char* wifi_password);
    void setWiFi_STA(char* wifi_ssid, char* wifi_password);
    void setWiFi_wokwi();
    void setFirebase(char* api_key, char* database_url, char* user_email, char* user_password);
    void setWebServer(int port);
    void addGETtoWeb(String path, void (*function)());
    void addPUTtoWeb(String path, void (*function)());
    WebServer* getServerPointer();
  private:
    WebServer* _serverPointer;
};

#endif