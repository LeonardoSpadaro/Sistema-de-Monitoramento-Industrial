# Testing Guide - Industrial Monitoring System

This guide provides step-by-step instructions to test and validate the Industrial Monitoring System.

## Prerequisites

- C++ compiler (MSVC, GCC, or Clang)
- CMake 3.10+
- A Windows terminal or PowerShell

## Build Instructions

### 1. Create Build Directory

```powershell
cd D:\VSCODE\industrial-monitoring
mkdir build
cd build
```

### 2. Configure CMake

```powershell
cmake ..
```

### 3. Build the Project

```powershell
cmake --build . --config Release
```

### 4. Verify Executable

```powershell
dir *.exe
```

You should see `industrial_monitoring.exe` (or similar name)

## Running Tests

### Test 1: Generate Synthetic Dataset

```powershell
.\industrial_monitoring.exe
```

Select option **2** and enter sample count:
- For quick test: **10000**
- For comprehensive test: **50000**
- For stress test: **100000**

**Expected Output:**
```
Generated XXXXX synthetic records.
```

### Test 2: Run Analytics

After generating or loading data, select option **3**.

**Expected Output:**
```
=== Analytics Results ===
Mean temperature: 99.XX °C
Max vibration: 4.XX m/s²
Min signal strength: 30.XX dBm
Anomalies detected: XXX (X.X%)
Temperature trend samples (first 5 deltas): ...
```

### Test 3: Single Data Structure Benchmark

Select option **5** and choose a data structure (1-6):

**Expected Output (example for AVL Tree):**
```
────────────────────────────────────────────────────────────────────────────
Scenario: Normal
Data structure: AVL Tree
Sample count: 10000
Insert time (ms): 2.50
Search time (ms): 0.80
Delete time (ms): 2.00
Throughput (ops/sec): 1.2e+06
Estimated memory (MB): 0.76
Peak memory (MB): 0.76
Constraints passed: YES
────────────────────────────────────────────────────────────────────────────
```

### Test 4: Comprehensive Benchmark (All Scenarios)

Select option **4** with 10,000 samples loaded.

**Expected Duration:** 2-5 minutes

**Expected Output:**
- 6 data structures × 6 scenarios = 36 total benchmark runs
- Each scenario shows performance metrics
- Summary statistics at the end

### Test 5: Load External Data

Select option **1** and provide a path to a data file:
```
data/sample_data.csv
```

**Expected Output:**
```
Loaded 50 records.
```

## Validation Checklist

### Data Structures
- [ ] AVL Tree: Balancing with rotations working
- [ ] Hash Table: Linear probing collision handling
- [ ] Min Heap: Heap property maintained
- [ ] Fenwick Tree: Range queries accurate
- [ ] Bloom Filter: Membership testing operational
- [ ] Optimized Array: Contiguous memory layout

### Operations
- [ ] Insert: No duplicates, correct ordering
- [ ] Search: Returns correct records
- [ ] Delete: Updates counts properly

### Analytics
- [ ] Statistics: Mean, max, min calculations correct
- [ ] Anomaly Detection: Identifies outliers (>3σ)
- [ ] Trend: Computes temperature deltas

### Benchmarking Scenarios
- [ ] Normal: Baseline established
- [ ] Memory Constrained: Fits in 64MB limit
- [ ] CPU Intensive: Handles load
- [ ] Latency Critical: <10ms compliance
- [ ] Large Dataset: Scales to 100K
- [ ] Algorithm Scaling: O(n log n) or better

## Performance Expectations

### Baseline (10K samples on modern CPU)

| Operation | Expected Time | Tolerance |
|-----------|---------------|-----------|
| Insert 10K | 0.5-5 ms | ±50% |
| Search 1K | 0.1-2 ms | ±50% |
| Delete 1K | 0.5-3 ms | ±50% |

### Memory Usage (10K samples)

| Structure | Expected MB | Tolerance |
|-----------|------------|-----------|
| AVL Tree | 0.7-1.0 | ±20% |
| Hash Table | 1.4-1.8 | ±20% |
| Min Heap | 0.7-0.8 | ±20% |
| Fenwick Tree | 0.07-0.1 | ±20% |
| Bloom Filter | 0.08-0.12 | ±20% |
| Optimized Array | 0.6-0.8 | ±20% |

## Troubleshooting

### Build Failures

**Error:** `cmake: command not found`
- **Solution:** Install CMake or add to PATH

**Error:** `CXX compiler not found`
- **Solution:** Install MSVC, GCC, or Clang

### Runtime Issues

**Error:** `Failed to load dataset`
- **Solution:** Check file path and format (space-separated columns)

**Error:** `Segmentation fault`
- **Solution:** Build in Release mode: `cmake --build . --config Release`

**Error:** Benchmarks running very slowly
- **Solution:** Reduce sample count or check CPU temperature

## Advanced Testing

### Stress Testing (100K samples)

```powershell
# Generate large dataset
Option 2, enter 100000

# Run full benchmark
Option 4
```

### Memory Profiling

Use your system's memory monitor:
```powershell
# Windows Task Manager
taskmgr.exe

# Or PowerShell
Get-Process industrial_monitoring | Select-Object *Memory*
```

### Comparing Data Structures

1. Run single benchmark for each (Option 5)
2. Record throughput and memory metrics
3. Compare against theoretical complexity

### Custom Dataset Testing

1. Prepare CSV file: `id temperature vibration signalStrength latency bandwidth`
2. Place in `data/` directory
3. Load via Option 1
4. Run analytics (Option 3)
5. Run benchmarks (Option 4 or 5)

## Expected Test Results Summary

A successful test run should show:

✅ All data structures operational
✅ Analytics calculations complete
✅ Benchmark metrics generated
✅ Constraint compliance verified
✅ No crashes or memory leaks
✅ Reasonable performance scaling
✅ All 6 scenarios completed

## Performance Validation

### Theoretical vs. Practical

**AVL Tree:**
- Theory: O(log n) operations
- Practice: Should see ~10-15 ms for 10K operations

**Hash Table:**
- Theory: O(1) average case
- Practice: Should be fastest among comparison structures

**Fenwick Tree:**
- Theory: O(log n) updates and queries
- Practice: Very fast for range queries

## Notes

- First run may be slower due to OS caching
- CPU temperature affects performance consistency
- Background processes can impact results
- Results are hardware-dependent

## Validation Scripts

### Quick Validation

```powershell
# Generate data and run one benchmark
$input = @"
2
10000
5
1
6
"@
$input | .\industrial_monitoring.exe
```

### Full Validation

```powershell
# Comprehensive test script
$input = @"
2
10000
3
5
1
4
5
2
5
3
6
"@
$input | .\industrial_monitoring.exe
```

## Documentation Files

- [README.md](README.md) - Main project documentation
- [ARCHITECTURE.md](ARCHITECTURE.md) - Detailed architecture (create as needed)
- [BENCHMARK_RESULTS.md](BENCHMARK_RESULTS.md) - Sample benchmark results (create as needed)

## Contact / Support

For issues or questions about the system, refer to:
- Code comments for implementation details
- Function headers for API documentation
- README.md for architecture overview
