#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define NUM_CUBETAS 100
#define SEED_DATASET 42u

static void generar(float *a, long long N, float lo, float hi) {
    #pragma omp parallel
    {
        unsigned int seed = SEED_DATASET + 7919u * (unsigned)omp_get_thread_num();
        #pragma omp for schedule(static)
        for (long long i = 0; i < N; i++)
            a[i] = lo + ((float)rand_r(&seed) / RAND_MAX) * (hi - lo);
    }
}

static void minMax(const float *a, long long N, float *min, float *max) {
    float mn = a[0], mx = a[0];
    #pragma omp parallel for schedule(static) reduction(min:mn) reduction(max:mx)
    for (long long i = 1; i < N; i++) {
        if (a[i] < mn) mn = a[i];
        if (a[i] > mx) mx = a[i];
    }
    *min = mn; *max = mx;
}

static void clasificar(const float *a, long long N, long long *hist, float min, float max) {
    float ancho = (max - min) / NUM_CUBETAS;
    #pragma omp parallel
    {
        long long local[NUM_CUBETAS] = {0};
        #pragma omp for schedule(static) nowait
        for (long long i = 0; i < N; i++) {
            int idx = (int)((a[i] - min) / ancho);
            if (idx >= NUM_CUBETAS) idx = NUM_CUBETAS - 1;
            if (idx < 0) idx = 0;
            local[idx]++;
        }
        #pragma omp critical
        for (int j = 0; j < NUM_CUBETAS; j++) hist[j] += local[j];
    }
}

static void clasificarSeq(const float *a, long long N, long long *hist, float min, float max) {
    float ancho = (max - min) / NUM_CUBETAS;
    for (long long i = 0; i < N; i++) {
        int idx = (int)((a[i] - min) / ancho);
        if (idx >= NUM_CUBETAS) idx = NUM_CUBETAS - 1;
        if (idx < 0) idx = 0;
        hist[idx]++;
    }
}

static int verificar(long long N) {
    float *a = malloc(N * sizeof(float));
    if (!a) { fprintf(stderr, "sin memoria\n"); return 1; }
    unsigned int seed = SEED_DATASET;
    for (long long i = 0; i < N; i++)
        a[i] = -30.0f + ((float)rand_r(&seed) / RAND_MAX) * 80.0f;

    float mnP, mxP, mnS = a[0], mxS = a[0];
    for (long long i = 1; i < N; i++) { if (a[i] < mnS) mnS = a[i]; if (a[i] > mxS) mxS = a[i]; }
    minMax(a, N, &mnP, &mxP);

    long long hP[NUM_CUBETAS] = {0}, hS[NUM_CUBETAS] = {0};
    clasificar(a, N, hP, mnP, mxP);
    clasificarSeq(a, N, hS, mnS, mxS);

    int ok = (mnP == mnS && mxP == mxS);
    for (int j = 0; j < NUM_CUBETAS && ok; j++) if (hP[j] != hS[j]) ok = 0;
    free(a);

    printf("Verificacion: %s\n", ok ? "OK (histograma identico al secuencial)" : "FALLO");
    return ok ? 0 : 1;
}

int main(int argc, char **argv) {
    long long N = 50000000L;
    int printHist = 0, doVerificar = 0;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--print-histograma") == 0) printHist = 1;
        else if (strcmp(argv[i], "--verificar") == 0) doVerificar = 1;
        else N = atoll(argv[i]);
    }
    if (N < 1) { fprintf(stderr, "N debe ser >= 1\n"); return 1; }

    if (doVerificar) return verificar(N);

    float *a = malloc(N * sizeof(float));
    if (!a) { fprintf(stderr, "sin memoria\n"); return 1; }

    generar(a, N, -30.0f, 50.0f);

    double t_ini = omp_get_wtime();
    generar(a, N, -30.0f, 50.0f);
    float min, max;
    minMax(a, N, &min, &max);
    long long hist[NUM_CUBETAS] = {0};
    clasificar(a, N, hist, min, max);
    double tiempo = omp_get_wtime() - t_ini;

    printf("=== Histograma de temperaturas (PARALELO - OpenMP) ===\n");
    printf("Mediciones (N):   %lld\n", N);
    printf("Cubetas:          %d\n", NUM_CUBETAS);
    printf("Hilos usados:     %d\n", omp_get_max_threads());
    printf("Rango [min, max]: [%.4f, %.4f]\n", min, max);
    printf("Tiempo (s):       %.6f\n", tiempo);

    if (printHist) {
        float ancho = (max - min) / NUM_CUBETAS;
        printf("\nHistograma (%d cubetas):\n", NUM_CUBETAS);
        for (int j = 0; j < NUM_CUBETAS; j++)
            printf("Cubeta %3d [%8.2f, %8.2f): %lld\n",
                   j, min + j * ancho, min + (j + 1) * ancho, hist[j]);
    }

    free(a);
    return 0;
}
