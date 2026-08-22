#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include <cmath>
using namespace std;

// Convert an integer to its 8-bit binary string (MSB to LSB)
string toBinary(int ruleNum) {
    string bin = bitset<8>(ruleNum).to_string(); // Always 8 bits
    return bin; // Already MSB to LSB
}

// Generate the next configuration based on current config and rules
string getNextConfig(const string& current, const vector<int>& ruleVector, bool periodic) {
    int n = current.size();
    string next = "";

    for (int i = 0; i < n; ++i) {
        char left  = (i == 0)      ? (periodic ? current[n - 1] : '0') : current[i - 1];
        char mid   = current[i];
        char right = (i == n - 1)  ? (periodic ? current[0]     : '0') : current[i + 1];

        // Build the 3-bit pattern as an integer
        int pattern = ((left - '0') << 2) | ((mid - '0') << 1) | (right - '0');

        // Get rule binary for this cell
        string ruleBin = toBinary(ruleVector[i]);

        // Wolfram indexing: pattern 111 → index 0, 000 → index 7 ⇒ use 7 - pattern
        next += ruleBin[7-pattern];
    }

    return next;
}

// Convert integer to n-bit binary string
string intToBinary(int num, int n) {
    string bin = bitset<32>(num).to_string(); // Enough bits
    return bin.substr(32 - n, n);
}

// Generate and display all transitions
void generateDOT(const vector<int>& ruleVector, bool periodic) {
    int n = ruleVector.size();
    int totalConfigs = 1 << n;

    for (int i = 0; i < totalConfigs; ++i) {
        string config = intToBinary(i, n);
        string next = getNextConfig(config, ruleVector, periodic);
        cout << "\"" << config << "\" -> \"" << next << "\";" << endl;


    }
}

int main() {
    int n;
    cout << "Enter number of cells: ";
    cin >> n;

    vector<int> ruleVector(n);
    cout << "Enter " << n << " rule numbers (0 to 255):\n";
    for (int i = 0; i < n; ++i) {
        cin >> ruleVector[i];
    }

    int bc;
    cout << "Enter boundary condition (1 = periodic, 0 = null): ";
    cin >> bc;
    bool periodic = (bc == 1);

    cout << "\n--- State Transitions ---\n";
    generateDOT(ruleVector, periodic);

    return 0;
}
