#include<iostream>
#include<bitset>
#include<vector>
#include<unordered_map>
#include<unordered_set>
#include<string>
#include<fstream>
#include<algorithm>
#include<limits>
#include<sstream>

#define MaxSize 10

using namespace std;
using config_t = bitset<MaxSize>;

class GMACA
{
    private:
        int cellCount;
        bool boundaryCondition;
        vector<int> ruleVector;
        vector<unordered_map<string, int>> ruleMaps;
        const vector<int> RULE_POOL = {
                                        204, 205, 206, 200, 196, 220, 236, 140,  76,
                                        207, 201, 202, 197, 198, 192, 221, 222, 216,
                                        212, 237, 238, 232, 228, 252, 141, 142, 136,
                                        132, 156, 172,  77,  78,  72,  68,  92, 108, 12};

        void createRules();
        config_t getNextState(const config_t&);
        string getIDfromVector(const vector<int>&);
        string getBitString(const config_t&);
        vector<int> generateRuleVector(const int& cellCount = 8);
        int checkReversibility();
        void logToCSV(const string&, const string&,
                    const int&, const int&, const int&,
                    const float&, const int&, const int&,
                    const float&, const int& );

        public:
            void takeInput(); // For manual testing
            void analyzeGMACA();
};


void GMACA::createRules()
{
    for(const int& ruleN : ruleVector)
    {
        unordered_map<string, int> result;
        for(int i = 0; i < 8; i++)
        {
            string pattern = bitset<3>(i).to_string();
            int rule = (ruleN >> i) & 1;
            result[pattern] = rule;
        }
        ruleMaps.emplace_back(result);
    }
}

config_t GMACA::getNextState(const config_t& curr)
{
    config_t next;
    for(int i = 0; i < cellCount; i++)
    {
        char left = (i == 0) ? '0' : (curr[i - 1] ? '1' : '0');
        char mid = curr[i] ? '1' : '0';
        char right = (i == cellCount - 1) ? '0' : (curr[i + 1] ? '1' : '0');
        string pattern = {left, mid, right};
        next[i] = ruleMaps[i].at(pattern);
    }
    return next;
}

string GMACA::getIDfromVector(const vector<int>& rv)        //{30,90,150} => "30-90-150" rule-vector
{
    stringstream ss;
    for(int i = 0; i < rv.size(); i++)
    {
        ss << rv[i];
        if(i != rv.size() - 1) ss << '-';
    }
    return ss.str();
}

string GMACA::getBitString(const config_t& b)          //bitse<maxsize>b is converted into binary string without 0 padding
{
    return b.to_string().substr(MaxSize - cellCount);
}

vector<int> GMACA::generateRuleVector(const int& cellCount)     //choose a random rule vector from RULE_POOL
{
    vector<int> rv;
    for(int i = 0; i < cellCount; i++)
    {
        int rule = RULE_POOL[rand() % RULE_POOL.size()];
        rv.emplace_back(rule);
    }
    return rv;
}

int GMACA::checkReversibility()
{
    int N = 1 << cellCount;
    unordered_map<string, vector<string>> nextToPred;

    for (int i = 0; i < N; ++i) {
        config_t curr(i);
        config_t next = getNextState(curr);
        string currStr = curr.to_string().substr(MaxSize - cellCount);
        string nextStr = next.to_string().substr(MaxSize - cellCount);
        nextToPred[nextStr].push_back(currStr);
    }

    for (const auto& [nextStr, preds] : nextToPred) {
        if (preds.size() > 1) return 1; // 1 for Irreversible
    }
    return 0; // 0 for Reversible
}

void GMACA::logToCSV(const string& filename, const string& ruleVectorID, const int& numAttractors, const int& numFixedPoints, const int& numCyclic, const float& avgCycleLen, const int& maxCycleLen, const int& maxChainLen, const float& pointAttractorDensity, const int& reversibleFlag)
{
    ofstream file;
    bool fileExists = ifstream(filename).good();
    file.open(filename, ios::app);

    if(!fileExists)
    {
        file << "RuleVectorID,RuleVector,NumAttractors,NumFixedPointAttractors,NumCyclicAttractors,AvgCycleLength,MaxCycleLength,MaxChainLength,PointAttractorDensity,Reversible\n";
    }

    file << ruleVectorID << "," << getIDfromVector(ruleVector) << ",";
    file << numAttractors << "," << numFixedPoints << "," << numCyclic << ",";
    file << avgCycleLen << "," << maxCycleLen << ",";
    file << maxChainLen << "," << pointAttractorDensity << "," << reversibleFlag << "\n";

    file.close();
}

void GMACA::takeInput() // It will help in manual testing
{
    while(true)
    {
        cout << "Enter the Number of Cells (1-" << MaxSize << "): ";
        cin >> cellCount;
        if(cin.fail() || cellCount < 1 || cellCount > MaxSize)
        {
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid cell count! Try again.\n";
        }
        else break;
    }

    boundaryCondition = false; // Force NULL boundary condition
    ruleVector.clear();
    for(int i = 0; i < cellCount; i++)
    {
        while(true)
        {
            int ruleN;
            cout << "Enter Rule for cell-" << i + 1 << ": ";
            cin >> ruleN;
            if(cin.fail() || ruleN < 0 || ruleN > 255)
            {
                cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid rule! Try again.\n";
            }
            else
            {
                ruleVector.emplace_back(ruleN);
                break;
            }
        }
    }

    createRules();
}

// void GMACA::analyzeGMACA()
// {
//     int iter = 1 << cellCount;
//     vector<bool> visited(iter, false);
//     unordered_map<string, int> cycleLengths;
//     int totalChains = 0, totalCycleLen = 0, maxCycleLen = 0, maxChainLen = 0, pointAttractors = 0;

//     for(int i = 0; i < iter; i++)
//     {
//         if(visited[i]) continue;

//         unordered_map<int, int> stateStep;
//         vector<int> path;
//         int current = i, step = 0, cycleStart = -1;
//         bool hitKnownCycle = false;
        
//         while(true)
//         {
//             if(stateStep.count(current))
//             {
//                 cycleStart = stateStep[current];
//                 break;
//             }
//             // if(visited[current]) break; // I guess redundant checkig, Testcase may confirm

//             stateStep[current] = step++;
//             path.push_back(current);

//             config_t curr(current);
//             config_t next = getNextState(curr);
//             current = next.to_ulong();

//             if(visited[current])
//             {
//                 hitKnownCycle = true;
//                 path.push_back(current);
//                 break;
//             }
//         }

//         for(int s : path) visited[s] = true;

//         if(cycleStart != -1) // Fresh Cycle
//         {
//             int cycleLen = path.size() - cycleStart;
//             int chainLen = cycleStart;
//             string attractorID = to_string(path[path.size() - 1]);
//             cycleLengths[attractorID] = cycleLen;

//             totalCycleLen += cycleLen;
//             totalChains += chainLen;
//             if(cycleLen > maxCycleLen) maxCycleLen = cycleLen;
//             if(chainLen > maxChainLen) maxChainLen = chainLen;

//             if(cycleLen == 1) pointAttractors++;
//         }
//         else if(hitKnownCycle) // Leads into a known cycle, not a new attractor
//         {
//             int chainLen = path.size() - 1;
//             totalChains += chainLen;
//             maxChainLen = max(maxChainLen, chainLen);
//         }
//     }

//     int totalAttractors = cycleLengths.size();
//     int cyclic = totalAttractors - pointAttractors;
//     float avgCycle = totalAttractors ? (float)totalCycleLen / totalAttractors : 0.0f;
//     float avgChain = totalAttractors ? (float)totalChains / totalAttractors : 0.0f;
//     float ptDensity = (float)(pointAttractors * 100) / iter;
//     int reversibleFlag = checkReversibility();

//     string ruleID = "RV_" + getIDfromVector(ruleVector);
//     logToCSV("gmaca_analysis.csv", ruleID, totalAttractors, pointAttractors, cyclic, avgCycle, maxCycleLen, avgChain, maxChainLen, ptDensity, reversibleFlag);

//     cout << "Logged analysis for Rule Vector: " << ruleID << endl;
// }

void GMACA::analyzeGMACA()
{
    int iter = 1 << cellCount;
    vector<bool> visited(iter, false);
    unordered_set<int> attractorRoots;
    int totalCycleLen = 0, maxCycleLen = 0, maxChainLen = 0, pointAttractors = 0;

    for (int i = 0; i < iter; i++)
    {
        if (visited[i]) continue;

        unordered_map<int, int> stateStep;
        vector<int> path;
        int current = i, step = 0, cycleStart = -1;

        while(true)
        {
            if(stateStep.count(current))
            {
                cycleStart = stateStep[current];
                break;
            }

            stateStep[current] = step++;
            path.push_back(current);

            config_t curr(current);
            config_t next = getNextState(curr);
            current = next.to_ulong();
        }

        // Mark all traversed states as visited
        for(int s : path) visited[s] = true;

        if(cycleStart != -1)
        {
            int cycleLen = path.size() - cycleStart;
            int chainLen = cycleStart;
            int root = path[cycleStart];  // First state in the cycle

            bool isNewAttractor = attractorRoots.insert(root).second;

            if(isNewAttractor)
            {
                totalCycleLen += cycleLen;
                maxCycleLen = max(maxCycleLen, cycleLen);
                maxChainLen = max(maxChainLen, chainLen);
                if (cycleLen == 1) pointAttractors++;
            }
            else
            {
                // Already known attractor, count only chain
                maxChainLen = max(maxChainLen, chainLen);
            }
        }
        else
        {
            // Shouldn't occur in deterministic CA, but safe fallback
            int chainLen = path.size();
            maxChainLen = max(maxChainLen, chainLen);
        }
    }

    int totalAttractors = attractorRoots.size();
    int cyclic = totalAttractors - pointAttractors;
    float avgCycle = totalAttractors ? (float)totalCycleLen / totalAttractors : 0.0f;
    float ptDensity = (float)(pointAttractors * 100) / iter;
    int reversibleFlag = checkReversibility();

    string ruleID = "RV_" + getIDfromVector(ruleVector);
    logToCSV("abc.csv", ruleID, totalAttractors, pointAttractors, cyclic,
             avgCycle, maxCycleLen, maxChainLen, ptDensity, reversibleFlag);

    cout << "Logged analysis for Rule Vector: " << ruleID << endl;
}


int main()
{
    GMACA automaton;
    automaton.takeInput();
    automaton.analyzeGMACA();
    return 0;
}