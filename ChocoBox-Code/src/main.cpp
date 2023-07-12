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
#include <SD.h>
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>

using namespace ChocoBoxREST;

std::vector<float> x;
std::vector<float> v_temp;
std::vector<float> v_humidity;
std::vector<float> xq;

std::vector<float> vq_temp;
std::vector<float> vq_humidity;

const int numOfHours = 336; // Número de horas que dura la fermentación
const float paso = 0.05; // Paso de la interpolación - Se cambia la temperatura y la humedad cada {paso} horas
const int buffer_size = numOfHours/paso;

//Archivos tarjeta SD
File dataFile;
File environmentFile;
File splineFile;


/* Funciones de ayuda */
void printVector(const std::vector<float>& vec) {
    for (float value : vec) {
        Serial.print(value, 3);
        Serial.print(",");
    }
    Serial.println();
}


void extractEnvironmentData(const String& data, std::vector<float>& hourVec,
                 std::vector<float>& temperatureVec, std::vector<float>& humidityVec) {
    int startIndex = 0;
    int endIndex = data.indexOf('\n');
    bool isFirstLine = true;  // Flag to skip the first line
    while (endIndex != -1) {
        String line = data.substring(startIndex, endIndex);
        int commaIndex1 = line.indexOf(',');
        int commaIndex2 = line.indexOf(',', commaIndex1 + 1);
        if (!isFirstLine && commaIndex1 != -1 && commaIndex2 != -1) {
            float hour = line.substring(0, commaIndex1).toFloat();
            float temperature = line.substring(commaIndex1 + 1, commaIndex2).toFloat();
            float humidity = line.substring(commaIndex2 + 1).toFloat();
            hourVec.push_back(hour);
            temperatureVec.push_back(temperature);
            humidityVec.push_back(humidity);
        }
        isFirstLine = false;
        startIndex = endIndex + 1;
        endIndex = data.indexOf('\n', startIndex);
        if (endIndex == -1 && startIndex < data.length()) {
            // Handle the last line if it doesn't end with a newline character
            endIndex = data.length();
        }
    }
}


void sendSplineSD(const std::vector<float>& hourVec,
                          const std::vector<float>& temperatureVec,
                          const std::vector<float>& humidityVec) {
    String dataString;
    for (size_t i = 0; i < hourVec.size(); ++i) {
        dataString += String(hourVec[i], 3) + "," + String(temperatureVec[i], 3) + "," +
                      String(humidityVec[i], 3) + "\n";
    }

    splineFile = SD.open("/spline.txt", FILE_WRITE);
    splineFile.print(dataString);
    splineFile.flush();
    splineFile.close();
}



/* Pines */
#define PIN_HUM 27 // Pin del Humidificador
#define PIN_HEAT 14 // Pin del Calentador

#define DHTTYPE DHT22  // DHT 22  (AM2302), AM2321 - SDA: 21, SCL: 22
#define DHTPIN_01 4 // Pin del sensor DHT 01
#define DHTPIN_02 2 // Pin del sensor DHT 02

#define ONE_WIRE_BUS 25

#define chipSelect 5


//IMPORTANTE - VARIABLE QUE HABILITA EL WIFI
bool useWiFi = false;

/* Sensores -  Variables */
float humidity_01 = 0; // Variable que almacena la humedad
float temperature_01 = 0; // Variable que almacena la temperatura
float humidity_02 = 0; // Variable que almacena la humedad
float temperature_02 = 0; // Variable que almacena la temperatura
uint32_t ferm_time = 0; // Variable que almacena el tiempo de fermentación
float ferm_time_hr = 0; // Variable que almacena el tiempo de fermentación en horas
uint32_t prev_ferm_time = 0; // Variable que almacena el tiempo de fermentación previo
int prev_index = -1; // Variable que almacena el índice previo de la interpolación

float ds18b20_A = 0;
float ds18b20_B = 0;
float ds18b20_C = 0;
float ds18b20_D = 0;

/* Variables de control */
float desiredHumidity = 0; // Variable que almacena la humedad deseada
float desiredTemperature = 0; // Variable que almacena la temperatura deseada
uint32_t now = 0; // Variable que almacena el tiempo actual
bool* resetPointer;
bool defaultReset = false; 
bool wifiAtSetup = true;
float timer_start = 0;

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
OneWire oneWire(ONE_WIRE_BUS); //Instancia de la clase OneWire, permite conectarse a los dispositivos que usen este protocolo
DallasTemperature tempSensors(&oneWire); //Instancia de DallasTemperature para los DS18B20


char dateBuff[20];

/* Funciones */

void updateEnvironment(){

  String environment_str;

  environmentFile = SD.open("/environment.txt");
  if (environmentFile) {
    while (environmentFile.available()) {
    	environment_str = environmentFile.readString();
    }
    // close the file:
    environmentFile.close();
  } else {
    Serial.println("Error abriendo environment.txt");
  }

  //Se actualizan los vectores de la interpolación
  extractEnvironmentData(environment_str, x, v_temp, v_humidity);

  xq = interpol.generateXq(x[0], x[x.size()-1], paso);

  vq_temp = interpol.cubicSpline(x, v_temp, xq);
  vq_humidity = interpol.cubicSpline(x, v_humidity, xq);

  /*Subir a la tarjeta SD el resultado (vq_temp y vq_humidity)*/
  sendSplineSD(xq, vq_temp, vq_humidity);

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


  /*Configuración tarjeta SD*/
  Serial.print("Iniciando tajeta SD...");
  pinMode(SS, OUTPUT);
  if (!SD.begin(chipSelect)) {
    Serial.println("La tarjeta falló");
    // don't do anything more:
    while (1) ;
  }
  Serial.println("SD inicializada.");

  /*Apertura de archivo de dataLog en la SD */
  dataFile = SD.open("/datalog.txt", FILE_APPEND);
  if (! dataFile) {
    Serial.println("error opening datalog.txt");
    while (1) ;
  }

  /* Conexión con el RTC */
  rtc.begin();
  now = rtc.now().unixtime();

  preferences.begin("chocoBox", false);  // Se inicia la memoria flash

  /* Se ingresa la hora en que inició la fermentación */
  preferences.putULong("ferm_start", now);

  if(!preferences.isKey("ferm_time")){
    preferences.putULong("ferm_time", 0);
  }
  else{
    ferm_time = preferences.getULong("ferm_time");
    ferm_time_hr = ferm_time/3600000;
  }


  // Se re-establece el tiempo cuando el RTC pierde potencia o cuando está nuevo
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  /* Configuración de los módulos */
  humidifier.setUp(PIN_HUM); // Configuración del humidificador
  heater.setUp(PIN_HEAT); //Configuración del calentador
  dht_01.begin(); // Configuración del sensor DHT
  dht_02.begin(); // Configuración del sensor DHT
  tempSensors.begin(); //Inicio DS18B20
  humidityController.setDesiredHumidity(&desiredHumidity); // Configuración de la humedad deseada
  temperatureController.setDesiredTemperature(&desiredTemperature); // Configuración de la temperatura deseada
  ChocoBoxREST::_bufferSize = buffer_size; // Configuración del tamaño del buffer de datos

  /* Configuración de la pantalla LCD */
  lcd.init();
  lcd.clear();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Humidity: ");

  lcd.setCursor(0, 1);
  lcd.print("Temperature: ");

  lcd.setCursor(0, 2);
  lcd.print("Ref: ");

  lcd.setCursor(0, 3);
  lcd.print("Time: ");

  /* Configuración del IOT */
  connecT.setDualMode();
  connecT.setWiFi_AP("ChocoBox", "chocoBox");
  connecT.setWebServer(80); // Creación del servidor web en el puerto 80

  /*ACTUALIZAR SPLINE*/
  updateEnvironment();


  /* Se vinculan los apuntadores de los arrays de medición al protocolo REST */
  ChocoBoxREST::linkServer(connecT.getServerPointer());
  ChocoBoxREST::linkReset(resetPointer);

  //Vincular el API REST con el servidor WiFi
  connecT.addGETtoWeb("/temperature", ChocoBoxREST::getTemperature);
  connecT.addGETtoWeb("/humidity", ChocoBoxREST::getHumidity);
  connecT.addPUTtoWeb("/reset", ChocoBoxREST::putReset);

  (connecT.getServerPointer())->begin();

}

void loop() {
  /* Se actualiza el tiempo actual */
  now = rtc.now().unixtime();


  /* Se revisa si se desea resetear la fermentación */
  if(*resetPointer){

    Serial.println("Reseteando la fermentación...");

    *resetPointer = false;
    
    //Se pone ferm_time en 0 y ferm_start en now
    preferences.putULong("ferm_time", 0);
    preferences.putULong("ferm_start", now);
    ferm_time = 0;
    ferm_time_hr = 0;

    //Se reinicia el índice previo
    prev_index = -1;
    prev_ferm_time = 0;

    //Se resetean los datos de la SD
    dataFile = SD.open("/datalog.txt", FILE_WRITE);
    dataFile = SD.open("/datalog.txt", FILE_APPEND);
    dataFile.print("time,temperature,humidity\n");
    dataFile.flush();

    //Se reinicia el display
    lcd.clear();
    lcd.backlight();

    lcd.setCursor(0, 0);
    lcd.print("Humidity: ");

    lcd.setCursor(0, 1);
    lcd.print("Temperature: ");

    lcd.setCursor(0, 2);
    lcd.print("Ref: ");

    lcd.setCursor(0, 3);
    lcd.print("Time: ");

  }


  /* Se establece el valor deseado de temperatura y humedad según la gráfica */
  int index = (int) (ferm_time/(paso*3600));

  /* Lectura de los sensores */
  humidity_01 = dht_01.readHumidity(); // Lectura de la humedad con DHT
  humidity_02 = dht_02.readHumidity(); // Lectura de la humedad con DHT

  temperature_01 = dht_01.readTemperature(); // Lectura de la temperatura con DHT
  temperature_02 = dht_02.readTemperature(); // Lectura de la temperatura con DHT

  tempSensors.requestTemperatures(); //Se solicitan las temperaturas a todos los sensores DS18B20 del bus 
  ds18b20_A = tempSensors.getTempCByIndex(0);
  ds18b20_B = tempSensors.getTempCByIndex(1);
  ds18b20_C = tempSensors.getTempCByIndex(2);
  ds18b20_D = tempSensors.getTempCByIndex(3);
  //TO-DO: Cambiar el index por la dirección
  
  
  Serial.println(tempSensors.getTempCByIndex(0));

  /* Control de la humedad */
  humidityController.update();

  /* Control de la temperatura */
  temperatureController.update();

  /* Actualización de la pantalla LCD */
  uint8_t hours = ferm_time/3600;
  uint8_t minutes = (ferm_time - (hours * 3600))/60;
  uint8_t seconds = (ferm_time - (hours*3600) - (minutes*60));

  lcd.setCursor(10, 0);
  lcd.print((humidity_01+humidity_02)/2);
  lcd.print("%");

  lcd.setCursor(13, 1);
  lcd.print((temperature_01+temperature_02)/2);
  lcd.print("C");

  lcd.setCursor(5, 2);
  lcd.print(desiredHumidity);
  lcd.print("% / ");
  lcd.print(desiredTemperature);
  lcd.print("C");

  lcd.setCursor(6, 3);
  if(hours < 10){
    lcd.print(0);
  }
  lcd.print(hours);
  lcd.print(":");
  if(minutes < 10){
    lcd.print(0);
  }
  lcd.print(minutes);
  lcd.print(":");
  if(seconds < 10){
    lcd.print(0);
  }
  lcd.print(seconds);





  /* Se almacena el instante actual de funcionamiento de la fermentación */
  preferences.putULong("ferm_now", now);

  /* Se almacena el tiempo de fermentación hasta el momento - Tiempo del dispositivo prendido */
  ferm_time = preferences.getULong("ferm_time");
  uint32_t ferm_diff= now - preferences.getULong("ferm_start");
  
  if (prev_ferm_time == 0 ){
    prev_ferm_time = ferm_time;
  }
  preferences.putULong("ferm_time", ferm_diff + prev_ferm_time);


  /* Se escribe el archivo con la data recolectada - Únicament con el cambio de índice */
  if(prev_index != index){
      Serial.println("Almacenando datos...");

      //TO-DO: Obtener el valor deseado de la SD
      splineFile = SD.open("/spline.txt");
      
      //Se recorren las líneas hasta llegar a la deseada...
      int lineCount = 0;
      while(lineCount < index){
        splineFile.find('\n');
        lineCount++;
      }
      String desiredLine = splineFile.readStringUntil('\n');

      int first_comma = desiredLine.indexOf(",");
      int last_comma = desiredLine.lastIndexOf(",");

      //Se actualiza el valor deseado
      desiredTemperature =  (desiredLine.substring(first_comma+1, last_comma)).toFloat();
      desiredHumidity = (desiredLine.substring(last_comma+1, desiredLine.length())).toFloat();

      prev_index = index;

      /*Se suben los datos a la SD*/
      dataFile.print(ferm_time);
      dataFile.print(",");
      dataFile.print((temperature_01+temperature_02)/2, 3);
      dataFile.print(",");
      dataFile.print((humidity_01+humidity_02)/2, 3);
      dataFile.print("\n");
      dataFile.flush();

      Serial.println("Datos almacenados.");
   }

  //Se revisan las posibles peticiones REST del cliente
  (connecT.getServerPointer())->handleClient(); 
}
