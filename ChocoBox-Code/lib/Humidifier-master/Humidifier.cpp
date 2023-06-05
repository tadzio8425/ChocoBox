#include "Arduino.h"
#include "Humidifier.h"	

void Humidifier::setUp(int humidifier_pin)
{
    _humidifier_pin = humidifier_pin;
    pinMode(_humidifier_pin, OUTPUT);
}

void Humidifier::setPressDelay(int pressDelay)
{
    _pressDelay = pressDelay;
}

void Humidifier::turnON()
{
    digitalWrite(_humidifier_pin, HIGH);
    delay(_pressDelay);
    digitalWrite(_humidifier_pin, LOW);
}

void Humidifier::turnOFF()
{
    digitalWrite(_humidifier_pin, HIGH);
    delay(_pressDelay);
    digitalWrite(_humidifier_pin, LOW);
    delay(_pressDelay);
    digitalWrite(_humidifier_pin, HIGH);
    delay(_pressDelay);
    digitalWrite(_humidifier_pin, LOW);
}