#include "Arduino.h"
#include <WebServer.h>
#include <ArduinoJson.h>

// JSON data buffer
StaticJsonDocument<10000> jsonDocument;
char buffer[10000];


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
    int _bufferSize;

    void add_json_object(char *tag, float value, char *unit) {
        JsonObject obj = jsonDocument.createNestedObject();
        obj["type"] = tag;
        obj["value"] = value;
        obj["unit"] = unit; 
    }

    void setBufferSize(int bufferSize){
        _bufferSize = bufferSize;
    }

    void linkServer(WebServer* serverPointer){
        _serverPointer = serverPointer;
    }

    // MÉTODOS GET
    void getTemperature(){
        jsonDocument.clear(); // Clear the JSON document before populating it
        JsonArray array = jsonDocument.to<JsonArray>(); // Create a JSON array in the JSON document

        for (int i = 0; i < _bufferSize; i++) {
            array.add(_temperatureArray[i]);
        }

        serializeJson(jsonDocument, buffer); // Serialize the JSON document to the buffer
        _serverPointer->send(200, "application/json", buffer);
    }

    void getHumidity(){
        jsonDocument.clear(); // Clear the JSON document before populating it
        JsonArray array = jsonDocument.to<JsonArray>(); // Create a JSON array in the JSON document

        for (int i = 0; i < _bufferSize; i++) {
            array.add(_humidityArray[i]);
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