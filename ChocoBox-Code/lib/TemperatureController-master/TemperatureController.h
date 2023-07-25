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
        double _output = 0;
        Heater* _heater;
        AutoPID _myPID;
};

#endif