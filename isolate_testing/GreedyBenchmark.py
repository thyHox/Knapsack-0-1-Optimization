import subprocess
import re
import matplotlib.pyplot as plt
import os

# --- CONFIGURATION ---
SOURCE_FILE = "perf_GD.c"
EXECUTABLE = "./perf_GD"

# X-Axis: Greedy is fast, so we use large N
N_VALUES = [5000000, 10000000, 15000000, 20000000, 25000000, 30000000, 35000000, 40000000]  # n = number of subjects

# Lines: Constraint Levels
# 2 = 20%, 5 = 50%, 8 = 80%
T_MULTIPLIERS = [10, 20, 30, 40, 50, 60, 70, 80, 90, 100]  # T = multiplier * n

# REPETITIONS: High count recommended for fast algorithms
NUM_RUNS = 10

def compile_c_code():
    if not os.path.exists(SOURCE_FILE):
        print(f"Error: {SOURCE_FILE} not found.")
        return False
    print("Compiling Greedy code...")
    try:
        subprocess.check_call(["gcc", SOURCE_FILE, "-o", EXECUTABLE, "-O0"])
        print("Compilation successful.\n")
        return True
    except subprocess.CalledProcessError:
        print("Compilation failed.")
        return False

def run_benchmark():
    benchmark_data = {m: {'n': [], 'time': []} for m in T_MULTIPLIERS}

    print(f"Running each test {NUM_RUNS} times to calculate average...")
    print(f"{'T':<6} | {'n':<8} | {'Avg Time (s)':<15}")
    print("-" * 50)

    for mult in T_MULTIPLIERS:
        for n in N_VALUES:
            
            total_time = 0.0
            runs_completed = 0
            error_occurred = False
            
            for run in range(NUM_RUNS):
            # --- REPETITION LOOP ---
                try:
                    process = subprocess.run(
                        [EXECUTABLE, str(n), str(mult)],
                        capture_output=True, text=True, check=True
                    )
                    match = re.search(r"Execution time:\s*([0-9.]+)", process.stdout)
                    
                    if match:
                        total_time += float(match.group(1))
                        runs_completed += 1
                    else:
                        error_occurred = True
                        break 

                except subprocess.CalledProcessError:
                    error_occurred = True
                    break

            # --- CALCULATE AVERAGE ---
            if not error_occurred and runs_completed == NUM_RUNS:
                avg_time = total_time / runs_completed
                
                benchmark_data[mult]['n'].append(n)
                benchmark_data[mult]['time'].append(avg_time)
                
                print(f"{mult:<6} | {n:<8} | {avg_time:.6f}")
            else:
                print(f"{mult:<6} | {n:<8} | Error in runs")

    return benchmark_data

def plot_results(data):
    plt.figure(figsize=(10, 6))
    
    for mult, values in data.items():
        if values['n']:
            label_text = f'T = {mult}% de sum(t)'
            plt.plot(values['n'], values['time'], marker='o', label=label_text)

    plt.title(f'Greedy Performance: Implementacion de qsort() O(n log n) - Promedio de {NUM_RUNS} ejecuciones')
    plt.xlabel('Número de asignaturas (n)')
    plt.ylabel('Tiempo promedio de ejecución (segundos)')
    plt.grid(True)
    plt.legend(title="Valores de T")
    
    output_file = "performance_greedy_avg.png"
    plt.savefig(output_file)
    print(f"\nGraph saved as '{output_file}'")
    plt.show()

if __name__ == "__main__":
    if compile_c_code():
        data = run_benchmark()
        plot_results(data)