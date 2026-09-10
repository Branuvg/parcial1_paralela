# Examen Parcial 1: Consultoría HPC usando OpenMP

**Universidad del Valle de Guatemala**
Facultad de Ingeniería
Departamento de Ciencias de la Computación
CC3069 - Computación Paralela y Distribuida
Semestre II - 2026

---

## Contexto

Cualquier programador puede escribir un ciclo *for*. Sin embargo, el verdadero valor de un ingeniero en High Performance Computing (HPC) viene en saber cuándo utilizar la paralelización y cuándo no. Muchos incluso aún no conocen sobre este tipo de implementaciones a pesar de existir ya por mucho tiempo. Ya saben que el objetivo de la paralelización no es simplemente "usar más threads", sino comprender el hardware para obtener una mejora real en los tiempos de procesamiento. Algunos algoritmos se pueden dividir de forma natural y sencilla, mientras que otros sufren tanto overhead (gasto de recursos creando y destruyendo threads) que la versión paralela termina siendo más lenta que la secuencial.

En esta actividad ustedes conforman una firma de consultoría de optimización de software. Dado que cada integrante del trío trae consigo la experiencia de haber analizado previamente uno de los 5 problemas originales (Histograma, Suma de Riemann, Multiplicación de Matrices, Grafos o Blur), la primera tarea de su consultora es tomar decisiones estratégicas.

Su primer paso es establecer la infraestructura de trabajo colaborativo. En equipo, discutan brevemente las soluciones que cada uno propuso originalmente para el algoritmo secuencial y consideraciones para convertirlos en paralelo. Basados en esa discusión, deberán elegir exactamente **2 problemas** para implementar en paralelo (de los 3 posibles que tienen en cada grupo).

## Infraestructura de trabajo

1. Creen un único repositorio en GitHub (o lo que usen de preferencia) por grupo. Los grupos están asignados para que puedan dialogar al menos uno de cada algoritmo original.
2. Creen un archivo `README.md` en la raíz del repositorio. Este archivo debe contener el nombre creativo de su "Consultora HPC" y los nombres completos de los tres integrantes.
3. Cada integrante del equipo debe clonar el repositorio en su máquina y realizar al menos un commit. Deben crear la siguiente estructura de carpetas básica:
   - `/secuencial` (Para los algoritmos base).
   - `/paralelo` (Para las soluciones optimizadas con OpenMP).
   - `/docs` (Para reportes, gráficas y análisis de datos).

## Documentación

La documentación debe contener, como mínimo, los siguientes elementos:

1. **Contexto y Datos:** Descripción de los 2 problemas elegidos junto con la propuesta secuencial original y la justificación de los datos de prueba utilizados (tamaño de la muestra, origen, estructuras de datos en memoria).
2. **Estrategia de Paralelización:** Luego de implementar su solución en paralelo ¿Qué directivas de OpenMP utilizaron y por qué? ¿Cómo evitaron las condiciones de carrera (race conditions) o el desbalance de carga (con el scheduling)?
3. **Resultados y Métricas (Requisito Individual):** Esta es la sección más importante. Argumentar cómo sus decisiones realmente mejoraron su algoritmo secuencial. Para garantizar la participación equitativa, el informe debe contener las mediciones de speedup y eficiencia respectivas por cada integrante con pruebas de corridas (screenshot o video de sus ejecuciones).
