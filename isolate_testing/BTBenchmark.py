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
N_VALUES = [20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30]

# Lines: The integer multipliers to pass to argv[2]
# The C code divides these by 10. 
# 2 = Tight (20%), 5 = Mid (50%), 8 = Loose (80%)
T_MULTIPLIERS = [200, 300, 400, 500, 600, 700, 800]

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
            try:
                # Calls: ./test_backtracking <n> <mult>
                process = subprocess.run(
                    [EXECUTABLE, str(n), str(mult)],
                    capture_output=True, text=True, check=True, timeout=4000
                )
                
                match = re.search(r"Execution time:\s*([0-9.]+)", process.stdout)
                
                if match:
                    exec_time = float(match.group(1))
                    benchmark_data[mult]['n'].append(n)
                    benchmark_data[mult]['time'].append(exec_time)
                    print(f"{mult:<6} | {n:<8} | {exec_time:.6f}")
                else:
                    print(f"{mult:<6} | {n:<8} | Parse Error")

            except subprocess.TimeoutExpired:
                print(f"{mult:<6} | {n:<8} | TIMEOUT (>4000s)")
                # Don't try larger N for this specific multiplier
                break
            except subprocess.CalledProcessError:
                print(f"{mult:<6} | {n:<8} | Runtime Error")

    return benchmark_data

def plot_results(data):
    plt.figure(figsize=(10, 6))
    
    # Plot a line for each Multiplier
    for mult, values in data.items():
        if values['n']: 
            label_text = f'T = {mult}'
            plt.plot(values['n'], values['time'], marker='o', label=label_text)

    plt.title('Backtracking Performance: Sin poda ni pasar los valores actuales')
    plt.xlabel('Cantidad de asignaturas (n)')
    plt.ylabel('Tiempo de ejecución (segundos)')
    plt.grid(True)
    plt.legend()
    
    output_file = "performance_TrulyNaiveTest.png"
    plt.savefig(output_file)
    print(f"\nGraph saved as '{output_file}'")
    plt.show()

if __name__ == "__main__":
    if compile_c_code():
        data = run_benchmark()
        plot_results(data)