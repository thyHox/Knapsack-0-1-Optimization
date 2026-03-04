#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "algoritmos.c"


int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Ejecutar usando la sintaxis: %s <Cantidad de temas (n)>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);

    srand(time(NULL));

    struct timespec start, end;

    printf("Opciones para distribución de datos:\n");
    printf("1. Generar tiempos y puntajes aleatorios\n");
    printf("2. Ingresar tiempos y puntajes manualmente\n");
    int option;
    scanf("%d", &option);

    int* t = (int *)malloc(n * sizeof(int));
    int* p = (int *)malloc(n * sizeof(int));

    switch (option)
    {
    case 1:
        int max;
        int min;
        printf("Ingrese el valor mínimo y maximo para tiempos y puntajes 'min max': ");
        scanf("%d %d", &min, &max);
        for (int i = 0; i < n; i++) {
            t[i] = random_int(min, max);
            p[i] = random_int(min, max);
        }
        break;
    
    case 2:
        printf("Ingrese los tiempos y puntajes para cada tema:\n");
        for (int i = 0; i < n; i++) {
            printf("Tema %d - Tiempo: ", i + 1);
            scanf("%d", &t[i]);
            printf("Tema %d - Puntaje: ", i + 1);
            scanf("%d", &p[i]);
        }
        break;
    default:
        printf("Opción no válida. Por favor, elija 1 o 2.\n");
        free(t);
        free(p);
        return 1;
    }

    printf("Opciones para T: ");
    printf("1. Ingresar T manualmente\n");
    printf("2. T = Porcentaje de la suma total de tiempos. Ingrese el porcentaje: \n");
    scanf("%d", &option);

    int T;

    switch(option) {
        case 1:
            printf("Ingrese el valor de T: ");
            scanf("%d", &T);
            break;
        case 2:
            int percentage;
            printf("Ingrese el porcentaje para calcular T: ");
            scanf("%d", &percentage);
            int total_time = 0;
            for (int i = 0; i < n; i++) {
                total_time += t[i];
            }
            T = (total_time * percentage) / 100;
            printf("T calculado: %d\n", T);
            break;
        default:
            printf("Opción no válida. Por favor, elija 1 o 2.\n");
            free(t);
            free(p);
            return 1;
    }

    int *max_valueGreedy = (int *)calloc(1, sizeof(int));
    int *sizeGreedy = (int *)calloc(1, sizeof(int));
    timespec_get(&start, TIME_UTC);
    int *final_greedy = Greedy_Solve(t, p, n, T, max_valueGreedy, sizeGreedy);
    timespec_get(&end, TIME_UTC);
    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Execution time Greedy: %f seconds\n", time_taken);

    if (final_greedy != NULL) {
        printf("Conjunto de temas Greedy: ");
        for (int i = 0; i < *sizeGreedy; i++) {
            printf("%d ", final_greedy[i]);
        }
        printf("\nMax value Greedy: %d\n", *max_valueGreedy);
    } else {
        printf("No solution found by Greedy.\n");
    }

    free(t);
    free(p);
    free(final_greedy);
    free(max_valueGreedy);
    free(sizeGreedy);
    return 0;
}