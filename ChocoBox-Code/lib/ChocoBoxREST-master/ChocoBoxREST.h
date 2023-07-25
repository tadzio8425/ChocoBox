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
    bool* _resetPointer;
    int _bufferSize;

    double*  _refTemp;
    float* _refHumid;

    float* _tempA;
    float* _tempB;
    float* _tempC;
    float* _tempD;

    float* _humidity;

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

    //Métodos GET
    void GETAll(){
      jsonDocument.clear();

      //Referencias
      add_json_object("refTemp", (*_refTemp), "C");
      add_json_object("refHumid", (*_refHumid), "%");

      //Valores
      add_json_object("tempA", (*_tempA), "C");
      add_json_object("tempB", (*_tempB), "C");
      add_json_object("tempC", (*_tempC), "C");
      add_json_object("tempD", (*_tempD), "C");
      add_json_object("humidity", (*_humidity), "%");
      serializeJson(jsonDocument, buffer); 
      (*_serverPointer).send(200, "application/json", buffer);
  }


    void GETReset(){
        jsonDocument.clear(); // Clear the JSON document before populating it

        add_json_object("reset", 1, "NA");

        //Obtener referencia
        (*_resetPointer) = true;
        
        //Se responde con la nueva referencia
        (*_serverPointer).send(200, "application/json", buffer);
    }


    void linkTempA(float* tempA){
      _tempA = tempA;
    }
    void linkTempB(float* tempB){
      _tempB = tempB;
    }
    void linkTempC(float* tempC){
      _tempC = tempC;
    }
    void linkTempD(float* tempD){
      _tempD = tempD;
    }

    void linkHumidity(float* hum){
      _humidity = hum;
    }

    void linkRefTemp(double* refT){
      _refTemp = refT;
    }
    void linkRefHumid(float* refH){
      _refHumid = refH;
    }



    void linkReset(bool* resetPointer){
        _resetPointer = resetPointer;
    }
    

}