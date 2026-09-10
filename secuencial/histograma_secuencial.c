#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_CUBETAS 100

// Genera un arreglo de N temperaturas aleatorias entre minTemp y maxTemp
float* generarMediciones(long long N, float minTemp, float maxTemp) {
    float *arreglo = malloc(N * sizeof(float));
    if (arreglo == NULL) {
        return NULL;
    }
    for (long long i = 0; i < N; i++) {
        arreglo[i] = minTemp + ((float)rand() / RAND_MAX) * (maxTemp - minTemp);
    }
    return arreglo;
}

// Recorre el arreglo para encontrar min y max reales dentro de los datos
void encontrarMinMax(float *arreglo, long long N, float *min, float *max) {
    *min = arreglo[0];
    *max = arreglo[0];
    for (long long i = 1; i < N; i++) {
        if (arreglo[i] < *min) *min = arreglo[i];
        if (arreglo[i] > *max) *max = arreglo[i];
    }
}

// Clasifica el arreglo en NUM_CUBETAS cubetas
void clasificarEnCubetas(float *arreglo, long long N, int histograma[NUM_CUBETAS],
                          float min, float max) {
    float anchoRango = (max - min) / NUM_CUBETAS;

    for (long long i = 0; i < N; i++) {
        int indice = (int)((arreglo[i] - min) / anchoRango);
        if (indice >= NUM_CUBETAS) indice = NUM_CUBETAS - 1;
        if (indice < 0) indice = 0;
        histograma[indice]++;
    }
}

void imprimirHistograma(int histograma[NUM_CUBETAS], float min, float anchoRango) {
    printf("\nHistograma (100 cubetas):\n");
    for (int j = 0; j < NUM_CUBETAS; j++) {
        float lo = min + j * anchoRango;
        float hi = min + (j + 1) * anchoRango;
        printf("Cubeta %3d [%8.2f, %8.2f): %d\n", j, lo, hi, histograma[j]);
    }
}

int main(void) {
    srand((unsigned int)time(NULL));

    long long N = 10000000;   // cantidad de mediciones a simular
    float *arreglo = generarMediciones(N, -30.0f, 50.0f); // temperaturas entre -30 y 50

    float min, max;
    encontrarMinMax(arreglo, N, &min, &max);

    int histograma[NUM_CUBETAS] = {0};
    clasificarEnCubetas(arreglo, N, histograma, min, max);

    imprimirHistograma(histograma, min, (max - min) / NUM_CUBETAS);

    free(arreglo);
    return 0;
}