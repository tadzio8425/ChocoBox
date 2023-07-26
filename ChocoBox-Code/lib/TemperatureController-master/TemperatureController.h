#include "Arduino.h"
#include <Heater.h>	
#include <AutoPID.h>

#ifndef TemperatureController_h
#define TemperatureController_h

class TemperatureController{
    public:
        bool _output = false;
        TemperatureController(double* desiredTemperature, double* temperature, Heater* heater);
        void update();

    private:
        
        Heater* _heater;
        AutoPIDRelay _myPID;
};

#endif