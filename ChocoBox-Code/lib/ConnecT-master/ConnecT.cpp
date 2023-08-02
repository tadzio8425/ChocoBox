#include "Arduino.h"
#include "ConnecT.h"

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>


void ConnecT::setDualMode(){
  WiFi.mode(WIFI_AP_STA);
}

void ConnecT::setWiFi_AP(char* wifi_ssid, char* wifi_password)
{
  /* Put IP Address details */
  IPAddress local_ip(192,168,1,1);
  IPAddress gateway(192,168,1,1);
  IPAddress subnet(255,255,255,0);
  WiFi.softAP(wifi_ssid, wifi_password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

}


void ConnecT::setWiFi_STA(char* wifi_ssid, char* wifi_password){


    IPAddress local_ip(157,253,207,128);
    IPAddress gateway(157,253,207,1);
    IPAddress subnet(255,255,255,0);
    IPAddress dns1(157,253,78,10);
    IPAddress dns2(157,253,79,100);

    WiFi.config(local_ip, gateway, subnet, dns1, dns2);
    WiFi.begin(wifi_ssid, wifi_password);
    Serial.println("\nConnecting");

    // Wait for connection result without blocking
    if (WiFi.waitForConnectResult() == WL_CONNECTED) {
        Serial.println("Connected to the WiFi network");
        Serial.print("Local ESP32 IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("Unable to connect to WiFi");
    }


}




void ConnecT::setWiFi_wokwi()
{
    Serial.print("Connecting to WiFi");
    WiFi.begin("Wokwi-GUEST", "", 6);
    while (WiFi.status() != WL_CONNECTED) {
      delay(100);
      Serial.print(".");
    }
    Serial.println(" Connected!");
}



void ConnecT::setWebServer(int port)
{
  _serverPointer = new WebServer(port);
  _serverPointer->enableCORS();
}


/***Función que agrega un path REST de tipo GET al servidor WEB
 *  @param path: Debe ser del tipo "\variableName"
 *  @param function: Función a llamar cuando se ejecute el GET
 * ***/
void ConnecT::addGETtoWeb(String path, void (*function)()){
  _serverPointer->on(path, HTTP_GET, *function);
}

void ConnecT::addPUTtoWeb(String path, void (*function)()){
  _serverPointer->on(path, HTTP_OPTIONS, *function);
  _serverPointer->on(path, HTTP_PUT, *function);
}

WebServer* ConnecT::getServerPointer(){
  return _serverPointer;
}

