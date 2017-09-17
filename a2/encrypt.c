
/*
Family Name: Wahi
Given Name: Gauri
Section: Z
Student Number: 212489126
CS Login: gauri9
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define TEN_MILLIS_IN_NANOS 10000000


// struct provided by the professor
typedef struct{
	char data;
	off_t offset;
	char state;
} BufferItem;


BufferItem *item;
int seed = 0;
struct timespec t;


//allows the threads to go to sleep for a specific amount of time
//t.tv_sec = 0;
//t.tv_nsec = rand_r(&seed)%(TEN_MILLIS_IN_NANOS+1);
//nanosleep(&t, NULL);

//The user will enter a given file, we have to iterate through it and get each character converting it with the given key
//and the given formula, then take that character, save it into the buffer and transfer it over to the output file specified
//by the user

//global variables we require
FILE *file_in;
FILE *file_out;

int buffSize;


//we have specified buffers that will tell us if it is empty, needs to be worked on, or is ready to be outputted
//this can specified with the characters e, w, and o respectively. 
//first we need to get the empty buffers
int empty_buffer()
{
	int i;
	//get the current state of the BufferItem
	for(i = 0; i < buffSize; i++)
	{
		char state = item[i].state;
		if(state == 'e')
		{
			//this means it is empty, so we return a boolean value
			//specifying that there does exist an empty buffer
			//in this case we can return i
			return i;
		}
	}
	
	
	
	//if it doesn't contain an empty buffer we return -1
	return -1;
}

//another buffer is for the working buffers, this will get a buffer with the state w, and then it will
//do work on it, and get the next buffer to be worked on
int get_work_buffer()
{
	int i;
	
	for(i = 0; i < buffSize; i++)
	{
		char state = item[i].state;
		if(state == 'w')
		{
			return i;
		}
	}
	
	return -1;
}

//once you have worked upon the character and it is encrypted/decrypted you have to output it into the
//output file. the only way the program knows it is ready to be outputted is if the state
//is signalled to be 'o'

//in this case we need mutual exclusion to do the threads
//we would need three threads
//one for IN, one for OUT, and one for WORK
pthread_mutex_t mutexIN;
pthread_mutex_t mutexOUT;
pthread_mutex_t mutexWORK;


//we would need a way to know which threads are active and which threads are working
int activeInputThreads;
int activeWorkThreads;

int out_buffer()
{
	int i = 0;
	while(i < buffSize)
	{
		char state = item[i].state;
		if(state == 'o')
		{
			//the item is ready to be outputted
			return i;
		}
		i++;
	}
	
	//else
	return -1;
}


//the input thread -  IN threads should not have one big critical section where they do all of the following together:
//(a) read from the file; (b) write to the buffer.  
//Instead, they should have 2 separate critical sections, one for each of (a) and (b)

void *thread_in(void *arg)
{
	int i;
	char d; //this is the character we are taking from file_in
	off_t offset;

	
	//each IN thread goes to sleep for a random time upon being created
	nanosleep(&t, NULL);
	
	
	do 
	{
		//first critical section
		//read from file
		//set i
		i = empty_buffer();
		
		
		//we have to add this into the "work" lock
		pthread_mutex_lock(&mutexWORK);		

		while(i > -1)
		{

			//here we will read from the file
			//and then add it to the buffer -- critical section
			pthread_mutex_lock(&mutexIN);
			offset = ftell(file_in);
			//get the chracter
			d = fgetc(file_in);
			pthread_mutex_unlock(&mutexIN);
			
			//to get out of the loop we have to reach the end of file
			if (d == EOF || d == '\0')
			{
				break;
			}

			//once we have retrieved the character we can change the state for
			//that buffer to 'w' and then get the next empty item
			item[i].offset = offset;
			item[i].state = 'w';
			item[i].data = d;
			i = empty_buffer();
			
		}

		pthread_mutex_unlock(&mutexWORK);

	} while (!feof(file_in));

	pthread_mutex_lock(&mutexWORK);
	activeInputThreads--;
	pthread_mutex_unlock(&mutexWORK);
	pthread_exit(0);
}

//the work thread will do the encrypting/decrypting depending upon the key the user has given
//the forumla we need to use is:
// if(data>31 && data<127 )
         //data = (((int)data-32)+2*95+KEY)%95+32 ;
         
//to decrypt the data the formula is:
//   if (data>31 && data<127 )
//        data = (((int)data-32)+2*95-KEY)%95+32 ;


void *thread_work(void *arg)
{
	//get the key from the user
	int key = atoi(arg);
	int i;
	char d;
	int localActiveInput;
	
	//the work thread goes to sleep for a random amount of time 
	nanosleep(&t, NULL);
	
	
	while(localActiveInput > 0 || i > -1)
	{
		//set i;
		i = get_work_buffer();
		pthread_mutex_lock(&mutexWORK);
		
		if(i > -1)
		{
			//set the character to d which we have gotten from the input
			//this is a critical section
			d = item[i].data;
			
			//now from the key have obtained from the user we have to either
			//encrypt it or decrypt it
			//we will encrypt the key if it is positive
			if(key > 0 && d > 31 && d < 127)
			{
				d = (((int)d-32)+2*95+key)%95+32;
			}
			
			//otherwise if the key is negative we are decrypting the code
			else if(key < 0 && d > 31 && d < 127)
			{
				d = (((int)d-32)+2*95-(key*-1))%95+32;
			}
			
			//CRITICAL SECTION
			//write the character back to the buffer and set the state to o
			
			item[i].data = d;
			item[i].state = 'o';
		}
		
		localActiveInput = activeInputThreads;
		pthread_mutex_unlock(&mutexWORK);
	}
	
	pthread_mutex_lock(&mutexWORK);
	activeWorkThreads--;
	pthread_mutex_unlock(&mutexWORK);
	pthread_exit(0);
}

//once we have encrypted or decrypted the code, we can then get that character from the buffer
//check if the state is o
//and then we can write it into the output file

void *thread_output()
{
	int i;
	char d;
	off_t offset;
	int localActiveWork;
	
	//the output thread goes to sleep for a limited amount of time
	nanosleep(&t, NULL);
	
	
	while(i > -1 || localActiveWork > 0)
	{
		//get the output buffer
		i = out_buffer();
		pthread_mutex_lock(&mutexWORK);
		
		if(i > -1)
		{
			//get the character you want to output
			d = item[i].data;
			offset = item[i].offset; //the position you want to put the character to
			
			//CRITICAL SECTION
			//writing to output file
			pthread_mutex_lock(&mutexOUT);
			if(fseek(file_out, offset, SEEK_SET) == -1)
			{
				fprintf(stderr, "error setting output file position to %u\n", (unsigned int)offset);
				exit(-1);
			}
			
			if(fputc(d, file_out) == EOF)
			{
				fprintf(stderr, "error writing byte %d to output file\n", d);
				exit(-1);
			}
			
			pthread_mutex_unlock(&mutexOUT);
			
			//so we have written the character to the output file, this means
			//the buffer we had occupied can now be empty and replaced for another 
			//character
			item[i].data = '\0';
			item[i].state = 'e';
			item[i].offset = 0;
		}
		
		localActiveWork = activeWorkThreads;
		pthread_mutex_unlock(&mutexWORK);
	}
	
	pthread_exit(0);
}


int main(int argc, char **argv)
{
	//format is: encrypt <KEY> <nIN> <nWORK> <nOUT> <file_in> <file_out> <bufSize>
	int key = atoi(argv[1]);
	int nIN = atoi(argv[2]);
	int nWORK = atoi(argv[3]);
	int nOUT = atoi(argv[4]);
	
	file_in = fopen(argv[5], "r");
	file_out = fopen(argv[6], "w");
	buffSize = atoi(argv[7]);
	
	activeInputThreads = nIN;
	activeWorkThreads = nWORK;
	int i;
	
	//error checking
	//the user must have at least 8 inputs
	if(argc < 8)
	{
		printf("Error: You have not a valid number of arguments. The correct format is - encrypt <KEY> <nIN> <nWORK> <nOUT> <file_in> <file_out> <bufSize>\n");
		exit(1);
	}
	
	//we must now check if the other inputs are valid and in their specified range
	if(key < -127 || key > 127)
	{
		printf("Error: The key is not in its valid range. It must be in between -127 and 127\n");
		exit(1);
	}
	
	//nIN, nWORK, nOUT, buffSize must be at least 1
	if(nIN < 1 || nWORK < 1 || nOUT < 1 || buffSize < 1)
	{
		printf("Error: Please recheck your arguments and make sure they are within the specified range!\n");
		exit(1);
	}
	
	
	//here we will create the threads
	pthread_t threadIN[nIN];
	pthread_t threadOUT[nOUT];
	pthread_t threadWORK[nWORK];
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	
	//initialize the locks
	pthread_mutex_init(&mutexIN, NULL);
	pthread_mutex_init(&mutexWORK, NULL);
	pthread_mutex_init(&mutexOUT, NULL);
	
	//for the BufferedItem we need to allocate memory
	item = (BufferItem*)malloc(sizeof(BufferItem)*buffSize);
	//initialize the buffer
	for(i = 0; i < buffSize; i++)
	{
		item[i].state = 'e';
	}
	
	
	//nanosleep
	t.tv_sec = 0;
	t.tv_nsec = rand_r((unsigned int*)&seed)%(TEN_MILLIS_IN_NANOS+1);
	
	//create the threads and then join them
	
	for(i = 0; i < nIN; i++)
	{
		pthread_create(&threadIN[i], &attr, thread_in, file_in);
	}

	
	//nWORK
	for(i = 0; i < nWORK; i++)
	{
		pthread_create(&threadWORK[i], &attr, thread_work, argv[1]);
	}

	//nOUT
	for(i = 0; i < nOUT; i++)
	{
		pthread_create(&threadOUT[i], &attr, thread_output, file_out);
	}
	
		
	//join the IN threads
	for(i = 0; i < nIN; i++)
	{
		pthread_join(threadIN[i], NULL);
	}
	
	for(i = 0; i < nWORK; i++)
	{
		pthread_join(threadWORK[i], NULL);
	}
	
	for(i = 0; i < nOUT; i++)
	{
		pthread_join(threadOUT[i], NULL);
	}
	
	
	//close the files
	fclose(file_in);
	fclose(file_out);
	//free the BufferItem
	free(item);
	
	return 0;
	
}
