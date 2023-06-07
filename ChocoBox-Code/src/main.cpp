#include <Arduino.h>
#include <ConnecT.h>
#include <Humidifier.h>
#include "DHT.h"
#include "HumidityController.h"
#include "RTClib.h"
#include <Adafruit_Sensor.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <TemperatureController.h>
#include <Heater.h>
#include <Interpol.h>
#include "SPIFFS.h"
#include <Preferences.h>
#include <ChocoBoxREST.h>

using namespace ChocoBoxREST;

std::vector<float> x;
std::vector<float> v_temp;
std::vector<float> v_humidity;
std::vector<float> xq;

std::vector<float> vq_temp;
std::vector<float> vq_humidity;

const int numOfHours = 336; // Número de horas que dura la fermentación
const float paso = 1; // Paso de la interpolación - Se cambia la temperatura y la humedad cada {paso} horas
const int buffer_size = numOfHours/paso;

float vq_tempBuffer[buffer_size] = {};
float vq_humidityBuffer[buffer_size] = {};

float temperature_buffer[buffer_size] = {};
float humidity_buffer[buffer_size] = {};



/* Funciones de ayuda */
void printVector(const std::vector<float>& vec) {
    for (float value : vec) {
        Serial.print(value);
        Serial.print(",");
    }
    Serial.println();
}

void getVectorsFromJson(FirebaseJson* json, std::vector<float>& x, std::vector<float>& v_temp, std::vector<float>& v_humidity) {
    size_t arraySize = json->iteratorBegin();
    for (size_t i = 0; i < arraySize; i+=3) {
        int type;
        String temperature, humidity;
        String key, value;

        json->iteratorGet(i, type, key, value);
        float time = key.toFloat();
        json->iteratorGet(i+1, type, key, humidity);
        json->iteratorGet(i+2, type, key, temperature);
        x.push_back(time);
        v_temp.push_back(temperature.toFloat());
        v_humidity.push_back(humidity.toFloat());
    }
}

/* Pines */
#define PIN_HUM 23 // Pin del Humidificador
#define PIN_HEAT 24 // Pin del Calentador

#define DHTTYPE DHT22  // DHT 22  (AM2302), AM2321 - SDA: 21, SCL: 22
#define DHTPIN_01 18 // Pin del sensor DHT 01
#define DHTPIN_02 19 // Pin del sensor DHT 02

/* Sensores -  Variables */
float humidity_01 = 0; // Variable que almacena la humedad
float temperature_01 = 0; // Variable que almacena la temperatura
float humidity_02 = 0; // Variable que almacena la humedad
float temperature_02 = 0; // Variable que almacena la temperatura
float ferm_time = 0; // Variable que almacena el tiempo de fermentación
float ferm_time_hr = 0; // Variable que almacena el tiempo de fermentación en horas
float prev_ferm_time = 0; // Variable que almacena el tiempo de fermentación previo
int prev_index = -1; // Variable que almacena el índice previo de la interpolación

/* Variables de control */
FirebaseJson* environment; // Variable que almacena el JSON con el ambiente a recrear
float desiredHumidity = 0; // Variable que almacena la humedad deseada
float desiredTemperature = 0; // Variable que almacena la temperatura deseada
float now = 0; // Variable que almacena el tiempo actual
bool* resetPointer;
bool defaultReset = false; 


/* Control del log - Persistencia de los datos */
int log_interval = (60*1000) * 30; // Intervalo de tiempo entre cada registro en la base de datos (en milisegundos)
long log_persistence = (24*60*60) * 30; // Tiempo de persistencia de los datos en la base de datos (en segundos) 


/* Objetos */
ConnecT connecT; // Instancia de la clase ConnecT: Permite la cone1pasoxión a internet y la comunicación con el servidor web
Humidifier humidifier; // Instancia de la clase Humidifier: Permite el control del humidificador
Heater heater; // Instancia de la clase Heater: Permite el control del calentador
DHT dht_01(DHTPIN_01, DHTTYPE); // Instancia de la clase DHT: Permite la lectura de los sensores DHT
DHT dht_02(DHTPIN_02, DHTTYPE); // Instancia de la clase DHT: Permite la lectura de los sensores DHT
HumidityController humidityController(&humidity_01, &humidity_02, &humidifier); // Instancia de la clase HumidityController: Permite el control de la humedad
TemperatureController temperatureController(&temperature_01, &temperature_02, &heater); // Instancia de la clase TemperatureController: Permite el control de la temperatura
RTC_DS3231 rtc; // Instancia de la clase RTC_DS3231: Permite la lectura del RTC
LiquidCrystal_I2C lcd(0x27, 20, 4); // Instancia de la clase LiquidCrystal_I2C: Permite la comunicación con la pantalla LCD
Interpol interpol; // Instancia de la clase Interpol: Permite la interpolación de los datos
Preferences preferences; // Instancia de la clase Preferences: Permite el almacenamiento de datos en la memoria flash


/* Funciones */

void putVector(char* key, std::vector<float>& vq) {
    size_t size = vq.size();
    float vq_array[size];
    std::copy(vq.begin(), vq.end(), vq_array);
    vq.clear();  // Clear the vector to save memory

    preferences.putBytes(key, vq_array, sizeof(vq_array));
}


void updateEnvironment(){

  environment = connecT.getJSON("/environment");
  getVectorsFromJson(environment, x, v_temp, v_humidity);
  xq = interpol.generateXq(x[0], x[x.size()-1], paso);

  vq_temp = interpol.cubicSpline(x, v_temp, xq);
  vq_humidity = interpol.cubicSpline(x, v_humidity, xq);

  putVector("vq_temp", vq_temp);
  putVector("vq_humidity", vq_humidity);

  x.clear();
  v_temp.clear();
  v_humidity.clear();
  xq.clear();
  vq_temp.clear();
  vq_humidity.clear();
}



void setup() {
  Serial.begin(115200); // Inicialización del puerto serial

  /* Inicialización apuntadores */
  resetPointer = &defaultReset; 

  /* Conexión con el RTC */
  rtc.begin();
  now = millis();

  preferences.begin("chocoBox", false);  // Se inicia la memoria flash

  /* Se ingresa la hora en que inició la fermentación */
  preferences.putFloat("ferm_start", now);

  if(!preferences.isKey("ferm_time")){
    preferences.putFloat("ferm_time", 0);
  }
  else{
    ferm_time = preferences.getFloat("ferm_time");
    ferm_time_hr = ferm_time/3600000;
  }


  // Se re-establece el tiempo cuando el RTC pierde potencia o cuando está nuevo
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  /* Configuración de los módulos */
  humidifier.setUp(PIN_HUM); // Configuración del humidificador
  dht_01.begin(); // Configuración del sensor DHT
  dht_02.begin(); // Configuración del sensor DHT
  humidityController.setDesiredHumidity(&desiredHumidity); // Configuración de la humedad deseada
  temperatureController.setDesiredTemperature(&desiredTemperature); // Configuración de la temperatura deseada
  ChocoBoxREST::_bufferSize = buffer_size; // Configuración del tamaño del buffer de datos

  /* Configuración de la pantalla LCD */
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Humidity: ");
  lcd.setCursor(0, 1);
  lcd.print("Temperature: ");

  /* Configuración del IOT */
  connecT.setDualMode();
  connecT.setWiFi_AP("ChocoBox", "chocoBox");
  connecT.setWebServer(80); // Creación del servidor web en el puerto 80
  connecT.setWiFi_STA("IncuPlant", "eusebio8425"); // Conexión a la red WiFi
  connecT.setFirebase("AIzaSyD2ldqxOE9shGk3XsHtYvBmwjK3NqKP0ew", "https://chocobox-73f90-default-rtdb.firebaseio.com", "juanse8425@gmail.com", "chocoBox"); // Conexión a la base de datos de Firebase


  // Se agregan los sensores a Firesense
  connecT.addSensor(&humidity_01);
  connecT.addSensor(&temperature_01);
  connecT.addSensor(&humidity_02);
  connecT.addSensor(&temperature_02);
  connecT.addSensor(&ferm_time_hr);
  connecT.addSensor(&desiredHumidity);
  connecT.addSensor(&desiredTemperature);

  // Conexión a Firesense - Último que debe hacerse
  connecT.setFiresense("/Sensors", "Node1", 3, log_interval, log_interval, log_persistence); 

  //Se obtiene el JSON con el ambiente a recrear y se interpola inicialmente
  if (WiFi.status() == WL_CONNECTED) {
      updateEnvironment();
  }


  /* Se obtiene la información almacenada en memoria flash - Curvas a replicar */
  preferences.getBytes("vq_temp", vq_tempBuffer, preferences.getBytesLength("vq_temp"));
  preferences.getBytes("vq_humidity", vq_humidityBuffer, preferences.getBytesLength("vq_humidity"));

  /* Se instancian los arreglos en memoria flash que almacenaran los datos de humedad y temperatura reales */
  if(!preferences.isKey("data_tem") && !preferences.isKey("data_hum")){
    preferences.putBytes("data_tem", temperature_buffer, sizeof(temperature_buffer));
    preferences.putBytes("data_hum", humidity_buffer, sizeof(humidity_buffer));
  }

  /* Se vinculan los apuntadores de los arrays de medición al protocolo REST */
  ChocoBoxREST::linkServer(connecT.getServerPointer());
  ChocoBoxREST::linkHumidity(humidity_buffer);
  ChocoBoxREST::linkTemperature(temperature_buffer);
  ChocoBoxREST::linkReset(resetPointer);

  //Vincular el API REST con el servidor WiFi
  connecT.addGETtoWeb("/temperature", ChocoBoxREST::getTemperature);
  connecT.addGETtoWeb("/humidity", ChocoBoxREST::getHumidity);
  connecT.addPUTtoWeb("/reset", ChocoBoxREST::putReset);

  (connecT.getServerPointer())->begin();

}

void loop() {
  /* Se actualiza el tiempo actual */
  now = millis();


  /* Se revisa si se desea resetear la fermentación */
  if(*resetPointer){

    Serial.println("Reseteando la fermentación...");

    *resetPointer = false;
    
    //Se pone ferm_time en 0
    preferences.putFloat("ferm_time", 0);
    ferm_time = 0;
    ferm_time_hr = 0;

    //Se reinician los arreglos de datos almacenados
    memset(temperature_buffer, 0, sizeof(temperature_buffer));
    memset(humidity_buffer, 0, sizeof(humidity_buffer));

    preferences.putBytes("data_tem", temperature_buffer, sizeof(temperature_buffer));
    preferences.putBytes("data_hum", humidity_buffer, sizeof(humidity_buffer));

    //Se reinicia el índice previo
    prev_index = -1;
    prev_ferm_time = 0;

  }


  /* Se establece el valor deseado de temperatura y humedad según la gráfica */
  int index = (int) (ferm_time_hr/paso);
  desiredTemperature = vq_tempBuffer[index];
  desiredHumidity = vq_humidityBuffer[index];

  /* Lectura de los sensores */
  humidity_01 = desiredHumidity;//dht_01.readHumidity(); // Lectura de la humedad
  humidity_02 = desiredHumidity;//dht_02.readHumidity(); // Lectura de la humedad

  temperature_01 = desiredTemperature;//dht_01.readTemperature(); // Lectura de la temperatura
  temperature_02 = desiredTemperature;//dht_02.readTemperature(); // Lectura de la temperatura

  /* Control de la humedad */
  humidityController.update();

  /* Control de la temperatura */
  temperatureController.update();

  /* Actualización de la pantalla LCD */
  lcd.setCursor(10, 0);
  lcd.print((humidity_01+humidity_02)/2);
  lcd.setCursor(13, 1);
  lcd.print((temperature_01+temperature_02)/2);

  /* Se almacena el instante actual de funcionamiento de la fermentación */
  preferences.putFloat("ferm_now", now);

  /* Se almacena el tiempo de fermentación hasta el momento - Tiempo del dispositivo prendido */
  ferm_time = preferences.getFloat("ferm_time");
  ferm_time_hr = ferm_time/3600000;
  float ferm_diff= now - preferences.getFloat("ferm_start");
  
  if (prev_ferm_time == 0 ){
    prev_ferm_time = ferm_time;
  }
  preferences.putFloat("ferm_time", ferm_diff + prev_ferm_time);


  /* Se escribe el archivo con la data recolectada - Únicament con el cambio de índice */
   if(prev_index != index){
      Serial.println("Almacenando datos...");

      prev_index = index;

      preferences.getBytes("data_tem", temperature_buffer, preferences.getBytesLength("data_tem"));
      preferences.getBytes("data_hum", humidity_buffer, preferences.getBytesLength("data_hum"));

      temperature_buffer[index] = (temperature_01+temperature_02)/2;
      humidity_buffer[index] = (humidity_01+humidity_02)/2;

      preferences.putBytes("data_tem", temperature_buffer, sizeof(temperature_buffer));
      preferences.putBytes("data_hum", humidity_buffer, sizeof(humidity_buffer));

      Serial.println("Datos almacenados.");
   }

  //Se revisan las posibles peticiones REST del cliente
  (connecT.getServerPointer())->handleClient(); 


  /* Corre FireSense (último en el loop)*/
   connecT.FS_run();  

}
