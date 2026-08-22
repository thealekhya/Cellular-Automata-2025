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

    const vector<int> pool_8 = {204};
    const vector<int> pool_7 = {205, 206, 200, 196, 220, 236, 140, 76};
    const vector<int> pool_6 = {207, 201, 202, 197, 198, 192, 221, 222, 216, 212, 237, 238, 252, 141, 142, 136, 132, 156, 172, 77, 78, 72, 68, 92, 108, 12};
    const vector<int> pool_5 = {203, 194, 218, 208, 234, 224, 248, 137, 134, 158, 173, 164, 73, 70, 94, 109, 100, 14, 28, 199, 223, 213, 239, 229, 253, 244, 138, 128, 152, 174, 188, 74, 64, 
    88, 110, 124, 8, 44, 193, 217, 214, 233, 230, 230, 254, 143, 133, 157, 148, 168, 79, 69, 93, 84, 104, 13, 4};
    const vector<int> pool_4 = {195, 209, 231, 255, 245, 139, 130, 154, 144, 170, 160, 184, 71, 95, 85, 111, 101, 125, 116, 10, 0, 24, 46, 60, 219, 210, 225,
    249, 246, 135, 159, 149, 175, 165, 189, 180, 65, 89, 86, 105, 102, 126, 15, 5, 29, 20, 40, 215, 235, 226, 250, 240, 129, 153, 150, 169, 166, 190,
    75, 66, 90, 80, 106, 96, 120, 9, 6, 30, 45, 36};

    void createRules();
    config_t getNextState(const config_t&);
    string getIDfromVector(const vector<int>&);
    string getBitString(const config_t&);
    int checkReversibility();
    //bool isMACA();
    string formatbasins();
    void logToCSV(const string&, const string&, const int&, const int&, const int&,
                  const float&, const int&, const int&, const float&, const int&,
                  const int&, const float&, const string&, const bool&);

public:
    void analyzeHybridGMACA(int id);
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
        if (preds.size() > 1) return 0;
    }
    return 1;
}

/* bool GMACA::isMACA(){
    int n=1<<cellCount;
    vector<bool> visited(n, false);
    unordered_map<int,unordered_set<int>> basin;
    for(int i=0;i<n;i++){
        if(visited[i]) continue;
        unordered_map<int, int> statestep;
        vector<int> path;
        int current = i, step = 0, cycleStart = -1,cyclelength=0;
        while(true){
            if(statestep.count(current)){
                cycleStart=statestep[current];
                break;
            }
            statestep[current]=step++;
            path.push_back(current);

            config_t curr(current);
            config_t next = getNextState(curr);
            current = next.to_ulong();
        }
        for(int s: path) visited[s]=true;
        if(cycleStart!=-1){
            cyclelength=path.size()-cycleStart;
        }
        if (cyclelength>1) return false;
        if(cyclelength==1){
            int attractor=path.back();
            if(path.size()==1) return false;
            for(int k=0;k<path.size()-1;k++){
                basin[attractor].insert(path[k]);
            }
        }
    }
    for(const auto& [attractor, predecesors]: basin){
        if(predecesors.size()==0) return false; // If any attractor has no predecessors, it's not MACA
    }
    return true; // MACA
} */

string GMACA:: formatbasins() {
    vector<string> entries;
    int n = 1 << cellCount;
    vector<bool> visited(n, false);
    unordered_map<int, unordered_set<int>> basin;

    for (int i = 0; i < n; i++) {
        if (visited[i]) continue;

        unordered_map<int, int> stateStep;
        vector<int> path;
        int current = i, step = 0, cycleStart = -1, cycleLength = 0;

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

        cycleLength = (cycleStart != -1) ? (path.size() - cycleStart) : 0;

        if (cycleLength > 1) {
            // Found a non-point attractor: NOT MACA
            return "0";
        }

        if (cycleLength == 1) {
            // Point attractor
            int attractor = path.back();
            if (path.size() == 1) {
                // Self-loop with no predecessors: NOT MACA
                return "0";
            }
            for (int k = 0; k < path.size() - 1; k++) {
                basin[attractor].insert(path[k]);
            }
        }
    }

    // Format basins
    for (const auto& [attractor, preds] : basin) {
        string attractorStr = to_string(attractor);
        string predsStr;

        size_t count = 0;
        for (const auto& pred : preds) {
            predsStr += to_string(pred);
            count++;
            if (count != preds.size()) predsStr += "-";
        }
        entries.push_back(attractorStr + ":" + predsStr);
    }

    string finalStr;
    for (size_t i = 0; i < entries.size(); ++i) {
        finalStr += entries[i];
        if (i != entries.size() - 1) finalStr += ";";
    }

    return finalStr.empty() ? "0" : finalStr;
}


void GMACA::logToCSV(const string& filename, const string& ruleVectorID,
                     const int& numAttractors, const int& numFixedPoints, const int& numCyclic,
                     const float& avgCycleLen, const int& maxCycleLen,
                     const int& maxChainLen, const float& pointAttractorDensity,
                     const int& reversibleFlag, const int& impurityCells, const float& impurityPercent, const string& basins, const bool& MACA_or_not) {

    lock_guard<mutex> guard(file_mutex);
    ofstream file;
    bool fileExists = ifstream(filename).good();
    file.open(filename, ios::app);

    if (!fileExists) {
        file << "RuleVectorID,RuleVector,NumAttractors,NumFixedPointAttractors,NumCyclicAttractors,";
        file << "AvgCycleLength,MaxCycleLength,MaxChainLength,PointAttractorDensity,Reversible,";
        file << "ImpurityCells,ImpurityPercent,basins,MACA_or_not\n";
    }

    file << ruleVectorID << "," << getIDfromVector(ruleVector) << ","
         << numAttractors << "," << numFixedPoints << "," << numCyclic << ","
         << avgCycleLen << "," << maxCycleLen << ","
         << maxChainLen << "," << pointAttractorDensity << "," << reversibleFlag << ","
         << impurityCells << "," << impurityPercent <<","<<basins<< ","<<MACA_or_not<<"\n";

    file.close();
}

void GMACA::analyzeHybridGMACA(int id) {
    cellCount = 8;
    ruleVector.clear();

    static thread_local mt19937 gen(time(0) + id);
    uniform_int_distribution<> countDist(2, 6); // Choose between 2 to 6 pure rules
    int pureCount = countDist(gen);

    vector<int> cellIndexes(cellCount);
    iota(cellIndexes.begin(), cellIndexes.end(), 0);
    shuffle(cellIndexes.begin(), cellIndexes.end(), gen);

    // Fill ruleVector with 204 in random positions
    ruleVector.resize(cellCount);
    for (int i = 0; i < pureCount; i++) {
        ruleVector[cellIndexes[i]] = 204;
    }

    // Fill remaining positions with other rules
    //vector<const vector<int>*> impurityPools = {&pool_4, &pool_5, &pool_6, &pool_7};
    for (int i = pureCount; i < cellCount; i++) {
        //const auto& chosenPool = pool_4[gen() % pool_4.size()];
        int rule = pool_4[gen() % pool_4.size()];
        ruleVector[cellIndexes[i]] = rule;
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
                maxChainLen = max(maxChainLen, chainLen);
            }
        } else {
            int chainLen = path.size();
            maxChainLen = max(maxChainLen, chainLen);
        }
    }

    int totalAttractors = attractorRoots.size();
    int cyclic = totalAttractors - pointAttractors;
    float avgCycle = totalAttractors ? (float)totalCycleLen / totalAttractors : 0.0f;
    float ptDensity = (float)(pointAttractors * 100) / iter;
    int reversibleFlag = checkReversibility();
    int impurityCells = cellCount - pureCount;
    float impurityPercent = (impurityCells * 100.0f) / cellCount;
    string basins=formatbasins();
    bool MACA_or_not = (basins != "0");
    

    string ruleID = "HybridRV_" + to_string(id);
    logToCSV("advanced_pool4_10M.csv", ruleID, totalAttractors, pointAttractors, cyclic,
             avgCycle, maxCycleLen, maxChainLen, ptDensity, reversibleFlag,
             impurityCells, impurityPercent, basins, MACA_or_not);
}

void runBatch(int startID, int count) {
    for (int i = 0; i < count; i++) {
        GMACA g;
        g.analyzeHybridGMACA(startID + i);
    }
}

int main() {
    int total = 500000;
    int threads = 10;
    int perThread = total / threads;

    vector<thread> pool;
    for (int i = 0; i < threads; i++) {
        pool.emplace_back(runBatch, i * perThread, perThread);
    }
    for (auto& t : pool) t.join();

    cout << "All 0.5M Hybrid GMACA analyses completed." << endl;
    return 0;
}
