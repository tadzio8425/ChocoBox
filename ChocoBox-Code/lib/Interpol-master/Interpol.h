#include "Arduino.h"
#include <vector>
#include <cmath>
#ifndef Interpol_h
#define Interpol_h

class Interpol{
    public:
        std::vector<float> cubicSpline(const std::vector<float>& x, const std::vector<float>& v, const std::vector<float>& xq);
        std::vector<float> gaussSeidelSOR(std::vector<std::vector<float>>& A, std::vector<float>& b, int n, float w);
        std::vector<float> generateXq(float start, float end, float step);
};

#endif