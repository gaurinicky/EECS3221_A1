/*Name: Gauri Wahi
 *Student number: 212489126
 *EECS login: gauri9
 *EECS 3221
 *Section Z*/
 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "sch-helpers.h"

//global variables

int numOfProcess;
int next_process;
int numWaiting;
int contextSwitch;
int cpuTime;
int clockTime;
int sumTurnarounds;
int numTempQ;
process processes[MAX_PROCESSES+1];
process_queue readyQ; /*This stores the processes in the ready state/queue*/
process_queue waitingQ;/*This stores the processes in the waiting state/queue*/
process *numCPUS[NUMBER_OF_PROCESSORS];/*The number of processes running -- the number of CPUs utilized*/
process *tempQ[MAX_PROCESSES+1];
process *prev; /*This is used to get the last pid*/



/*We have to check the running to waiting states -- so if a running process is now a waiting process this means it is waiting for a signal from the I/O
  so we have to put them in the waiting queue and wait for the action to take place. If the process is incomplete we have to put it in the waiting queue otherwise
  if it's done we can terminate the process and continue with the rest. This would free the CPU up for the other processes. */

void runToWait(void)
{
 	int i;
 	for (i = 0; i < NUMBER_OF_PROCESSORS; i++){
 		if (numCPUS[i] != NULL){
 			
 			int current = numCPUS[i]->currentBurst;
 			int step = numCPUS[i]->bursts[current].step;
 			int length = numCPUS[i]->bursts[current].length;
 			if (step == length){
 				numCPUS[i]->currentBurst++;
 				
 				
 				if (numCPUS[i]->currentBurst < numCPUS[i]->numberOfBursts)
 				{
 					enqueueProcess(&waitingQ, numCPUS[i]);
 				}
 				else
 				{
 					numCPUS[i]->endTime = clockTime;
 					prev = numCPUS[i];
 					prev->endTime = clockTime;
 				}
 				
 				
 				
 				numCPUS[i] = NULL;
 			}
 		}	
 	}
 }
 
 /*This state is when processes from the ready queue are running so we would have to remove the process from the ready queue, and we have to set
   the queue in such a way that the next free CPU (when the CPU is idle) will have another process to run*/
 
void *readyToRun(void)
{
 	int i;
 	//sort
 	qsort(tempQ, numTempQ, sizeof(process*), compareID);
 	
 	
 	
 	for (i = 0; i < numTempQ; i++)
 	{
 		//TESTER -- printf("%d \n", tempQ[i]);
 		enqueueProcess(&readyQ, tempQ[i]);
 	}
	numTempQ = 0;
	
	
 	for (i = 0; i < NUMBER_OF_PROCESSORS; i++)
 	{
 		if (numCPUS[i] == NULL)
 		{
 			//numCPUS[i] = nextOne();
 			if(readyQ.size == 0)
 			{
 				return NULL;
 			}
 			
 			process *next = readyQ.front->data;
 			dequeueProcess(&readyQ);
 			numCPUS[i] = next;
 		}
 	}
 }
 
 
 /*We have to check the states of the processes in the waiting queue, so if the process has received the signal from their respective I/O burst then we would have to
   remove them waiting queue and add them to the ready queue, as well as updating the waiting queue to the next process. */
 void waitToReady(void){
 	int i;
 	int waitingQueueSize = waitingQ.size;
 	
 	
 	for(i = 0; i < waitingQueueSize; i++)
 	{
 		process *next = waitingQ.front->data;
 		dequeueProcess(&waitingQ);
 		
 		int current = next->currentBurst;
 		int step = next->bursts[current].step;
 		int length = next->bursts[current].length;
 		//TESTER printf("Step = %d and Length = %d\n", step, length);
 		
 		if(step == length)
 		{
 			next->currentBurst++;
 			tempQ[numTempQ++] = next;
 		}
 		else
 		{
 			enqueueProcess(&waitingQ, next);
 		}
 	}
 }
 
 /***FUNCTIONS TO UPDATE THE STATES******/
 //these functions update the number of processes in each state and keeps track of them
 
  void updateReady(void){
 	int i;
 	//update everything in the readyQ
 	for (i = 0; i < readyQ.size; i++){
 		process *nextReady = readyQ.front->data;
 		dequeueProcess(&readyQ);
 		nextReady->waitingTime++;
 		enqueueProcess(&readyQ, nextReady);
 	}
 	
 }
 
 void updateRunning(void)
 {
 	 int i;
 	// update CPU's
 	for (i = 0; i < NUMBER_OF_PROCESSORS; i++){
 		if(numCPUS[i] != NULL){
 			numCPUS[i]->bursts[numCPUS[i]->currentBurst].step++;
 		}
 	} 
 }
 
 void updateWaiting(void)
 {
 	 int i;
 	 int waitSize = waitingQ.size;
 	 
 	 for(i = 0; i < waitSize; i++)
 	 {
 	 	 process *next = waitingQ.front->data;
 	 	 
 	 	 dequeueProcess(&waitingQ);
 	 	 next->bursts[next->currentBurst].step++;
 	 	 enqueueProcess(&waitingQ, next);
 	 }
 }

 
 // THIS COUTNS THE NUMBER OF RUNNING PROCESSES
int runningProcesses(void)
{
	int runningProcesses = 0;
	int i;
	for (i = 0; i < NUMBER_OF_PROCESSORS; i++)
	{
		if (numCPUS[i] != NULL)
		{
			runningProcesses++;
			//TESTER printf(" %d ", runningProcesses);
		}
	}
	return runningProcesses;
}


int main(int argc, char **argv){
	//fields/variables
	int i;
	int status = 0;
	float used, waitingTime, turnAround;
	int lastPID;
	int timeTaken;
	

	//initialize queues
	initializeProcessQueue(&readyQ);
	initializeProcessQueue(&waitingQ);
	
	
	//read in all the processes. 
	while(status = readProcess(&processes[numOfProcess]))
	{
		//TESTER printf("%d \n", statusOfProcess);
		if(status == 1)
		{
			//TESTER -- printf("%d\n", numOfProcess);
			//TESTER -- printf("Max = %d\n", MAX_PROCESSES);
			numOfProcess++;
		}
		
		
	}
	
	//sort all of the processes by arrival time -- if we don't hve this the average waiting time is longer sizeof(process*) is taking l
	qsort(processes, numOfProcess, sizeof(process), compareByArrival);
	
	
	//main loop
	while(1)
	{
		//again, we have to check for the incomming processes and add them into the ready queue
		while(next_process < numOfProcess && processes[next_process].arrivalTime <= clockTime){
			//numTempQ++;
			//next_process++;
			tempQ[numTempQ++] = &processes[next_process++];
		}
		
		
		readyToRun();
		runToWait();
		waitToReady();
		
		//update all the states
		updateReady();
		updateWaiting();
		updateRunning();
		
		
		cpuTime += runningProcesses(); //the cpu time = the time for all of the running processes to end
		
		int incomming = numOfProcess - next_process;
		//to break out of the loop there should not be a waiting state, there should not be any incoming processes nor should there be any running processes
		//in the system
		if(waitingQ.size == 0 && incomming == 0 && runningProcesses() == 0)
		{
			break;
		}
		
		//increment the clock time for the final system -- this is for the final time which we can also use prev->endTime -- prev not giving same result
		clockTime++;
		
		//TESTER -- printf("%s", readLine()); -- does not work
		
		
	}
	
	
	/*****CALCULATIONS*******/
	for(i = 0; i < numOfProcess; i++){
		
		//the turnaround time is the time taken when you have submitted the process and the time it took till the completion hence why we need the 
		//endTime - arrival time
		sumTurnarounds += processes[i].endTime - processes[i].arrivalTime;
		
		//waiting time is the sum of all the time each process has waited out
		numWaiting += processes[i].waitingTime;

	}
	
	lastPID = prev->pid; //numCPUS[numOfProcess-1]->pid; //
	timeTaken = prev->endTime;
	
	waitingTime = numWaiting/(double)numOfProcess;
	turnAround = sumTurnarounds/(double)numOfProcess;
	//multiply with 100 so we get the percentage
	used = (100.0 * cpuTime/clockTime);
	//TESTER -- printf("waitingtime = %.2f\n", waitingTime);
	//TESTER -- printf("turnAround = %.2f\n", turnAround);
	//TESTER -- printf("ClockTime = %d\n", clockTime);
	//this function is in sch-helpers which I have added there. 
	printResult(waitingTime, turnAround, timeTaken, used, contextSwitch, lastPID);
	
	
	
	return 0;

}



