# Knapsack 0-1

## Laboratorio 2 — Diseño de Algoritmos

**Curso:** Diseño de Algoritmos  
**Profesores:** Javier Baladrón, Pablo Román  
**Fecha de inicio:** 5 de enero 2026  

---

## Descripción del Problema

El repositorio resuelve un problema de **selección óptima de temas de estudio**, una variante del problema de la mochila 0/1 (_Knapsack Problem_):

> Un estudiante dispone de un tiempo máximo **T** para preparar un examen. Existen **n** temas, cada uno con un tiempo requerido `t[i]` y un puntaje `p[i]`. El objetivo es seleccionar un subconjunto de temas que maximice el puntaje total sin exceder el tiempo disponible. Un tema se estudia completamente o no se estudia.

**Formalmente:** dados `t[1..n]`, `p[1..n]` y `T`, encontrar `S ⊆ {1,…,n}` tal que `∑t[i] ≤ T` y `∑p[i]` sea máximo.

---

## Algoritmos Implementados

Los tres algoritmos están definidos en `algoritmos.c` y comparten la estructura `Tema { tiempo, puntaje, ratio, index }`.

| Algoritmo | Función | Complejidad | Optimalidad |
|-----------|---------|-------------|-------------|
| **Backtracking** | `BT_Solve` | Caso Promedio O(n log n) | ✅ Óptimo |
| **Programación Dinámica** | `DP_Solve` | Pseudo-polinomial — O(n·T) | ✅ Óptimo |
| **Goloso** | `Greedy_Solve` | O(n log n) | ⚠️ >= 50% del Optimo |

### Backtracking (`BT_Solve`)
Explora recursivamente todas las combinaciones de temas ordenados por ratio `p[i]/t[i]`. Implementa **poda por cota superior**: si el valor actual más el valor potencial restante (suma de puntajes de temas aún no evaluados) no supera el mejor encontrado, se descarta la rama. Utiliza Upper Pruning y Prefix Sums especializando el algorimo de Backtracking usando B&B.
CAMBIO: Cota superior: Fractional Knapsack - Cota inferior: Knapsack 0/1 (Greedy)

### Programación Dinámica (`DP_Solve`)
Solución _bottom-up_ con tabla `DP[i][j]` = máximo puntaje usando los primeros `i` temas con tiempo `j`. Reconstruye la solución trackeando decisiones tomadas.
CAMBIO: Optimizado para mejorar acceso a cache.

### Algoritmo Goloso (`Greedy_Solve`)
Ordena los temas por ratio `puntaje/tiempo` de mayor a menor y los selecciona de forma voraz mientras haya tiempo disponible. No garantiza solución óptima.
CAMBIO: Modificado para asegurar al menos un 50% del valor optimo.

---

## Estructura del Repositorio

```
AlgorithmDes/
├── algoritmos.c          # Implementación de los tres algoritmos (BT, DP, Greedy)
├── test_backtracking.c   # Runner: ejecuta BT con n y T como args
├── test_dinamico.c       # Runner: ejecuta DP con n y T como args
├── test_goloso.c         # Runner: ejecuta Greedy con n y T como args
├── makefile              # Compila todos los ejecutables (usa GCC -O0; BT con stack 1 GiB)
├── isolate_testing/      # Scripts de benchmarking de rendimiento
│   ├── BTBenchmark.py    # Benchmark de Backtracking (n, T = %sum(t))
│   ├── DPBenchmark.py    # Benchmark de Programación Dinámica
│   ├── GreedyBenchmark.py# Benchmark de Greedy (n hasta 40000000)
│   ├── perf_BT.c         # Código C para medir BT
│   ├── perf_DP.c         # Código C para medir DP
│   └── perf_GD.c         # Código C para medir Greedy
├── graphs/               # Gráficos de rendimiento generados por los benchmarks
│   ├── BT
│   │   ├── performance_TrulyNaiveTest.png   # BT sin poda ni valores
│   │   ├── performance_IniTest.png          # BT sin poda
│   │   ├── performance_TpruningBT.png       # BT con poda de tiempo
│   │   ├── performance_FullPruning.png      # BT con cota ineficiente
│   │   ├── FinalBT.png                      # BT con cota eficiente pero solucion lenta O(n) por nodo
│   │   └── B&BDFS.png                       # BT B&B Final con solucion rapida O(log(n)) por nodo
│   ├── GD
│   │   ├── performance_greedyStart.png      # Greedy (BubbleSort)
│   │   ├── performance_pruningGD.png        # Greedy con poda sobre T
│   │   └── performance_greedyFinal.png      # Greedy modificado (qsort)
│   └── DP
│       ├── performance_dp_avg.png           # DP básico
│       └── performance_dpoptimized.png      # DP optimizado para acceso a cache
├── Context.pdf           # Enunciado oficial del laboratorio
└── Informe.pdf           # Análisis completo: complejidad, correctitud y benchmarks
```

---

## Entorno y Requisitos

- **Compilador:** GCC (compatible con C11)
- **Sistema operativo recomendado:** WSL (Windows Subsystem for Linux) u otro entorno Unix/Linux. El `makefile` incluye soporte para compilación en Windows.
- **Python 3** (para los scripts de benchmarking en `isolate_testing/`)

---

## Uso con Makefile

El repositorio incluye un `makefile` para compilar todos los ejecutables de forma sencilla.

```bash
# Compilar todos los ejecutables (test_backtracking, test_dinamico, test_goloso)
make

# Compilar un ejecutable individual
make test_backtracking
make test_dinamico
make test_goloso

# Limpiar ejecutables generados
make clean
```

> **Nota sobre el stack de Backtracking:** El `makefile` configura automáticamente un stack de 1 GiB (`-Wl,--stack,1073741824`) para `test_backtracking`, necesario para valores grandes de `n` donde la recursión profunda agota el stack por defecto (1 MiB en Windows, 8 MiB en Linux/WSL).

> **Nota sobre `make clean`:** El target `clean` usa el comando `del` de Windows. En sistemas Unix/Linux usar `rm -f *.exe` manualmente o adaptar el `makefile`.

Una vez compilado, ejecutar cada test:

```bash
# Backtracking
./test_backtracking <n>

# Programación Dinámica
./test_dinamico <n>

# Goloso
./test_goloso <n>
```

Los valores de entrada `t[i]` y `p[i]` se pueden generar aleatoriamente en el rango escogido o ingresar manualmente.
El valor de T puede se un porcentaje de la suma total de tiempos (sum(t)) o impuesto manualmente.

### Compilación manual con GCC

Si se prefiere compilar sin el `makefile`:

```bash
# Backtracking (requiere 1 GiB de stack en Windows/WSL)
gcc test_backtracking.c -o test_backtracking -O0 "-Wl,--stack,1073741824"

# Programación Dinámica
gcc test_dinamico.c -o test_dinamico -O0

# Goloso
gcc test_goloso.c -o test_goloso -O0
```

Para los benchmarks de rendimiento, ejecutar los scripts Python desde `isolate_testing/`:

```bash
cd isolate_testing
python3 BTBenchmark.py
python3 DPBenchmark.py
python3 GreedyBenchmark.py
```

---

## Guía de Selección de Algoritmo

Basado en el análisis del `Informe.pdf`:

| Escenario | Restricciones | Enfoque recomendado |
|-----------|---------------|---------------------|
| `n` bajo y `T` bajo | Ninguna | **Programación Dinámica** (rápido, óptimo y estable) |
| `n` masivo y `T` masivo | RAM y tiempo de ejecución | **Backtracking B&B** (caso promedio O(n log n)) |
| Casos simétricos | Combinatoria | **Programación Dinámica** (inmune a la simetría si `T` lo permite) |
| Tiempo real | Ejecución instantánea | **Goloso modificado** (garantiza ≥ 50% del óptimo) |

---

## Análisis Completo

Para el análisis detallado de complejidad, correctitud, limitaciones prácticas y el desarrollo iterativo de cada algoritmo, consultar [`Informe.pdf`](./Informe.pdf).
