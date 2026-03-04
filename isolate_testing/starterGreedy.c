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
    float *ratios = (float *)malloc(n * sizeof(float));
    for (int i = 0; i < n; i++) {
        ratios[i] = (float)p[i] / t[i];
    }

    int free_time = T;
    for (int i = 0; i < n; i++) {
        int max_index = -1;
        float max_ratio = -1.0f;
        for (int j = 0; j < n; j++) {
            if (ratios[j] > max_ratio) {
                max_ratio = ratios[j];
                max_index = j;
            }
        }
        if (max_index == -1) break;

        if (t[max_index] <= free_time) {
            (*size)++;
            sol[max_index] = 1;
            *max_value += p[max_index];
            free_time -= t[max_index];
        }
        else {
            break;
        }

        ratios[max_index] = -1.0f; // Marcar como usado
    }

    int *ans = (int *)malloc((*size) * sizeof(int));
    for (int i = 0, j = 0; i < n; i++) {
        if (sol[i] == 1) {
            ans[j++] = i;
        }
    }

    free(sol);
    free(ratios);
    return ans;
}

int main(int argc, char *argv[]) {

    srand(time(NULL));

    struct timespec start, end;

    int n = atoi(argv[1]);
    int T_mult = atoi(argv[2]);
    int *t = random_array(n, 1, 20);
    int *p = random_array(n, 1, 50);

    int T = 0;
    for (int i = 0; i < n; i++) {
        T += t[i];
    }
    T = (T * T_mult) / 100;

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