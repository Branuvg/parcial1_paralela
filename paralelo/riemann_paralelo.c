#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

/* Función a integrar (idéntica a la versión secuencial). */
static inline double f(double x) {
    return sin(x) * exp(-x * x / 1000.0) + x * x;
}

double riemann_paralelo(double a, double b, long n) {
    const double dx = (b - a) / (double) n;
    double area_total = 0.0;

    #pragma omp parallel for schedule(static) reduction(+:area_total)
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

    int hilos = omp_get_max_threads();

    double t_ini = omp_get_wtime();
    double resultado = riemann_paralelo(a, b, n);
    double tiempo = omp_get_wtime() - t_ini;

    printf("=== Suma de Riemann (PARALELO - OpenMP) ===\n");
    printf("Intervalo:        [%.2f, %.2f]\n", a, b);
    printf("Rectangulos (n):  %ld\n", n);
    printf("Hilos usados:     %d\n", hilos);
    printf("Area aproximada:  %.10f\n", resultado);
    printf("Tiempo (s):       %.6f\n", tiempo);

    return 0;
}
