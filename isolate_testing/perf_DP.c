#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#include <stdlib.h>
#include <stdio.h>
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
    int n = atoi(argv[1]);
    int T = atoi(argv[2]);
    int *t = random_array(n, 1, 100);
    int *p = random_array(n, 1, 100);
    int max_value = 0;
    int size = 0;

    struct timespec start, end;
    timespec_get(&start, TIME_UTC);
    int *sol = DP_Solve(t, p, n, T, &max_value, &size);
    timespec_get(&end, TIME_UTC);
    long seconds = end.tv_sec - start.tv_sec;
    long nanoseconds = end.tv_nsec - start.tv_nsec;
    double elapsed = seconds + nanoseconds*1e-9;
    printf("Elapsed time: %f seconds\n", elapsed);

    /*
    printf("Max value: %d\n", max_value);
    printf("Size: %d\n", size);
    printf("Selected items: ");
    for (int i = 0; i < size; i++) {
        printf("%d ", sol[i]);
    }
    printf("\n");
    */

    free(t);
    free(p);
    free(sol);

    return 0;
}