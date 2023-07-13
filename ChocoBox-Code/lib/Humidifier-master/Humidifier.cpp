#include "Arduino.h"
#include "Humidifier.h"	

void Humidifier::setUp(int humidifier_pin)
{
    _humidifier_pin = humidifier_pin;
    pinMode(_humidifier_pin, OUTPUT);
    digitalWrite(_humidifier_pin, LOW);

    turnON();
}

void Humidifier::setPressDelay(int pressDelay)
{
    _pressDelay = pressDelay;
}

void Humidifier::turnON()
{
    if(!isOn){
        digitalWrite(_humidifier_pin, HIGH);
        delay(_pressDelay);
        digitalWrite(_humidifier_pin, LOW);
    }
    isOn = true;
}

void Humidifier::turnOFF()
{
    if(isOn){
        digitalWrite(_humidifier_pin, HIGH);
        delay(_pressDelay);
        digitalWrite(_humidifier_pin, LOW);
        delay(_pressDelay);
        digitalWrite(_humidifier_pin, HIGH);
        delay(_pressDelay);
        digitalWrite(_humidifier_pin, LOW);
    }
    isOn = false;
}