#include "Arduino.h"
#include <Interpol.h>

float* Interpol::cubicSpline(float* x, float* v, float* xq) {
    int n = sizeof(x) / sizeof(float);
    int m = n - 1;
    float* y = v;

    float* delta_x = new float[m];
    float* delta_y = new float[m];
    float* d = new float[m];

    // Se definen las ecuaciones base de diferencias entre puntos
    for (int i = 1; i < n; i++) {
        delta_y[i - 1] = y[i] - y[i - 1];
        delta_x[i - 1] = x[i] - x[i - 1];
        d[i - 1] = delta_y[i - 1] / delta_x[i - 1];
    }

    // Se contruye la matriz A de tamaño nxn
    float** A = new float*[n];
    for (int f = 0; f < n; f++) {
        A[f] = new float[n];
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

    // Se contruye el vector b
    float* b = new float[n];
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

    // Se utiliza Gauss-Seidel para encontrar los coeficientes K
    float* k = gaussSeidelSOR(A, b, n, 1.3);

    // Con los coeficientes encontrados, se procede a generar las ecuaciones
    float paso = xq[1] - xq[0];

    delta_x = new float[n];
    delta_x[0] = 0;
    for (int i = 1; i < n; i++) {
        delta_x[i] = x[i] - x[i - 1];
    }

    d = new float[n];
    d[0] = 0;
    for (int i = 1; i < n; i++) {
        d[i] = delta_y[i - 1] / delta_x[i];
    }

    int contador = 0;
    int arraySize = static_cast<int>((xq[n - 1] - xq[0]) / paso);
    float* vq = new float[arraySize];
    
    // Iterar a través de los puntos en x
    for (int i = 0; i < m; i++) {
        float inicio = x[i];
        float final = x[i + 1];
        float rango = inicio;

        while (rango <= final) {
            float jx = rango;
            if (jx == inicio) {
                vq[contador] = y[i];
            } else if (jx == final) {
                vq[contador] = y[i + 1];
            } else {
                float t = (jx - x[i]) / delta_x[i + 1];
                float tp = 1 - t;
                float q = (t * y[i + 1]) + (tp * y[i]) + delta_x[i + 1] * (((k[i] - d[i + 1]) * t * (tp * tp)) - (k[i + 1] - d[i + 1]) * (t * t) * tp);
                vq[contador] = q;
            }
            contador++;
            rango += paso;
        }
    }

    // Liberar memoria
    delete[] delta_x;
    delete[] delta_y;
    delete[] d;
    for (int f = 0; f < n; f++) {
        delete[] A[f];
    }
    delete[] A;
    delete[] b;
    delete[] k;

    return vq;
}


float* gaussSeidelSOR(float** A, float* b, int n, float w) {
    float eps = 1e-6;
    int N = 100;
    int k = 1; // Iteración actual
    float dif = 1; // Diferencia inicial

    float* x0 = new float[n]; // Valores iniciales
    float* x = new float[n]; // Vector con la solución

    for (int i = 0; i < n; i++) {
        x0[i] = 0; // Inicializar valores iniciales en 0
        x[i] = 0; // Inicializar solución en 0
    }

    while (k <= N && dif > eps) {
        for (int i = 0; i < n; i++) {
            // Sumatoria para los valores ya calculados (iteración actual)
            float sum_j_act = 0;
            for (int j = 0; j < i; j++) {
                if (j != i) {
                    sum_j_act += (-A[i][j] * x[j]);
                }
            }

            // Sumatoria para los valores todavía no calculados (iteración anterior)
            float sum_j_ant = 0;
            for (int j = i + 1; j < n; j++) {
                if (j != i) {
                    sum_j_ant += (-A[i][j] * x0[j]);
                }
            }

            // Cálculo de la incógnita actual, siendo sobre-relajada por w
            x[i] = (1 - w) * x0[i] + (w / A[i][i]) * (sum_j_act + sum_j_ant + b[i]);
        }

        dif = 0;
        for (int i = 0; i < n; i++) {
            dif = std::max(dif, std::abs(x0[i] - x[i])); // Calcular la máxima diferencia entre x0 y x
        }

        k++;
        for (int i = 0; i < n; i++) {
            x0[i] = x[i];
        }
    }

    delete[] x0;

    return x;
}

