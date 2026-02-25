import subprocess
import re
import matplotlib.pyplot as plt
import os

# --- CONFIGURATION ---
SOURCE_FILE = "perf_DP.c"
EXECUTABLE = "./test_dinamico"

# X-Axis: Problem Size
N_VALUES = [500, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000]

# Lines: T values
T_VALUES = [50, 250, 500, 750, 1000, 1500, 2000, 3000, 4000, 5000, 10000]

# REPETITIONS: How many times to run each test to calculate average
NUM_RUNS = 10

# Safety Limit: ~8GB RAM (2 billion integers)
SAFETY_LIMIT = 2000000000

def compile_c_code():
    if not os.path.exists(SOURCE_FILE):
        print(f"Error: {SOURCE_FILE} not found.")
        return False
    print("Compiling DP code...")
    try:
        subprocess.check_call(["gcc", SOURCE_FILE, "-o", "test_dinamico", "-O0"])
        print("Compilation successful.\n")
        return True
    except subprocess.CalledProcessError:
        print("Compilation failed.")
        return False

def run_benchmark():
    benchmark_data = {m: {'n': [], 'time': []} for m in T_VALUES}

    print(f"Running each test {NUM_RUNS} times to calculate average...")
    print(f"{'Mult':<6} | {'n':<8} | {'T':<10} | {'Est RAM':<10} | {'Avg Time (s)':<15}")
    print("-" * 65)

    for t in T_VALUES:
        for n in N_VALUES:
            
            # Memory Safety Check
            if (n * t) > SAFETY_LIMIT:
                print(f"{t:<6} | {n:<8} | {t:<10} | >8GB       | SKIPPED (RAM Limit)")
                continue

            total_time = 0.0
            runs_completed = 0
            error_occurred = False

            # --- REPETITION LOOP ---
            for run_i in range(NUM_RUNS):
                try:
                    process = subprocess.run(
                        [EXECUTABLE, str(n), str(t)],
                        capture_output=True, text=True, check=True
                    )
                    match = re.search(r"Elapsed time:\s*([0-9.]+)", process.stdout)
                    
                    if match:
                        total_time += float(match.group(1))
                        runs_completed += 1
                    else:
                        error_occurred = True
                        break # Stop repeating if output is broken

                except subprocess.CalledProcessError:
                    error_occurred = True
                    break # Stop repeating if runtime error

            # --- CALCULATE AVERAGE ---
            if not error_occurred and runs_completed == NUM_RUNS:
                avg_time = total_time / NUM_RUNS
                
                # Store data
                benchmark_data[t]['n'].append(n)
                benchmark_data[t]['time'].append(avg_time)
                
                ram_mb = (n * t * 4) / (1024*1024)
                print(f"{t:<6} | {n:<8} | {t:<10} | {ram_mb:<10.0f} MB     | {avg_time:<6.6f}")
            else:
                print(f"{t:<6} | {n:<8} | {t:<10} | -          | Error in runs")

    return benchmark_data

def plot_results(data):
    plt.figure(figsize=(10, 6))
    
    for t, values in data.items():
        if values['n']:
            label_text = f'T = {t}'
            plt.plot(values['n'], values['time'], marker='o', label=label_text)

    plt.title(f'DP Performance: O(n·T) - Promedio de {NUM_RUNS} ejecuciones')
    plt.xlabel('Número de elementos (n)')
    plt.ylabel('Tiempo promedio de ejecución (segundos)')
    plt.grid(True)
    plt.legend(title="Tiempo total")
    
    output_file = "performance_dp_avg.png"
    plt.savefig(output_file)
    print(f"\nGraph saved as '{output_file}'")
    plt.show()

if __name__ == "__main__":
    if compile_c_code():
        data = run_benchmark()
        plot_results(data)