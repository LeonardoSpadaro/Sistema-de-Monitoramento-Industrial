#include <fstream>
#include <random>
#include <iomanip>
#include <iostream>
#include <cmath>

int main(int argc, char* argv[]) {
    // Determine output file
    std::string outputFile = argc > 1 ? argv[1] : "data/sample_data_10k.csv";
    size_t numSamples = argc > 2 ? std::stoul(argv[2]) : 10000;
    
    std::cout << "Generating " << numSamples << " sensor records...\n";
    
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open output file " << outputFile << "\n";
        return 1;
    }
    
    // Seed for reproducibility
    std::mt19937_64 rng(42);
    
    // Distributions matched to realistic sensor ranges
    std::normal_distribution<double> tempDist(100.0, 15.0);      // Mean 100°C, StdDev 15
    std::exponential_distribution<double> vibDist(1.0 / 1.5);    // Mean vibration 1.5
    std::normal_distribution<double> signalDist(65.0, 10.0);     // Mean 65 dBm
    std::lognormal_distribution<double> latencyDist(1.5, 0.5);   // Realistic latency
    std::normal_distribution<double> bandwidthDist(500.0, 150.0);// Mean 500 Mbps
    
    // Clamp lambda for consistent ranges
    auto clamp = [](double value, double min, double max) {
        return std::max(min, std::min(max, value));
    };
    
    // Generate header
    file << "id temperature vibration signalStrength latency bandwidth\n";
    
    // Generate data
    for (size_t i = 0; i < numSamples; ++i) {
        long id = i + 1;
        double temperature = clamp(tempDist(rng), 20.0, 120.0);
        double vibration = clamp(vibDist(rng), 0.0, 5.0);
        double signalStrength = clamp(signalDist(rng), 30.0, 100.0);
        double latency = clamp(latencyDist(rng), 0.5, 20.0);
        double bandwidth = clamp(bandwidthDist(rng), 50.0, 1000.0);
        
        file << std::fixed << std::setprecision(2);
        file << id << " "
             << temperature << " "
             << vibration << " "
             << signalStrength << " "
             << latency << " "
             << bandwidth << "\n";
    }
    
    file.close();
    std::cout << "Successfully generated " << numSamples << " records to " << outputFile << "\n";
    
    return 0;
}
