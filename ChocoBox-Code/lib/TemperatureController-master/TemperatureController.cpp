#include "Arduino.h"
#include <Heater.h>	
#include <TemperatureController.h>

TemperatureController::TemperatureController(float* temperature, Heater* heater)
{
    _temperature = temperature;
    _heater = heater;
}

void TemperatureController::setDesiredTemperature(float* desiredTemperature)
{
    _desiredTemperature = desiredTemperature;
}

void TemperatureController::setOffSetTemperature(float* offSetTemperature)
{
    _offSetTemperature = offSetTemperature;
}

void TemperatureController::update()
{

    if(*_temperature < *_desiredTemperature)
    {
        _heater->turnON();
    }
    else
    {
        _heater->turnOFF();
    }
}