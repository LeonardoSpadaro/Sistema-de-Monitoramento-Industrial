#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <string>
#include <vector>
#include "data_structures.h"

enum class RestrictiveScenario {
    NORMAL,
    MEMORY_CONSTRAINED,
    CPU_INTENSIVE,
    LATENCY_CRITICAL,
    LARGE_DATASET,
    ALGORITHM_SCALING
};

struct BenchmarkResult {
    std::string dataStructure;
    std::string restrictionType;
    size_t sampleCount;
    double insertTimeMs;
    double searchTimeMs;
    double deleteTimeMs;
    size_t memoryBytes;
    double throughputOpsPerSec;
    bool passedConstraints;
    double peakMemoryMB;
    size_t peakMemoryBytes;
};

class Benchmark {
public:
    Benchmark();
    BenchmarkResult runSingleStructure(int structureChoice, const std::vector<SensorRecord>& data);
    std::vector<BenchmarkResult> runAllScenarios(const std::vector<SensorRecord>& baseData);

private:
    std::string scenarioToString(RestrictiveScenario scenario) const;
    size_t estimateMemory(const std::string& ds, size_t n) const;
    double measureInsertTime(const std::vector<SensorRecord>& data, const std::string& ds);
    BenchmarkResult runScenario(const std::string& ds, const std::vector<SensorRecord>& data, size_t n, RestrictiveScenario type);
};

#endif 