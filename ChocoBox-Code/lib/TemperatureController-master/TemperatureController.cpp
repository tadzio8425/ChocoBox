#include "Arduino.h"
#include <Heater.h>	
#include <TemperatureController.h>
#include <AutoPID.h>

//Configuración PID y constantes
#define KP 1.8
#define KI 0.0003
#define KD 0

//Instanciación PID


TemperatureController::TemperatureController(double* desiredTemperature, double* temperature, Heater* heater):
    _myPID(temperature, desiredTemperature, &_output, 5000, KP, KI, KD)
{
    _heater = heater;

    //Si la temperatura es mayor o menor a 2 grados, se satura al mínimo o máximo
    (_myPID).setBangBang(2);
    //Intervalo de actualización a 4000ms
    (_myPID).setTimeStep(4000);
}


void TemperatureController::update()
{
    _myPID.run();

    if(_output == 1)
    {
        _heater->turnON();
    }
    else
    {
        _heater->turnOFF();
    }

    Serial.println(_output);
}