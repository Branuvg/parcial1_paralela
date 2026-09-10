#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NUM_CUBETAS 100
#define SEED_DATASET 42u

/* Llena `a` con N temperaturas pseudoaleatorias en [lo, hi]. */
static void generar(float *a, long long N, float lo, float hi) {
    unsigned int seed = SEED_DATASET;
    for (long long i = 0; i < N; i++)
        a[i] = lo + ((float)rand_r(&seed) / RAND_MAX) * (hi - lo);
}

static void encontrarMinMax(const float *a, long long N, float *min, float *max) {
    float mn = a[0], mx = a[0];
    for (long long i = 1; i < N; i++) {
        if (a[i] < mn) mn = a[i];
        if (a[i] > mx) mx = a[i];
    }
    *min = mn; *max = mx;
}

static void clasificarEnCubetas(const float *a, long long N, long long *histograma,
                                float min, float max) {
    float ancho = (max - min) / NUM_CUBETAS;
    for (long long i = 0; i < N; i++) {
        int idx = (int)((a[i] - min) / ancho);
        if (idx >= NUM_CUBETAS) idx = NUM_CUBETAS - 1;
        if (idx < 0) idx = 0;
        histograma[idx]++;
    }
}

int main(int argc, char **argv) {
    long long N = 50000000L;
    int printHist = 0;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--print-histograma") == 0) printHist = 1;
        else N = atoll(argv[i]);
    }
    if (N < 1) { fprintf(stderr, "N debe ser >= 1\n"); return 1; }

    float *a = malloc(N * sizeof(float));
    if (a == NULL) { fprintf(stderr, "sin memoria\n"); return 1; }

    generar(a, N, -30.0f, 50.0f);

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);

    generar(a, N, -30.0f, 50.0f);
    float min, max;
    encontrarMinMax(a, N, &min, &max);
    long long histograma[NUM_CUBETAS] = {0};
    clasificarEnCubetas(a, N, histograma, min, max);

    clock_gettime(CLOCK_MONOTONIC, &t1);
    double tiempo = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;

    printf("=== Histograma de temperaturas (SECUENCIAL) ===\n");
    printf("Mediciones (N):   %lld\n", N);
    printf("Cubetas:          %d\n", NUM_CUBETAS);
    printf("Rango [min, max]: [%.4f, %.4f]\n", min, max);
    printf("Tiempo (s):       %.6f\n", tiempo);

    if (printHist) {
        float ancho = (max - min) / NUM_CUBETAS;
        printf("\nHistograma (%d cubetas):\n", NUM_CUBETAS);
        for (int j = 0; j < NUM_CUBETAS; j++)
            printf("Cubeta %3d [%8.2f, %8.2f): %lld\n",
                   j, min + j * ancho, min + (j + 1) * ancho, histograma[j]);
    }

    free(a);
    return 0;
}
