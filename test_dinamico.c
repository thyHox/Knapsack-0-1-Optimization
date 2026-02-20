#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "algoritmos.c"

int main(int argc, char *argv[]) {

    srand(time(NULL));

    struct timespec start, end;

    int n = atoi(argv[1]);
    int T = atoi(argv[2]);
    int *t = random_array(n, 1, 10);
    int *p = random_array(n, 1, 10);

    /*
    printf("t: ");
    for (int i = 0; i < n; i++) {
        printf("%d ", t[i]);
    }
    printf("\n");
    printf("p: ");
    for (int i = 0; i < n; i++) {
        printf("%d ", p[i]);
    }
    printf("\n");
    printf("n: %d, T: %d\n", n, T);
    */
   
    int *max_valueDP = (int *)calloc(1, sizeof(int));
    int *sizeDP = (int *)calloc(1, sizeof(int));
    timespec_get(&start, TIME_UTC);
    int *final_dp = DP_Solve(t, p, n, T, max_valueDP, sizeDP);
    timespec_get(&end, TIME_UTC);
    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Execution time DP: %f seconds\n", time_taken);

    if (final_dp != NULL) {
    printf("Conjunto de temas DP: ");
        for (int i = 0; i < *sizeDP; i++) {
            printf("%d ", final_dp[i]);
        }
        printf("\nMax value DP: %d\n", *max_valueDP);
    } else {
        printf("No solution found by DP.\n");
    }

    free(t);
    free(p);
    if (final_dp != NULL) {
        free(final_dp);
    }
    free(max_valueDP);
    free(sizeDP);
    return 0;
}