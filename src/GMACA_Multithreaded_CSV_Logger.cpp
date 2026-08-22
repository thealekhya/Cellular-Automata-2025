#include <iostream>
#include <bitset>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <fstream>
#include <algorithm>
#include <limits>
#include <sstream>
#include <thread>
#include <mutex>
#include <random>
#include <ctime>

#define MaxSize 10

using namespace std;
using config_t = bitset<MaxSize>;

mutex file_mutex;

class GMACA {
private:
    int cellCount;
    vector<int> ruleVector;
    vector<unordered_map<string, int>> ruleMaps;
    const vector<int> RULE_POOL = {
        204, 205, 206, 200, 196, 220, 236, 140, 76,
        207, 201, 202, 197, 198, 192, 221, 222, 216,
        212, 237, 238, 232, 228, 252, 141, 142, 136,
        132, 156, 172, 77, 78, 72, 68, 92, 108, 12
    };

    void createRules();
    config_t getNextState(const config_t&);
    string getIDfromVector(const vector<int>&);
    string getBitString(const config_t&);
    int checkReversibility();
    void logToCSV(const string&, const string&, const int&, const int&, const int&,
                  const float&, const int&, const int&, const float&, const int&);

public:
    void analyzeRandomGMACA(int id);
};

void GMACA::createRules() {
    ruleMaps.clear();
    for (const int& ruleN : ruleVector) {
        unordered_map<string, int> result;
        for (int i = 0; i < 8; i++) {
            string pattern = bitset<3>(i).to_string();
            int rule = (ruleN >> i) & 1;
            result[pattern] = rule;
        }
        ruleMaps.emplace_back(result);
    }
}

config_t GMACA::getNextState(const config_t& curr) {
    config_t next;
    for (int i = 0; i < cellCount; i++) {
        char left = (i == 0) ? '0' : (curr[i - 1] ? '1' : '0');
        char mid = curr[i] ? '1' : '0';
        char right = (i == cellCount - 1) ? '0' : (curr[i + 1] ? '1' : '0');
        string pattern = {left, mid, right};
        next[i] = ruleMaps[i].at(pattern);
    }
    return next;
}

string GMACA::getIDfromVector(const vector<int>& rv) {
    stringstream ss;
    for (int i = 0; i < rv.size(); i++) {
        ss << rv[i];
        if (i != rv.size() - 1) ss << '-';
    }
    return ss.str();
}

string GMACA::getBitString(const config_t& b) {
    return b.to_string().substr(MaxSize - cellCount);
}

int GMACA::checkReversibility() {
    int N = 1 << cellCount;
    unordered_map<string, vector<string>> nextToPred;

    for (int i = 0; i < N; ++i) {
        config_t curr(i);
        config_t next = getNextState(curr);
        string currStr = getBitString(curr);
        string nextStr = getBitString(next);
        nextToPred[nextStr].push_back(currStr);
    }

    for (const auto& [nextStr, preds] : nextToPred) {
        if (preds.size() > 1) return 1;
    }
    return 0;
}

void GMACA::logToCSV(const string& filename, const string& ruleVectorID,
                     const int& numAttractors, const int& numFixedPoints, const int& numCyclic,
                     const float& avgCycleLen, const int& maxCycleLen,
                     const int& maxChainLen, const float& pointAttractorDensity,
                     const int& reversibleFlag) {
    lock_guard<mutex> guard(file_mutex);
    ofstream file;
    bool fileExists = ifstream(filename).good();
    file.open(filename, ios::app);

    if (!fileExists) {
        file << "RuleVectorID,RuleVector,NumAttractors,NumFixedPointAttractors,NumCyclicAttractors,";
        file << "AvgCycleLength,MaxCycleLength,MaxChainLength,PointAttractorDensity,Reversible\n";
    }

    file << ruleVectorID << "," << getIDfromVector(ruleVector) << ","
         << numAttractors << "," << numFixedPoints << "," << numCyclic << ","
         << avgCycleLen << "," << maxCycleLen << ","
         << maxChainLen << "," << pointAttractorDensity << "," << reversibleFlag << "\n";

    file.close();
}

void GMACA::analyzeRandomGMACA(int id) {
    cellCount = 8;
    ruleVector.clear();

    static thread_local mt19937 gen(time(0) + id);
    uniform_int_distribution<> dist(0, RULE_POOL.size() - 1);
    for (int i = 0; i < cellCount; i++) {
        int rule = RULE_POOL[dist(gen)];
        ruleVector.emplace_back(rule);
    }

    createRules();

    int iter = 1 << cellCount;
    vector<bool> visited(iter, false);
    unordered_set<int> attractorRoots;

    int totalCycleLen = 0;
    int maxCycleLen = 0, maxChainLen = 0, pointAttractors = 0;

    for (int i = 0; i < iter; i++) {
        if (visited[i]) continue;

        unordered_map<int, int> stateStep;
        vector<int> path;
        int current = i, step = 0, cycleStart = -1;

        while (true) {
            if (stateStep.count(current)) {
                cycleStart = stateStep[current];
                break;
            }

            stateStep[current] = step++;
            path.push_back(current);

            config_t curr(current);
            config_t next = getNextState(curr);
            current = next.to_ulong();
        }

        for (int s : path) visited[s] = true;

        if (cycleStart != -1) {
            int cycleLen = path.size() - cycleStart;
            int chainLen = cycleStart;
            int cycleStartState = path[cycleStart];

            bool isNewAttractor = attractorRoots.insert(cycleStartState).second;

            if (isNewAttractor) {
                totalCycleLen += cycleLen;
                maxCycleLen = max(maxCycleLen, cycleLen);
                maxChainLen = max(maxChainLen, chainLen);
                if (cycleLen == 1) pointAttractors++;
            } else {
                // Already-known attractor, still count chain
                maxChainLen = max(maxChainLen, chainLen);
            }
        } else {
            // Should not occur in deterministic CA
            int chainLen = path.size();
            maxChainLen = max(maxChainLen, chainLen);
        }
    }

    int totalAttractors = attractorRoots.size();
    int cyclic = totalAttractors - pointAttractors;
    float avgCycle = totalAttractors ? (float)totalCycleLen / totalAttractors : 0.0f;
    float ptDensity = (float)(pointAttractors * 100) / iter;
    int reversibleFlag = checkReversibility();

    string ruleID = "RV_" + to_string(id);
    logToCSV("GMACA_analysis_1M_final.csv", ruleID, totalAttractors, pointAttractors, cyclic,
             avgCycle, maxCycleLen, maxChainLen, ptDensity, reversibleFlag);
}

void runBatch(int startID, int count) {
    for (int i = 0; i < count; i++) {
        GMACA g;
        g.analyzeRandomGMACA(startID + i);
    }
}

int main() {
    int total = 1000000;
    int threads = 10;
    int perThread = total / threads;

    vector<thread> pool;
    for (int i = 0; i < threads; i++) {
        pool.emplace_back(runBatch, i * perThread, perThread);
    }
    for (auto& t : pool) t.join();

    cout << "All 10,000 GMACA analyses completed using 8 threads.\n";
    return 0;
}
