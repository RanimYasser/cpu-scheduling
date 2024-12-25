#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include <sstream>
#include <iomanip>
#include <queue>
#include <cmath>

using namespace std;
int total_service_time=0;
struct Process
{
    char process_name; // Character representing the process name
    int arrivalTime;   // The time the process arrived
    int timeRemaining; // Time left to finish the process
    int serviceTime;
    int finishTime;
    int turnAround;    // TA=Wait + service time
    int start_running; // first time process runs
    float normTurn;
    int priority;
    int waiting_time;
    int isCompleted = 0;
    vector<int> running; // keeps track of a process's timeline
};

struct CompareServiceTime {
    bool operator()(Process *p1, Process *p2) {
        return p1->serviceTime > p2->serviceTime;
    }
};

class Scheduler
{
private:
    int quantum;
    std::string algorithm_name;
    std::vector<Process> processes; // Using vector as the number of processes is not fixed
    enum Mode
    {
        TRACE,
        STATS
    } mode;

public:
    Scheduler() : quantum(0), mode(STATS) {}

    void add_process(Process process)
    {
        processes.push_back(process);
    }

    void set_mode(const std::string &m)
    {
        mode = (m == "trace") ? TRACE : STATS;
    }
    void set_quantum(int quantum_value){
quantum=quantum_value;

    }
    void FCFS();
    void RR();
    void SPN();
    void SRT(std::vector<Process> &processes);
    void HRRN(std::vector<Process> &processes);
    void FB_ONE(std::vector<Process> &processes);
    void FB_TWOi();
    void Aging();
    void set_mode(Mode m);
    void execute_algorithm(int algorithm_number);
    void read_input();
    void trace_mode(std::string algorithm_name, int timeLine);
    void stats_mode(std::string algorithm_name);

    static bool compare_Processes_wrt_ArrivalTime(const Process &a, const Process &b)
    {
        return a.arrivalTime < b.arrivalTime;
    }
    void sort_processes_by_arrival_time()
    {
        std::sort(processes.begin(), processes.end(), compare_Processes_wrt_ArrivalTime);
    }

    double responseRatio(const Process &process, int currentTime)
    {
        int waitingTime = currentTime - process.arrivalTime;
        return (double)(waitingTime + process.serviceTime) / process.serviceTime;
    }
};

 void Scheduler::FCFS()
{
    int current_time=0;
    sort_processes_by_arrival_time(); //sort processes with arrival time to handle any  input file

    for (auto &process:processes)
    {
    if (current_time<process.arrivalTime){
        current_time=process.arrivalTime;
    }

    process.waiting_time=current_time-process.arrivalTime;
    process.turnAround=process.waiting_time+process.serviceTime;
    process.start_running=current_time;
    current_time+=process.serviceTime;
    process.finishTime=current_time;
    process.normTurn = (double)process.turnAround / process.serviceTime;
    for (int i=process.start_running;i<current_time;i++)
    {
        process.running.push_back(i);
    }
    }
}


void Scheduler::RR() {
    sort_processes_by_arrival_time();
    std::queue<Process *> ready_queue;
    int current_time = 0;
    bool all_completed = false;

    while (!all_completed) {
        for (auto &process : processes) {
            if (process.arrivalTime <= current_time && process.isCompleted == 0) {
                ready_queue.push(&process);
                process.isCompleted = -1;
                std::cout << "Time " << current_time << ": Process " << process.process_name
                          << " added to the ready queue.\n";
            }
        }

        if (ready_queue.empty()) {
            std::cout << "Time " << current_time << ": CPU idle.\n";
            current_time++;
            continue;
        }

        Process *current_process = ready_queue.front();
        ready_queue.pop();

        int execution_time = std::min(quantum, current_process->timeRemaining);


        std::cout << "Time " << current_time << ": Executing Process "
                  << current_process->process_name << " for "
                  << execution_time << " units.\n";

        for (int i = 0; i < execution_time; ++i) {
            current_process->running.push_back(current_time++);
        }

        current_process->timeRemaining -= execution_time;

        for (auto &process : processes) {
            if (process.arrivalTime <= current_time && process.isCompleted == 0) {
                ready_queue.push(&process);
                process.isCompleted = -1; =
                std::cout << "Time " << current_time << ": Process " << process.process_name
                          << " added to the ready queue.\n";
            }
        }

        if (current_process->timeRemaining == 0) {
            current_process->finishTime = current_time;
            current_process->turnAround = current_process->finishTime - current_process->arrivalTime;
            current_process->waiting_time = current_process->turnAround - current_process->serviceTime;
            current_process->normTurn = (double)current_process->turnAround / current_process->serviceTime;
            current_process->isCompleted = 1;
            std::cout << "Time " << current_time << ": Process " << current_process->process_name
                      << " completes.\n";
        } else {

            ready_queue.push(current_process);
            std::cout << "Time " << current_time << ": Process " << current_process->process_name
                      << " returned to the ready queue.\n";
        }


        all_completed = true;
        for (const auto &process : processes) {
            if (process.isCompleted != 1) {
                all_completed = false;
                break;
            }
        }
    }
}
void Scheduler::SPN(){
sort_processes_by_arrival_time();
    std::priority_queue<Process *, std::vector<Process *>, CompareServiceTime> ready_queue;
    int current_time = 0;

    while (true)
    {
        for (auto &process : processes)
        {
            if (process.arrivalTime <= current_time && process.isCompleted == 0)
            {
                ready_queue.push(&process);
                process.isCompleted = -1;
            }
        }

        if (!ready_queue.empty())
        {
            Process *current_process = ready_queue.top();
            ready_queue.pop();
            current_process->start_running = current_time;
            current_process->waiting_time = current_time - current_process->arrivalTime;
            current_time += current_process->serviceTime;
            current_process->finishTime = current_time;
            current_process->turnAround = current_process->finishTime - current_process->arrivalTime;
            current_process->normTurn = (double)current_process->turnAround / current_process->serviceTime;
            current_process->isCompleted = 1;
            for (int i = current_process->start_running; i < current_time; ++i)
            {
                current_process->running.push_back(i);
            }
        }
        else
        {
            bool processes_completed = true;
            for (const auto &process : processes)
            {
                if (process.isCompleted == 0)
                {
                     processes_completed= false;
                    break;
                }
            }
            if (processes_completed)
                break;

            current_time++;
        }
    }
}

void Scheduler::SRT(std::vector<Process> &processes)
{
    sort_processes_by_arrival_time();
    int currentTime = 0;
    int n = processes.size();
    int j = n;
    int selectedProcess = -1;
    int remaining = INT_MAX;
    while (j > 0)
    {
        remaining = INT_MAX;
        selectedProcess = -1;
        for (int i = 0; i < n; i++)
        {

            if (processes[i].arrivalTime <= currentTime && processes[i].isCompleted == 0)
            {

                if (processes[i].timeRemaining < remaining)
                {
                    remaining = processes[i].timeRemaining;
                    selectedProcess = i;
                }
            }
        }

        if (selectedProcess == -1)
        {
            currentTime++;
        }
        else
        {

            processes[selectedProcess].running.push_back(currentTime);
            currentTime++;
            processes[selectedProcess].timeRemaining--;
            if (processes[selectedProcess].timeRemaining == 0)
            {
                processes[selectedProcess].finishTime = currentTime;
                processes[selectedProcess].isCompleted = 1;
                processes[selectedProcess].turnAround = processes[selectedProcess].finishTime - processes[selectedProcess].arrivalTime;
                processes[selectedProcess].normTurn = (double)processes[selectedProcess].turnAround / processes[selectedProcess].serviceTime;
                j--;
            }
        }
    }
}

void Scheduler::HRRN(std::vector<Process> &processes)
{ // depends on response ratio R=(Waiting time + service time)/service time , non-preemptive
    sort_processes_by_arrival_time();
    int currentTime = 0;
    double hrr = -9999; // lower limit for response time
    int n = processes.size();
    int j = n;
    int selectedProcess = -1;

    while (j > 0)
    {
        selectedProcess = -1;
        hrr = -9999;
        for (int i = 0; i < n; i++)
        { // gets process with highest RR

            if (processes[i].arrivalTime <= currentTime && processes[i].isCompleted == 0)
            {
                double temp_hrr = responseRatio(processes[i], currentTime);
                if (temp_hrr >= hrr)
                {
                    hrr = temp_hrr;
                    selectedProcess = i;
                }
            }
        }
        if (selectedProcess == -1)
        {
            currentTime++;
        }
        else
        {
            processes[selectedProcess].start_running = currentTime;
            currentTime += processes[selectedProcess].serviceTime;
            for (int i = processes[selectedProcess].start_running; i < currentTime; i++)
            {
                processes[selectedProcess].running.push_back(i);
            }
            processes[selectedProcess].finishTime = currentTime;
            processes[selectedProcess].isCompleted = 1;
            processes[selectedProcess].turnAround = processes[selectedProcess].finishTime - processes[selectedProcess].arrivalTime;
            processes[selectedProcess].normTurn = (double)processes[selectedProcess].turnAround / processes[selectedProcess].serviceTime;
            j--;
        }
    }
}

void Scheduler::FB_ONE(std::vector<Process> &processes)
{
    sort_processes_by_arrival_time();
    int currentTime = 0;
    int n = processes.size();
    int remainingProcesses = n;
    int numQueues = 5;
    int execute = -1;
    int numReady = 0;
    std::queue<Process *> queues[numQueues];

    while (remainingProcesses > 0)
    {
        // Add new processes to the highest priority queue
        for (Process &p : processes)
        {
            if (p.arrivalTime == currentTime && p.timeRemaining > 0)
            {
                queues[0].push(&p);
                numReady++;
            }
        }

        execute = -1;
        for (int i = 0; i < numQueues; i++)
        {
            if (!queues[i].empty())
            {
                execute = i;
                break;
            }
        }

        if (execute != -1)
        {
            Process *selectedProcess = queues[execute].front();
            queues[execute].pop();

            selectedProcess->running.push_back(currentTime);
            currentTime++;
            selectedProcess->timeRemaining--;

            if (selectedProcess->timeRemaining == 0)
            {
                selectedProcess->finishTime = currentTime;
                selectedProcess->turnAround = selectedProcess->finishTime - selectedProcess->arrivalTime;
                selectedProcess->normTurn = (double)selectedProcess->turnAround / selectedProcess->serviceTime;
                remainingProcesses--;
                numReady--;
            }

            else
            {
                // add process to the next lower priority queue
                if (execute != 0 && numReady == 1)
                {
                    queues[execute].push(selectedProcess);
                }
                else if (execute + 1 < numQueues)
                {
                    queues[execute + 1].push(selectedProcess);
                }
                else
                {
                    queues[execute].push(selectedProcess); // Stay in the lowest-priority queue
                }
            }
        }

        else
        {
            currentTime++; // No process to execute
        }
    }
}

void Scheduler::FB_TWOi()
{
    sort_processes_by_arrival_time();
    int currentTime = 0;
    int n = processes.size();
    int remainingProcesses = n;
    int numQueues = 4;
    int execute = -1;
    int numReady = 0;
    std::queue<Process *> queues[numQueues];

    while (remainingProcesses > 0)
    {
        // Add new processes to the highest priority queue
        for (Process &p : processes)
        {
            if (p.arrivalTime == currentTime && p.timeRemaining > 0)
            {
                queues[0].push(&p);
                numReady++;
            }
        }

        execute = -1;
        for (int i = 0; i < numQueues; i++)
        {
            if (!queues[i].empty())
            {
                execute = i;
                break;
            }
        }

        if (execute != -1)
        {
            Process *selectedProcess = queues[execute].front();
            queues[execute].pop();

            for (int i = execute; i < pow(2, execute); i++)
            {
                selectedProcess->running.push_back(currentTime);
                currentTime++;
                selectedProcess->timeRemaining--;
            }

            if (selectedProcess->timeRemaining == 0)
            {
                selectedProcess->finishTime = currentTime;
                selectedProcess->turnAround = selectedProcess->finishTime - selectedProcess->arrivalTime;
                selectedProcess->normTurn = (double)selectedProcess->turnAround / selectedProcess->serviceTime;
                remainingProcesses--;
                numReady--;
            }

            else
            {
                // add process to the next lower priority queue
                if (execute != 0 && numReady == 1)
                {
                    queues[execute].push(selectedProcess);
                }
                else if (execute + 1 < numQueues)
                {
                    queues[execute + 1].push(selectedProcess);
                }
                else
                {
                    queues[execute].push(selectedProcess); // Stay in the lowest-priority queue
                }
            }
        }

        else
        {
            currentTime++; // No process to execute
        }
    }
}

void Scheduler::Aging()
{
}

void Scheduler::read_input()
{
    try
    {
        // Line 1: Mode (trace or stats)
        std::string line;
        std::getline(std::cin, line);
        set_mode(line);

        std::cout << "Mode set to: " << (mode == TRACE ? "TRACE" : "STATS") << std::endl;

        // Line 2: Algorithm number and optional quantum
        std::getline(std::cin, line);
        int algorithm_number = 0;
        quantum = 0; // Default value for quantum

        size_t dash_pos = line.find('-');
        if (dash_pos != std::string::npos)
        {
            algorithm_number = std::stoi(line.substr(0, dash_pos));
            quantum = std::stoi(line.substr(dash_pos + 1));
        }
        else
        {
            algorithm_number = std::stoi(line);
        }

        std::cout << "Algorithm selected: " << algorithm_number;
        if (quantum > 0)
        {
            std::cout << " (Quantum = " << quantum << ")";
        }
        std::cout << std::endl;

        // Line 3: Simulation end time
        std::getline(std::cin, line);
        int simulation_end_time = std::stoi(line);
        std::cout << "Simulation end time: " << simulation_end_time << std::endl;

        // Line 4: Number of processes
        std::getline(std::cin, line);
        int processes_count = std::stoi(line);
        std::cout << "Number of processes: " << processes_count << std::endl;

        // Line 5 onward: Process descriptions
        for (int i = 0; i < processes_count; i++)
        {
            std::getline(std::cin, line);
            Process p;
            char comma;
            std::istringstream iss(line);

            // Parse the process line
            if (!(iss >> p.process_name >> comma >> p.arrivalTime >> comma >> p.serviceTime))
            {
                std::cerr << "Error: Invalid process description on line " << (5 + i) << std::endl;
                return;
            }

            // Initialize process attributes
            p.timeRemaining = p.serviceTime;
            add_process(p);

            // Print for debugging
            std::cout << "Process added: "
                      << "Name = " << p.process_name
                      << ", Arrival Time = " << p.arrivalTime
                      << ", Service Time = " << p.serviceTime
                      << std::endl;
        }

        // Execute the selected algorithm
        execute_algorithm(algorithm_number);

        // Display results based on the selected mode
        if (mode == TRACE)
        {
            std::cout << "Executing in TRACE mode.\n";
            trace_mode(algorithm_name, simulation_end_time);
        }
        else
        {
            std::cout << "Executing in STATS mode.\n";
            stats_mode(algorithm_name);
        }
    }
    catch (const std::exception &e)
    {
        // Catch exceptions for invalid inputs or parsing issues
        std::cerr << "Error reading input: " << e.what() << std::endl;
    }
    }

void Scheduler::execute_algorithm(int algorithm_number)
{
    switch (algorithm_number)
    {
    case 1:
        FCFS();
        algorithm_name = "FCFS";
        break;
    case 2:
        RR();
        algorithm_name="RR";
        break;
    case 3:
        SPN();
        algorithm_name = "SPN";
        break;
    case 4:
        SRT(processes);
        algorithm_name = "SRT";
        break;
    case 5:
        HRRN(processes);
        algorithm_name = "HRRN";
        break;
    case 6:
        FB_ONE(processes);
        algorithm_name = "FB-1";
        break;
    case 7:
        FB_TWOi();
        algorithm_name = "FB-2";
        break;
    case 8:
        Aging();
        break;
    default:
        std::cout << "Invalid algorithm number." << std::endl;
    }
}

void Scheduler::trace_mode(std::string algorithm_name, int timeLine)
{
    std::cout << algorithm_name << "  ";
    for (int i = 0; i <= timeLine; ++i)
    {
        std::cout << i % 10 << " ";
    }
    std::cout << "\n------------------------------------------------\n";

    for (const Process &p : processes)
    {
        int k = 0;
        std::cout << p.process_name << std::setw(6) << "|";
        while (k < timeLine)
        {
            while (k >= p.arrivalTime && k < p.finishTime)
            {
                auto it = find(p.running.begin(), p.running.end(), k);

                // Check if k is in running time of the process value was found
                if (it != p.running.end())
                {
                    std::cout << "*|";
                    k++;
                }
                else
                {
                    std::cout << ".|";
                    k++;
                }
            }
            if (k == timeLine)
            {
                break;
            }
            std::cout << " |";
            k++;
        }
        std::cout << "\n";
    }

    std::cout << "------------------------------------------------\n\n";
}

void Scheduler::stats_mode(std::string algorithm_name)
{
    int n = processes.size();
    double totalTurnaround = 0.0, totalNormTurnaround = 0.0;

    for (const auto &p : processes)
    {
        totalTurnaround += p.turnAround;
        totalNormTurnaround += p.normTurn;
    }

    double meanTurnaround = totalTurnaround / n;
    double meanNormTurnaround = totalNormTurnaround / n;

    std::cout << algorithm_name << "\n";
    std::cout << "Process    |";
    for (const Process &p : processes)
    {
        std::cout << "  " << p.process_name << "  |";
    }
    std::cout << "\n";

    std::cout << "Arrival    |";
    for (const Process &p : processes)
    {
        std::cout << setw(3) << p.arrivalTime << "  |";
    }
    std::cout << "\n";

    std::cout << "Service    |";
    for (const Process &p : processes)
    {
        std::cout << setw(3) << p.serviceTime << "  |";
    }
    std::cout << " Mean|\n";

    std::cout << "Finish     |";
    for (const Process &p : processes)
    {
        std::cout << setw(3) << p.finishTime << "  |";
    }
    std::cout << "-----|\n";

    std::cout << "Turnaround |";
    for (const Process &p : processes)
    {
        std::cout << setw(3) << p.turnAround << "  |";
    }
    std::cout << fixed << setprecision(2) << setw(5) << meanTurnaround << "|\n";

    std::cout << "NormTurn   | ";
    for (const Process &p : processes)
    {
        std::cout << setw(3) << fixed << setprecision(2) << p.normTurn << "| ";
    }
    std::cout << setw(3) << fixed << setprecision(2) << meanNormTurnaround << "|\n\n";
}

int main()
{
    Scheduler scheduler;
    scheduler.read_input();
    return 0;
}
