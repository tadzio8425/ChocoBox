#include "Arduino.h"
#include <Heater.h>	
#include <TemperatureController.h>
#include <AutoPID.h>

//Configuración PID y constantes
#define OUTPUT_MIN 0
#define OUTPUT_MAX 1
#define KP 0.12
#define KI 0.03
#define KD 0

//Instanciación PID


TemperatureController::TemperatureController(double* desiredTemperature, double* temperature, Heater* heater):
    _myPID(temperature, desiredTemperature, &_output, OUTPUT_MIN, OUTPUT_MAX, KP, KI, KD)
{
    _heater = heater;

    //Si la temperatura es mayor o menor a 0.5 grados, se satura al mínimo o máximo
    (_myPID).setBangBang(0.5);
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
}