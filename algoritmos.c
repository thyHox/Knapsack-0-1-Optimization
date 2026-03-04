#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>
#include <math.h>
#include <stdbool.h>

// Estructura para representar un tema con su tiempo, puntaje, ratio y su índice original
typedef struct Tema {
    int tiempo;
    int puntaje;
    float ratio;
    int index;
} Tema;


// Función de comparación para ordenar los temas por ratio (puntaje/tiempo) y luego por puntaje
int compareTema(const void *a, const void *b) {

    Tema *temaA = (Tema *)a;
    Tema *temaB = (Tema *)b;

    // Desempate por puntaje si los ratios son iguales
    if (fabs(temaA->ratio - temaB->ratio) < FLT_EPSILON) {
        if (temaA->puntaje < temaB->puntaje) {
            return 1;
        } else if (temaA->puntaje > temaB->puntaje) {
            return -1;
        } else {
            return 0;
        }

    // Ordenar por ratio mayor si no son iguales
    } else if (temaA->ratio < temaB->ratio) {
        return 1;
        
    } else {
        return -1;
    }
}

// Funciones auxiliares para generar enteros y arreglos de enteros aleatorios
int random_int(int min, int max) {
    return min + rand() % (max - min + 1);
}

int *random_array(int n, int min, int max) {
    int *arr = (int *)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        arr[i] = random_int(min, max);
    }
    return arr;
}

// Funcion para resolver el problema utilizando la estrategia Backtracking B&B
void Backtracking(int depth, Tema* temas, int* pref_t, int* pref_p, int n, int T, int *max_value, bool *perm, bool *sol, long long current_T, long long current_value) {


    // Caso base: si se han considerado todos los temas, actualizar la solución si el valor actual es mejor
    if (n == depth) {
        if (current_value > *max_value) {
            *max_value = current_value;
            for (int i = 0; i < n; i++) sol[i] = perm[i];
        }
        return;
    }

    // Caso especial para el último tema: evaluar directamente si se puede tomar o no para evitar llamadas recursivas innecesarias
    if (depth == n-1){
        bool can_take = current_T + temas[depth].tiempo <= T;
        if (can_take) {
            current_T += temas[depth].tiempo;
            current_value += temas[depth].puntaje;
        }
        if (current_value > *max_value) {
            *max_value = current_value;
            for (int i = 0; i < n; i++) sol[i] = perm[i];
            if (current_T <= T) sol[temas[depth].index] = can_take;
        }
        return;
    }


    // Cálculo del límite superior para la poda: se calcula el valor máximo hipotetico considerando los temas restantes y el tiempo disponible
    int remaining_T = T - current_T;
    int target_t = pref_t[depth] + remaining_T;

    // Búsqueda binaria para encontrar el punto de corte en los temas ordenados por ratio
    int low = depth, high = n, split = depth;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (pref_t[mid] <= target_t) {
            split = mid;
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }

    // El limite superior sera el valor actual mas el valor acumulado de los temas hasta el punto de corte
    // Si el punto de corte no incluye todos los temas, se añade una fracción del siguiente tema para llenar el tiempo restante
    int upper_bound = current_value + (pref_p[split] - pref_p[depth]);

    if (split < n) {
        int time_accumulated = current_T + (pref_t[split] - pref_t[depth]);
        upper_bound += (temas[split].puntaje * (T - time_accumulated)) / temas[split].tiempo;
    }

    // Si el límite superior no supera el valor máximo encontrado hasta ahora, se poda esta rama de la búsqueda
    if (upper_bound <= *max_value) {
        return;
    }


    // Se explora la rama donde se toma el tema actual, siempre y cuando no exceda el tiempo disponible
    // Luego se explora la rama donde no se toma el tema actual
    if (current_T + temas[depth].tiempo <= T) {
        perm[temas[depth].index] = true;
        Backtracking(depth + 1, temas, pref_t, pref_p, n, T, max_value, perm, sol, current_T + temas[depth].tiempo, current_value + temas[depth].puntaje);
        perm[temas[depth].index] = false;
    }

    Backtracking(depth + 1, temas, pref_t, pref_p, n, T, max_value, perm, sol, current_T, current_value);
    
}

// Función principal para resolver el problema utilizando Backtracking con poda
int *BT_Solve(int *t, int *p, int n, int T, int *max_value, int *size){

    // Se crean arreglos auxiliares para la permutación actual y la solución óptima
    bool *perm = (bool* )calloc(n, sizeof(bool));
    bool *sol = (bool *)calloc(n, sizeof(bool));
    // Son de tipo booleano por eficiencia en cache

    // Se crea un arreglo de estructuras Tema para almacenar la información de cada tema
    Tema *temas = (Tema *)malloc(n * sizeof(Tema));
    int v_max = 0;      // Variable para almacenar el valor máximo encontrado para asegurar que el limite inferior de la búsqueda no sea menor al 50% del valor optimo
    int v_maxIdx = -1;  // Variable para almacenar el índice del tema con el valor máximo encontrado
    for (int i = 0; i < n; i++){
        temas[i].tiempo = t[i];
        temas[i].puntaje = p[i];
        temas[i].ratio = (float)p[i] / t[i];
        temas[i].index = i;
        if (v_max < temas[i].puntaje && temas[i].tiempo <= T) { // Se asegura que el valor máximo sea un tema que pueda ser tomado individualmente
            v_max = temas[i].puntaje;
            v_maxIdx = i;
        }
    }

    // Ordenamiento
    qsort(temas, n, sizeof(Tema), compareTema);

    // Arreglos de prefijos para tiempos y puntajes, optimiza el tiempo de calculo del límite superior por nodo
    int* pref_t = (int*)calloc((n + 1), sizeof(int));
    int* pref_p = (int*)calloc((n + 1), sizeof(int));
    for (int i = 0; i < n; i++) {
        pref_t[i + 1] = pref_t[i] + temas[i].tiempo;
        pref_p[i + 1] = pref_p[i] + temas[i].puntaje;
    }

    // Busqueda binaria para encontra el valor goloso (limite inferior inicial)
    int low = 0, high = n;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (pref_t[mid] <= T) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }

    int v_greedy = pref_p[high];

    // Si el valor máximo encontrado para un solo tema es mejor que la solución golosa, se actualiza la solución óptima con ese tema
    if (v_max > v_greedy) {
        sol[temas[v_maxIdx].index] = true;
        *max_value = v_max;  
    }
    // No se toma en cuenta el caso contrario por que la recursion hara el mismo camino igualmente

    Backtracking(0, temas, pref_t, pref_p, n, T, max_value, perm, sol, 0, 0); // Se inicia la búsqueda recursiva

    free(perm);
    free(temas);
    free(pref_t);
    free(pref_p);


    // Reconstruccion de la solucion
    for (int i = 0; i < n; i++){
        if (sol[i]) (*size)++;
    }

    if (*size == 0){    // Si no se encontró ninguna solución retorna NULL
        free(sol);
        return NULL;
    }

    // Retornar arreglo que almacena los numeros de los temas seleccionados (índices + 1)
    int *ans = (int *)malloc((*size) * sizeof(int));
    for (int i = 0, j = 0; i < n; i++){
        if (sol[i]) ans[j++] = i + 1;
    }
    free(sol);
    return ans;
}


// Función para resolver el problema utilizando la estrategia DP
int *DP_Solve(int *t, int*p, int n, int T, int *max_value, int *size) {

    // Inicializacion de matriz DP y asignacion de valores base 0
    int **dp = (int **)malloc((n + 1) * sizeof(int *));
    for (int i = 0; i <= n; i++) {
        dp[i] = (int *)calloc((T + 1), sizeof(int));
    }

    // Recorrido de la matriz DP
    for (int i = 1; i <= n; i++) {

        // Punteros para acceder a la fila actual y la fila anterior de la matriz DP, optimiza el acceso a cache
        int *dp_row = dp[i];
        int *dp_prev_row = dp[i - 1];

        // Recorrido de las capacidades desde 1 hasta T para llenar la fila DP[i]
        for (int j = 1; j <= T; j++) {
            if (t[i - 1] > j) { // Si el tema no se puede tomar, mantiene el valor de la fila anterior
                dp_row[j] = dp_prev_row[j];
            } else {            // De lo contrario, toma la mejor opcion entre no tomar y tomar el tema actual
                dp_row[j] = max(dp_prev_row[j], dp_prev_row[j - t[i - 1]] + p[i - 1]);
            }
        }
    }

    // Guardar el maximo valor
    *max_value = dp[n][T];

    // Reconstruccion de la solucion a partir de la matriz DP
    int *sol = (int *)calloc(n, sizeof(int));
    for (int i = n, j = T; i > 0 && j > 0; i--) {   // Recorrer desde DP[n][T] subiendo una fila cada vez
        if (dp[i][j] != dp[i - 1][j]) {             // Si se tomo el tema i
            sol[i - 1] = 1;                         // Añadir a la solucion
            j -= t[i - 1];                          // Irse a la columna correspondiente al tiempo restante
            (*size)++;
        }
    }

    for (int i = 0; i <= n; i++) {
        free(dp[i]);
    }
    free(dp);

    // Construccion del arreglo de temas seleccionados (índices + 1) para retornar
    int *ans = (int *)malloc((*size) * sizeof(int));
    for (int i = 0, j = 0; i < n; i++) {
        if (sol[i] == 1) {
            ans[j++] = i + 1;
        }
    }

    return ans;
}


// Función para resolver el problema utilizando la estrategia Golosa
int *Greedy_Solve(int *t, int *p, int n, int T, int *max_value, int *size) {

    // Inicializacion de arreglos auxiliares para la solución y para almacenar la información de los temas
    bool *sol = (bool *)calloc(n, sizeof(bool));
    Tema *temas = (Tema *)malloc(n * sizeof(Tema));

    for (int i = 0; i < n; i++){
        temas[i].index = i;
        temas[i].tiempo = t[i];
        temas[i].puntaje = p[i];
        temas[i].ratio = (float)p[i] / t[i];
    }

    // Ordenamiento
    qsort(temas, n, sizeof(Tema), compareTema);

    // Seleccion de temas
    int time_accumulated = 0;
    Tema *maxvalue_tema = NULL;
    for (int i = 0; i < n; i++) {
        // Si el tema actual se puede tomar, lo tomamos
        if (time_accumulated + temas[i].tiempo <= T) {
            time_accumulated += temas[i].tiempo;
            *max_value += temas[i].puntaje;
            sol[temas[i].index] = true;
            (*size)++;
        }

        // Mantener registro del tema con mayor puntaje que se pueda tomar individualmente
        if ((maxvalue_tema == NULL || temas[i].puntaje > maxvalue_tema->puntaje) && temas[i].tiempo <= T) {
            maxvalue_tema = &temas[i];
        }
    }

    // Si el tema con mayor puntaje individual es mejor que la solución golosa, se actualiza la solución óptima con ese tema
    if (maxvalue_tema != NULL && maxvalue_tema->puntaje > *max_value) {
        *max_value = maxvalue_tema->puntaje;
        *size = 1;
        int *ans = (int *)malloc(sizeof(int));
        ans[0] = maxvalue_tema->index + 1;
        free(sol);
        free(temas);
        return ans;
    }

    // Construccion del arreglo de temas seleccionados (índices + 1) para retornar
    int *ans = (int *)malloc((*size) * sizeof(int));
    for (int i = 0, j = 0; i < n; i++) {
        if (sol[i]) {
            ans[j++] = i + 1;
        }
    }

    free(sol);
    free(temas);
    return ans;
}