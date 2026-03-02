#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <time.h>

typedef struct Tema{
    int index;
    int tiempo;
    int puntaje;
    float ratio;
} Tema;

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

int compareTema(const void *a, const void *b) {

    Tema *temaA = (Tema *)a;
    Tema *temaB = (Tema *)b;

    if (fabs(temaA->ratio - temaB->ratio) < FLT_EPSILON) {  //Si son iguales por ratio, desempatar por puntaje

        if (temaA->puntaje < temaB->puntaje) {
            return 1;
        } else if (temaA->puntaje > temaB->puntaje) {
            return -1;
        } else {
            return 0;
        }

    } else if (temaA->ratio < temaB->ratio) {               //Si no, ordenar por ratio
        return 1;
        
    } else {
        return -1;
    }
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
    T = (int)((long long)(T * T_mult) / 100);

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