#include "Arduino.h"
#include <Humidifier.h>

#ifndef HumidityController_h
#define HumidityController_h

class HumidityController
    {
        public:
            HumidityController(float* humidity, Humidifier* humidifier);
            void update();
            void setDesiredHumidity(float* desiredHumidity);
            void setOffSetHumidity(float* offSetHumidity);
        private:
            float* _humidity;
            float* _desiredHumidity;
            float* _offSetHumidity;
            Humidifier* _humidifier;
    };

#endif