CC      = gcc
CFLAGS  = -O2
LDLIBS  = -lm

all: secuencial paralelo

secuencial: secuencial/riemann_secuencial.c
	$(CC) $(CFLAGS) -o secuencial/riemann_secuencial secuencial/riemann_secuencial.c $(LDLIBS)

paralelo: paralelo/riemann_paralelo.c
	$(CC) $(CFLAGS) -fopenmp -o paralelo/riemann_paralelo paralelo/riemann_paralelo.c $(LDLIBS)

clean:
	rm -f secuencial/riemann_secuencial paralelo/riemann_paralelo

.PHONY: all secuencial paralelo clean
