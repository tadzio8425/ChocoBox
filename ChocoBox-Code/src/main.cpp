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

/* Variables de control */
float desiredHumidity = 50; // Variable que almacena la humedad deseada
float desiredTemperature = 25; // Variable que almacena la temperatura deseada

/* Objetos */
ConnecT connecT; // Instancia de la clase ConnecT: Permite la conexión a internet y la comunicación con el servidor web
Humidifier humidifier; // Instancia de la clase Humidifier: Permite el control del humidificador
Heater heater; // Instancia de la clase Heater: Permite el control del calentador
DHT dht_01(DHTPIN_01, DHTTYPE); // Instancia de la clase DHT: Permite la lectura de los sensores DHT
DHT dht_02(DHTPIN_02, DHTTYPE); // Instancia de la clase DHT: Permite la lectura de los sensores DHT
HumidityController humidityController(&humidity_01, &humidity_02, &humidifier); // Instancia de la clase HumidityController: Permite el control de la humedad
TemperatureController temperatureController(&temperature_01, &temperature_02, &heater); // Instancia de la clase TemperatureController: Permite el control de la temperatura
RTC_DS3231 rtc; // Instancia de la clase RTC_DS3231: Permite la lectura del RTC
LiquidCrystal_I2C lcd(0x27, 20, 4); // Instancia de la clase LiquidCrystal_I2C: Permite la comunicación con la pantalla LCD

void setup() {
  Serial.begin(115200); // Inicialización del puerto serial

  /* Conexión con el RTC */
  rtc.begin();

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

  /* Configuración de la pantalla LCD */
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Humidity: ");
  lcd.setCursor(0, 1);
  lcd.print("Temperature: ");

  /* Configuración del IOT */
  connecT.setWiFi_STA("HOTELLASFLORES", "HOSPEDERIA"); // Conexión a la red WiFi
  connecT.setFirebase("AIzaSyD2ldqxOE9shGk3XsHtYvBmwjK3NqKP0ew", "https://chocobox-73f90-default-rtdb.firebaseio.com", "juanse8425@gmail.com", "chocoBox"); // Conexión a la base de datos de Firebase
  connecT.setWebServer(80); // Creación del servidor web en el puerto 80

  // Se agregan los sensores a Firesense
  connecT.addSensor(&humidity_01);
  connecT.addSensor(&temperature_01);
  connecT.addSensor(&humidity_02);
  connecT.addSensor(&temperature_02);

  // Conexión a Firesense - Último que debe hacerse
  connecT.setFiresense("/Sensors", "Node1", -5, 60*1000, 60*1000, 20, 24*60*60); 
}

void loop() {

  /* Tiempo actual dado por el RTC */
  DateTime now = rtc.now();

  /* Lectura de los sensores */
  humidity_01 = dht_01.readHumidity(); // Lectura de la humedad
  temperature_01 = dht_01.readTemperature(); // Lectura de la temperatura
  humidity_02 = dht_02.readHumidity(); // Lectura de la humedad
  temperature_02 = dht_02.readTemperature(); // Lectura de la temperatura

  /* Control de la humedad */
  humidityController.update();

  /* Control de la temperatura */
  temperatureController.update();

  /* Actualización de la pantalla LCD */
  lcd.setCursor(10, 0);
  lcd.print((humidity_01+humidity_02)/2);
  lcd.setCursor(13, 1);
  lcd.print((temperature_01+temperature_02)/2);
}
