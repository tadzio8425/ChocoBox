#include "Arduino.h"
#include <WebServer.h>
#include <ArduinoJson.h>
#include <Preferences.h>

// JSON data buffer
StaticJsonDocument<1200> jsonDocument;
char buffer[1200];

Preferences* _preferences;

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

    float* _humidity_L;
    float* _humidity_R;

    double* _global_temp;

    bool* _heaterOn;
    bool* _humidifierOn;

    float* _step;

    int* _waterLevel;
    
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
      add_json_object("global_humidity", (*_humidity), "%");

      add_json_object("humidity_L", (*_humidity_L), "%");
      add_json_object("humidity_R", (*_humidity_R), "%");

      add_json_object("global_temp", (*_global_temp), "C");


      add_json_object("heaterON", (*_heaterOn), "B");
      add_json_object("humidON", (*_humidifierOn), "B");

      add_json_object("waterLvl", (*_waterLevel), "V");

      serializeJson(jsonDocument, buffer); 
      (*_serverPointer).send(200, "application/json", buffer);
  }


    //Métodos PUT
    void PUTReset(){
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


    void PUTStep(){
        jsonDocument.clear(); // Clear the JSON document before populating it

        if ((*_serverPointer).hasArg("plain") == false) {
        Serial.println("Esperaba un float, recibí: nada.");
        }
        String body = (*_serverPointer).arg("plain");
        deserializeJson(jsonDocument, body);
        
        //Obtener step nuevo
        (*_step) = (double) jsonDocument["step"];
        (_preferences) -> putFloat("step", (*_step));
        
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

    void linkTemperature(double* temp){
      _global_temp = temp;
    }

    void linkHumidity(float* hum){
      _humidity = hum;
    }

    
    void linkHumLeft(float* hum){
      _humidity_L = hum;
    }


    void linkHumRight(float* hum){
      _humidity_R = hum;
    }

    void linkRefTemp(double* refT){
      _refTemp = refT;
    }
    void linkRefHumid(float* refH){
      _refHumid = refH;
    }

    void linkHeaterOn(bool* on){
      _heaterOn = on;
    }

    void linkHumidOn(bool* on){
      _humidifierOn = on;
    }



    void linkReset(bool* resetPointer){
        _resetPointer = resetPointer;
    }

    void linkStep(float* step){
      _step = step;
    }

    void linkPreferences(Preferences* pref){
      _preferences = pref;
    }


    void linkWater(int* wtr){
      _waterLevel = wtr;
    }
    

}