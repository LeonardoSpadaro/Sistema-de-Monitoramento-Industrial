#pragma once
#include <cstddef>
#include <functional>
#include <string>
#include <vector>

// Estrutura para os dados do Dataset [4]
struct SensorRecord {
    long id;
    double temperature;
    double vibration;
    double signalStrength;
    double latency;
    double bandwidth;
};

// Estrutura para armazenar resultados das operações analíticas [3]
struct DataStats {
    double meanTemperature;
    double maxVibration;
    double minSignal;
    double anomalyScore;
};

// Estrutura Otimizada: OptimizedSensorArray (Domínio Industrial) [5, 6]
class OptimizedSensorArray {
public:
    OptimizedSensorArray();
    ~OptimizedSensorArray() = default;

    bool insert(const SensorRecord& record);
    bool remove(long id);
    SensorRecord* search(long id);
    size_t size() const;

    
    double computeTemperatureMean() const;
    double computeVibrationStd() const;

private:
    static constexpr size_t INITIAL_CAPACITY = 1024;

   
    std::vector<long> ids;
    std::vector<double> temperatures;
    std::vector<double> vibrations;
    std::vector<double> signalStrengths;
    std::vector<double> latencies;
    std::vector<double> bandwidths;
    std::vector<bool> active;

    size_t findIndex(long id) const;
    void reallocate();
};

// 1. Árvore AVL:  O(log n) [7]
struct AVLNode {
    SensorRecord record;
    AVLNode* left;
    AVLNode* right;
    int height;
};

class AVLTree {
public:
    AVLTree();
    ~AVLTree();
    bool insert(const SensorRecord& record);
    bool remove(long id);
    SensorRecord* search(long id);
    size_t size() const;
private:
    AVLNode* root;
    size_t count;
    AVLNode* insertNode(AVLNode* node, const SensorRecord& record, bool& inserted);
    AVLNode* removeNode(AVLNode* node, long id, bool& removed);
    AVLNode* minValueNode(AVLNode* node);
    int height(AVLNode* node);
    int balanceFactor(AVLNode* node);
    AVLNode* rotateRight(AVLNode* y);
    AVLNode* rotateLeft(AVLNode* x);
    void deleteTree(AVLNode* node);
};

// 2. Tabela Hash:  O(1) [7]
class HashTable {
public:
    explicit HashTable(size_t capacity = 16384);
    ~HashTable();
    bool insert(const SensorRecord& record);
    bool remove(long id);
    SensorRecord* search(long id);
    size_t size() const;
private:
    struct Entry { long key; SensorRecord value; bool occupied; bool deleted; };
    std::vector<Entry> table;
    size_t count;
    size_t capacity;
    size_t hashKey(long key) const;
};

// 3. Min Heap: [7]
class MinHeap {
public:
    MinHeap();
    bool insert(const SensorRecord& record);
    bool removeMin();
    SensorRecord* top();
    size_t size() const;
private:
    std::vector<SensorRecord> heap;
    void siftUp(size_t idx);
    void siftDown(size_t idx);
};

// 4. Fenwick Tree: [5, 8]
class FenwickTree {
public:
    explicit FenwickTree(size_t n);
    void update(size_t idx, double delta);
    double query(size_t idx) const;
    double rangeQuery(size_t left, size_t right) const;
private:
    std::vector<double> data;
    size_t n;
};

// 5. Bloom Filter: [5, 9]
class BloomFilter {
public:
    explicit BloomFilter(size_t sizeBits, size_t hashCount);
    void insert(const std::string& value);
    bool contains(const std::string& value) const;
private:
    std::vector<bool> bits;
    size_t sizeBits;
    size_t hashCount;
    size_t hash1(const std::string& value) const;
    size_t hash2(const std::string& value) const;
};



DataStats calculateStatistics(const std::vector<SensorRecord>& records);


bool detectAnomaly(const SensorRecord& record);


std::vector<double> computeTrend(const std::vector<SensorRecord>& records);