#include <iostream>
#include <vector>
#include <algorithm>
struct Process {
    char process_name;     // Character representing the process name
    int arrivalTime;       // The time the process arrived
    int timeRemaining;     // Time left to finish the process
    int serviceTime;
    int priority;
};

struct Scheduler {
    int quantum;
    std::vector<Process> processes; // Using vector as the number of processes is not fixed
    enum Mode {
        TRACE,
        STATS
    }mode;
    void add_process(Process process){
    processes.push_back(process);

}
    void FCFS(); // First Come, First Served
    void RR();   // Round Robin
    void SPN();  // Shortest Process Next
    void SRT();  // Shortest Remaining Time
    void HRRN(); // Highest Response Ratio Next
    void FB_ONE(); // Feedback queue with single level
    void FB_TWOi(); // Feedback queue with multiple levels
    void Aging(); // Aging algorithm
    void set_mode(Mode m);
    void read_input();
    void trace_mode();
    void stats_mode();
    static bool compare_Processes_wrt_ArrivalTime (const Process& a, const Process& b){
        return a.arrivalTime<b.arrivalTime;
    }
    void sort_processes(){
        std::sort(processes.begin(), processes.end(),compare_Processes_wrt_ArrivalTime);
        }
        };



void Scheduler::FCFS() {


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

}

void Scheduler::set_mode(Mode m){
// call trace_mode function in case trace , stats_mode otherwise
mode=m;
if (mode==TRACE){
    trace_mode();
}
else if (mode==STATS){
    stats_mode();
}
else {
    std::cout << "Invalid mode" << std::endl;
}
}
void Scheduler::trace_mode() {

}

void Scheduler::stats_mode() {

}

int main() {
    std::cout << "Hello world!" << std::endl;

    return 0;
}
