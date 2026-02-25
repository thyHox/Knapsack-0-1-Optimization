import subprocess
import re
import matplotlib.pyplot as plt
import os

# --- CONFIGURATION ---
SOURCE_FILE = "perf_BT.c"
EXECUTABLE = "./test_backtracking"

# X-Axis: Problem Size
# Warning: If you implemented aggressive pruning, you might reach N=40.
# If simple recursion, N=30 might be your limit.
N_VALUES = [50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65]

# Lines: The integer multipliers to pass to argv[2]
# The C code divides these by 10. 
# 2 = Tight (20%), 5 = Mid (50%), 8 = Loose (80%)
T_MULTIPLIERS = [10, 20, 30, 40, 50, 60, 70, 80, 90, 100]

NUM_RUNS = 10 # How many times to run each test to calculate average

def compile_c_code():
    if not os.path.exists(SOURCE_FILE):
        print(f"Error: {SOURCE_FILE} not found.")
        return False
    print("Compiling BT code...")
    try:
        subprocess.check_call(["gcc", SOURCE_FILE, "-o", "test_backtracking", "-lrt"])
        print("Compilation successful.\n")
        return True
    except subprocess.CalledProcessError:
        print("Compilation failed.")
        return False

def run_benchmark():
    # Structure: { multiplier: { 'n': [], 'time': [] } }
    benchmark_data = {m: {'n': [], 'time': []} for m in T_MULTIPLIERS}

    print(f"{'%T':<6} | {'n':<8} | {'Time (s)':<15}")
    print("-" * 35)

    for mult in T_MULTIPLIERS:
        for n in N_VALUES:
            total_time = 0.0
            runs_completed = 0
            error_occurred = False
            
            # Ejectuar el programa varias veces para obtener un promedio
            for run_i in range(NUM_RUNS):
                try:
                    # Calls: ./test_backtracking <n> <mult>
                    process = subprocess.run(
                        [EXECUTABLE, str(n), str(mult)],
                        capture_output=True, text=True, check=True, timeout=4000
                    )
                    
                    match = re.search(r"Execution time:\s*([0-9.]+)", process.stdout)
                    
                    # Si search encontró el tiempo, lo parsea y acumula
                    if match:
                        exec_time = float(match.group(1))
                        total_time += exec_time
                        runs_completed += 1
                    else:
                        print(f"{mult:<6} | {n:<8} | Parse Error")
                        error_occurred = True

                except subprocess.TimeoutExpired:
                    print(f"{mult:<6} | {n:<8} | TIMEOUT (>4000s)")
                    error_occurred = True
                    # Don't try larger N for this specific multiplier
                    break
                except subprocess.CalledProcessError:
                    error_occurred = True
                    print(f"{mult:<6} | {n:<8} | Runtime Error")
            
            # Si no hubo errores y se completaron las runs, calcula el promedio y guarda los datos
            if not error_occurred and runs_completed == NUM_RUNS:
                avg_time = total_time / runs_completed
                benchmark_data[mult]['n'].append(n)
                benchmark_data[mult]['time'].append(avg_time)
                print(f"{mult:<6} | {n:<8} | {avg_time:.6f}")

    return benchmark_data

def plot_results(data):
    plt.figure(figsize=(10, 6))
    
    # Plot a line for each Multiplier
    for mult, values in data.items():
        if values['n']: 
            label_text = f'T = sum(t) * {mult}%'
            plt.plot(values['n'], values['time'], marker='o', label=label_text)

    plt.title('Backtracking Performance: Implementacion de criterio goloso')
    plt.xlabel('Cantidad de asignaturas (n)')
    plt.ylabel('Tiempo de ejecución (segundos)')
    plt.grid(True)
    plt.legend(title= "Porcentaje de la suma de los tiempos (T)")
    
    output_file = "performance_TrulyNaiveTest.png"
    plt.savefig(output_file)
    print(f"\nGraph saved as '{output_file}'")
    plt.show()

if __name__ == "__main__":
    if compile_c_code():
        data = run_benchmark()
        plot_results(data)