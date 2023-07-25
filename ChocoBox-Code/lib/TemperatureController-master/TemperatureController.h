#include "Arduino.h"
#include <Heater.h>	
#include <AutoPID.h>

#ifndef TemperatureController_h
#define TemperatureController_h

class TemperatureController{
    public:
        TemperatureController(double* desiredTemperature, double* temperature, Heater* heater);
        void update();

    private:
        bool _output = false;
        Heater* _heater;
        AutoPIDRelay _myPID;
};

#endif