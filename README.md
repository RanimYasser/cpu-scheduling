# -CPU-Scheduling-Policies

This project is a C/C++ implementation designed to analyze and visualize various CPU scheduling algorithms. It provides both statistical data and trace visualizations to understand how different scheduling strategies manage processes.

## Description

The simulator supports the following CPU scheduling algorithms:
- **FCFS** (First Come First Serve)
- **RR** (Round Robin)
- **SPN** (Shortest Process Next)
- **SRT** (Shortest Remaining Time)
- **HRRN** (Highest Response Ratio Next)
- **FB-1** (Feedback with quantum q=1 for all queues)
- **FB-2i** (Feedback with quantum q=2^i for queues)
- **Aging** (Priority increases as processes wait)

### Input Format

1. **First line:** Mode of operation (`trace` or `stats`).
2. **Second line:** List of scheduling policies to analyze, with parameters if necessary (e.g., `2-4` for RR with quantum `4`).
3. **Third line:** Simulation end time.
4. **Fourth line:** Number of processes.
5. **Subsequent lines:** Process details per selected algorithm.

### Output Format

- **Trace:** Visual representation of process scheduling.
- **Stats:** Statistical analysis of the scheduling performance.

## Features

- **Visualization and Statistics:** Offers both graphical simulation (`trace`) and performance metrics (`stats`).
- **Multiple Algorithms:** Can simulate various complex scheduling algorithms.
- **Input Redirection:** Facilitates easy testing and automation with input redirection.
