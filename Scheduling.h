//
// Created by noureldeen on 12/24/24.
//

#ifndef SCHEDULING_H
#define SCHEDULING_H

#include <bits/stdc++.h>
using namespace std;

typedef struct Process
{
    char process_id;
    int arrival_time;
    int service_time;
    int initial_priority;
    int priority;
    int waiting_time;
    int turnaround_time;
    int remaining_time;
    int start_time;
    int end_time;
    vector<char> state;
} Process;

vector<Process> FCFS(vector<Process> processes, int time_interval);
vector<Process> RR(vector<Process> processes, int time_quantum, int time_interval);
vector<Process> SPN(vector<Process> processes, int time_interval);
vector<Process> SRT(vector<Process> processes, int time_interval);
vector<Process> HRRN(vector<Process> processes, int time_interval);
vector<Process> FB1(vector<Process> processes, int time_interval);
vector<Process> FB2i(vector<Process> processes, int time_interval);
vector<Process> Aging(vector<Process> processes, int time_interval);
void printTable(vector<Process> processes, int time_interval);
void printState(vector<Process> processes);
void readInput();
vector<string> split(string str, char delimiter);
#endif // SCHEDULING_H
