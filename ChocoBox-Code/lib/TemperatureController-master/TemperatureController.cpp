#include "Arduino.h"
#include <Heater.h>	
#include <TemperatureController.h>

TemperatureController::TemperatureController(float* temperature_01, float* temperature_02, Heater* heater)
{
    _temperature_01 = temperature_01;
    _temperature_02 = temperature_02;
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
    float temperature_mean = (*_temperature_01 + *_temperature_02)/2;

    if(temperature_mean < *_desiredTemperature)
    {
        _heater->turnON();
        Serial.println("xdxd");
    }
    else
    {
        _heater->turnOFF();
    }
}