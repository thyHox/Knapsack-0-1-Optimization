#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#include    <stdlib.h>
#include    <stdio.h>
#include    <time.h>
#include    <float.h>
#include    <math.h>

typedef struct Tema {
    int tiempo;
    int puntaje;
    float ratio;
} Tema;

int compareTema(const void *a, const void *b) {

    Tema *temaA = (Tema *)a;
    Tema *temaB = (Tema *)b;

    if (fabs(temaA->ratio - temaB->ratio) < FLT_EPSILON) {  //Si son iguales por ratio, desempatar por puntaje

        if (temaA->puntaje < temaB->puntaje) {
            return 1;
        } else if (temaA->puntaje > temaB->puntaje) {
            return -1;
        } else {
            return 0;
        }

    } else if (temaA->ratio < temaB->ratio) {               //Si no, ordenar por ratio
        return 1;
        
    } else {
        return -1;
    }
}

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

void Backtracking(int depth, Tema* temas, int *remainingSum, int n, int T, int *max_value, int *perm, int *sol, int current_T, int current_value) {

    if (current_value + remainingSum[depth] <= *max_value) {
        return;
    }

    if (n == depth) {
        *max_value = current_value;
        for (int i = 0; i < n; i++) sol[i] = perm[i];
        return;
    }

    float upper_bound = current_value;
    int time_accumulated = current_T;
    for (int i = depth; i < n; i++) {
        if (time_accumulated + temas[i].tiempo <= T) {
            time_accumulated += temas[i].tiempo;
            upper_bound += temas[i].puntaje;
        } else {
            upper_bound += temas[i].puntaje * (float)(T - time_accumulated) / temas[i].tiempo;
            break;
        }
    }

    if (upper_bound <= *max_value) {
        return;
    }

    if (current_T + temas[depth].tiempo <= T) {
        perm[depth] = 1;
        Backtracking(depth + 1, temas, remainingSum, n, T, max_value, perm, sol, current_T + temas[depth].tiempo, current_value + temas[depth].puntaje);
        perm[depth] = 0;
    }

    Backtracking(depth + 1, temas, remainingSum, n, T, max_value, perm, sol, current_T, current_value);
    
}

int *BT_Solve(int *t, int *p, int n, int T, int *max_value, int *size){
    int *perm = (int *)calloc(n, sizeof(int));
    int *sol = (int *)calloc(n, sizeof(int));
    int *remainingSum = (int *)calloc(n + 1, sizeof(int));

    Tema *temas = (Tema *)malloc(n * sizeof(Tema));
    for (int i = 0; i < n; i++){
        temas[i].tiempo = t[i];
        temas[i].puntaje = p[i];
        temas[i].ratio = (float)p[i] / t[i];
    }

    qsort(temas, n, sizeof(Tema), compareTema);

    for (int i = n - 1; i >= 0; i--){
        remainingSum[i] = remainingSum[i + 1] + temas[i].puntaje;
    }

    Backtracking(0, temas, remainingSum, n, T, max_value, perm, sol, 0, 0);

    free(perm);

    for (int i = 0; i < n; i++){
        if (sol[i] == 1) (*size)++;
    }

    if (*size == 0){
        free(sol);
        return NULL;
    }

    int *ans = (int *)malloc((*size) * sizeof(int));
    for (int i = 0, j = 0; i < n; i++){
        if (sol[i] == 1) ans[j++] = i + 1;
    }
    free(sol);
    return ans;
}

int *DP_Solve(int *t, int*p, int n, int T, int *max_value, int *size) {
    int **dp = (int **)malloc((n + 1) * sizeof(int *));
    for (int i = 0; i <= n; i++) {
        dp[i] = (int *)malloc((T + 1) * sizeof(int));
    }

    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= T; j++) {
            if (i == 0) {
                dp[i][j] = 0;
            } else if (j == 0) {
                dp[i][j] = 0;
            } else if (t[i - 1] > j) {
                dp[i][j] = dp[i - 1][j];
            } else {
                dp[i][j] = max(dp[i - 1][j], dp[i - 1][j - t[i - 1]] + p[i - 1]);
            }
        }
    }

    *max_value = dp[n][T];

    int *sol = (int *)calloc(n, sizeof(int));
    for (int i = n, j = T; i > 0 && j > 0; i--) {
        if (dp[i][j] != dp[i - 1][j]) {
            sol[i - 1] = 1;
            j -= t[i - 1];
            (*size)++;
        }
    }

    for (int i = 0; i <= n; i++) {
        free(dp[i]);
    }
    free(dp);

    int *ans = (int *)malloc((*size) * sizeof(int));
    for (int i = 0, j = 0; i < n; i++) {
        if (sol[i] == 1) {
            ans[j++] = i + 1;
        }
    }

    return ans;
}

int main(int argc, char *argv[]) {
    int n = atoi(argv[1]); // Número de temas
    int T_mult = atoi(argv[2]); // Tiempo total disponible

    int *t = random_array(n, 1, 20); // Tiempos aleatorios entre 1 y 20
    int *p = random_array(n, 1, 100); // Puntajes aleatorios entre 1 y 100

    int T = 0;
    for (int i = 0; i < n; i++) {
        T += t[i];
    }

    T = (T * T_mult) / 100; // Ajustar el tiempo total según el multiplicador

    int max_valueBT = 0, sizeBT = 0;
    int max_valueDP = 0, sizeDP = 0;

    int* solution_BT = BT_Solve(t, p, n, T, &max_valueBT, &sizeBT);
    int* solution_DP = DP_Solve(t, p, n, T, &max_valueDP, &sizeDP);

    if (max_valueBT != max_valueDP) {
        printf("Error: Los valores máximos no coinciden. BT: %d, DP: %d\n", max_valueBT, max_valueDP);
    } else {
        printf("Éxito: Los valores máximos coinciden. Valor máximo: %d\n", max_valueBT);
    }

    return 0;
}