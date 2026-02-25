import subprocess
import re
import matplotlib.pyplot as plt
import os

# --- CONFIGURATION ---
SOURCE_FILE = "perf_GD.c"
EXECUTABLE = "./perf_GD"

# X-Axis: Greedy is fast, so we use large N
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
            4100000, 4200000, 4300000, 4400000, 4500000, 4600000, 4700000, 4800000, 4900000, 5000000]

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

            # --- REPETITION LOOP ---
            for run_i in range(NUM_RUNS):
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
                avg_time = total_time / NUM_RUNS
                
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
            label_text = f'T = {mult}% de suma total de tiempos'
            plt.plot(values['n'], values['time'], marker='o', label=label_text)

    plt.title(f'Greedy Performance: O(nlog(n)) - Promedio de {NUM_RUNS} ejecuciones')
    plt.xlabel('Número de elementos (n)')
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