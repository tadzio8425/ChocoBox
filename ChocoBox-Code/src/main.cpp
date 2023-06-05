#include <Arduino.h>
#include <ConnecT.h>
#include <Humidifier.h>

/* Pines */
#define PIN_HUM 23 // Pin del Humidificador

/* Objetos */
ConnecT connecT; // Instancia de la clase ConnecT: Permite la conexión a internet y la comunicación con el servidor web
Humidifier humidifier; // Instancia de la clase Humidifier: Permite el control del humidificador

void setup() {

  Serial.begin(115200); // Inicialización del puerto serial

  /* Configuración de los módulos */
  humidifier.setUp(PIN_HUM); // Configuración del humidificador

  /* Configuración del IOT */
  connecT.setWiFi_STA("HOTELLASFLORES", "HOSPEDERIA"); // Conexión a la red WiFi
  connecT.setFirebase("AIzaSyD2ldqxOE9shGk3XsHtYvBmwjK3NqKP0ew", "https://chocobox-73f90-default-rtdb.firebaseio.com", "juanse8425@gmail.com", "chocoBox"); // Conexión a la base de datos de Firebase
  connecT.setWebServer(80); // Creación del servidor web en el puerto 80
}

void loop() {
  // put your main code here, to run repeatedly:

}
