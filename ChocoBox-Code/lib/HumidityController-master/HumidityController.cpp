#include "Arduino.h"
#include "Humidifier.h"	
#include "HumidityController.h"

HumidityController::HumidityController(float* humidity_01, float* humidity_02, Humidifier* humidifier)
{
    _humidity_01 = humidity_01;
    _humidity_02 = humidity_02;
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
    float humidity_mean = (*_humidity_01 + *_humidity_02)/2;

    if(humidity_mean < *_desiredHumidity)
    {
        _humidifier->turnON();
    }
    else
    {
        _humidifier->turnOFF();
    }
}