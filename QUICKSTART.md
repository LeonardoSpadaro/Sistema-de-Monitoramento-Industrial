# QUICK START GUIDE

## Project: Industrial Real-Time Monitoring System

### 30-Second Overview

A complete C++ system for industrial sensor monitoring with:
- **6 Data Structures** (AVL, Hash, Heap, Fenwick, Bloom, OptimizedArray)
- **3 Analytics Operations** (Statistics, Anomaly Detection, Trends)  
- **6 Benchmarking Scenarios** (Normal, Memory, CPU, Latency, Large Dataset, Scaling)
- **Terminal UI** for easy interaction

---

## Build in 3 Steps

```powershell
# Step 1: Navigate to project
cd D:\VSCODE\industrial-monitoring

# Step 2: Create and enter build directory
mkdir build
cd build

# Step 3: Build
cmake ..
cmake --build . --config Release
```

**Result:** `industrial_monitoring.exe` in the `build` folder

---

## Run in Seconds

```powershell
.\industrial_monitoring.exe
```

### First Run Test (2 minutes)

```
Select option: 2                 [Generate synthetic data]
Enter sample count: 10000        [Press Enter]
Select option: 3                 [Run analytics]
Select option: 5                 [Benchmark single structure]
Select option: 1                 [AVL Tree]
Select option: 6                 [Exit]
```

---

## Complete Benchmark (4-5 minutes)

```
Select option: 2                 [Generate data]
Enter sample count: 10000        [Press Enter]
Select option: 4                 [Run comprehensive benchmark]
                                 [Watch all 36 tests complete]
Select option: 6                 [Exit]
```

---

## Menu Reference

| Option | Action | Time |
|--------|--------|------|
| 1 | Load CSV data file | <1s |
| 2 | Generate 10K synthetic data | ~1s |
| 3 | Analytics & anomalies | ~1s |
| 4 | **Full benchmark (36 tests)** | **3-5 min** |
| 5 | Test one data structure | ~10s |
| 6 | Exit | N/A |

---

## What You'll See

### After Analytics (Option 3):
```
=== Analytics Results ===
Mean temperature: 99.XX °C
Max vibration: 4.XX m/s²
Min signal strength: 30.XX dBm
Anomalies detected: XXX (X.X%)
Temperature trend samples (first 5 deltas): ...
```

### After Single Benchmark (Option 5):
```
────────────────────────────────────────────────────────
Scenario: Normal
Data structure: AVL Tree
Sample count: 10000
Insert time (ms): 2.50
Search time (ms): 0.80
Delete time (ms): 2.00
Throughput (ops/sec): 1.2e+06
Estimated memory (MB): 0.76
Constraints passed: YES
────────────────────────────────────────────────────────
```

---

## Key Files

| File | Purpose |
|------|---------|
| `README.md` | Full documentation |
| `ARCHITECTURE.md` | Design details |
| `TESTING.md` | Testing guide |
| `SUMMARY.md` | Project overview |
| `src/main.cpp` | Entry point |
| `src/data_structures.cpp` | All 6 structures |
| `src/benchmark.cpp` | 6 scenarios |
| `src/ui.cpp` | Menu interface |

---

## System Information

- **Language**: C++17
- **Platform**: Windows (PowerShell)
- **Build System**: CMake 3.10+
- **Compilation**: 5-10 seconds
- **Runtime Memory**: <200MB typical
- **Dependencies**: None (standard library only)

---

## Troubleshooting

### CMake not found
```powershell
# Install from: https://cmake.org/download/
# Or use: choco install cmake
```

### No C++ compiler
```powershell
# Install: Visual Studio Build Tools
# Or: gcc/clang via MinGW or WSL
```

### "Failed to load dataset"
- Check file exists in `data/` folder
- Use space-separated format: `id temp vib signal latency bandwidth`

### Program crashes
- Build in **Release** mode: `cmake --build . --config Release`
- Run with smaller dataset: 10000 samples

---

## What Gets Tested

### Data Structures
- ✅ AVL Tree (self-balancing)
- ✅ Hash Table (O(1) lookup)
- ✅ Min Heap (priority queue)
- ✅ Fenwick Tree (range queries)
- ✅ Bloom Filter (membership test)
- ✅ Optimized Sensor Array (cache-friendly)

### Scenarios
1. **Normal** - Baseline performance
2. **Memory Constrained** - 64MB limit
3. **CPU Intensive** - Heavy load
4. **Latency Critical** - <10ms operations
5. **Large Dataset** - 100K samples
6. **Algorithm Scaling** - Big-O verification

### Analytics
- Temperature statistics
- Vibration analysis
- Signal strength monitoring
- Anomaly detection (3-sigma rule)
- Trend prediction

---

## Expected Performance

| Structure | Insert | Search | Memory |
|-----------|--------|--------|--------|
| AVL | 2.5ms | 0.8ms | 0.76MB |
| Hash | 1.2ms | 0.3ms | 1.52MB |
| Heap | 1.8ms | N/A | 0.76MB |
| Fenwick | 0.5ms | 0.4ms | 0.08MB |
| Bloom | 0.3ms | 0.2ms | 0.10MB |
| Optimized | 0.8ms | 0.2ms | 0.61MB |

*(Times for 10K samples, varies by hardware)*

---

## Documentation

- **README.md** - Complete feature guide (2,500 words)
- **ARCHITECTURE.md** - Design patterns (1,500 words)
- **TESTING.md** - Validation procedures
- **SUMMARY.md** - Project completion report

---

## Next Steps

1. **Build the project** (3 commands above)
2. **Run first test** (menu options 2→3→6)
3. **Run full benchmark** (option 4)
4. **Review results** in terminal output
5. **Check documentation** for details

---

## Support Resources

- **Code Comments**: See `src/` files for implementation details
- **Headers**: Function documentation in `.h` files
- **Makefile**: `CMakeLists.txt` for build configuration
- **Data**: `data/sample_data.csv` has 50 example records

---

**Total Time to Working System: ~5 minutes**

Good luck! 🚀
