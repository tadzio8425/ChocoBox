#include "Arduino.h"
#include <Heater.h>	

#ifndef TemperatureController_h
#define TemperatureController_h

class TemperatureController{
    public:
        TemperatureController(float* temperature_01, float* temperature_02, Heater* heater);
        void setDesiredTemperature(float* desiredTemperature);
        void setOffSetTemperature(float* offSetTemperature);
        void update();

    private:
        float* _temperature_01;
        float* _temperature_02;
        float* _desiredTemperature;
        float* _offSetTemperature;
        Heater* _heater;
};

#endif