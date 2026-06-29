#include "dataset.h"
#include <fstream>
#include <random>
#include <sstream>
#include <vector>
#include <string>

std::vector<SensorRecord> loadDataset(const std::string& path) {
    std::vector<SensorRecord> records;
    std::ifstream file(path);

    if (!file.is_open()) {
        return records;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        SensorRecord rec;

        if (!(ss >> rec.id >> rec.temperature >> rec.vibration >> rec.signalStrength >> rec.latency >> rec.bandwidth)) {
            continue;
        }
        records.push_back(rec);
    }
    return records;
}

std::vector<SensorRecord> generateSyntheticData(size_t n) {
    std::vector<SensorRecord> records;
    records.reserve(n);

    std::mt19937_64 rng(123456789);

   
    std::normal_distribution<double> tempDist(70.0, 10.0);    
    std::normal_distribution<double> vibDist(2.0, 0.5);       
    std::normal_distribution<double> signalDist(70.0, 8.0);   
    std::normal_distribution<double> latencyDist(5.0, 1.5);
    std::normal_distribution<double> bandwidthDist(500.0, 50.0);

    for (size_t i = 0; i < n; ++i) {
        records.push_back(SensorRecord{
            static_cast<long>(i + 1),
            tempDist(rng),
            vibDist(rng),
            signalDist(rng),
            latencyDist(rng),
            bandwidthDist(rng)
            });
    }

    return records;
}