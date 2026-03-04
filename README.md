# AlgorithmDes

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
├── isolate_testing/      # Scripts de benchmarking de rendimiento
│   ├── BTBenchmark.py        # Benchmark de Backtracking (n, T = %sum(t))
│   ├── BBWorstCase.py        # Benchmark del peor caso del B&B (n fijo, T variable)
│   ├── DeviationBT.py        # Benchmark BT con análisis de desviación estándar
│   ├── DPBenchmark.py        # Benchmark de Programación Dinámica
│   ├── GreedyBenchmark.py    # Benchmark de Greedy (n hasta 40000000)
│   ├── perf_BT.c             # Código C para medir BT
│   ├── perf_DP.c             # Código C para medir DP
│   ├── perf_GD.c             # Código C para medir Greedy
│   ├── check.c               # Código C para verificar correctitud de las soluciones
│   ├── starterGreedy.c       # Implementación inicial del algoritmo Greedy (BubbleSort)
│   ├── performance_BT_Deviation.png  # Gráfico de desviación estándar BT
│   └── performance_greedy_avg.png    # Gráfico de rendimiento promedio Greedy
├── graphs/               # Gráficos de rendimiento generados por los benchmarks
│   ├── BT
│   │   ├── performance_TrulyNaiveTest.png   # BT sin poda ni valores
│   │   ├── performance_basicBT.png          # BT básico con valores
│   │   ├── performance_IniTest.png          # BT sin poda
│   │   ├── performance_NaivePruning.png     # BT con poda ingenua
│   │   ├── performance_TpruningBT.png       # BT con poda de tiempo
│   │   ├── performance_FullPruning.png      # BT con cota ineficiente
│   │   ├── FinalBT.png                      # BT con cota eficiente pero solución lenta O(n) por nodo
│   │   ├── B&BDFS.png                       # BT B&B Final con solución rápida O(log(n)) por nodo
│   │   ├── PrefixSum.png                    # BT con prefix sums (optimización del límite superior)
│   │   ├── performance_BT_Deviation.png     # Análisis de desviación estándar del BT final
│   │   ├── CoeficienteBinomial.png          # Peor caso del B&B: coeficiente binomial C(n, n/2)
│   │   └── Best BT graph.png               # Mejor gráfico de rendimiento del BT final
│   ├── GD
│   │   ├── performance_greedyStart.png      # Greedy (BubbleSort)
│   │   ├── performance_pruningGD.png        # Greedy con poda sobre T
│   │   └── performance_greedyFinal.png      # Greedy modificado (qsort)
│   └── DP
│       ├── performance_dp_avg.png           # DP básico
│       └── performance_dpoptimized.png      # DP optimizado para acceso a cache
└── Context.pdf           # Enunciado oficial del laboratorio
```

---

## Uso

Compilar y ejecutar cada test directamente con `gcc`:

```bash
# Backtracking: n temas, T = T_mult% de la suma total de tiempos
gcc test_backtracking.c -o test_bt -lm && ./test_bt <n> <T_mult>

# Programación Dinámica: n temas, tiempo máximo T
gcc test_dinamico.c -o test_dp -lm && ./test_dp <n> <T>

# Goloso: n temas, T = T_mult% de la suma total de tiempos
gcc test_goloso.c -o test_greedy -lm && ./test_greedy <n> <T_mult>
```

Los tiempos de entrada se generan aleatoriamente (`t[i]`, `p[i]` ∈ [1, 10]) (Pueden ser modificados).

Para los benchmarks de rendimiento, ejecutar los scripts Python desde `isolate_testing/`:

```bash
cd isolate_testing
python3 BTBenchmark.py
python3 DPBenchmark.py
python3 GreedyBenchmark.py
```
