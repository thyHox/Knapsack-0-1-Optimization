#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

void Backtracking(int depth, int *t, int *p, int n, int T, int *max_value, int *perm, int *sol) {

    if (n == depth){
        int sum_T = 0;
        int sum_P = 0;
        for (int i = 0; i < n; i++){
            if (perm[i] == 1){
                sum_T += t[i];
                sum_P += p[i];
            }
        }
        if (sum_T <= T && sum_P > *max_value){
            *max_value = sum_P;
            for (int i = 0; i < n; i++){
                sol[i] = perm[i];
            }
        }
        return;
    }

    perm[depth] = 1;
    Backtracking(depth + 1, t, p, n, T, max_value, perm, sol);

    perm[depth] = 0;
    Backtracking(depth + 1, t, p, n, T, max_value, perm, sol);
}

int *BT_Solve(int *t, int *p, int n, int T, int *max_value, int *size){
    int *perm = (int *)calloc(n, sizeof(int));
    int *sol = (int *)calloc(n, sizeof(int));

    Backtracking(0, t, p, n, T, max_value, perm, sol);

    free(perm);

    for (int i = 0; i < n; i++){
        if (sol[i] == 1){
            (*size)++;
        }
    }

    if (*size == 0){
        free(sol);
        return NULL;
    }

    int *ans = (int *)malloc((*size) * sizeof(int));
    for (int i = 0, j = 0; i < n; i++){
        if (sol[i] == 1){
            ans[j++] = i + 1;
        }
    }
    free(sol);
    return ans;
}

int main(int argc, char *argv[]) {

    srand(time(NULL));

    struct timespec start, end;

    int n = atoi(argv[1]);
    int T = atoi(argv[2]);

    int *t = random_array(n, 1, 10);
    int *p = random_array(n, 1, 10);

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