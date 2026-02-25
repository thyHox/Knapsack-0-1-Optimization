#define _POSIX_C_SOURCE 199309L

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "algoritmos.c"


int main(int argc, char *argv[]) {

    srand(time(NULL));

    struct timespec start, end;

    int n = atoi(argv[1]);
    int T_mult = atoi(argv[2]);
    int *t = random_array(n, 1, 10);
    int *p = random_array(n, 1, 10);

    int T_total = 0;
    for (int i = 0; i < n; i++) {
        T_total += t[i];
    }

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
    */

    int T = T_total * ((float)T_mult / 100);

    int *max_valueGreedy = (int *)calloc(1, sizeof(int));
    int *sizeGreedy = (int *)calloc(1, sizeof(int));
    timespec_get(&start, TIME_UTC);
    int *final_greedy = Greedy_Solve(t, p, n, T, max_valueGreedy, sizeGreedy);
    timespec_get(&end, TIME_UTC);
    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Execution time Greedy: %f seconds\n", time_taken);

    /*
    if (final_greedy != NULL) {
        printf("Conjunto de temas Greedy: ");
        for (int i = 0; i < *sizeGreedy; i++) {
            printf("%d ", final_greedy[i]);
        }
        printf("\nMax value Greedy: %d\n", *max_valueGreedy);
    } else {
        printf("No solution found by Greedy.\n");
    }
    */

    free(t);
    free(p);
    if (final_greedy != NULL) {
        free(final_greedy);
    }
    free(max_valueGreedy);
    free(sizeGreedy);
    return 0;
}