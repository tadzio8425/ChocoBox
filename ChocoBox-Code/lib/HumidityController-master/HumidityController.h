#include "Arduino.h"
#include <Humidifier.h>

#ifndef HumidityController_h
#define HumidityController_h

class HumidityController
    {
        public:
            HumidityController(float* humidity_01, float* humidity_02, Humidifier* humidifier);
            void update();
            void setDesiredHumidity(float* desiredHumidity);
            void setOffSetHumidity(float* offSetHumidity);
        private:
            float* _humidity_01;
            float* _humidity_02;
            float* _desiredHumidity;
            float* _offSetHumidity;
            Humidifier* _humidifier;
    };

#endif