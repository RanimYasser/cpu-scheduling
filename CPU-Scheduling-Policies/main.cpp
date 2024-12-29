#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <queue>
#include <cmath>
#include <climits>

using namespace std;
int total_service_time = 0;
struct Process
{
    char process_name; // Character representing the process name
    int arrivalTime;   // The time the process arrived
    int timeRemaining; // Time left to finish the process
    int serviceTime;
    int finishTime;
    int turnAround;         // TA=Wait + service time
    int start_running = -1; // first time process runs changedddd
    float normTurn;
    int priority = 0;
    int waiting_time;
    int isCompleted = 0;
    int initialPriority = 0;
    int order_in_ready_queue = 0;
    bool inQueue = false;
    vector<int> running; // keeps track of a process's timeline
};

struct CompareServiceTime
{
    bool operator()(Process *p1, Process *p2)
    {
        return p1->serviceTime > p2->serviceTime;
    }
};

struct ComparePriority
{
    bool operator()(Process *p1, Process *p2)
    {
        return p1->priority > p2->priority;
    }
};

struct algorithm
{
    int algorithm_number;
    int quantum;
    char algorithm_name;
};

class Scheduler
{
private:
    int quantum;
    int simulation_end_time;
    std::string algorithm_name;
    std::vector<Process> processes; // Using vector as the number of processes is not fixed
    std::vector<algorithm> algorithms;
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
    void set_quantum(int quantum_value)
    {
        quantum = quantum_value;
    }
    void FCFS();
    void RR(int quantum);
    void SPN();
    void SRT();
    void HRRN();
    void FB_ONE();
    void FB_TWOi();
    void Aging(int quantum);
    void Increment_priorities(std::vector<Process *> &ready_queue);
    void Increment_waitingTime(std::vector<Process *> &ready_queue);
    void Preempt_process_to_ready_queue(std::vector<Process *> &ready_queue, Process *process);
    Process *Get_highest_priority_process(std::vector<Process *> &ready_queue);
    void reset_processes();
    void set_aging();
    void set_mode(Mode m);
    void execute_algorithm(int algorithm_number, int quantum);
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
    int current_time = 0;
    sort_processes_by_arrival_time(); // sort processes with arrival time to handle any  input file

    for (auto &process : processes)
    {
        if (current_time < process.arrivalTime)
        {
            current_time = process.arrivalTime;
        }

        process.waiting_time = current_time - process.arrivalTime;
        process.turnAround = process.waiting_time + process.serviceTime;
        process.start_running = current_time;
        current_time += process.serviceTime;
        process.finishTime = current_time;
        process.normTurn = (double)process.turnAround / process.serviceTime;
        for (int i = process.start_running; i < current_time; i++)
        {
            process.running.push_back(i);
        }
    }
}
void Scheduler::RR(int quantum)
{
    sort_processes_by_arrival_time();
    std::queue<Process *> ready_queue;
    int current_time = 0;
    bool all_completed = false;

    while (!all_completed)
    {
        for (auto &process : processes)
        {
            if (process.arrivalTime <= current_time && process.isCompleted == 0)
            {
                ready_queue.push(&process);
                process.isCompleted = -1;
            }
        }

        if (ready_queue.empty())
        {
            current_time++;
            continue;
        }

        Process *current_process = ready_queue.front();
        ready_queue.pop();

        int execution_time = std::min(quantum, current_process->timeRemaining);

        for (int i = 0; i < execution_time; ++i)
        {
            current_process->running.push_back(current_time++);
        }

        current_process->timeRemaining -= execution_time;

        for (auto &process : processes)
        {
            if (process.arrivalTime <= current_time && process.isCompleted == 0)
            {
                ready_queue.push(&process);
                process.isCompleted = -1;
            }
        }

        if (current_process->timeRemaining == 0)
        {
            current_process->finishTime = current_time;
            current_process->turnAround = current_process->finishTime - current_process->arrivalTime;
            current_process->waiting_time = current_process->turnAround - current_process->serviceTime;
            current_process->normTurn = (double)current_process->turnAround / current_process->serviceTime;
            current_process->isCompleted = 1;
        }
        else
        {

            ready_queue.push(current_process);
        }

        all_completed = true;
        for (const auto &process : processes)
        {
            if (process.isCompleted != 1)
            {
                all_completed = false;
                break;
            }
        }
    }
}
void Scheduler::SPN()
{
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
                    processes_completed = false;
                    break;
                }
            }
            if (processes_completed)
                break;

            current_time++;
        }
    }
}

void Scheduler::SRT()
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

void Scheduler::HRRN()
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

void Scheduler::FB_ONE()
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
            if (execute == 0 && queues[execute].size() > 1 && queues[execute].front()->timeRemaining < queues[execute].front()->serviceTime)
            {
                Process *tempProcess = queues[execute].front();
                queues[execute].pop();
                queues[execute].push(tempProcess);
            }
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
                if (numReady == 1)
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
    int numQueues = 5;
    int execute = -1;
    int numReady = 0;
    int quantum = 0;
    std::queue<Process *> queues[numQueues];

    while (remainingProcesses > 0)
    {
        // Add new processes to the highest priority queue
        for (Process &p : processes)
        {
            if (p.arrivalTime <= currentTime && p.timeRemaining > 0 && p.start_running == -1)
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

        quantum = pow(2, execute);
        if (execute != -1)
        {
            if (execute == 0 && queues[execute].size() > 1 && queues[execute].front()->timeRemaining < queues[execute].front()->serviceTime)
            {
                Process *tempProcess = queues[execute].front();
                queues[execute].pop();
                queues[execute].push(tempProcess);
            }

            Process *selectedProcess = queues[execute].front();
            queues[execute].pop();

            selectedProcess->start_running = currentTime;

            for (int i = 0; i < quantum; i++)
            {
                selectedProcess->running.push_back(currentTime);
                currentTime++;

                selectedProcess->timeRemaining--;
                if (selectedProcess->timeRemaining == 0)
                {
                    break;
                }
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
                if (numReady == 1)
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

void Scheduler::Aging(int quantum)
{

    std::vector<Process *> ready_queue;
    int current_time = 0;
    while (current_time < simulation_end_time)
    {
        for (auto &process : processes)
        {
            if (process.arrivalTime <= current_time && !process.inQueue)
            {
                process.inQueue = true;
                process.priority = process.initialPriority + 1;
                process.waiting_time = 0;
                process.finishTime = simulation_end_time;
                ready_queue.push_back(&process);
            }
        }

        if (!ready_queue.empty())
        {

            Process *current_process = Get_highest_priority_process(ready_queue);
            if (current_process)
            {

                for (int i = current_time; i < current_time + quantum; i++)
                {
                    current_process->running.push_back(i);

                    for (auto &process : processes)
                    {
                        if (process.arrivalTime <= i && !process.inQueue)
                        {
                            process.inQueue = true;
                            process.priority = process.initialPriority + 1;
                            process.waiting_time = 0;
                            process.finishTime = simulation_end_time;
                            ready_queue.push_back(&process);
                        }
                    }
                    Increment_priorities(ready_queue);
                    Increment_waitingTime(ready_queue);
                }
                current_time += quantum;
                Preempt_process_to_ready_queue(ready_queue, current_process);
            }
            else
            {
                current_time++;
            }
        }
        else
        {
            current_time++;
        }
    }
}

void Scheduler::Increment_priorities(std::vector<Process *> &ready_queue)
{
    for (auto *process : ready_queue)
    {
        process->priority++;
    }
}

void Scheduler::Increment_waitingTime(std::vector<Process *> &ready_queue)
{
    for (auto *process : ready_queue)
    {
        process->waiting_time++;
    }
}

void Scheduler::Preempt_process_to_ready_queue(std::vector<Process *> &ready_queue, Process *process)
{
    process->priority = process->initialPriority;
    process->waiting_time = 0;
    ready_queue.push_back(process);
}

Process *Scheduler::Get_highest_priority_process(std::vector<Process *> &ready_queue)
{
    auto it = std::max_element(ready_queue.begin(), ready_queue.end(), [](const Process *a, const Process *b)
                               { return a->priority < b->priority || (a->priority == b->priority && a->waiting_time < b->waiting_time); });

    if (it != ready_queue.end())
    {
        Process *selected_process = *it;
        ready_queue.erase(it);
        return selected_process;
    }
    return nullptr;
}
void Scheduler::set_aging()
{
    for (auto &p : processes)
    {
        p.priority = p.serviceTime;
        p.initialPriority = p.priority;
    }
}
void Scheduler::reset_processes()
{
    for (auto &p : processes)
    {
        p.timeRemaining = p.serviceTime;
        p.isCompleted = 0;
        p.priority = p.serviceTime;
        p.initialPriority = p.priority;
        p.start_running = -1;
        p.waiting_time = 0;
        p.inQueue = false;
        p.running.clear();
    }
}

void Scheduler::read_input()
{
    try
    {
        // Line 1: Mode (trace or stats)
        std::string line;
        std::getline(std::cin, line);
        set_mode(line);

        // Line 2: Algorithm and quantum if applicable
        std::getline(std::cin, line);
        vector<string> v;
        stringstream ss(line);

        while (ss.good())
        {
            string substr;
            getline(ss, substr, ',');
            v.push_back(substr);
        }

        for (int i = 0; i < v.size(); i++)
        {
            algorithm a;
            size_t dash_pos = v[i].find('-');
            if (dash_pos != std::string::npos)
            {
                a.algorithm_number = std::stoi(v[i].substr(0, dash_pos));
                a.quantum = std::stoi(v[i].substr(dash_pos + 1));
            }
            else
            {
                a.algorithm_number = std::stoi(v[i]);
                a.quantum = 0;
            }
            algorithms.push_back(a);
        }

        // Line 3: Simulation end time
        std::getline(std::cin, line);
        simulation_end_time = std::stoi(line); // Set the simulation end time

        // Line 4: Number of processes
        std::getline(std::cin, line);
        int processes_count = std::stoi(line);

        // Lines 5 onward: Process descriptions

        for (int i = 0; i < processes_count; i++)
        {

            std::getline(std::cin, line);
            Process p;
            char comma;
            std::istringstream iss(line);

            if (!(iss >> p.process_name >> comma >> p.arrivalTime >> comma >> p.serviceTime))
            {
                std::cerr << "Error: Invalid process description on line " << (5 + i) << std::endl;
                continue;
            }

            p.timeRemaining = p.serviceTime; // Set remaining time for service
            p.priority = p.serviceTime;      // handles aging
            p.initialPriority = p.priority;
            add_process(p);
        }

        for (const algorithm &a : algorithms)
        {

            execute_algorithm(a.algorithm_number, a.quantum);

            // Display results based on the selected mode
            if (mode == TRACE)
            {
                trace_mode(algorithm_name, simulation_end_time);
            }
            else
            {
                stats_mode(algorithm_name);
            }

            reset_processes(); // reset process after each algorithm
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error reading input: " << e.what() << std::endl;
    }
}

void Scheduler::execute_algorithm(int algorithm_number, int quantum)
{
    switch (algorithm_number)
    {
    case 1:
        FCFS();
        algorithm_name = "FCFS";
        break;
    case 2:
        RR(quantum);
        {
            std::ostringstream oss;
            oss << "RR-" << quantum;
            algorithm_name = oss.str();
        }
        break;
    case 3:
        SPN();
        algorithm_name = "SPN";
        break;
    case 4:
        SRT();
        algorithm_name = "SRT";
        break;
    case 5:
        HRRN();
        algorithm_name = "HRRN";
        break;
    case 6:
        FB_ONE();
        algorithm_name = "FB-1";
        break;
    case 7:
        FB_TWOi();
        algorithm_name = "FB-2i";
        break;
    case 8:
        Aging(quantum);
        algorithm_name = "Aging";
        break;
    default:
        std::cout << "Invalid algorithm number.";
    }
}

void Scheduler::trace_mode(std::string algorithm_name, int timeLine)
{
    std::cout << left << setw(6) << algorithm_name;

    for (int i = 0; i <= timeLine; ++i)
    {
        std::cout << i % 10 << " ";
    }
    std::cout << "\n------------------------------------------------\n";

    for (const Process &p : processes)
    {
        int k = 0;
        std::cout << left << setw(6) << p.process_name << "|";
        while (k < timeLine)
        {
            while (k >= p.arrivalTime && k < p.finishTime)
            {
                auto it = find(p.running.begin(), p.running.end(), k);

                // Check if k is in running time of the process
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
        std::cout << " ";
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
