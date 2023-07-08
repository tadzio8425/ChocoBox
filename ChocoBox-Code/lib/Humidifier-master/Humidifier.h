#include "Arduino.h"
#ifndef Humidifier_h
#define Humidifier_h

class Humidifier
    {
        public:
            void setUp(int humidifier_pin);
            void setPressDelay(int pressDelay);
            void turnON();
            void turnOFF();
        private:
            int _humidifier_pin;
            int _pressDelay = 400;
            bool isOn = false;  
    };

#endif