# Architecture Document - Industrial Monitoring System

## System Overview

```
┌─────────────────────────────────────────────────────────────┐
│                   USER INTERFACE (Terminal)                  │
│              Menu-driven interaction system                  │
└──────────────────┬──────────────────────────────────────────┘
                   │
        ┌──────────┼──────────┐
        ▼          ▼          ▼
    ┌────────┐ ┌────────┐ ┌─────────┐
    │Dataset │ │Analytics│ │Benchmark│
    │Manager │ │ Module  │ │ System  │
    └────────┘ └────────┘ └─────────┘
        │          │          │
        └──────────┼──────────┘
                   ▼
    ┌──────────────────────────────────────┐
    │    Data Structures (6 types)         │
    │  - AVL Tree                          │
    │  - Hash Table                        │
    │  - Min Heap                          │
    │  - Fenwick Tree                      │
    │  - Bloom Filter                      │
    │  - Optimized Sensor Array (NEW)      │
    └──────────────────────────────────────┘
```

## Core Components

### 1. Data Structures (`data_structures.h/cpp`)

#### Classical Structures

**AVLTree**
- Self-balancing binary search tree
- Operations: O(log n)
- Use case: Sorted sequential access
- Memory: 32 bytes per node (pointer + data + metadata)
- Trade-off: Balancing overhead vs guaranteed O(log n)

```cpp
struct AVLNode {
    SensorRecord record;
    AVLNode* left;
    AVLNode* right;
    int height;  // For balance factor calculation
};
```

**HashTable**
- Linear probing collision resolution
- Operations: O(1) average, O(n) worst case
- Use case: Fast lookups
- Load factor: ~0.5 (resizes when exceeded)
- Memory: Entry = key + value + flags

```cpp
struct Entry {
    long key;
    SensorRecord value;
    bool occupied;
    bool deleted;  // Tombstone for lazy deletion
};
```

**MinHeap**
- Binary heap using array
- Operations: O(log n) insert/delete, O(1) peek
- Use case: Priority queue, event processing
- Memory: Simple vector of SensorRecords
- Ordering: By temperature (domain-specific)

#### Advanced Structures

**FenwickTree (Binary Indexed Tree)**
- Prefix sum data structure
- Operations: O(log n) update and query
- Use case: Range queries on temperatures
- Memory: O(n) single vector
- Advantage: No balancing overhead

```cpp
// Update delta at index i
void update(size_t idx, double delta) {
    for (size_t i = idx + 1; i <= n; i += i & -i)
        data[i] += delta;
}

// Query prefix sum up to index i
double query(size_t idx) const {
    double sum = 0.0;
    for (size_t i = idx + 1; i > 0; i -= i & -i)
        sum += data[i];
    return sum;
}
```

**BloomFilter**
- Probabilistic set membership
- Operations: O(1) insert and query
- Use case: Anomaly pre-filtering
- Memory: k hash functions × m bits
- Trade-off: False positives allowed, no false negatives

#### Domain-Optimized Structure (NEW)

**OptimizedSensorArray**
- Struct-of-arrays layout for cache efficiency
- Separate vectors per field: ids, temperatures, vibrations, etc.
- Benefits:
  - Cache line alignment (typically 64 bytes)
  - Reduced memory fragmentation
  - Sequential access patterns preferred by modern CPUs
  - Batch operations on single fields

```cpp
class OptimizedSensorArray {
    std::vector<long> ids;
    std::vector<double> temperatures;
    std::vector<double> vibrations;
    std::vector<double> signalStrengths;
    std::vector<double> latencies;
    std::vector<double> bandwidths;
    std::vector<bool> active;
};
```

**Memory Layout Comparison:**

Array-of-Structs (Traditional):
```
[id:1][temp:100][vib:1.5][signal:65][lat:5][bw:500] [id:2][temp:101]...
```
- Pointer chasing required
- Cache misses on access
- SIMD-unfriendly

Struct-of-Arrays (Optimized):
```
[id:1][id:2][id:3]... [temp:100][temp:101][temp:102]...
```
- Sequential memory access
- Cache line friendly
- SIMD-friendly for batch ops

### 2. Analytics Module (`data_structures.cpp`)

**Statistical Calculations**
```cpp
DataStats calculateStatistics(const std::vector<SensorRecord>& records) {
    // Compute mean, max, min across all metrics
    double sum = 0.0;
    for (const auto& r : records) {
        sum += r.temperature;
        stats.maxVibration = std::max(stats.maxVibration, r.vibration);
        stats.minSignal = std::min(stats.minSignal, r.signalStrength);
    }
    stats.meanTemperature = sum / records.size();
}
```

**Anomaly Detection**
```cpp
bool detectAnomaly(const SensorRecord& record, const DataStats& stats) {
    // Three-sigma rule for temperature
    // Threshold-based rules for other metrics
    return std::abs(record.temperature - stats.meanTemperature) > 3.0 ||
           record.vibration > stats.maxVibration * 0.9 ||
           record.signalStrength < stats.minSignal * 1.1;
}
```

**Trend Computation**
```cpp
std::vector<double> computeTrend(const std::vector<SensorRecord>& records) {
    // Calculate temperature deltas between consecutive samples
    std::vector<double> trend;
    for (size_t i = 1; i < records.size(); ++i) {
        trend.push_back(records[i].temperature - records[i-1].temperature);
    }
    return trend;
}
```

### 3. Benchmarking System (`benchmark.h/cpp`)

#### Design Patterns

**Strategy Pattern** for different scenarios:
- Each scenario implements constraint differently
- Pluggable performance measurement

**Template Method** for measurements:
- Start timer → Execute operation → End timer
- Consistent measurement methodology

```cpp
double measureTime(Operation op) {
    auto start = std::chrono::high_resolution_clock::now();
    op();  // Execute data structure operation
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}
```

#### The 5+1 Restrictive Scenarios

**1. Normal (Baseline)**
- Full system resources
- Reference performance metrics
- All structures tested equally

**2. Memory Constrained (64MB)**
- Simulates embedded systems, IoT devices
- Validates memory efficiency
- Fail-fast if structure exceeds limit
- Formula: `memory = (n * sizeof(data) + overhead)`

**3. CPU Intensive**
- Simulates manufacturing floor load
- Background CPU saturation via sin/cos loops
- Tests real-world contention
- Measurement: `baseline + cpu_overhead`

**4. Latency Critical (<10ms)**
- Real-time control constraints
- Safety-critical operations
- Validates operation timing
- Pass condition: All ops < 10ms per thousand samples

**5. Large Dataset (100K)**
- Scalability validation
- Long-running processes
- Memory stress testing
- Identifies scaling issues early

**6. Algorithm Scaling**
- Tests at 10K, 50K, 100K sizes
- Empirical Big-O verification
- Linear vs logarithmic behavior
- Expected: O(n log n) operations should show ~4-5x increase for 10x data

#### Measurement Methodology

**Throughput Calculation**
```cpp
double throughput_ops_per_sec = (total_operations / total_time_ms) * 1000
// For 10K insert + 1K search + 1K delete = 12K ops
// If total time = 5ms, then throughput = (12000 / 5) * 1000 = 2.4M ops/sec
```

**Memory Estimation**
```cpp
// Theoretical model based on data structure properties
size_t estimateMemory(const std::string& ds, size_t n) {
    if (ds == "AVL")      return n * sizeof(AVLNode);              // ~32 bytes
    if (ds == "Hash")     return n * (sizeof(long) + sizeof(...)); // ~48 bytes
    if (ds == "Heap")     return n * sizeof(SensorRecord);         // ~40 bytes
    if (ds == "Fenwick")  return (n + 1) * sizeof(double);         // ~8 bytes
    if (ds == "Bloom")    return n * 8 / 8;                        // ~1 byte (bitset)
    if (ds == "Optimized")return n * (sizeof(long) + 5*8 + 1);     // ~49 bytes
}
```

### 4. User Interface (`ui.h/cpp`)

**Menu-Driven Architecture**
```
Main Menu
├── Load Dataset (CSV file)
├── Generate Synthetic Dataset
├── Run Analytics
│   ├── Statistics calculation
│   ├── Anomaly detection
│   └── Trend computation
├── Run Full Benchmark
│   └── 6 DS × 6 scenarios = 36 runs
├── Run Single DS Benchmark
│   └── Select DS 1-6
└── Exit
```

**Output Formatting**
- Tabular results with separators
- Fixed and scientific notation
- Summary statistics
- Constraint compliance indicators

### 5. Data Management (`dataset.h/cpp`)

**CSV Loading**
```cpp
bool loadDataset(const std::string& path, std::vector<SensorRecord>& records) {
    std::ifstream file(path);
    std::string line;
    while (std::getline(file, line)) {
        // Parse: id temperature vibration signalStrength latency bandwidth
        SensorRecord rec;
        std::stringstream ss(line);
        ss >> rec.id >> rec.temperature >> rec.vibration >> ...;
        records.push_back(rec);
    }
}
```

**Synthetic Data Generation**
```cpp
std::vector<SensorRecord> generateSyntheticData(size_t n) {
    std::mt19937_64 rng(123456789);
    std::normal_distribution<> tempDist(100, 15);      // Realistic ranges
    std::exponential_distribution<> vibDist(1.0/1.5);
    
    // Generate realistic sensor readings
    for (size_t i = 0; i < n; ++i) {
        records.push_back({
            i+1,
            tempDist(rng),
            vibDist(rng),
            signalDist(rng),
            latencyDist(rng),
            bandwidthDist(rng)
        });
    }
}
```

## Design Decisions & Rationale

### Why These 5 Data Structures?

1. **AVL Tree**: Classic balanced structure, demonstrates balancing algorithms
2. **Hash Table**: Performance baseline for O(1) operations
3. **Min Heap**: Domain relevance (temperature monitoring priority)
4. **Fenwick Tree**: Range query optimization for time-series analytics
5. **Bloom Filter**: Probabilistic structures for anomaly pre-filtering

### Why OptimizedSensorArray?

- **Domain-specific optimization**: Leverages sensor data characteristics
- **Cache efficiency**: Struct-of-arrays layout exploits modern CPU caches
- **Real-world relevance**: Real industrial systems often use columnar storage
- **Batch operations**: Enables vectorization and SIMD opportunities

### Benchmarking Strategy

**Why 5+1 Scenarios?**
- Covers different industrial constraints
- Tests edge cases systematically
- Provides realistic validation
- Enables trade-off analysis

**Why Not Just Worst-Case?**
- Worst-case is overly pessimistic
- Average case better reflects real workloads
- Constraint-based scenarios more practical

## Compilation & Execution Flow

```
Source Code (.cpp/.h)
    ↓
Preprocessor (includes, macros)
    ↓
Compiler (CMake → MSVC/GCC/Clang)
    ↓
Linker (resolves symbols)
    ↓
Executable (industrial_monitoring.exe)
    ↓
Runtime
├── Load data structures
├── Initialize benchmark system
├── Execute UI loop
├── Process user commands
│   ├── Load/generate data
│   ├── Run analytics
│   └── Execute benchmarks
└── Display results
```

## Performance Characteristics Summary

| Structure | Insert | Search | Delete | Memory | Use Case |
|-----------|--------|--------|--------|--------|----------|
| AVL | O(log n) | O(log n) | O(log n) | O(n) | Sorted access |
| Hash | O(1) avg | O(1) avg | O(1) avg | O(n) | Random access |
| Heap | O(log n) | O(n) | O(log n) | O(n) | Priority queue |
| Fenwick | O(log n) | O(log n) | N/A | O(n) | Range queries |
| Bloom | O(1) | O(1) | N/A | O(1) | Membership test |
| Optimized | O(n) | O(n) | O(n) | O(n) | Batch ops |

## Extensibility Points

1. **New Data Structures**: Add to `data_structures.h/cpp`, update UI
2. **New Analytics**: Add functions in `data_structures.cpp`
3. **New Scenarios**: Add methods to `Benchmark` class
4. **Custom Metrics**: Extend `BenchmarkResult` struct
5. **Different Data Formats**: Extend `dataset.cpp` loading

## Thread Safety

Current implementation: **NOT thread-safe**
- Single-threaded by design
- Simplifies benchmarking (no threading overhead)
- Could add thread support via:
  - Fine-grained locking on data structures
  - Thread-local benchmark results
  - Atomic operations for metrics

## Future Enhancements

1. **Persistent Storage**: SQLite backend
2. **Real-time Dashboard**: Web UI via HTTP server
3. **Machine Learning**: Predictive maintenance via anomaly patterns
4. **Distributed**: Multi-sensor aggregation
5. **GPU Acceleration**: CUDA for batch operations
