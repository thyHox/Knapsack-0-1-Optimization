import subprocess
import re
import matplotlib.pyplot as plt
import os
import statistics # Importante para calcular mean y stdev
import numpy as np # Importante para graficar las áreas sombreadas

# --- CONFIGURATION ---
SOURCE_FILE = "perf_BT.c"
EXECUTABLE = "./test_backtracking"

N_VALUES = [1000000, 2000000, 3000000, 4000000, 5000000] # Ajustado para enfocarnos en N más grandes
T_MULTIPLIERS = [10, 20, 30, 40, 50, 60, 70, 80, 90, 100] # Peor caso
NUM_RUNS = 1

def compile_c_code():
    if not os.path.exists(SOURCE_FILE):
        print(f"Error: {SOURCE_FILE} not found.")
        return False
    print("Compiling BT code...")
    try:
        # Añadido -O2 para optimizar el código recursivo
        subprocess.check_call(["gcc", SOURCE_FILE, "-o", "test_backtracking", "-O0", "-Wl,--stack,1073741824"]) # Aumentar el stack a 1GB para evitar overflow
        print("Compilation successful.\n")
        return True
    except subprocess.CalledProcessError:
        print("Compilation failed.")
        return False

def run_benchmark():
    # 1. Nueva estructura: Ahora guardamos 'std' (Standard Deviation)
    benchmark_data = {m: {'n': [], 'time': [], 'std': []} for m in T_MULTIPLIERS}

    print(f"{'%T':<6} | {'n':<8} | {'Avg Time (s)':<15} | {'Std Dev':<10}")
    print("-" * 50)

    for mult in T_MULTIPLIERS:
        for n in N_VALUES:
            run_times = [] # Guardamos cada run individualmente
            error_occurred = False
            
            for run_i in range(NUM_RUNS):
                try:
                    process = subprocess.run(
                        [EXECUTABLE, str(n), str(mult)],
                        capture_output=True, text=True, check=True, timeout=4000
                    )
                    
                    match = re.search(r"Execution time:\s*([0-9.]+)", process.stdout)
                    
                    if match:
                        exec_time = float(match.group(1))
                        run_times.append(exec_time) # Agregar a la lista
                    else:
                        print(f"{mult:<6} | {n:<8} | Parse Error")
                        error_occurred = True
                        break # Salir del loop de runs si hay error

                except subprocess.TimeoutExpired:
                    print(f"{mult:<6} | {n:<8} | TIMEOUT (>4000s)")
                    error_occurred = True
                    break # Si hace timeout una vez, abortamos este N
                except subprocess.CalledProcessError:
                    error_occurred = True
                    print(f"{mult:<6} | {n:<8} | Runtime Error")
                    break
            
            # 2. Calcular Promedio y Desviación Estándar
            if not error_occurred and len(run_times) == NUM_RUNS:
                avg_time = statistics.mean(run_times)
                # La desviación estándar requiere al menos 2 datos
                std_dev = statistics.stdev(run_times) if NUM_RUNS > 1 else 0.0 
                
                benchmark_data[mult]['n'].append(n)
                benchmark_data[mult]['time'].append(avg_time)
                benchmark_data[mult]['std'].append(std_dev) # Guardar std
                
                print(f"{mult:<6} | {n:<8} | {avg_time:.6f}        | {std_dev:.6f}")
            elif error_occurred:
                # Si este N falló/hizo timeout, detenemos los N más grandes para este multiplicador
                break 

    return benchmark_data

def plot_results(data):
    plt.figure(figsize=(10, 6))
    
    for mult, values in data.items():
        if values['n']: 
            # 3. Convertir a arrays de numpy para poder sumar/restar fácilmente
            n_arr = np.array(values['n'])
            time_arr = np.array(values['time'])
            std_arr = np.array(values['std'])
            
            label_text = f'T = {mult}% de suma de tiempos' if mult < 100 else 'T = suma total (Peor Caso)'
            
            # Graficar la línea principal
            line, = plt.plot(n_arr, time_arr, marker='o', label=label_text)
            
            # Graficar la desviación estándar como un área sombreada
            plt.fill_between(n_arr, 
                             np.maximum(0, time_arr - std_arr), # Prevenir que el error baje de 0 segs
                             time_arr + std_arr, 
                             color=line.get_color(), 
                             alpha=0.2) # Alpha hace que sea semitransparente

    plt.title('Optimizacion nodos hoja')
    plt.ylabel('Cantidad de asignaturas (n)')
    plt.xlabel('Tiempo de ejecución (segundos)')
    plt.grid(True, alpha=0.3)
    
    # 1. Mover la leyenda fuera del gráfico a la derecha
    plt.legend(
        title="Restricción de Capacidad", 
        bbox_to_anchor=(1.02, 1), # Coordenadas X, Y fuera del cuadro (1.02 es justo a la derecha)
        loc='upper left',         # Qué esquina de la leyenda anclar a esas coordenadas
        borderaxespad=0.          # Espaciado del borde
    )
    
    # 2. Ajustar el diseño para que la leyenda no quede recortada al guardar la imagen
    plt.tight_layout() 
    
    output_file = "performance_BT_Deviation.png"
    plt.savefig(output_file, bbox_inches='tight') # bbox_inches='tight' asegura que guarde todo
    print(f"\nGraph saved as '{output_file}'")
    plt.show()

if __name__ == "__main__":
    if compile_c_code():
        data = run_benchmark()
        plot_results(data)