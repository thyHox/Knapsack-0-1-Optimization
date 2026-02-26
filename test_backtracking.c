#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "algoritmos.c"

int main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("Ejecutar usando la sintaxis: %s <Cantidad de temas (n)> <Porcentaje de tiempo total sum(t)' (T)>\n", argv[0]);
        return 1;
    }

    srand(time(NULL));

    struct timespec start, end;

    int n = atoi(argv[1]);
    int T_mult = atoi(argv[2]);
    int *t = random_array(n, 1, 10);
    int *p = random_array(n, 1, 10);

    /*
    printf("t: ");
    for (int i = 0; i < n; i++) {
        printf("%d ", t[i]);
    }
    */

    int T_total = 0;
    for (int i = 0; i < n; i++) {
        T_total += t[i];
    }

    int T = T_total * ((float) T_mult / 100.0);

    /*
    printf("\n");
    printf("p: ");
    for (int i = 0; i < n; i++) {
        printf("%d ", p[i]);
    }
    printf("\n");
    printf("n: %d, T: %d\n", n, T);
    */

    int *max_valueBT = (int *)calloc(1, sizeof(int));
    int *sizeBT = (int *)calloc(1, sizeof(int));

    timespec_get(&start, TIME_UTC);
    int *final_bt = BT_Solve(t, p, n, T, max_valueBT, sizeBT);
    timespec_get(&end, TIME_UTC);
    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Execution time BT: %f seconds\n", time_taken);

    if (final_bt != NULL) {
        printf("Conjunto de temas BT: ");
        for (int i = 0; i < *sizeBT; i++) {
            printf("%d ", final_bt[i]);
        }
        printf("\nMax value BT: %d\n", *max_valueBT);
    } else {
        printf("No solution found by BT.\n");
    }

    free(t);
    free(p);
    if (final_bt != NULL) {
        free(final_bt);
    }
    free(max_valueBT);
    free(sizeBT);
    return 0;
}