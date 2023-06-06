#include <Arduino.h>
#include <Heater.h>

void Heater::setUp(int pin)
{
    _pin = pin;
    pinMode(_pin, OUTPUT);
}

void Heater::turnON()
{
    digitalWrite(_pin, HIGH);
}

void Heater::turnOFF()
{
    digitalWrite(_pin, LOW);
}

