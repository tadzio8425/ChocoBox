#include <Arduino.h>
#include <ConnecT.h>
#include <Humidifier.h>
#include "DHT.h"
#include "HumidityController.h"

#define DHTTYPE DHT22  // DHT 22  (AM2302), AM2321

/* Pines */
#define PIN_HUM 23 // Pin del Humidificador
#define DHTPIN_01 22 // Pin del sensor DHT 01
#define DHTPIN_02 21 // Pin del sensor DHT 02

/* Objetos */
ConnecT connecT; // Instancia de la clase ConnecT: Permite la conexión a internet y la comunicación con el servidor web
Humidifier humidifier; // Instancia de la clase Humidifier: Permite el control del humidificador'
DHT dht_01(DHTPIN_01, DHTTYPE); // Instancia de la clase DHT: Permite la lectura de los sensores DHT
DHT dht_02(DHTPIN_02, DHTTYPE); // Instancia de la clase DHT: Permite la lectura de los sensores DHT
HumidityController humidityController(&humidity_01, &humidity_02, &humidifier); // Instancia de la clase HumidityController: Permite el control de la humedad

/* Sensores -  Variables */
float humidity_01 = 0; // Variable que almacena la humedad
float temperature_01 = 0; // Variable que almacena la temperatura
float humidity_02 = 0; // Variable que almacena la humedad
float temperature_02 = 0; // Variable que almacena la temperatura

/* Variables de control */
float desiredHumidity = 50; // Variable que almacena la humedad deseada

void setup() {
  Serial.begin(115200); // Inicialización del puerto serial

  /* Configuración de los módulos */
  humidifier.setUp(PIN_HUM); // Configuración del humidificador
  dht_01.begin(); // Configuración del sensor DHT
  dht_02.begin(); // Configuración del sensor DHT
  humidityController.setDesiredHumidity(&desiredHumidity); // Configuración de la humedad deseada

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
  /* Lectura de los sensores */
  humidity_01 = dht_01.readHumidity(); // Lectura de la humedad
  temperature_01 = dht_01.readTemperature(); // Lectura de la temperatura
  humidity_02 = dht_02.readHumidity(); // Lectura de la humedad
  temperature_02 = dht_02.readTemperature(); // Lectura de la temperatura

}
