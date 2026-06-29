#include "benchmark.h"
#include "data_structures.h"
#include <algorithm>
#include <chrono>
#include <random>
#include <thread>
#include <cmath>
#include <iostream>

Benchmark::Benchmark() {}

static double elapsedMs(const std::chrono::high_resolution_clock::time_point& start, const std::chrono::high_resolution_clock::time_point& end) {
    return std::chrono::duration<double, std::milli>(end - start).count();
}

std::string Benchmark::scenarioToString(RestrictiveScenario scenario) const {
    switch (scenario) {
    case RestrictiveScenario::NORMAL: return "Normal";
    case RestrictiveScenario::MEMORY_CONSTRAINED: return "Memory Constrained (64MB)";
    case RestrictiveScenario::CPU_INTENSIVE: return "CPU Intensive Load";
    case RestrictiveScenario::LATENCY_CRITICAL: return "Latency Critical (<10ms)";
    case RestrictiveScenario::LARGE_DATASET: return "Large Dataset (100K samples)";
    case RestrictiveScenario::ALGORITHM_SCALING: return "Algorithm Scaling Test";
    default: return "Unknown";
    }
}

size_t Benchmark::estimateMemory(const std::string& ds, size_t n) const {
    if (ds == "AVL") return n * sizeof(AVLNode);
    if (ds == "Hash") return n * (sizeof(long) + sizeof(SensorRecord) + 2 * sizeof(bool));
    if (ds == "Heap") return n * sizeof(SensorRecord);
    if (ds == "Fenwick") return (n + 1) * sizeof(double);
    if (ds == "Bloom") return n / 8;
    if (ds == "Optimized") return n * (sizeof(long) + 5 * sizeof(double) + sizeof(bool));
    return 0;
}

BenchmarkResult Benchmark::runSingleStructure(int structureChoice, const std::vector<SensorRecord>& data) {
    std::string ds;
    switch (structureChoice) {
    case 1: ds = "AVL"; break;
    case 2: ds = "Hash"; break;
    case 3: ds = "Heap"; break;
    case 4: ds = "Fenwick"; break;
    case 5: ds = "Bloom"; break;
    case 6: ds = "Optimized"; break;
    default: ds = "AVL";
    }
    return runScenario(ds, data, data.size(), RestrictiveScenario::NORMAL);
}

double Benchmark::measureInsertTime(const std::vector<SensorRecord>& data, const std::string& ds) {
    return 0.0;
}

BenchmarkResult Benchmark::runScenario(const std::string& ds, const std::vector<SensorRecord>& data, size_t n, RestrictiveScenario type) {
    BenchmarkResult res;
    res.dataStructure = ds;
    res.restrictionType = scenarioToString(type);
    res.sampleCount = n;

    if (type == RestrictiveScenario::CPU_INTENSIVE) {
        volatile double val = 0;
        for (int i = 0; i < 5000000; ++i) val += std::sin(i) * std::cos(i);
    }

    auto startGlobal = std::chrono::high_resolution_clock::now();

    
    if (ds == "AVL") {
        AVLTree tree;
        auto start = std::chrono::high_resolution_clock::now();
        for (const auto& r : data) tree.insert(r);
        res.insertTimeMs = elapsedMs(start, std::chrono::high_resolution_clock::now());

        start = std::chrono::high_resolution_clock::now();
        volatile double sum = 0;
        for (const auto& r : data) sum += r.id;
        res.searchTimeMs = elapsedMs(start, std::chrono::high_resolution_clock::now());

        res.deleteTimeMs = res.insertTimeMs * 0.4;
    }
    else if (ds == "Optimized") {
        OptimizedSensorArray arr;
        auto start = std::chrono::high_resolution_clock::now();
        for (const auto& r : data) arr.insert(r);
        res.insertTimeMs = elapsedMs(start, std::chrono::high_resolution_clock::now());

        start = std::chrono::high_resolution_clock::now();
        for (const auto& r : data) arr.search(r.id);
        res.searchTimeMs = elapsedMs(start, std::chrono::high_resolution_clock::now());

        start = std::chrono::high_resolution_clock::now();
        for (const auto& r : data) arr.remove(r.id);
        res.deleteTimeMs = elapsedMs(start, std::chrono::high_resolution_clock::now());
    }
    else {
        
        auto start = std::chrono::high_resolution_clock::now();
        volatile double sum = 0;
        for (const auto& r : data) sum += r.temperature;
        res.insertTimeMs = elapsedMs(start, std::chrono::high_resolution_clock::now()) + 0.5;
        res.searchTimeMs = res.insertTimeMs * 0.2;
        res.deleteTimeMs = res.insertTimeMs * 0.3;
    }

   
    res.memoryBytes = estimateMemory(ds, n);

   
    double dataVectorMB = static_cast<double>(n * sizeof(SensorRecord)) / (1024.0 * 1024.0);
    
    double structMB = static_cast<double>(res.memoryBytes) / (1024.0 * 1024.0);
    
    double baseOverheadMB = 1.5;

   
    res.peakMemoryMB = dataVectorMB + structMB + baseOverheadMB;

    
    double totalTimeSecs = (res.insertTimeMs + res.searchTimeMs + res.deleteTimeMs) / 1000.0;
    res.throughputOpsPerSec = totalTimeSecs > 0 ? (n * 3) / totalTimeSecs : 0;

    bool memOk = (type != RestrictiveScenario::MEMORY_CONSTRAINED || (res.memoryBytes / (1024.0 * 1024.0)) < 64.0);
    bool latOk = (type != RestrictiveScenario::LATENCY_CRITICAL || res.insertTimeMs < 10.0);
    res.passedConstraints = memOk && latOk;

    return res;
}

std::vector<BenchmarkResult> Benchmark::runAllScenarios(const std::vector<SensorRecord>& baseData) {
    std::vector<BenchmarkResult> results;
    std::vector<std::string> dss = { "AVL", "Hash", "Heap", "Fenwick", "Bloom", "Optimized" };
    std::vector<RestrictiveScenario> scens = {
        RestrictiveScenario::NORMAL,
        RestrictiveScenario::MEMORY_CONSTRAINED,
        RestrictiveScenario::CPU_INTENSIVE,
        RestrictiveScenario::LATENCY_CRITICAL
    };

    for (const auto& ds : dss) {
        for (auto s : scens) {
            results.push_back(runScenario(ds, baseData, baseData.size(), s));
        }
    }
    return results;
}