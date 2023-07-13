#include "Arduino.h"
#include <MovingAverage.h>

void MovingAverage::append(float* varPointer){
    _len++; //Se suma 1 a la variable len
    _avg_buffer[_len - 1] = varPointer; //Se agrega el apuntador al buffer
}

float MovingAverage::getAverage(){

    float sum = 0;

    for(int i = 0; i < _len; i++){
        sum += (*_avg_buffer[i]);
    }

    return sum/_len;
}