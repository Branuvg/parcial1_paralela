# Compilar con GCC (trae OpenMP). En Linux: make
# En macOS: brew install gcc  &&  make CC=gcc-16
CC      = gcc
CFLAGS  = -O2 -Wall
OMP     = -fopenmp
LDLIBS  = -lm

SEC = secuencial
PAR = paralelo

all: riemann histograma

riemann:    $(SEC)/riemann_secuencial    $(PAR)/riemann_paralelo
histograma: $(SEC)/histograma_secuencial $(PAR)/histograma_paralelo

$(SEC)/riemann_secuencial: $(SEC)/riemann_secuencial.c
	$(CC) $(CFLAGS) -o $@ $< $(LDLIBS)

$(PAR)/riemann_paralelo: $(PAR)/riemann_paralelo.c
	$(CC) $(CFLAGS) $(OMP) -o $@ $< $(LDLIBS)

$(SEC)/histograma_secuencial: $(SEC)/histograma_secuencial.c
	$(CC) $(CFLAGS) -o $@ $< $(LDLIBS)

$(PAR)/histograma_paralelo: $(PAR)/histograma_paralelo.c
	$(CC) $(CFLAGS) $(OMP) -o $@ $< $(LDLIBS)

# make run NOMBRE=Ernesto  -> compila y corre el benchmark de speedup/eficiencia
run: all
	python3 benchmark.py $(NOMBRE)

clean:
	rm -f $(SEC)/riemann_secuencial $(PAR)/riemann_paralelo \
	      $(SEC)/histograma_secuencial $(PAR)/histograma_paralelo

.PHONY: all riemann histograma run clean
