#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

/* Función a integrar. */
static inline double f(double x) {
    return sin(x) * exp(-x * x / 1000.0) + x * x;
}

/* Suma de Riemann (punto medio) en [a, b] con n rectángulos. */
double riemann_secuencial(double a, double b, long n) {
    const double dx = (b - a) / (double) n;
    double area_total = 0.0;

    for (long i = 0; i < n; i++) {
        double xi = a + (i + 0.5) * dx;
        double altura = f(xi);
        area_total += altura * dx;
    }

    return area_total;
}

int main(void) {
    long n = 1000000000L; /* fijo en 10^9 */
    double a = 0.0;
    double b = 10000000.0;

    struct timespec t_ini, t_fin;
    clock_gettime(CLOCK_MONOTONIC, &t_ini);

    double resultado = riemann_secuencial(a, b, n);

    clock_gettime(CLOCK_MONOTONIC, &t_fin);
    double tiempo = (t_fin.tv_sec - t_ini.tv_sec) +
                     (t_fin.tv_nsec - t_ini.tv_nsec) / 1e9;

    printf("=== Suma de Riemann (SECUENCIAL) ===\n");
    printf("Intervalo:        [%.2f, %.2f]\n", a, b);
    printf("Rectangulos (n):  %ld\n", n);
    printf("Area aproximada:  %.10f\n", resultado);
    printf("Tiempo (s):       %.6f\n", tiempo);

    return 0;
}
