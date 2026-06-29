#pragma once
#include "data_structures.h"
#include <string>
#include <vector>


std::vector<SensorRecord> loadDataset(const std::string& path);


std::vector<SensorRecord> generateSyntheticData(size_t n);