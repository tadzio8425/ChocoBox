#include <Arduino.h>
#include <ConnecT.h>
#include <Humidifier.h>

/* Pines */
#define PIN_HUM 23 // Pin del Humidificador

/* Objetos */
ConnecT connecT; // Instancia de la clase ConnecT: Permite la conexión a internet y la comunicación con el servidor web
Humidifier humidifier; // Instancia de la clase Humidifier: Permite el control del humidificador'

/* Sensores -  Variables */
float humidity_01 = 0; // Variable que almacena la humedad
float temperature_01 = 0; // Variable que almacena la temperatura
float humidity_02 = 0; // Variable que almacena la humedad
float temperature_02 = 0; // Variable que almacena la temperatura

void setup() {
  Serial.begin(115200); // Inicialización del puerto serial

  /* Configuración de los módulos */
  humidifier.setUp(PIN_HUM); // Configuración del humidificador

  /* Configuración del IOT */
  connecT.setWiFi_STA("HOTELLASFLORES", "HOSPEDERIA"); // Conexión a la red WiFi
  connecT.setFirebase("AIzaSyD2ldqxOE9shGk3XsHtYvBmwjK3NqKP0ew", "https://chocobox-73f90-default-rtdb.firebaseio.com", "juanse8425@gmail.com", "chocoBox"); // Conexión a la base de datos de Firebase
  connecT.setWebServer(80); // Creación del servidor web en el puerto 80

  // Se agregan los sensores a Firesense
  connecT.addSensor(&humidity_01);
  connecT.addSensor(&temperature_01);
  connecT.addSensor(&humidity_02);
  connecT.addSensor(&temperature_02);

  connecT.setFiresense("/Sensors", "Node1", -5, 60*1000, 60*1000, 20, 24*60*60); // Conexión a Firesense - Último que debe hacerse


}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  humidity_01 = humidity_01 + 1;
}
