# Consultoria THE BIG THREE

## Integrantes del equipo:
- Gabriel Bran - 23590
- Ana Laura - 231645
- Ernesto Ascencio - 23009

## Estructura

Dos programas, cada uno con versión secuencial y paralela (OpenMP):

- `secuencial/riemann_secuencial.c` · `paralelo/riemann_paralelo.c` — suma de Riemann.
- `secuencial/histograma_secuencial.c` · `paralelo/histograma_paralelo.c` — histograma de temperaturas.
- `benchmark.py` — compila y corre los 4 binarios, mide speedup/eficiencia y escribe `docs/resultados_<nombre>.csv`.

## Compilar

- Linux: `sudo apt install gcc make python3`
- macOS: `brew install gcc` (luego `make CC=gcc-16`)

## Uso rápido

```bash
make                       # requiere GCC con OpenMP (macOS: brew install gcc && make CC=gcc-16)
make run NOMBRE=TuNombre   # compila y corre benchmark.py
```
