import sys
import os
import subprocess
import pandas as pd
import numpy as np

def verify():
    print("Running Verification...")

    # 1. Clean previous dump
    if os.path.exists("memory_dump.csv"):
        os.remove("memory_dump.csv")

    # 2. Run Simulator
    # Assuming we are running from project root
    cmd = ["./build/simutpu_cli", "tests/vector_add_dump.asm", "config.json"]

    # Create dummy config if not exists
    if not os.path.exists("config.json"):
        with open("config.json", "w") as f:
            f.write("{}")

    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        print("Simulator failed:")
        print(result.stderr)
        sys.exit(1)

    print("Simulator Output:")
    print(result.stdout)

    # 3. Read Dump
    if not os.path.exists("memory_dump.csv"):
        print("Error: No memory dump produced.")
        sys.exit(1)

    df = pd.read_csv("memory_dump.csv")

    # Reconstruct data from bytes
    # We expect 3 64-bit integers: 20, 40, 60
    # Address 0x3000 starts first int

    # Helper to read int64 from df at specific address
    def read_int64(start_addr):
        val = 0
        for i in range(8):
            byte_val = df.loc[df['Address'] == (start_addr + i), 'Value'].values[0]
            val |= (byte_val << (i * 8))
        return val

    sim_res = [
        read_int64(0x3000),
        read_int64(0x3008),
        read_int64(0x3010)
    ]

    print(f"Simulator Result: {sim_res}")

    # 4. Golden Model (NumPy)
    a = np.array([10, 20, 30], dtype=np.int64)
    b = np.array([10, 20, 30], dtype=np.int64)
    expected = a + b

    print(f"Golden Model: {expected}")

    if np.array_equal(sim_res, expected):
        print("VERIFICATION PASSED")
        sys.exit(0)
    else:
        print("VERIFICATION FAILED")
        sys.exit(1)

if __name__ == "__main__":
    verify()
