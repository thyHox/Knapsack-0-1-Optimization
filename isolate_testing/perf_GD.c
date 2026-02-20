#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
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

int *Greedy_Solve(int *t, int *p, int n, int T, int *max_value, int *size) {
    int *sol = (int *)calloc(n, sizeof(int));
    float *ratio = (float *)malloc(n * sizeof(float));

    for (int i = 0; i < n; i++) {
        ratio[i] = (float)p[i] / t[i];
    }

    int current_time = 0;

    for (int i = 0; i < n; i++) {
        int selected = i;
        for (int j = 0; j < n; j++) {
            if (fabs(ratio[j] - ratio[selected]) < FLT_EPSILON) {
                if (p[j] > p[i]) {
                    selected = j;
                }
            }
            else if (ratio[j] > ratio[i]) {
                selected = j;
            }
        }

        if (current_time + t[selected] <= T) {
            sol[selected] = 1;
            ratio[selected] = -1;
            current_time += t[selected];
            *max_value += p[selected];
            (*size)++;
        }
    }

    int *ans = (int *)malloc((*size) * sizeof(int));
    for (int i = 0, j = 0; i < n; i++) {
        if (sol[i] == 1) {
            ans[j++] = i;
        }
    }

    free(ratio);
    free(sol);
    return ans;
}

int main(int argc, char *argv[]) {

    srand(time(NULL));

    struct timespec start, end;

    int n = atoi(argv[1]);
    int T = atoi(argv[2]);
    int *t = random_array(n, 1, 20);
    int *p = random_array(n, 1, 50);

    int *max_value_greedy = (int *)calloc(1, sizeof(int));
    int *size_greedy = (int *)calloc(1, sizeof(int));

    timespec_get(&start, TIME_UTC);
    int *greedy_solution = Greedy_Solve(t, p, n, T, max_value_greedy, size_greedy);
    timespec_get(&end, TIME_UTC);
    double time_greedy = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Execution time: %f seconds\n", time_greedy);

    /*
    printf("Greedy Solution:\n");
    printf("Max Value: %d\n", *max_value_greedy);
    printf("Size: %d\n", *size_greedy);
    printf("Selected Items: ");
    for (int i = 0; i < *size_greedy; i++) {
        printf("%d ", greedy_solution[i]);
    }
    printf("\n");
    */

    free(t);
    free(p);
    free(max_value_greedy);
    free(size_greedy);
    free(greedy_solution);

    return 0;
}