#include "Arduino.h"
#include "Humidifier.h"	
#include "HumidityController.h"

HumidityController::HumidityController(float* humidity, Humidifier* humidifier)
{
    _humidity = humidity;
    _humidifier = humidifier;
}

void HumidityController::setDesiredHumidity(float* desiredHumidity)
{
    _desiredHumidity = desiredHumidity;
}

void HumidityController::setOffSetHumidity(float* offSetHumidity)
{
    _offSetHumidity = offSetHumidity;
}

void HumidityController::update()
{
    if((*_humidity) < *_desiredHumidity)
    {
        _humidifier->turnON();
    }
    else
    {
        _humidifier->turnOFF();
    }
}