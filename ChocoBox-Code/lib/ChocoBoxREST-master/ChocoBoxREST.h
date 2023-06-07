#include "Arduino.h"
#include <WebServer.h>
#include <ArduinoJson.h>

// JSON data buffer
StaticJsonDocument<1000> jsonDocument;
char buffer[1000];


//Funciones de manejo JSON Auxiliares
void create_json(char *tag, float value, char *unit) {  
  jsonDocument.clear();
  jsonDocument["type"] = tag;
  jsonDocument["value"] = value;
  jsonDocument["unit"] = unit;
  serializeJson(jsonDocument, buffer);  
}


namespace ChocoBoxREST{

    WebServer* _serverPointer;
    float* _temperatureArray;
    float* _humidityArray;

    void add_json_object(char *tag, float value, char *unit) {
        JsonObject obj = jsonDocument.createNestedObject();
        obj["type"] = tag;
        obj["value"] = value;
        obj["unit"] = unit; 
    }

    void linkServer(WebServer* serverPointer){
        _serverPointer = serverPointer;
    }

    // MÉTODOS GET
    void getTemperature(){
        Serial.println("GET /temperature");
        Serial.println(_temperatureArray[0]);
        jsonDocument.clear(); // Clear the JSON document before populating it
        JsonArray array = jsonDocument.createNestedArray(); // Create a nested array in the JSON document
        for (int i = 0; i < sizeof(_temperatureArray)/sizeof(float); i++) {
            JsonObject obj = array.createNestedObject();
            obj["type"] = "temperature";
            obj["value"] = _temperatureArray[i];
            obj["unit"] = "C";
        }

        serializeJson(jsonDocument, buffer); // Serialize the JSON document to the buffer
        _serverPointer->send(200, "application/json", buffer);
    }

    void getHumidity(){
        Serial.println("GET /humidity");
        jsonDocument.clear(); // Clear the JSON document before populating it
        JsonArray array = jsonDocument.createNestedArray(); // Create a nested array in the JSON document

        for (int i = 0; i < sizeof(_humidityArray)/sizeof(float); i++) {
            JsonObject obj = array.createNestedObject();
            obj["type"] = "humidity";
            obj["value"] = _humidityArray[i];
            obj["unit"] = "%";
        }

        serializeJson(jsonDocument, buffer); // Serialize the JSON document to the buffer
        _serverPointer->send(200, "application/json", buffer);
    }



    void linkTemperature(float temperatureArray[]){
      _temperatureArray = temperatureArray;
    }

    void linkHumidity(float humidityArray[]){
      _humidityArray = humidityArray;
    } 
    

}