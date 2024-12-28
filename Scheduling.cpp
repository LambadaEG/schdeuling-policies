#include "Scheduling.h"

void printTable(vector<Process> processes, int time_interval)
{

    for (int i = 0; i <= time_interval; ++i)
    {
        cout << i % 10 << " ";
    }
    cout << "\n";
    cout << string(48, '-') << "\n";
    for (const auto &process : processes)
    {
        cout << process.process_id << "     ";
        for (int i = 0; i < time_interval; ++i)
        {
            if (i < process.state.size())
            {
                cout << "|" << process.state[i];
            }
            else
            {
                cout << "| ";
            }
        }
        cout << "| \n";
    }
    cout << string(48, '-') << "\n";
}
void printState(vector<Process> processes)
{
    cout << "Process    |";
    for (const auto &process : processes)
        cout << "  " << process.process_id << "  |";
    cout << "\n";
    cout << "Arrival    |";
    for (const auto &process : processes)
        cout << setw(3) << right << process.arrival_time << "  |";
    cout << "\n";
    cout << "Service    |";
    for (const auto &process : processes)
        cout << setw(3) << right << process.service_time << "  |";
    cout << " Mean|";
    cout << "\n";
    cout << "Finish     |";
    for (const auto &process : processes)
        cout << setw(3) << right << process.end_time << "  |";
    cout << "-----|\n";
    double sum = 0;
    cout << "Turnaround |";
    for (const auto &process : processes)
    {
        cout << setw(3) << right << process.turnaround_time << "  |";
        sum += process.turnaround_time;
    }
    cout << fixed << setprecision(2) << setw(5) << sum / processes.size() << "|";
    cout << "\n";
    cout << "NormTurn   |";
    sum = 0;
    for (const auto &process : processes)
    {
        cout << setw(5) << right << process.turnaround_time * 1.0 / process.service_time << "|";
        sum += process.turnaround_time * 1.0 / process.service_time;
    }
    cout << fixed << setprecision(2) << setw(5) << sum / processes.size() << "|";
    cout << "\n";
}
vector<string> split(string str, char delimiter)
{
    vector<string> result;
    stringstream ss(str);
    string token;
    while (getline(ss, token, delimiter))
    {
        result.push_back(token);
    }
    return result;
}
vector<Process> FCFS(vector<Process> processes, int time_interval)
{
    int current_time = 0;
    sort(processes.begin(), processes.end(), [](const Process &a, const Process &b)
         { return a.arrival_time < b.arrival_time; });
    deque<Process *> ready_queue;
    int index = 0;
    while (current_time < time_interval)
    {
        while (index < processes.size() && processes[index].arrival_time <= current_time)
        {
            ready_queue.push_back(&processes[index]);
            index++;
        }
        if (ready_queue.empty())
        {
            current_time++;
            continue;
        }
        auto process = ready_queue.front();
        ready_queue.pop_front();
        process->start_time = current_time;
        for (int i = 0; i < process->service_time; i++)
        {
            process->state[current_time] = '*';
            current_time++;
            while (index < processes.size() && processes[index].arrival_time <= current_time)
            {
                ready_queue.push_back(&processes[index]);
                index++;
            }
            for (auto other_process : ready_queue)
            {
                other_process->state[current_time] = '.';
                other_process->waiting_time++;
            }
        }
        process->end_time = current_time;
        process->turnaround_time = process->end_time - process->arrival_time;
    }
    return processes;
}

vector<Process> RR(vector<Process> processes, int time_quantum, int time_interval)
{
    vector<vector<Process *>> time_slots(time_interval + 1);
    for (int i = 0; i < processes.size(); i++)
    {
        time_slots[processes[i].arrival_time].push_back(&processes[i]);
    }
    int current_time = -1;
    deque<Process *> ready_queue;
    while (current_time < time_interval)
    {
        if (ready_queue.empty())
        {
            current_time++;
            for (auto process : time_slots[current_time])
            {
                ready_queue.push_back(process);
            }
            continue;
        }
        auto process = ready_queue.front();
        ready_queue.pop_front();
        int count = 0;
        while (count < time_quantum && process->remaining_time > 0)
        {
            process->state[current_time] = '*';
            for (auto other_process : ready_queue)
            {
                other_process->state[current_time] = '.';
                other_process->waiting_time++;
            }
            current_time++;
            for (auto process : time_slots[current_time])
            {
                ready_queue.push_back(process);
            }
            count++;
            process->remaining_time--;
        }
        if (process->remaining_time <= 0)
        {
            process->end_time = current_time;
            process->turnaround_time = process->end_time - process->arrival_time;
        }
        else
        {
            ready_queue.push_back(process);
        }
    }
    return processes;
}

vector<Process> SPN(vector<Process> processes, int time_interval)
{
    vector<vector<Process *>> time_slots(time_interval + 1);
    for (int i = 0; i < processes.size(); i++)
    {
        time_slots[processes[i].arrival_time].push_back(&processes[i]);
    }
    int current_time = -1;
    deque<Process *> ready_queue;
    while (current_time < time_interval)
    {
        sort(ready_queue.begin(), ready_queue.end(), [](const Process *a, const Process *b)
             { return a->remaining_time < b->remaining_time || (a->remaining_time == b->remaining_time && a->arrival_time < b->arrival_time); });
        if (ready_queue.empty())
        {
            current_time++;
            for (auto process : time_slots[current_time])
            {
                ready_queue.push_back(process);
            }
            continue;
        }
        auto process = ready_queue.front();
        ready_queue.pop_front();
        int count = 0;
        while (count < process->service_time)
        {
            process->state[current_time] = '*';
            for (auto other_process : ready_queue)
            {
                other_process->state[current_time] = '.';
                other_process->waiting_time++;
            }
            current_time++;
            for (auto process : time_slots[current_time])
            {
                ready_queue.push_back(process);
            }
            count++;
            process->remaining_time--;
        }
        process->end_time = current_time;
        process->turnaround_time = process->end_time - process->arrival_time;
    }
    return processes;
}

vector<Process> SRT(vector<Process> processes, int time_interval)
{
    vector<vector<Process *>> time_slots(time_interval + 1);
    for (int i = 0; i < processes.size(); i++)
    {
        time_slots[processes[i].arrival_time].push_back(&processes[i]);
    }
    int current_time = -1;
    deque<Process *> ready_queue;
    while (current_time < time_interval)
    {
        if (ready_queue.empty())
        {
            current_time++;
            for (auto process : time_slots[current_time])
            {
                ready_queue.push_back(process);
            }
            continue;
        }
        auto process = ready_queue.front();
        process->state[current_time] = '*';
        for (int i = 1; i < ready_queue.size(); i++)
        {
            ready_queue[i]->state[current_time] = '.';
            ready_queue[i]->waiting_time++;
        }
        current_time++;
        for (auto process : time_slots[current_time])
        {
            ready_queue.push_back(process);
        }
        process->remaining_time--;
        if (process->remaining_time <= 0)
        {
            process->end_time = current_time;
            process->turnaround_time = process->end_time - process->arrival_time;
            ready_queue.pop_front();
        }
        sort(ready_queue.begin(), ready_queue.end(), [](const Process *a, const Process *b)
             { return a->remaining_time < b->remaining_time || (a->remaining_time == b->remaining_time && a->arrival_time < b->arrival_time); });
    }
    return processes;
}

vector<Process> HRRN(vector<Process> processes, int time_interval)
{
    vector<vector<Process *>> time_slots(time_interval + 1);
    for (int i = 0; i < processes.size(); i++)
    {
        time_slots[processes[i].arrival_time].push_back(&processes[i]);
    }
    int current_time = -1;
    deque<Process *> ready_queue;
    while (current_time < time_interval)
    {
        sort(ready_queue.begin(), ready_queue.end(), [](const Process *a, const Process *b)
             { return (a->waiting_time + a->remaining_time) / (double)a->service_time > (b->waiting_time + b->remaining_time) / (double)b->service_time || (a->waiting_time + a->remaining_time) / (double)a->service_time == (b->waiting_time + b->remaining_time) / (double)b->remaining_time || (a->remaining_time == b->remaining_time && a->arrival_time < b->arrival_time); });
        if (ready_queue.empty())
        {
            current_time++;
            for (auto process : time_slots[current_time])
            {
                ready_queue.push_back(process);
            }
            continue;
        }
        auto process = ready_queue.front();
        ready_queue.pop_front();
        int count = 0;
        while (count < process->service_time)
        {
            process->state[current_time] = '*';
            for (auto other_process : ready_queue)
            {
                other_process->state[current_time] = '.';
                other_process->waiting_time++;
            }
            current_time++;
            for (auto process : time_slots[current_time])
            {
                ready_queue.push_back(process);
            }
            count++;
            process->remaining_time--;
        }
        process->end_time = current_time;
        process->turnaround_time = process->end_time - process->arrival_time;
    }
    return processes;
}

vector<Process> FB1(vector<Process> processes, int time_interval)
{
    vector<vector<Process *>> time_slots(time_interval + 1);
    for (int i = 0; i < processes.size(); i++)
    {
        time_slots[processes[i].arrival_time].push_back(&processes[i]);
    }
    int current_time = -1;
    vector<deque<Process *>> ready_queue(time_interval + 1);
    while (current_time < time_interval)
    {
        int first = -1;
        for (int i = 0; i < ready_queue.size(); i++)
        {
            if (!ready_queue[i].empty())
            {
                first = i;
                break;
            }
        }
        if (first != -1)
        {
            auto process = ready_queue[first].front();
            ready_queue[first].pop_front();
            process->state[current_time] = '*';
            for (int i = 0; i < ready_queue.size(); i++)
            {
                if (!ready_queue[i].empty())
                {
                    for (int j = 0; j < ready_queue[i].size(); j++)
                    {
                        ready_queue[i][j]->state[current_time] = '.';
                        ready_queue[i][j]->waiting_time++;
                    }
                }
            }
            current_time++;
            for (auto process : time_slots[current_time])
            {
                ready_queue[0].push_back(process);
            }
            process->remaining_time--;
            if (process->remaining_time <= 0)
            {
                process->end_time = current_time;
                process->turnaround_time = process->end_time - process->arrival_time;
            }
            else
            {
                int new_first = -1;
                for (int i = 0; i < ready_queue.size(); i++)
                {
                    if (!ready_queue[i].empty())
                    {
                        new_first = i;
                        break;
                    }
                }
                if (new_first == -1)
                {
                    ready_queue[first].push_back(process);
                }
                else
                {
                    ready_queue[first + 1].push_back(process);
                }
            }
        }
        else
        {
            current_time++;
            for (auto process : time_slots[current_time])
            {
                ready_queue[0].push_back(process);
            }
        }
    }
    return processes;
}

vector<Process> FB2i(vector<Process> processes, int time_interval)
{
    vector<vector<Process *>> time_slots(time_interval + 1);
    for (int i = 0; i < processes.size(); i++)
    {
        time_slots[processes[i].arrival_time].push_back(&processes[i]);
    }
    int current_time = -1;
    vector<deque<Process *>> ready_queue(time_interval + 1);
    while (current_time < time_interval)
    {
        int first = -1;
        for (int i = 0; i < ready_queue.size(); i++)
        {
            if (!ready_queue[i].empty())
            {
                first = i;
                break;
            }
        }
        if (first != -1)
        {
            auto process = ready_queue[first].front();
            ready_queue[first].pop_front();
            int time_quantum = 1 << first;
            while (time_quantum != 0 && process->remaining_time > 0)
            {
                time_quantum--;
                process->state[current_time] = '*';
                for (int i = 0; i < ready_queue.size(); i++)
                {
                    if (!ready_queue[i].empty())
                    {
                        for (int j = 0; j < ready_queue[i].size(); j++)
                        {
                            ready_queue[i][j]->state[current_time] = '.';
                            ready_queue[i][j]->waiting_time++;
                        }
                    }
                }
                current_time++;
                for (auto process : time_slots[current_time])
                {
                    ready_queue[0].push_back(process);
                }
                process->remaining_time--;
            }
            if (process->remaining_time <= 0)
            {
                process->end_time = current_time;
                process->turnaround_time = process->end_time - process->arrival_time;
            }
            else
            {
                int new_first = -1;
                for (int i = 0; i < ready_queue.size(); i++)
                {
                    if (!ready_queue[i].empty())
                    {
                        new_first = i;
                        break;
                    }
                }
                if (new_first == -1)
                {
                    ready_queue[first].push_back(process);
                }
                else
                {
                    ready_queue[first + 1].push_back(process);
                }
            }
        }
        else
        {
            current_time++;
            for (auto process : time_slots[current_time])
            {
                ready_queue[0].push_back(process);
            }
        }
    }
    return processes;
}

vector<Process> Aging(vector<Process> processes, int time_interval, int quantum)
{
    vector<vector<Process *>> time_slots(time_interval + 1);
    for (int i = 0; i < processes.size(); i++)
    {
        time_slots[processes[i].arrival_time].push_back(&processes[i]);
    }
    int current_time = -1;
    deque<Process *> ready_queue;
    while (current_time < time_interval)
    {
        stable_sort(ready_queue.begin(), ready_queue.end(), [](const Process *a, const Process *b)
                    { return a->priority > b->priority; });
        if (ready_queue.empty())
        {
            current_time++;
            for (auto process : time_slots[current_time])
            {
                process->priority++;
                ready_queue.push_back(process);
            }
            continue;
        }
        auto process = ready_queue.front();
        ready_queue.pop_front();
        int count = 0;
        while (count < quantum)
        {
            process->state[current_time] = '*';
            for (auto other_process : ready_queue)
            {
                other_process->state[current_time] = '.';
                other_process->priority++;
            }
            current_time++;
            for (auto process : time_slots[current_time])
            {
                process->priority++;
                ready_queue.push_back(process);
            }
            count++;
        }
        process->priority = process->initial_priority;
        ready_queue.push_back(process);
    }
    return processes;
}

void readInput()
{
    string mode;
    getline(cin, mode);
    string policy;
    getline(cin, policy);
    vector<string> policies = split(policy, ',');
    string time_interval;
    getline(cin, time_interval);
    int time_interval_int = stoi(time_interval);
    string num_of_processes;
    getline(cin, num_of_processes);
    int num_of_processes_int = stoi(num_of_processes);
    vector<Process> processes;
    for (int i = 0; i < num_of_processes_int; i++)
    {
        string line;
        getline(cin, line);
        vector<string> tokens = split(line, ',');
        Process process;
        process.process_id = tokens[0][0];
        process.arrival_time = stoi(tokens[1]);
        process.service_time = stoi(tokens[2]);
        process.initial_priority = stoi(tokens[2]);
        process.priority = process.initial_priority;
        process.waiting_time = 0;
        process.turnaround_time = 0;
        process.remaining_time = process.service_time;
        process.state = vector<char>(time_interval_int + 2, ' ');
        process.start_time = -1;
        process.end_time = -1;
        processes.push_back(process);
    }
    for (auto current_policy : policies)
    {
        int policy_number = stoi(current_policy.substr(0, 1));
        int time_quantum;
        if (current_policy.size() > 1)
        {
            time_quantum = stoi(current_policy.substr(2));
        }
        vector<Process> finished;
        switch (policy_number)
        {
        case 1:
            if (mode == "trace")
                cout << left << setw(6) << "FCFS";
            else
                cout << "FCFS";
            finished = FCFS(processes, time_interval_int);
            break;
        case 2:
            if (mode == "trace")
                cout << left << setw(6) << "RR-" + to_string(time_quantum);
            else
                cout << "RR-" + to_string(time_quantum);
            finished = RR(processes, time_quantum, time_interval_int);
            break;
        case 3:
            if (mode == "trace")
                cout << left << setw(6) << "SPN";
            else
                cout << "SPN";
            finished = SPN(processes, time_interval_int);
            break;
        case 4:
            if (mode == "trace")
                cout << left << setw(6) << "SRT";
            else
                cout << "SRT";
            finished = SRT(processes, time_interval_int);
            break;
        case 5:
            if (mode == "trace")
                cout << left << setw(6) << "HRRN";
            else
                cout << "HRRN";
            finished = HRRN(processes, time_interval_int);
            break;
        case 6:
            if (mode == "trace")
                cout << left << setw(6) << "FB-" + to_string(1);
            else
                cout << "FB-" + to_string(1);
            finished = FB1(processes, time_interval_int);
            break;
        case 7:
            if (mode == "trace")
                cout << left << setw(6) << "FB-" + to_string(2) + "i";
            else
                cout << "FB-" + to_string(2) + "i";
            finished = FB2i(processes, time_interval_int);
            break;
        case 8:
            if (mode == "trace")
                cout << left << setw(6) << "Aging";
            else
                cout << "Aging";
            finished = Aging(processes, time_interval_int, time_quantum);
            break;
        }
        if (mode == "trace")
        {
            printTable(finished, time_interval_int);
            cout << "\n";
        }
        else
        {
            cout << "\n";
            printState(finished);
            cout << "\n";
        }
    }
}