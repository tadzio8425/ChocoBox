#include "Arduino.h"
#ifndef MovingAverage_h
#define MovingAverage_h

class MovingAverage{
    public:
        void append(float* varPointer);
        float getAverage();

    private:
        float* _avg_buffer[50]; 
        int _len = 0;
};

#endif