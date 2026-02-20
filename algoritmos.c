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
    int index;
} Tema;

int compareTema(const void *a, const void *b) {

    Tema *temaA = (Tema *)a;
    Tema *temaB = (Tema *)b;

    if (fabs(temaA->ratio - temaB->ratio) < FLT_EPSILON) {
        if (temaA->puntaje < temaB->puntaje) {
            return 1;
        } else if (temaA->puntaje > temaB->puntaje) {
            return -1;
        } else {
            return 0;
        }

    } else if (temaA->ratio < temaB->ratio) {
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

void BT_Recursion(Tema* sorted_temas, int *remainingSum, int *current_permutation, int *best_permutation,int n, int T, int depth, int current_T, int current_value, int *max_value){

    if (current_value + remainingSum[depth] <= *max_value){
        return;
    }

    if (n == depth){
        *max_value = current_value;
        for (int i = 0; i < n; i++){
            best_permutation[i] = current_permutation[i];
        }
        return;
    }

    if (current_T + sorted_temas[depth].tiempo <= T){

        current_permutation[sorted_temas[depth].index] = 1;
        BT_Recursion(sorted_temas, remainingSum, current_permutation, best_permutation, n, T, depth + 1, current_T + sorted_temas[depth].tiempo, current_value + sorted_temas[depth].puntaje, max_value);
            
        current_permutation[sorted_temas[depth].index] = 0;
    }
    BT_Recursion(sorted_temas, remainingSum, current_permutation, best_permutation, n, T, depth + 1, current_T, current_value, max_value);
}   

int *BT_Solve(int *t, int *p, int n, int T, int *max_value, int *size){
    int *current_permutation = (int *)calloc(n, sizeof(int));
    int *best_permutation = (int *)calloc(n, sizeof(int));
    
    Tema *sorted_temas = (Tema *)malloc(n * sizeof(Tema));
    for (int i = 0; i < n; i++){
        sorted_temas[i].tiempo = t[i];
        sorted_temas[i].puntaje = p[i];
        sorted_temas[i].ratio = (float)p[i] / t[i];
        sorted_temas[i].index = i;
    }
    qsort(sorted_temas, n, sizeof(Tema), compareTema);

    int *remainingSum = (int *)calloc((n + 1), sizeof(int));
    for (int i = n - 1; i >= 0; i--) {
        remainingSum[i] = remainingSum[i + 1] + sorted_temas[i].puntaje;
    }

    BT_Recursion(sorted_temas, remainingSum, current_permutation, best_permutation, n, T, 0, 0, 0, max_value);
    free(current_permutation);
    free(remainingSum);
    free(sorted_temas);

    for (int i = 0; i < n; i++){
        if (best_permutation[i] == 1){
            (*size)++;
        }
    }

    if (*size == 0){
        free(best_permutation);
        return NULL;
    }

    int *ans = (int *)malloc((*size) * sizeof(int));
    for (int i = 0, j = 0; i < n; i++){
        if (best_permutation[i] == 1){
            ans[j++] = i + 1;
        }
    }
    free(best_permutation);
    return ans;
}

int *DP_Solve(int *t, int *p, int n, int T, int *max_value, int *size) {
    int **DP = (int **)malloc((n + 1) * sizeof(int *));
    for (int i = 0; i <= n; i++) {
        DP[i] = (int *)calloc((T + 1), sizeof(int));
    }

    for (int i = 1; i <= n; i++) {
        int *tema_actual = DP[i];
        int *tema_anterior = DP[i - 1];
        for (int j = 1; j <= T; j++) {
            if (t[i - 1] <= j) {
                tema_actual[j] = max(tema_anterior[j], tema_anterior[j - t[i - 1]] + p[i - 1]);
            } 
            else {
                tema_actual[j] = tema_anterior[j];
            }
        }
    }
    *max_value = DP[n][T];

    int *ans = (int *)malloc(*size * sizeof(int));

    for (int i = n, j = T; i > 0 && j > 0; ) {
        if (DP[i][j] != DP[i - 1][j]) {
            (*size)++;
            j -= t[i - 1];
            i--;
            ans = (int *)realloc(ans, (*size) * sizeof(int));
            ans[(*size) - 1] = i + 1;
        }
        else {
            i--;
        }
    }

    if (*size == 0) {
        free(ans);
        return NULL;
    }

    for (int i = 0; i < *size / 2; i++) {
        int temp = ans[i];
        ans[i] = ans[(*size) - i - 1];
        ans[(*size) - i - 1] = temp;
    }

    for (int i = 0; i <= n; i++) {
        free(DP[i]);
    }
    free(DP);
    return ans;
}

int *Greedy_Solve(int *t, int *p, int n, int T, int *max_value, int *size) {

    Tema *sorted_temas = (Tema *)malloc(n * sizeof(Tema));
    for (int i = 0; i < n; i++){
        sorted_temas[i].tiempo = t[i];
        sorted_temas[i].puntaje = p[i];
        sorted_temas[i].ratio = (float)p[i] / t[i];
        sorted_temas[i].index = i;
    }

    qsort(sorted_temas, n, sizeof(Tema), compareTema);

    int current_T = T;
    int *best_solution = (int *)calloc(n, sizeof(int));
    for (int i = 0; i < n && current_T > 0; i++){
        if (current_T - sorted_temas[i].tiempo >= 0){
            current_T -= sorted_temas[i].tiempo;
            *max_value += sorted_temas[i].puntaje;
            (*size)++;
            best_solution[sorted_temas[i].index] = 1;
        }   
    }

    if (*size == 0){
        free(best_solution);
        free(sorted_temas);
        return NULL;
    }

    int *ans = (int *)malloc((*size) * sizeof(int));
    for (int i = 0, j = 0; i < n; i++){
        if (best_solution[i] == 1){
            ans[j++] = i + 1;
        }
    }
    free(best_solution);
    free(sorted_temas);
    return ans;
}