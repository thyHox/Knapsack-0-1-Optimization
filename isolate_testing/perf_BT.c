#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>
#include <math.h>

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

int main(int argc, char *argv[]) {

    srand(time(NULL));

    struct timespec start, end;

    int n = atoi(argv[1]);
    int T_mult = atoi(argv[2]);

    int *t = random_array(n, 1, 10);
    int *p = random_array(n, 1, 10);

    int T = 0;
    for (int i = 0; i < n; i++) {
        T += t[i];
    }
    T = (T * T_mult) / 100;

    int *max_valueBT = (int *)calloc(1, sizeof(int));
    int *sizeBT = (int *)calloc(1, sizeof(int));

    timespec_get(&start, TIME_UTC);
    int *final_BT = BT_Solve(t, p, n, T, max_valueBT, sizeBT);
    timespec_get(&end, TIME_UTC);
    double time_BT = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Execution time: %f seconds\n", time_BT);
    
    free(t);
    free(p);
    free(max_valueBT);
    free(sizeBT);
    free(final_BT);
}