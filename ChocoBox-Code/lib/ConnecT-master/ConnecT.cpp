#include "Arduino.h"
#include "ConnecT.h"
#include <Firebase_ESP_Client.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

void ConnecT::setWiFi_AP(char* wifi_ssid, char* wifi_password)
{
  /* Put IP Address details */
  IPAddress local_ip(192,168,1,1);
  IPAddress gateway(192,168,1,1);
  IPAddress subnet(255,255,255,0);

  WiFi.mode(WIFI_AP);

  WiFi.softAP(wifi_ssid, wifi_password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

}


void ConnecT::setWiFi_STA(char* wifi_ssid, char* wifi_password){
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_ssid, wifi_password);
    Serial.println("\nConnecting");

    while(WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(100);
    }

    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());
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




void ConnecT::setFirebase(char* api_key, char* database_url, char* user_email, char* user_password)
{
  FirebaseData fbdo;
  FirebaseAuth auth;
  FirebaseConfig config;
  unsigned long sendDataPrevMillis = 0;
  int count = 0;

  config.api_key = api_key;
  config.database_url = database_url;

  auth.user.email = user_email;
  auth.user.password = user_password;
  
  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);

  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);

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
  _serverPointer->on(path, HTTP_PUT, *function);
}

WebServer* ConnecT::getServerPointer(){
  return _serverPointer;
}
