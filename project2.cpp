//Colin Sawyer
//CPSC 3220 Operating Systems Section 2
//Program 2
// Simulating a Scheduler with FIFO, Shortest Job First, and Round Robin policies

#include<iostream>
#include<vector>
#include<stdio.h>
#include<string>
#include<queue>

using namespace std;
//create a task struct to store all of the process data
struct Task{
  int id;
  int length;
  int arrival;
  int finish;
  int response;
  int remaining;
  Task(int i, int l, int a){
    id = i;
    length = l;
    arrival = a;
    finish = 0;
    response = 0;
    remaining = length;
  }
  void printTask(){
    cout << "Process ID: " << id << endl;
    cout << "   Arrival Time: " << arrival << endl;
    cout << "   Process Length: " << length << endl;
    cout << "   Completion Time: " << finish << endl;
    cout << "   Response Time: " << response << endl;
    cout << endl;
  }
};
int main(){
//create a vector for the list of tasks and queue for the ready tasks
  vector<Task> list;
  queue<Task> ready;
//open file and read in information on the schedule being performed
  FILE *input = fopen("simulation_load.txt", "r");

  int policy;
  int preemption;
  int timeSlice;
  int numProcesses;

  char line[100];
  fscanf(input, "%d", &policy);
  fgets(line, 100, input);

  fscanf(input, "%d", &preemption);
  fgets(line, 100, input);

  fscanf(input, "%d", &timeSlice);
  fgets(line, 100, input);

  fscanf(input, "%d", &numProcesses);
  fgets(line, 100, input);

//check input file for errors in schedule data
  if(policy > 2 || policy < 0){
    policy = 0;
    cout << "Error, the Schedule data is invalid, defaulting to FIFO!" << endl;
  }
//override schedule data based on policy if needed
  if(policy == 0){preemption = 0;}
  if(policy == 2) {preemption = 1;}
  if(preemption < 0 || preemption > 1){preemption = 1;}
//parse input file into data for the task structs to hold
  for(int i = 0; i < numProcesses; i++){
    int id;
    int length;
    int arrival;
    fscanf(input, "%d", &id);
    fgets(line, 100, input);
    fscanf(input, "%d", &length);
    fgets(line, 100, input);
    fscanf(input, "%d", &arrival);
    fgets(line, 100, input);
    Task next = Task(id, length, arrival);
    list.push_back(next);
  }
//variables for the elapsed time and the number of context switches
  int clockTime = 0;
  int contexts = 0;
  //if it is FIFO
  if(policy == 0){
    cout << "Scheduling Policy: FIFO" << endl;
    cout << "Premption: OFF" << endl;
    cout << "Time Quantum: " << timeSlice << endl;
    cout << "Number of Processes: " << numProcesses << "\n" << endl;
    while(true){

      //if it is currently any process's arrival time, add it to ready queue
      for(int i= 0; i < numProcesses; i++){
          if(clockTime == list[i].arrival){
            Task temp = list[i];
            ready.push(temp);
          }
      }
      clockTime++;
      ready.front().remaining--;
      //when a task finishes, remove it from the queue and push new info to vector of tasks
      if(ready.front().remaining == 0){
        ready.front().finish = clockTime;
        ready.front().response = ready.front().finish - ready.front().arrival;
        list[ready.front().id] = ready.front();
        ready.pop();
        contexts++;
      }
      //break when there are no more ready tasks
      if(ready.empty()){break;}
    }
    //calculate footer data for average response time and numbe of context switches
    int sum = 0;
    for(int i = 0; i < numProcesses; i++){
      sum = list[i].response + sum;
      list[i].printTask();
    }
    cout << "\nAvg Response Time: " << sum/numProcesses << endl;
    cout << "Number of Context Switches: " << contexts << endl;
  }
  //if it is SJF
  else if(policy == 1){
    cout << "Scheduling Policy: SJF" << endl;
    cout << "Premption: ON" << endl;
    cout << "Time Quantum: " << timeSlice << endl;
    cout << "Number of Processes: " << numProcesses << "\n" << endl;
    //need an extra int to monitor for early finishes not at a timeSlice
    int stopEarly = 0;
    // use a vector instead of a queue here so that we can search through for shortest
    int current = 0;
    vector<Task> sjf;

    while(true){
      //if it is currently any process's arrival time, add it to sjf vector
      for(int i= 0; i < numProcesses; i++){
          if(clockTime == list[i].arrival){
            Task temp = list[i];
            sjf.push_back(temp);
            //list.remove(temp);
          }
      }
      //increment the time counter
      clockTime++;
      sjf[current].remaining--;
      //if a task finishes, erase from sjf vector and update task list information
      //find new shortest job and increment contexts
      if(sjf[current].remaining == 0){
        sjf[current].finish = clockTime;
        sjf[current].response = sjf[current].finish - sjf[current].arrival;
        list[sjf[current].id] = sjf[current];
        sjf.erase(sjf.begin() + current);
        current = 0;
        for(int i = 0; i < sjf.size(); i++){
          if(sjf[current].remaining > sjf[i].remaining){current = i;}
        }
        contexts++;
        stopEarly = clockTime;
      }
      //if we reach a time slice end, find new
      //shortest job if it exists and increment contexts
      else if((clockTime - stopEarly) % timeSlice == 0){
        for(int i = 0; i < sjf.size(); i++){
          if(sjf[current].remaining > sjf[i].remaining){current = i;}
        }
        contexts++;
      }
      else {;}
      //break when there are no more jobs ready
      if(sjf.empty()){break;}
    }
    //calculate footer info for average response time and number of contexts
    int sum = 0;
    for(int i = 0; i < numProcesses; i++){
      sum = list[i].response + sum;
      list[i].printTask();
    }
    cout << "\nAvg Response Time: " << sum/numProcesses << endl;
    cout << "Number of Context Switches: " << contexts << endl;
  }
  // //if it is RR
  else {
    cout << "Scheduling Policy: RR" << endl;
    cout << "Premption: ON" << endl;
    cout << "Time Quantum: " << timeSlice << endl;
    cout << "Number of Processes: " << numProcesses << "\n" << endl;
    //need an extra variable for monitoring early finishes not at a timeSlice
    int stopEarly = 0;
    while(true){

      //if it is currently any process's arrival time, add it to ready queue
      for(int i= 0; i < numProcesses; i++){
          if(clockTime == list[i].arrival){
            Task temp = list[i];
            ready.push(temp);
          }
      }
      //increment the time counter and decrement the remaining time
      clockTime++;
      ready.front().remaining--;
      //if a task finishes, pop it from queue, increment contexts, and update list
      if(ready.front().remaining == 0){
        ready.front().finish = clockTime;
        ready.front().response = ready.front().finish - ready.front().arrival;
        list[ready.front().id] = ready.front();
        ready.pop();
        contexts++;
        stopEarly = clockTime;
      }
      //if we reach a timeSlice, pop task from queue and add it to the back
      else if((clockTime - stopEarly) % timeSlice == 0){
        Task temp = ready.front();
        ready.pop();
        ready.push(temp);
        contexts++;
      }
      else{;}
      //break when there are no more tasks to run
      if(ready.empty()){break;}
    }
    //calculate footer info to show average response time and context switches
    int sum = 0;
    for(int i = 0; i < numProcesses; i++){
      sum = list[i].response + sum;
      list[i].printTask();
    }
    cout << "\nAvg Response Time: " << sum/numProcesses << endl;
    cout << "Number of Context Switches: " << contexts << endl;
  }
  return 0;
}
