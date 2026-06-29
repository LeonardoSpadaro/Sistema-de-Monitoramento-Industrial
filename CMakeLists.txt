cmake_minimum_required(VERSION 3.10)
project(IndustrialMonitoring CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(industrial_monitoring
    src/main.cpp
    src/data_structures.cpp
    src/data_structures.h
    src/benchmark.cpp
    src/benchmark.h
    src/ui.cpp
    src/ui.h
    src/dataset.cpp
    src/dataset.h
)
