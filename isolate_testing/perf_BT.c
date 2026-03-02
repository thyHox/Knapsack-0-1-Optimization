#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>
#include <math.h>
#include <stdbool.h>

typedef struct Tema {
    int tiempo;
    int puntaje;
    int index;
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

void Backtracking(int depth, Tema* temas, int* pref_t, int* pref_p, int n, int T, int *max_value, bool *perm, bool *sol, long long current_T, long long current_value) {

    if (n == depth) {
        if (current_value > *max_value) {
            *max_value = current_value;
            for (int i = 0; i < n; i++) sol[i] = perm[i];
        }
        return;
    }

    int remaining_T = T - current_T;
    int target_t = pref_t[depth] + remaining_T;

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

    int upper_bound = current_value + (pref_p[split] - pref_p[depth]);

    if (split < n) {
        int time_accumulated = current_T + (pref_t[split] - pref_t[depth]);
        upper_bound += (temas[split].puntaje * (T - time_accumulated)) / temas[split].tiempo;
    }

    if (upper_bound <= *max_value) {
        return;
    }

    if (current_T + temas[depth].tiempo <= T) {
        perm[temas[depth].index] = true;
        Backtracking(depth + 1, temas, pref_t, pref_p, n, T, max_value, perm, sol, current_T + temas[depth].tiempo, current_value + temas[depth].puntaje);
        perm[temas[depth].index] = false;
    }

    Backtracking(depth + 1, temas, pref_t, pref_p, n, T, max_value, perm, sol, current_T, current_value);
    
}

int *BT_Solve(int *t, int *p, int n, int T, int *max_value, int *size){

    bool *perm = (bool* )calloc(n, sizeof(bool));
    bool *sol = (bool *)calloc(n, sizeof(bool));

    Tema *temas = (Tema *)malloc(n * sizeof(Tema));
    int v_max = 0;
    int v_maxIdx = -1;
    for (int i = 0; i < n; i++){
        temas[i].tiempo = t[i];
        temas[i].puntaje = p[i];
        temas[i].ratio = (float)p[i] / t[i];
        temas[i].index = i;
        if (v_max < temas[i].puntaje && temas[i].tiempo <= T) {
            v_max = temas[i].puntaje;
            v_maxIdx = i;
        }
    }

    qsort(temas, n, sizeof(Tema), compareTema);

    int* pref_t = (int*)calloc((n + 1), sizeof(int));
    int* pref_p = (int*)calloc((n + 1), sizeof(int));
    for (int i = 0; i < n; i++) {
        pref_t[i + 1] = pref_t[i] + temas[i].tiempo;
        pref_p[i + 1] = pref_p[i] + temas[i].puntaje;
    }

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

    if (v_max > v_greedy) {
        sol[temas[v_maxIdx].index] = true;
        *max_value = v_max;
        
    }

    Backtracking(0, temas, pref_t, pref_p, n, T, max_value, perm, sol, 0, 0);

    free(perm);
    free(temas);
    free(pref_t);
    free(pref_p);

    for (int i = 0; i < n; i++){
        if (sol[i]) (*size)++;
    }

    if (*size == 0){
        free(sol);
        return NULL;
    }

    int *ans = (int *)malloc((*size) * sizeof(int));
    for (int i = 0, j = 0; i < n; i++){
        if (sol[i]) ans[j++] = i + 1;
    }
    free(sol);
    return ans;
}

int main(int argc, char *argv[]) {

    srand(time(NULL));

    struct timespec start, end;

    int n = atoi(argv[1]);
    int T = atoi(argv[2]);

    /*
    int *t = random_array(n, 1, 100);
    int *p = random_array(n, 1, 100);

    int T = 0;
    for (int i = 0; i < n; i++) {
        T += t[i];
    }
    T = (int)(((long long)T * T_mult) / 100);
    */

    int t[30];
    int p[30];

    for (int i = 0; i < n; i++) {
        t[i] = 10;
        p[i] = 10;
    }

    int *max_valueBT = (int *)calloc(1, sizeof(int));
    int *sizeBT = (int *)calloc(1, sizeof(int));

    timespec_get(&start, TIME_UTC);
    int *final_BT = BT_Solve(t, p, n, T, max_valueBT, sizeBT);
    timespec_get(&end, TIME_UTC);
    double time_BT = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Execution time: %f seconds\n", time_BT);

    /*
    printf("Max value: %d\n", *max_valueBT);
    printf("Selected items: ");
    for (int i = 0; i < *sizeBT; i++) {
        printf("%d ", final_BT[i]);
    }
    printf("\n");

    free(t);
    free(p);
    */

    free(max_valueBT);
    free(sizeBT);
    free(final_BT);
}