/*Name: Gauri Wahi
 *Student number: 212489126
 *EECS login: gauri9
 *EECS 3221
 *Section Z*/
 
 
 
 /*Basic idea: CPU picks process from readyQ, then it sets a time slice/timer
  * If process takes longer than the time slice we would have to stop the process in between when
  * the time slice expires and put it at the back of the queue, while the CPU chooses another process*/
  //PROBLEM TO FIX -- AVERAGE WAITING TIME + CPU BURSTS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "sch-helpers.h"

//global variables
process processes[MAX_PROCESSES+1];
int numOfProcess;
int next_process;
int totalWait;
int contextSwitch;
int cpuTime;
int clockTime;
int sumTurnarounds;
int numTempQ;
int quantumTime;
process_queue readyQ; /*This stores the processes in the ready state/queue*/
process_queue waitingQ;/*This stores the processes in the waiting state/queue*/
process *numCPUS[NUMBER_OF_PROCESSORS];/*The number of processes running -- the number of CPUs utilized*/
process *tempQ[MAX_PROCESSES+1];
process *prev; /*This is used to get the last pid*/

/*We have to check the running to waiting states -- so if a running process is now a waiting process this means it is waiting for a signal from the I/O
  so we have to put them in the waiting queue and wait for the action to take place. If the process is incomplete we have to put it in the waiting queue otherwise
  if it's done we can terminate the process and continue with the rest. This would free the CPU up for the other processes. */

void runToWait(void){
	
	
 	int i, value = 0;
 	process *quantum[NUMBER_OF_PROCESSORS];
 	for (i = 0; i < NUMBER_OF_PROCESSORS; i++)
 	{
 		if (numCPUS[i] != NULL)
 		{
 			int current = numCPUS[i]->currentBurst;
 			int step = numCPUS[i]->bursts[current].step;
 			int length = numCPUS[i]->bursts[current].length;
 			if (step == length)
 			{
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
 			
 			else if(numCPUS[i]->quantumRemaining == 0)
 			{
 				quantum[value] = numCPUS[i];//get the value from the CPU
 				quantum[value]->quantumRemaining = quantumTime;//set the quantum time
 				value++;
 				contextSwitch++;
 				numCPUS[i] = NULL;
 			}//end of else if
 		}//end of if	
 	}
 	// sort them by the ID, and then we have to add them into the readyQ
 	qsort(quantum, value, sizeof(process*), compareID);
 	for (i = 0; i < value; i++)
 	{
 		enqueueProcess(&readyQ, quantum[i]);//add the processes into the readyQ
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
 			next->quantumRemaining = quantumTime;//get the quantum time
 			next->endTime = clockTime;
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
 		if(nextReady->startTime == 0)
 		{
 			nextReady->startTime = clockTime;
 			
 		}
 		
 	}
 	
 }
 
 void updateRunning(void)
 {
 	 int i;
 	// update CPU's
 	for (i = 0; i < NUMBER_OF_PROCESSORS; i++){
 		if(numCPUS[i] != NULL){
 			numCPUS[i]->bursts[numCPUS[i]->currentBurst].step++;
 			//decerease the remaining quantum time
 			numCPUS[i]->quantumRemaining = numCPUS[i]->quantumRemaining - 1;
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




int main(int argc, char *argv[]){
	int i;
	int status = 0;
	float used, waitingTime, turnAround;
	int lastPID;
	int timeTaken;
	//char qTime = argv[1];
	//now change it into an integer
	quantumTime = atoi(argv[1]);

	//check if the user inputted the right amount of arguments aka the quantum time
	if(argc < 2)
	{
		printf("Error: Please enter quantum time for calculation!\n");
		exit(1);
	}

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
	
	qsort(processes, numOfProcess, sizeof(process), compareByArrival);

	while (1)
	{
		//again, we have to check for the incomming processes and add them into the ready queue
		while(next_process < numOfProcess && processes[next_process].arrivalTime <= clockTime)
		{
			tempQ[numTempQ] = &processes[next_process];
			tempQ[numTempQ]->quantumRemaining = quantumTime;
			numTempQ++;
			next_process++;
		}
		//readyToRun();
		//you must order these properly otherwise there is a variation in your answer
		runToWait();
		waitToReady();
		readyToRun();
		
		
		//update all the states
		updateWaiting();
		updateReady();
		updateRunning();

		cpuTime += runningProcesses(); //the cpu time = the time for all of the running processes to end
		int incomming = numOfProcess - next_process;
		//to break out of the loop there should not be a waiting state, there should not be any incoming processes nor should there be any running processes
		//in the system (if any are missing there's a problem aka segmentation fault)
		if (runningProcesses() == 0 && incomming == 0 && waitingQ.size == 0){
			break;
		}

		//increment the clock time for the final system -- this is for the final time which we can also use prev->endTime -- prev not giving same result
		clockTime++;
		
		//TESTER -- printf("%s", readLine()); -- does not work
	}

	// calculations
	for(i = 0; i < numOfProcess; i++)
	{
		//the turnaround time is the time taken when you have submitted the process and the time it took till the completion hence why we need the 
		//endTime - arrival time
		sumTurnarounds += processes[i].endTime - processes[i].arrivalTime;
		
		//waiting time is the sum of all the time each process has waited out
		totalWait += processes[i].waitingTime;

	}
	lastPID = prev->pid;
	timeTaken = prev->endTime;
	waitingTime = totalWait/(double)numOfProcess;
	turnAround = sumTurnarounds/(double)numOfProcess;
	//multiply with 100 so we get the percentage
	used = (100.0 * cpuTime/clockTime);
	//TESTER -- printf("waitingtime = %.2f\n", waitingTime);
	//TESTER -- printf("turnAround = %.2f\n", turnAround);
	//TESTER -- printf("ClockTime = %d\n", clockTime);
	//this function is in sch-helpers which I have added there. 
	int qTime = quantumTime;
	//printf("QTime = %d\n", qTime);
	printResult(waitingTime, turnAround, timeTaken, used, contextSwitch, lastPID, qTime);
	return 0;
}
