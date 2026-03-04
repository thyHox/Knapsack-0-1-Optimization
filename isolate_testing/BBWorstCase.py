import subprocess
import re
import matplotlib.pyplot as plt
import os

# --- CONFIGURACIÓN ---
C_FILENAME = "perf_BT.c"
EXECUTABLE = "./mochila_benchmark" if os.name != 'nt' else "mochila_benchmark.exe"
N = 30
# Generamos T desde 5 hasta 295, saltando de 10 en 10 (Garantiza el hueco de 5)
T_values = list(range(5, 305, 10))
tiempos = []

def compilar_c():
    print(f"⚙️ Compilando {C_FILENAME} con máxima optimización (-O3)...")
    try:
        # Usamos -O3 para que el compilador vuele y solo midamos la complejidad matemática
        subprocess.run(["gcc", "-O0", C_FILENAME, "-o", EXECUTABLE], check=True)
        print("✅ Compilación exitosa.\n")
    except subprocess.CalledProcessError:
        print("❌ Error al compilar. Asegúrate de tener gcc instalado y que el archivo se llame mochila.c")
        exit(1)

def ejecutar_benchmark():
    print(f"🚀 INICIANDO BENCHMARK DEL PEOR CASO (N={N})")
    print("-" * 50)
    
    for T in T_values:
        # Llamamos al ejecutable pasándole N y T como argumentos
        result = subprocess.run([EXECUTABLE, str(N), str(T)], capture_output=True, text=True)
        
        # Buscamos la línea "Execution time: X.XXXXX seconds" en la salida del C
        match = re.search(r"Execution time:\s*([0-9.]+)", result.stdout)
        
        if match:
            tiempo = float(match.group(1))
            tiempos.append(tiempo)
            print(f"Capacidad T = {T:3d} | Tiempo = {tiempo:.6f} s")
        else:
            print(f"Capacidad T = {T:3d} | ❌ Error al leer la salida: {result.stdout}")
            tiempos.append(0.0)

    print("-" * 50)

def graficar():
    plt.figure(figsize=(10, 6))
    plt.plot(T_values, tiempos, marker='o', linestyle='-', color='#d62728', linewidth=2)
    plt.fill_between(T_values, tiempos, color='#d62728', alpha=0.15)

    # Etiquetas y título
    plt.title(f'Maximo del Coeficiente Binomial: El Peor Caso del B&B\nN = {N} | $t_i$ = 10, $p_i$ = 10', fontsize=14, pad=15)
    plt.xlabel('Tiempo total (T)', fontsize=12)
    plt.ylabel('Tiempo de Ejecución (segundos)', fontsize=12)
    plt.grid(True, linestyle='--', alpha=0.7)

    # Marcar el punto crítico (La mitad exacta del peso total)
    ecuador = (N * 10) // 2
    plt.axvline(x=ecuador, color='black', linestyle='--', label=f'n/2 / Pico Máximo (T = [151, 159])')
    
    plt.legend()
    plt.tight_layout()
    plt.show()

if __name__ == '__main__':
    if not os.path.exists(C_FILENAME):
        print(f"⚠️ No se encontró el archivo '{C_FILENAME}'. Guárdalo en esta misma carpeta.")
    else:
        compilar_c()
        ejecutar_benchmark()
        graficar()