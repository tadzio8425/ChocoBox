#include "Arduino.h"

#ifndef Interpol_h
#define Interpol_h

class Interpol{
    public:
        float* cubicSpline(float* x, float* v, float* xq);
        float* gaussSeidelSOR(float** A, float* b, int n, float w);
};

#endif