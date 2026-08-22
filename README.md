# Cellular Automata — GMACA & MACA Analysis Toolkit

A C++ and Python toolkit for studying **Group Modified Asynchronous Cellular Automata (GMACA)** and **Maximum length Additive Cellular Automata (MACA)** — focusing on attractor analysis, reversibility, basin-of-attraction decomposition, and impurity effects.

## Repository Structure

```
├── src/                          # C++ source code
│   ├── eca.cpp                   # Basic ECA state-transition generator
│   ├── cycles_eca.cpp            # Cycle/attractor detection for uniform ECA
│   ├── reversibility.cpp         # Reversibility (bijectivity) check for ECA
│   ├── non_uniform_ca.cpp        # Non-uniform CA transition graph (DOT output)
│   ├── CSV_logger.cpp            # GMACA analyzer (single-threaded, CSV output)
│   ├── GMACA_Multithreaded_CSV_Logger.cpp  # Mass GMACA survey (multithreaded)
│   ├── MACA_with_impurity_analysis_Multithreaded.cpp  # Hybrid MACA impurity analysis
│   └── MACA_To_Basin.cpp         # Detailed basin-of-attraction decomposition
├── notebooks/                    # Jupyter notebooks for analysis and visualization
│   ├── maca_analysis.ipynb
│   ├── graph_maca_analysis.ipynb
│   ├── MACA_Basin_Analysis_Advanced.ipynb
│   ├── advanced_4_8_analysis.ipynb
│   ├── maca_pool5_8_analysis.ipynb
│   └── maca_pool6_8_analysis.ipynb
└── data/sample/                  # Small representative output samples
    ├── abc.csv
    └── MACA_4_8_1k_impurity_analysis.csv
```

## Tools Overview

### Elementary CA Tools
| Tool | Purpose |
|------|---------|
| eca.cpp | Prints the complete state-transition table for any ECA rule (0-255) |
| cycles_eca.cpp | Finds all attractors and their cycle lengths in the state-transition graph |
| reversibility.cpp | Checks if an ECA rule's global map is bijective (reversible) |
| non_uniform_ca.cpp | Generates state transitions for non-uniform CA (per-cell rules) in DOT format |

### GMACA Analysis Pipeline
| Tool | Purpose |
|------|---------|
| CSV_logger.cpp | Single-threaded GMACA analyzer with attractor metrics and CSV logging |
| GMACA_Multithreaded_CSV_Logger.cpp | Mass survey - analyzes 1M random rule vectors using 10 threads |
| MACA_with_impurity_analysis_Multithreaded.cpp | Studies hybrid CAs mixing pure cells (rule 204) with impurity rules |
| MACA_To_Basin.cpp | Deep basin-of-attraction analysis on filtered candidate rule vectors |

## Compilation

Requires a C++17 compatible compiler. For multithreaded tools, link with pthread:

```bash
# Basic tools
g++ -std=c++17 -O2 src/eca.cpp -o eca
g++ -std=c++17 -O2 src/cycles_eca.cpp -o cycles_eca
g++ -std=c++17 -O2 src/reversibility.cpp -o reversibility
g++ -std=c++17 -O2 src/non_uniform_ca.cpp -o non_uniform_ca

# Multithreaded tools
g++ -std=c++17 -O2 -pthread src/GMACA_Multithreaded_CSV_Logger.cpp -o gmaca_analyzer
g++ -std=c++17 -O2 -pthread src/MACA_with_impurity_analysis_Multithreaded.cpp -o maca_impurity
g++ -std=c++17 -O2 src/MACA_To_Basin.cpp -o maca_basin
```

## Usage

```bash
# Generate state transitions for ECA Rule 90, 4 cells, periodic boundary
./eca
# Enter: 90, 4, 1

# Find all cycles for ECA Rule 150
./cycles_eca
# Enter: 150, 5, 1

# Check reversibility
./reversibility
# Enter: 90, 4, 1

# Run mass GMACA analysis (outputs to CSV)
./gmaca_analyzer
```

## Rule Pool

The GMACA tools sample from a curated pool of 37 rules derived from group-theoretic classification of ECA rules. These rules span multiple algebraic groups and are selected for their interesting dynamical properties.

## Notebooks

The Jupyter notebooks analyze the CSV output from the C++ tools - visualizing attractor landscapes, basin structures, impurity effects, and identifying MACA candidates. Requires Python with pandas, matplotlib, seaborn, and numpy.
