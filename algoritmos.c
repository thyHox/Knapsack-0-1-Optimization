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

void Backtracking(int depth, Tema* temas, int n, int T, int *max_value, int *perm, int *sol, int current_T, int current_value) {

    if (n == depth) {
        *max_value = current_value;
        for (int i = 0; i < n; i++) sol[i] = perm[i];
        return;
    }

    int upper_bound = current_value;
    int time_accumulated = current_T;
    for (int i = depth; i < n; i++) {
        if (time_accumulated + temas[i].tiempo <= T) {
            time_accumulated += temas[i].tiempo;
            upper_bound += temas[i].puntaje;
        } else {
            upper_bound += (temas[i].puntaje * (T - time_accumulated)) / temas[i].tiempo;
            break;
        }
    }

    if (upper_bound <= *max_value) {
        return;
    }

    if (current_T + temas[depth].tiempo <= T) {
        perm[temas[depth].index] = 1;
        Backtracking(depth + 1, temas, n, T, max_value, perm, sol, current_T + temas[depth].tiempo, current_value + temas[depth].puntaje);
        perm[temas[depth].index] = 0;
    }

    Backtracking(depth + 1, temas, n, T, max_value, perm, sol, current_T, current_value);
    
}

int *BT_Solve(int *t, int *p, int n, int T, int *max_value, int *size){
    int *perm = (int *)calloc(n, sizeof(int));
    int *sol = (int *)calloc(n, sizeof(int));

    Tema *temas = (Tema *)malloc(n * sizeof(Tema));
    for (int i = 0; i < n; i++){
        temas[i].index = i;
        temas[i].tiempo = t[i];
        temas[i].puntaje = p[i];
        temas[i].ratio = (float)p[i] / t[i];
    }

    qsort(temas, n, sizeof(Tema), compareTema);

    Backtracking(0, temas, n, T, max_value, perm, sol, 0, 0);

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
        dp[i] = (int *)calloc((T + 1), sizeof(int));
    }

    for (int i = 1; i <= n; i++) {
        int *dp_row = dp[i];
        int *dp_prev_row = dp[i - 1];
        for (int j = 1; j <= T; j++) {
            if (t[i - 1] > j) {
                dp_row[j] = dp_prev_row[j];
            } else {
                dp_row[j] = max(dp_prev_row[j], dp_prev_row[j - t[i - 1]] + p[i - 1]);
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

int *Greedy_Solve(int *t, int *p, int n, int T, int *max_value, int *size) {

    int *sol = (int *)calloc(n, sizeof(int));
    Tema *temas = (Tema *)malloc(n * sizeof(Tema));

    for (int i = 0; i < n; i++){
        temas[i].index = i;
        temas[i].tiempo = t[i];
        temas[i].puntaje = p[i];
        temas[i].ratio = (float)p[i] / t[i];
    }

    qsort(temas, n, sizeof(Tema), compareTema);

    int time_accumulated = 0;
    for (int i = 0; i < n; i++) {
        if (time_accumulated + temas[i].tiempo <= T) {
            time_accumulated += temas[i].tiempo;
            *max_value += temas[i].puntaje;
            sol[temas[i].index] = 1;
            (*size)++;
        } else {
            break;
        }
    }

    int *ans = (int *)malloc((*size) * sizeof(int));
    for (int i = 0, j = 0; i < n; i++) {
        if (sol[i] == 1) {
            ans[j++] = i + 1;
        }
    }

    free(sol);
    free(temas);
    return ans;
}