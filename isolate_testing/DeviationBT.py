import subprocess
import re
import matplotlib.pyplot as plt
import os
import statistics # Importante para calcular mean y stdev
import numpy as np # Importante para graficar las áreas sombreadas

# --- CONFIGURATION ---
SOURCE_FILE = "perf_BT.c"
EXECUTABLE = "./test_backtracking"

N_VALUES = [10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000, 100000,
            110000, 120000, 130000, 140000, 150000, 160000, 170000, 180000, 190000, 200000,
            210000, 220000, 230000, 240000, 250000, 260000, 270000, 280000, 290000, 300000,
            310000, 320000, 330000, 340000, 350000, 360000, 370000, 380000, 390000, 400000,
            410000, 420000, 430000, 440000, 450000, 460000, 470000, 480000, 490000, 500000,
            510000, 520000, 530000, 540000, 550000, 560000, 570000, 580000, 590000, 600000,
            610000, 620000, 630000, 640000, 650000, 660000, 670000, 680000, 690000, 700000,
            710000, 720000, 730000, 740000, 750000, 760000, 770000, 780000, 790000, 800000,
            810000, 820000, 830000, 840000, 850000, 860000, 870000, 880000, 890000, 900000,
            910000, 920000, 930000, 940000, 950000, 960000, 970000, 980000, 990000, 1000000,
            1100000, 1200000, 1300000, 1400000, 1500000, 1600000, 1700000, 1800000, 1900000, 2000000,
            2100000, 2200000, 2300000, 2400000, 2500000, 2600000, 2700000, 2800000, 2900000, 3000000,
            3100000, 3200000, 3300000, 3400000, 3500000, 3600000, 3700000, 3800000, 3900000, 4000000,
            4100000, 4200000, 4300000, 4400000, 4500000, 4600000, 4700000, 4800000, 4900000, 5000000] # Ajustado para enfocarnos en N más grandes
T_MULTIPLIERS = [10, 20, 30, 40, 50, 60, 70, 80, 90, 100]
NUM_RUNS = 10 

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
            
            label_text = f'T = {mult}% sum(t)'
            
            # Graficar la línea principal
            line, = plt.plot(n_arr, time_arr, marker='o', label=label_text)
            
            # Graficar la desviación estándar como un área sombreada
            plt.fill_between(n_arr, 
                             np.maximum(0, time_arr - std_arr), # Prevenir que el error baje de 0 segs
                             time_arr + std_arr, 
                             color=line.get_color(), 
                             alpha=0.2) # Alpha hace que sea semitransparente

    plt.title('Backtracking Performance: Integracion de Prefix Sums')
    plt.xlabel('Cantidad de asignaturas (n)')
    plt.ylabel('Tiempo de ejecución (segundos)')
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