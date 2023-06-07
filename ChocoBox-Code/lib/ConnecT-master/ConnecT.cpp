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
// The FireSense class
#include <addons/FireSense/FireSense.h>

// The config data for FireSense class
Firesense_Config fsConfig;

// Define Firebase Data object
FirebaseData fbdo1;
FirebaseData fbdo2;

FirebaseAuth auth;
FirebaseConfig config;

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
  unsigned long sendDataPrevMillis = 0;
  int count = 0;

  config.api_key = api_key;
  config.database_url = database_url;

  auth.user.email = user_email;
  auth.user.password = user_password;
  
  Firebase.reconnectWiFi(true);
  fbdo1.setResponseSize(4096);

  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);

}

void ConnecT::sendFloat(String path, float data){
  if(Firebase.ready()){
    Firebase.RTDB.setFloat(&fbdo1, path, data);
  }

}

void ConnecT::setFiresense(char* basePath, char* deviceID, int timeZone, int lastSeenInterval,
 int logInterval, long dataRetainingPeriod){

    Serial.println("Espere a que FireSense configure sus sensores...");

    // Set up the config
    fsConfig.basePath = basePath;
    fsConfig.deviceId = "Node1";
    fsConfig.time_zone = timeZone; // change for your local time zone
    fsConfig.daylight_offset_in_sec = 0;
    fsConfig.last_seen_interval = lastSeenInterval;     // store timestamp
    fsConfig.log_interval =logInterval;           // store log data every 60 seconds
    fsConfig.condition_process_interval = 20;    // check conditions every 20 mSec
    fsConfig.dataRetainingPeriod = dataRetainingPeriod; // keep the log data for 1 day
    fsConfig.shared_fbdo = &fbdo1;               // for store/restore the data
    fsConfig.stream_fbdo = &fbdo2;               // for stream, if set this stream_fbdo to nullptr, the stream will connected through shared FirebaseData object.
    fsConfig.debug = false;

    FireSense.begin(&fsConfig, "");

    load_FsConfig();
    FireSense.loadConfig();
    FireSense.updateConfig();

    Serial.println("Sensores listos!");
}

void ConnecT::addSensor(float* sensor_value){
    FireSense.addUserValue(sensor_value);
}


FirebaseJson* ConnecT::getJSON(String path){
  Firebase.RTDB.getJSON(&fbdo1, path);
  FirebaseJson* json = fbdo1.jsonObjectPtr();
  return json;
}

void ConnecT::setWebServer(int port)
{
  _serverPointer = new WebServer(port);
  _serverPointer->enableCORS();
}

void ConnecT::FS_run(){
  FireSense.run();
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


void ConnecT::load_FsConfig(){
  FireSense_Channel channel[5];

  channel[0].id = "HUMID1";
  channel[0].name = "Humidity sensor data";
  channel[0].location = "Room1 1";
  channel[0].type = Firesense_Channel_Type::Value;
  channel[0].status = true;   // to store value to the database status
  channel[0].log = true;      // to store value to the database log
  channel[0].value_index = 0; // this the index of bound user variable which added with FireSense.addUserValue
  FireSense.addChannel(channel[0]);

  channel[1].id = "TEMP1";
  channel[1].name = "Temperature sensor data";
  channel[1].location = "Room1 1";
  channel[1].type = Firesense_Channel_Type::Value;
  channel[1].status = true;   // to store value to the database status
  channel[1].log = true;      // to store value to the database log
  channel[1].value_index = 1; // this the index of bound user variable which added with FireSense.addUserValue
  FireSense.addChannel(channel[1]);


  channel[2].id = "HUMID2";
  channel[2].name = "Humidity sensor data";
  channel[2].location = "Room1 2";
  channel[2].type = Firesense_Channel_Type::Value;
  channel[2].status = true;   // to store value to the database status
  channel[2].log = true;      // to store value to the database log
  channel[2].value_index = 2; // this the index of bound user variable which added with FireSense.addUserValue
  FireSense.addChannel(channel[2]);


  channel[3].id = "TEMP2";
  channel[3].name = "Temperature sensor data";
  channel[3].location = "Room1 2";
  channel[3].type = Firesense_Channel_Type::Value;
  channel[3].status = true;   // to store value to the database status
  channel[3].log = true;      // to store value to the database log
  channel[3].value_index = 3; // this the index of bound user variable which added with FireSense.addUserValue
  FireSense.addChannel(channel[3]);


  channel[4].id = "ferm_time";
  channel[4].name = "Fermentation Time";
  channel[4].location = "Room1";
  channel[4].type = Firesense_Channel_Type::Value;
  channel[4].status = true;   // to store value to the database status
  channel[4].log = true;      // to store value to the database log
  channel[4].value_index = 4; // this the index of bound user variable which added with FireSense.addUserValue
  FireSense.addChannel(channel[4]);
}