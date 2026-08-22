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

#define MaxSize 10

using namespace std;
using config_t = bitset<MaxSize>;

vector<string> ruleVectorIDs;

class GMACA {
private:
    int cellCount;
    bool boundaryCondition;
    vector<int> ruleVector;
    vector<unordered_map<string, int>> ruleMaps;

    void createRules();
    config_t getNextState(const config_t&);
    string getIDfromVector(const vector<int>&);
    string getBitString(const config_t&);
    vector<int> parseRuleVectorFromID(const string&);
    int checkReversibility();
    int getReversedDecimal(int state);
    void logToCSV(const string&, const string&, const int&, const int&, const int&,
                  const float&, const int&, const int&, const float&, const int&,
                  const vector<string>& = vector<string>());

public:
    void analyzeAdvancedGMACA();
    void analyzeGMACAfromCSV(int index);
};

void loadRuleVectorsFromCSV(const string& filename) {
    ifstream file(filename);
    string line;
    getline(file, line); // Skip header
    
    while (getline(file, line)) {
        stringstream ss(line);
        string ruleVectorID;
        getline(ss, ruleVectorID, ',');
        ruleVectorIDs.push_back(ruleVectorID);
    }
    file.close();
}

vector<int> GMACA::parseRuleVectorFromID(const string& ruleVectorID) {
    // Parse "RV_78-141-77-236-238-204-78-141" format
    size_t start = ruleVectorID.find("RV_") + 3;
    string rulesStr = ruleVectorID.substr(start);
    
    vector<int> rules;
    stringstream ss(rulesStr);
    string rule;
    
    while (getline(ss, rule, '-')) {
        rules.push_back(stoi(rule));
    }
    
    return rules;
}

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
        if (i != rv.size() - 1)
            ss << '-';
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

    for (const auto& [_, preds] : nextToPred) {
        if (preds.size() > 1) return 1;
    }
    return 0;
}

int GMACA::getReversedDecimal(int state) {
    // Convert state to 8-bit bitstring
    bitset<8> bits(state);
    string bitStr = bits.to_string();
    
    // Reverse the bitstring
    reverse(bitStr.begin(), bitStr.end());
    
    // Convert reversed bitstring back to decimal
    return bitset<8>(bitStr).to_ulong();
}

void GMACA::logToCSV(const string& filename, const string& ruleVectorID,
                     const int& numAttractors, const int& numFixedPoints, const int& numCyclic,
                     const float& avgCycleLen, const int& maxCycleLen, const int& maxChainLen,
                     const float& pointAttractorDensity, const int& reversibleFlag,
                     const vector<string>& basinData) {
    ofstream file;
    bool fileExists = ifstream(filename).good();
    file.open(filename, ios::app);

    if (!fileExists) {
        file << "RuleVectorID,NumAttractors,NumFixedPointAttractors,NumCyclicAttractors,AvgCycleLength,MaxCycleLength,MaxChainLength,PointAttractorDensity,Reversible";
        for (int i = 1; i <= 100; i++) file << ",Basin" << i;
        file << "\n";
    }

    file << ruleVectorID << "," << numAttractors << "," << numFixedPoints << "," << numCyclic << ","
         << avgCycleLen << "," << maxCycleLen << "," << maxChainLen << "," << pointAttractorDensity
         << "," << reversibleFlag;

    if (basinData.size() <= 100) {
        int writeSize = basinData.size();
        for (int i = 0; i < writeSize; i++) file << "," << basinData[i];
        for (int i = writeSize; i < 100; i++) file << ",-1";
    }
    else {
        for (int i = 0; i < 99; i++) file << "," << basinData[i];
        file << ",-5";
    }
    
    file << "\n";
    file.close();
}

void GMACA::analyzeAdvancedGMACA() {
    int iter = 1 << cellCount;
    vector<bool> visited(iter, false);
    unordered_set<int> attractorRoots;
    unordered_map<int, int> stateToAttractor;
    unordered_map<int, vector<int>> attractorToBasinStates;

    int totalCycleLen = 0, maxCycleLen = 0, maxChainLen = 0, pointAttractors = 0;

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
            current = getNextState(config_t(current)).to_ulong();
        }

        if (cycleStart != -1) {
            int cycleLen = path.size() - cycleStart;
            int chainLen = cycleStart;
            int root = path[cycleStart];
            for (int s : path) {
                stateToAttractor[s] = root;
                visited[s] = true;
            }

            bool isNew = attractorRoots.insert(root).second;
            if (isNew) {
                totalCycleLen += cycleLen;
                maxCycleLen = max(maxCycleLen, cycleLen);
                maxChainLen = max(maxChainLen, chainLen);
                if (cycleLen == 1) pointAttractors++;
            } else {
                maxChainLen = max(maxChainLen, chainLen);
            }
        }
    }

    // Collect attractor states
    unordered_set<int> attractorStates;
    unordered_map<int, vector<int>> attractorCycles;
    for (int root : attractorRoots) {
        int current = root;
        vector<int> cycle;
        do {
            attractorStates.insert(current);
            cycle.push_back(current);
            current = getNextState(config_t(current)).to_ulong();
        } while (current != root);
        attractorCycles[root] = cycle;
    }

    // Collect basin states (non-attractor states that converge to each attractor)
    for (int i = 0; i < iter; i++) {
        if (stateToAttractor.count(i) && !attractorStates.count(i)) {
            attractorToBasinStates[stateToAttractor[i]].push_back(i);
        }
    }

    // Create basin data with proper sorting
    vector<pair<int, pair<int, vector<int>>>> basinInfo; // (basin_size, (attractor_root, basin_states))
    for (int root : attractorRoots) {
        vector<int>& basinStates = attractorToBasinStates[root];
        basinInfo.push_back({static_cast<int>(basinStates.size()), {root, basinStates}});
    }

    // Sort by basin size (descending), then by attractor root (ascending for lower decimal equivalent)
    sort(basinInfo.begin(), basinInfo.end(), [](const auto& a, const auto& b) {
        if (a.first != b.first) return a.first > b.first;
        return a.second.first < b.second.first; // Changed from > to < for lower decimal first
    });

    // Format basin data as strings
    vector<string> basinData;
    for (const auto& [basinSize, basinInfo_] : basinInfo) {
        const auto& [attractorRoot, basinStates] = basinInfo_;
        
        // Create basin string: "state1-state2-state3|attractor"
        stringstream basinStr;
        if (!basinStates.empty()) {
            sort(const_cast<vector<int>&>(basinStates).begin(), const_cast<vector<int>&>(basinStates).end());
            for (int i = 0; i < basinStates.size(); i++) {
                if (i > 0) basinStr << "-";
                basinStr << getReversedDecimal(basinStates[i]);
            }
        }
        basinStr << "|";
        
        // Add attractor cycle states
        vector<int>& attractorCycle = attractorCycles[attractorRoot];
        sort(attractorCycle.begin(), attractorCycle.end());
        for (int i = 0; i < attractorCycle.size(); i++) {
            if (i > 0) basinStr << "-";
            basinStr << getReversedDecimal(attractorCycle[i]);
        }
        
        basinData.push_back(basinStr.str());
    }

    int totalAttractors = attractorRoots.size();
    int cyclic = totalAttractors - pointAttractors;
    float avgCycle = totalAttractors ? (float)totalCycleLen / totalAttractors : 0.0f;
    float ptDensity = (float)(pointAttractors * 100) / iter;
    int reversibleFlag = checkReversibility();

    string ruleID = "RV_" + getIDfromVector(ruleVector);
    logToCSV("Basin60M.csv", ruleID, totalAttractors, pointAttractors, cyclic,
             avgCycle, maxCycleLen, maxChainLen, ptDensity, reversibleFlag, basinData);
}

void GMACA::analyzeGMACAfromCSV(int index) {
    if (index >= ruleVectorIDs.size()) return;
    
    cellCount = 8;
    boundaryCondition = false;
    ruleVector = parseRuleVectorFromID(ruleVectorIDs[index]);
    createRules();
    
    cout << "Processing rule vector " << (index + 1) << "/" << ruleVectorIDs.size() 
         << ": " << ruleVectorIDs[index] << endl;
    
    analyzeAdvancedGMACA();
}

int main() {
    // Load rule vectors from CSV file
    loadRuleVectorsFromCSV("filtered_gmaca_candidates_60M.csv");
    
    cout << "Loaded " << ruleVectorIDs.size() << " rule vectors from CSV.\n";
    
    // Process each rule vector sequentially
    for (int i = 0; i < ruleVectorIDs.size(); i++) {
        GMACA g;
        g.analyzeGMACAfromCSV(i);
    }

    cout << "All GMACA basin analyses completed.\n";
    return 0;
}
