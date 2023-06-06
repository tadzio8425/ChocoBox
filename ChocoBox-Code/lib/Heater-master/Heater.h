#include "Arduino.h"
#ifndef Heater_h
#define Heater_h

class Heater{
    public:
        void turnON();
        void turnOFF();
        void setUp(int pin);

    private:
        int _pin;
};

#endif
