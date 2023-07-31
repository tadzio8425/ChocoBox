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
#include <MovingAverage.h>


using namespace ChocoBoxREST;

std::vector<float> x;
std::vector<float> v_temp;
std::vector<float> v_humidity;
std::vector<float> xq;

std::vector<float> vq_temp;
std::vector<float> vq_humidity;

const int numOfHours = 336; // Número de horas que dura la fermentación
float paso = 0.5; // Paso de la interpolación - Se cambia la temperatura y la humedad cada {paso} horas - Default cada media hora
const int buffer_size = numOfHours/paso;

//Archivos tarjeta SD
File dataFile;
File environmentFile;
File splineFile;

File dataFileApp;


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


//Direcciones DS18B20
uint8_t sensA_add[] = {0x28, 0xFF, 0x64, 0x1E, 0XF6, 0x66, 0x6D, 0x5F};
uint8_t sensB_add[] = {0x28, 0xFF, 0x64, 0x1E, 0xF7, 0x84, 0x80, 0x04};
uint8_t sensC_add[] = {0x28, 0xFF, 0x64, 0x1E, 0xF7, 0x96, 0x3D, 0xB6};
uint8_t sensD_add[] = {0x28, 0xFF, 0x64, 0x1E, 0x1F, 0x84, 0x68, 0xCD};



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

double global_temp = 0;
float global_humidity = 0;

/* Variables de control */
float desiredHumidity = 0; // Variable que almacena la humedad deseada
double desiredTemperature = 0; // Variable que almacena la temperatura deseada
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
HumidityController humidityController(&global_humidity, &humidifier); // Instancia de la clase HumidityController: Permite el control de la humedad
TemperatureController temperatureController(&desiredTemperature, &global_temp, &heater); // Instancia de la clase TemperatureController: Permite el control de la temperatura
RTC_DS3231 rtc; // Instancia de la clase RTC_DS3231: Permite la lectura del RTC
LiquidCrystal_I2C lcd(0x27, 20, 4); // Instancia de la clase LiquidCrystal_I2C: Permite la comunicación con la pantalla LCD
Interpol interpol; // Instancia de la clase Interpol: Permite la interpolación de los datos
Preferences preferences; // Instancia de la clase Preferences: Permite el almacenamiento de datos en la memoria flash
OneWire oneWire(ONE_WIRE_BUS); //Instancia de la clase OneWire, permite conectarse a los dispositivos que usen este protocolo
DallasTemperature tempSensors(&oneWire); //Instancia de DallasTemperature para los DS18B20
MovingAverage* tempAverage = new MovingAverage(); //Objeto que almacenará el promedio de las temperaturas
MovingAverage* humidAverage = new MovingAverage();

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

//Download a file from the SD, it is called in void SD_dir()
void downloadDataLog()
{
  {
    File download = SD.open("/datalog.txt");
    if (download) 
      {
        (connecT.getServerPointer()) -> sendHeader("Content-Type", "text/text");
        (connecT.getServerPointer()) -> sendHeader("Content-Disposition", "attachment; filename=datalog.txt");
        (connecT.getServerPointer()) -> sendHeader("Connection", "close");
        (connecT.getServerPointer()) -> streamFile(download, "application/octet-stream");
        download.close();
      } else Serial.println("Error sending a file!"); 
  }

}


//Handles the file upload a file to the SD
File UploadFile;
//Upload a new file to the Filing system
void handleFileUpload()
{ 
  HTTPUpload& uploadfile = (connecT.getServerPointer()) -> upload(); //See https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer/srcv
                                            //For further information on 'status' structure, there are other reasons such as a failed transfer that could be used
  if(uploadfile.status == UPLOAD_FILE_START)
  {
    String filename = uploadfile.filename;
    if(!filename.startsWith("/")) filename = "/"+filename;
    Serial.print("Upload File Name: "); Serial.println(filename);
    SD.remove(filename);                         //Remove a previous version, otherwise data is appended the file again
    UploadFile = SD.open(filename, FILE_WRITE);  //Open the file for writing in SD (create it, if doesn't exist)
    filename = String();
  }
  else if (uploadfile.status == UPLOAD_FILE_WRITE)
  {
    if(UploadFile) UploadFile.write(uploadfile.buf, uploadfile.currentSize); // Write the received bytes to the file
  } 
  else if (uploadfile.status == UPLOAD_FILE_END)
  {
    if(UploadFile)          //If the file was successfully created
    {                                    
      UploadFile.close();   //Close the file again
      Serial.print("Upload Size: "); Serial.println(uploadfile.totalSize);
      (connecT.getServerPointer()) -> send(200,"text/html", "File uploaded!");
    } 
    else
    {
      (connecT.getServerPointer()) -> send(501,"text/html", "Error!");
    }
  }
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


  //Se guarda el STEP en memoria secundaria
  if(!preferences.isKey("step")){
    preferences.putFloat("step", paso);
  }
  else{
    paso = preferences.getFloat("step");
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
  ChocoBoxREST::_bufferSize = buffer_size; // Configuración del tamaño del buffer de datos

  //Se asocian las variables de temperatura con su promedio
  tempAverage -> append(&ds18b20_A);
  tempAverage -> append(&ds18b20_B);
  tempAverage -> append(&ds18b20_C);

  //Se asocian las variables de humedad con su promedio
  humidAverage -> append(&humidity_01);
  humidAverage -> append(&humidity_02);


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
  ChocoBoxREST::linkTempA(&ds18b20_A);
  ChocoBoxREST::linkTempB(&ds18b20_B);
  ChocoBoxREST::linkTempC(&ds18b20_C);
  ChocoBoxREST::linkTempD(&ds18b20_D);
  ChocoBoxREST::linkHumidity(&global_humidity);
  ChocoBoxREST::linkRefTemp(&desiredTemperature);
  ChocoBoxREST::linkRefHumid(&desiredHumidity);
  ChocoBoxREST::linkTemperature(&global_temp);
  ChocoBoxREST::linkHumLeft(&humidity_01);
  ChocoBoxREST::linkHumRight(&humidity_02);
  ChocoBoxREST::linkHeaterOn(&(temperatureController._output));
  ChocoBoxREST::linkHumidOn(&(humidifier.isOn));
  ChocoBoxREST::linkStep(&paso);	
  ChocoBoxREST::linkPreferences(&preferences);

  //Vincular el API REST con el servidor WiFi
  connecT.addGETtoWeb("/", ChocoBoxREST::GETAll);
  connecT.addPUTtoWeb("/reset", ChocoBoxREST::PUTReset);
  connecT.addPUTtoWeb("/step",  ChocoBoxREST::PUTStep);
  connecT.addGETtoWeb("/dataLog", downloadDataLog);
  connecT.getServerPointer() -> on("/upload",  HTTP_POST,[](){ (connecT.getServerPointer()) -> send(200);}, handleFileUpload);
  (connecT.getServerPointer())->begin();

  Serial.println("Void setUp complete!");
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
    dataFile.print("time(min),tempA(degC),tempB(degC),tempC(degC),tempD(degC),humidLeft(%),humidRight(%)\n");
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
    

    //Re calcula el spline
    updateEnvironment();

  }


  /* Se establece el valor deseado de temperatura y humedad según la gráfica */
  int index = (int) (ferm_time/(paso*3600));

  /* Lectura de los sensores */
  humidity_01 = dht_01.readHumidity(); // Lectura de la humedad con DHT
  humidity_02 = dht_02.readHumidity(); // Lectura de la humedad con DHT

  temperature_01 = dht_01.readTemperature(); // Lectura de la temperatura con DHT
  temperature_02 = dht_02.readTemperature(); // Lectura de la temperatura con DHT

  tempSensors.requestTemperatures(); //Se solicitan las temperaturas a todos los sensores DS18B20 del bus 
  ds18b20_A = tempSensors.getTempC(sensA_add);
  ds18b20_B = tempSensors.getTempC(sensB_add);
  ds18b20_C = tempSensors.getTempC(sensC_add);
  ds18b20_D = tempSensors.getTempC(sensD_add);

  /* Una vez leídos los sensores, se obtiene el promedio de las temperaturas y humedades*/
  global_temp = tempAverage->getAverage();
  global_humidity = humidAverage ->getAverage();
  
  /* Control de la humedad */
  humidityController.update();

  /* Control de la temperatura */
  temperatureController.update();

  /* Actualización de la pantalla LCD */
  uint8_t hours = ferm_time/3600;
  uint8_t minutes = (ferm_time - (hours * 3600))/60;
  uint8_t seconds = (ferm_time - (hours*3600) - (minutes*60));

  lcd.setCursor(10, 0);
  lcd.print(global_humidity);
  lcd.print("%");

  lcd.setCursor(13, 1);
  lcd.print(global_temp);
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
      dataFile.print(ferm_time/60.0);
      dataFile.print(",");
      dataFile.print(ds18b20_A, 3);
      dataFile.print(",");
      dataFile.print(ds18b20_B, 3);
      dataFile.print(",");
      dataFile.print(ds18b20_C, 3);
      dataFile.print(",");
      dataFile.print(ds18b20_D, 3);
      dataFile.print(",");
      dataFile.print(humidity_01, 3);
      dataFile.print(",");
      dataFile.print(humidity_02, 3);
      dataFile.print("\n");
      dataFile.flush();

      Serial.println("Datos almacenados.");
   }

  //Se revisan las posibles peticiones REST del cliente
  (connecT.getServerPointer())->handleClient(); 
}
