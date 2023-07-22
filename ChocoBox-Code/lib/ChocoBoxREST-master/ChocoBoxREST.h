#include "Arduino.h"
#include <WebServer.h>
#include <ArduinoJson.h>

// JSON data buffer
StaticJsonDocument<500> jsonDocument;
char buffer[500];


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
    bool* _resetPointer;
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


    //Métodos PUT
    void putReset(){
        jsonDocument.clear(); // Clear the JSON document before populating it

        if ((*_serverPointer).hasArg("plain") == false) {
        Serial.println("Esperaba un booleano, recibí: nada.");
        }
        String body = (*_serverPointer).arg("plain");
        deserializeJson(jsonDocument, body);
        
        //Obtener referencia
        (*_resetPointer) = (double) jsonDocument["reset"];
        
        //Se responde con la nueva referencia
        (*_serverPointer).send(200, "application/json", buffer);
    }

    void linkTemperature(float temperatureArray[]){
      _temperatureArray = temperatureArray;
    }

    void linkHumidity(float humidityArray[]){
      _humidityArray = humidityArray;
    } 

    void linkReset(bool* resetPointer){
        _resetPointer = resetPointer;
    }
    

}