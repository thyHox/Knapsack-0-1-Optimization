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

void Backtracking(int depth, Tema* temas, int* pref_t, int* pref_p, int n, int T, int *max_value, bool *perm, bool *sol, long long current_T, long long current_value) {

    if (n == depth) {
        if (current_value > *max_value) {
            *max_value = current_value;
            for (int i = 0; i < n; i++) sol[i] = perm[i];
        }
        return;
    }

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
    Tema *maxvalue_tema = NULL;
    for (int i = 0; i < n; i++) {
        if (time_accumulated + temas[i].tiempo <= T) {
            time_accumulated += temas[i].tiempo;
            *max_value += temas[i].puntaje;
            sol[temas[i].index] = 1;
            (*size)++;
        }
        if ((maxvalue_tema == NULL || temas[i].puntaje > maxvalue_tema->puntaje) && temas[i].tiempo <= T) {
            maxvalue_tema = &temas[i];
        }
    }

    if (maxvalue_tema != NULL && maxvalue_tema->puntaje > *max_value) {
        *max_value = maxvalue_tema->puntaje;
        *size = 1;
        int *ans = (int *)malloc(sizeof(int));
        ans[0] = maxvalue_tema->index + 1;
        free(sol);
        free(temas);
        return ans;
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