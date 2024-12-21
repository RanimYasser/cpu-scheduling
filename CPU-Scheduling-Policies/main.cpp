#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>

struct Process {
    char process_name;     // Character representing the process name
    int arrivalTime;       // The time the process arrived
    int timeRemaining;     // Time left to finish the process
    int serviceTime;
    int finishTime;
    int turnAround;
    float normTurn;
    int priority;
};

class Scheduler{
private :
    int quantum;
    std::vector<Process> processes; // Using vector as the number of processes is not fixed
    enum Mode {
        TRACE,
        STATS
    }mode;

    public:
    Scheduler() : quantum(0), mode(STATS) {}

    void add_process(Process process){
        processes.push_back(process);
    }

    void set_mode(const std::string& m) {
        mode = (m == "trace") ? TRACE : STATS;
    }
    void FCFS();
    void RR();
    void SPN();
    void SRT();
    void HRRN();
    void FB_ONE();
    void FB_TWOi();
    void Aging();
    void set_mode(Mode m);
    void execute_algorithm(int algorithm_number);
    void read_input();
    void trace_mode();
    void stats_mode();

    static bool compare_Processes_wrt_ArrivalTime (const Process& a, const Process& b){
        return a.arrivalTime < b.arrivalTime;
    }
    void sort_processes(){
        std::sort(processes.begin(), processes.end(), compare_Processes_wrt_ArrivalTime);
    }
};

void Scheduler::FCFS() {
sort_processes();

}

void Scheduler::RR() {

}

void Scheduler::SPN() {

}

void Scheduler::SRT() {

}

void Scheduler::HRRN() {

}

void Scheduler::FB_ONE() {

}

void Scheduler::FB_TWOi() {

}

void Scheduler::Aging() {

}

void Scheduler::read_input() {
    //Line 1: “trace” or “stats”
    std::string line;
    std::getline(std::cin, line);
    set_mode(line);
    std::cout << "Mode set to: " << line << std::endl;
    // Line 2: a comma-separated list telling which CPU scheduling policies to be analyzed/visualized along with their parameters, if applicable.
    //Each algorithm is represented by a number as listed
    std::getline(std::cin, line);
    int algorithm_number = std::stoi(line);
    std::cout << "Algorithm number: " << algorithm_number << std::endl;
    execute_algorithm(algorithm_number);
    //Line 3: An integer specifying the last instant to be used in your simulation and to be shown on the timeline.
     std::getline(std::cin, line);
    int simulation_end_time = std::stoi(line);
    std::cout << "Simulation end time: " << simulation_end_time << std::endl;

    //Line 4: An integer specifying the number of processes to be simulated.
    // None of the processes is making a blocking call
    std::getline(std::cin, line);
    int processes_count = std::stoi(line);
    std::cout << "Number of processes: " << processes_count << std::endl;

    // Line 5: Start of description of processes.
    // Each process is to be described on a separate line.
    // For algorithms 1 through 7, each process is described using a comma-separated list specifying:
    //- One character specifying a process name
    //- Arrival Time
    //- Service Time
    for (int i = 0; i < processes_count; i++) {
        std::getline(std::cin, line);
        Process p;
        char comma;
        std::istringstream iss(line);
        iss >> p.process_name >> comma >> p.arrivalTime >> comma >> p.serviceTime;
        p.timeRemaining = p.serviceTime;
        add_process(p);
    }
    //print processes for debugging
for (const auto &process : processes) { //In C++, the auto keyword allows the compiler to automatically deduce the type of a variable based on its initializer.
        std::cout << "Process: " << process.process_name
                  << ", Arrival Time: " << process.arrivalTime
                  << ", Service Time: " << process.serviceTime
                  << ", Time Remaining: " << process.timeRemaining << std::endl;
    }
}



void Scheduler::execute_algorithm(int algorithm_number){
    switch(algorithm_number){
        case 1:
            FCFS();
            break;
        case 2:
            RR();
            break;
        case 3:
            SPN();
            break;
        case 4:
            SRT();
            break;
        case 5:
            HRRN();
            break;
        case 6:
            FB_ONE();
            break;
        case 7:
            FB_TWOi();
            break;
        case 8:
            Aging();
            break;
        default:
            std::cout << "Invalid algorithm number." << std::endl;
    }
}

void Scheduler::trace_mode() {

}

void Scheduler::stats_mode() {

}

int main() {
    Scheduler scheduler;
    scheduler.read_input();
    return 0;
}
