#include "ui.h"
#include "dataset.h"
#include "benchmark.h"
#include "data_structures.h"
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm> 

static void printMenu() {
    std::cout << "\n=== Industrial Monitoring System ===\n";
    std::cout << "1. Load dataset from file\n";
    std::cout << "2. Generate synthetic dataset\n";
    std::cout << "3. Run analytics and anomaly detection\n";
    std::cout << "4. Run comprehensive benchmark (36 tests)\n";
    std::cout << "5. Run single data structure benchmark\n";
    std::cout << "6. Exit\n";
    std::cout << "Select option: ";
}

void displayBenchmarkResult(const BenchmarkResult& result) {
    std::cout << "\n" << std::string(80, '-') << "\n";
    std::cout << "Scenario: " << result.restrictionType << "\n";
    std::cout << "Data structure: " << result.dataStructure << "\n";
    std::cout << "Sample count: " << result.sampleCount << "\n";

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Insert time (ms): " << result.insertTimeMs << "\n";
    std::cout << "Search time (ms): " << result.searchTimeMs << "\n";
    std::cout << "Delete time (ms): " << result.deleteTimeMs << "\n";

    std::cout << std::scientific;
    std::cout << "Throughput (ops/sec): " << result.throughputOpsPerSec << "\n";

    std::cout << std::fixed;
    std::cout << "Estimated memory (MB): " << (double)result.memoryBytes / (1024 * 1024) << "\n";

    
    std::cout << "Peak memory (MB): " << result.peakMemoryMB << "\n";

    std::cout << "Constraints passed: " << (result.passedConstraints ? "YES" : "NO") << "\n";
    std::cout << std::string(80, '-') << "\n";
}

void runUserInterface() {
    std::vector<SensorRecord> records;
    Benchmark benchmark;
    bool running = true;
    int choice;

    while (running) {
        printMenu();
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            continue;
        }

        switch (choice) {
        case 1: {
            std::string path;
            std::cout << "Enter dataset path (e.g., data/data.csv): ";
            std::cin >> path;
            records = loadDataset(path);
            if (!records.empty()) {
                std::cout << "Successfully loaded " << records.size() << " records.\n";
            }
            break;
        }
        case 2: {
            int count;
            std::cout << "Enter sample count (min 10000): ";
            std::cin >> count;
            records = generateSyntheticData(count);
            break;
        }
        case 3: {
            if (records.empty()) {
                std::cout << "No data loaded. Load (1) or Generate (2) first.\n";
                break;
            }
            calculateStatistics(records);

            long long anomalies = std::count_if(records.begin(), records.end(),
                [](const SensorRecord& r) { return detectAnomaly(r); });

            std::cout << "Anomalies detected (3-sigma rule): " << anomalies << "\n";
            computeTrend(records);
            break;
        }
        case 4: {
            if (records.empty()) {
                std::cout << "Generating 10000 samples for benchmarking...\n";
                records = generateSyntheticData(10000);
            }
            auto results = benchmark.runAllScenarios(records);
            for (const auto& res : results) displayBenchmarkResult(res);
            break;
        }
        case 5: {
            int structChoice;
            std::cout << "Select structure (1:AVL, 2:Hash, 3:Heap, 4:Fenwick, 5:Bloom, 6:Optimized): ";
            std::cin >> structChoice;
            auto result = benchmark.runSingleStructure(structChoice, records);
            displayBenchmarkResult(result);
            break;
        }
        case 6:
            running = false;
            break;
        default:
            std::cout << "Invalid option.\n";
        }
    }
}