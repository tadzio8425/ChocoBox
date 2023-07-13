#include "Arduino.h"
#include <Heater.h>	

#ifndef TemperatureController_h
#define TemperatureController_h

class TemperatureController{
    public:
        TemperatureController(float* temperature, Heater* heater);
        void setDesiredTemperature(float* desiredTemperature);
        void setOffSetTemperature(float* offSetTemperature);
        void update();

    private:
        float* _temperature;
        float* _desiredTemperature;
        float* _offSetTemperature;
        Heater* _heater;
};

#endif