#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "algoritmos.c"

int main(int argc, char *argv[]) {

    // Fallo de ejecucion
    if (argc != 2) {
        printf("Ejecutar usando la sintaxis: %s <Cantidad de temas (n)>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);

    // Semilla para generación de números aleatorios
    srand(time(NULL));
    struct timespec start, end;

    // Generación de datos
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

    int T;
    printf("Ingrese el valor de T: ");
    scanf("%d", &T);
   

    int *max_value = (int *)calloc(1, sizeof(int));
    int *size = (int *)calloc(1, sizeof(int));

    // Medición del tiempo de ejecución
    timespec_get(&start, TIME_UTC);
    int *final_dp = DP_Solve(t, p, n, T, max_value, size);  //Ejecucion
    timespec_get(&end, TIME_UTC);
    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Tiempo de ejecucion: %f seconds\n", time_taken);

    // Impresion de resultados
    if (final_dp != NULL) {
    printf("Conjunto de temas: ");
        for (int i = 0; i < *size; i++) {
            printf("%d ", final_dp[i]);
        }
        printf("\nPuntaje maximo alcanzado: %d\n", *max_value);
    } else {
        printf("No se encontró solución.\n");
    }

    free(t);
    free(p);
    free(final_dp);
    free(max_value);
    free(size);
    return 0;
}