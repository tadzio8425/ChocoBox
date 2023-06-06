#include "Arduino.h"
#include <Interpol.h>
#include <vector>
#include <cmath>

std::vector<float> Interpol::generateXq(float start, float end, float step) {
    std::vector<float> xq;
    for (float x = start; x <= end; x += step) {
        xq.push_back(x);
    }
    return xq;
}


std::vector<float> Interpol::cubicSpline(const std::vector<float>& x, const std::vector<float>& v, const std::vector<float>& xq) {
    int n = v.size();
    int m = n - 1;
    std::vector<float> y = v;

    std::vector<float> delta_x(m);
    std::vector<float> delta_y(m);
    std::vector<float> d(m);

    for (int i = 1; i < n; i++) {
        delta_y[i - 1] = y[i] - y[i - 1];
        delta_x[i - 1] = x[i] - x[i - 1];
        d[i - 1] = delta_y[i - 1] / delta_x[i - 1];
    }

    std::vector<std::vector<float>> A(n, std::vector<float>(n, 0));
    for (int f = 0; f < n; f++) {
        for (int c = 0; c < n; c++) {
            if (f == c) {
                if (c == 0 || c == n - 1) {
                    A[f][c] = 2;
                } else {
                    A[f][c] = 2 * (delta_x[c - 1] + delta_x[c]);
                }
            }

            if (f == c - 1) {
                if (c == 1) {
                    A[f][c] = 1;
                } else {
                    A[f][c] = delta_x[c - 2];
                }
            }

            if (c == f - 1) {
                if (c == n - 2) {
                    A[f][c] = 1;
                } else {
                    A[f][c] = delta_x[f];
                }
            }
        }
    }

    std::vector<float> b(n, 0);
    for (int i = 0; i < n; i++) {
        if (i == 0) {
            b[i] = d[i];
        } else if (i == n - 1) {
            b[i] = d[m];
        } else {
            b[i] = d[i - 1] * delta_x[i] + d[i] * delta_x[i - 1];
        }
    }

    for (int i = 0; i < n; i++) {
        b[i] = 3 * b[i];
    }

    std::vector<float> k = gaussSeidelSOR(A, b, n, 1.3);

    std::vector<float> vq;
    float paso = xq[1] - xq[0];
    std::vector<float> delta_x_extended = {0};
    delta_x_extended.insert(delta_x_extended.end(), delta_x.begin(), delta_x.end());
    std::vector<float> d_extended = {0};
    d_extended.insert(d_extended.end(), d.begin(), d.end());
    int contador = 1;

    for (int i = 0; i < m; i++) {
        float inicio = x[i];
        float final = x[i + 1];
        std::vector<float> rango;
        for (float jx = inicio; jx <= final; jx += paso) {
            rango.push_back(jx);
        }

        for (int pasos = 1; pasos < rango.size(); pasos++) {
            float jx = inicio + paso * pasos;
            if (jx == inicio) {
                vq.push_back(v[i]);
            } else if (jx == final) {
                vq.push_back(v[i + 1]);
            } else {
                float t = (jx - x[i]) / delta_x_extended[i + 1];
                float tp = 1 - t;
                float q = (t * v[i + 1]) + (tp * v[i]) + delta_x_extended[i + 1] * (((k[i] - d_extended[i + 1]) * t * (tp * tp)) - (k[i + 1] - d_extended[i + 1]) * (t * t * tp));
                vq.push_back(q);
            }
            contador++;
        }
    }

    return vq;
}

std::vector<float> Interpol::gaussSeidelSOR(std::vector<std::vector<float>>& A, std::vector<float>& b, int n, float w) {
    float eps = 1e-6;
    int N = 100;
    int k = 1; // Iteration counter
    float dif = 1; // Initial difference

    std::vector<float> x0(n, 0.0f); // Initial values
    std::vector<float> x(n, 0.0f); // Solution vector

    while (k <= N && dif > eps) {
        for (int i = 0; i < n; i++) {
            float sum_j_act = 0;
            for (int j = 0; j < i; j++) {
                sum_j_act += -A[i][j] * x[j];
            }

            float sum_j_ant = 0;
            for (int j = i + 1; j < n; j++) {
                sum_j_ant += -A[i][j] * x0[j];
            }

            x[i] = (1 - w) * x0[i] + (w / A[i][i]) * (sum_j_act + sum_j_ant + b[i]);
        }

        dif = 0;
        for (int i = 0; i < n; i++) {
            dif = std::max(dif, std::abs(x0[i] - x[i]));
        }

        k++;
        x0 = x;
    }

    return x;
}



