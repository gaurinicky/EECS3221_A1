/*
 *Family Name: Wahi
 *Given Name: Gauri
 *Section: Z
 *Student Number: 212489126
 *CSE Login: gauri9
 */
 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>

char *findSimilarity(char *);
int compare(const void *, const void *);

struct Similarities
{
	char words1[500];
	char words2[500];
	double similarity;
};

//global variables -- so the function, find Similarity can return a global variable


char *findSimilarity(char *fileName){
	
	 
	int row = 1005, cols = 105; //We had to get the accurate number of rows and columns in the files to go through it
	int i = 0, j = 0, k = 0,l = 0, m = 0, index = 0; 
	char temp[100];
	//variables required for calculating the cosine similarity
	double num, denom, num1, num2, similarity;
	
	struct Similarities *sim1;
	struct Similarities *sim2;
	char *endResult = (char *)malloc(1000);
	char *token;
	FILE *fp;
	//open the file
	fp = fopen(fileName, "r");
	//check if the file is a valid file.
	if(fp == NULL)
	{
		printf("Please enter a valid file. File %s is not valid!\n", fileName);    
		exit(0);
	}
	
				/**MEMORY ALLOCATION**/ 
					char **words=malloc(sizeof(char *) * 1005); 
	
	
					for(i=0; i< 1005; i++)
					{
						words[i]=(char *)malloc(500);        
		  
					}
					
					double **numbers =malloc(1005 *sizeof(double *)); 
					
					
					for(i=0; i < 1005; i++)
					{
						numbers[i]= malloc(105 *sizeof(double)); 
						
					}
					
					sim1 = malloc(50000 * sizeof(struct Similarities));
					sim2 = malloc(50000 * sizeof(struct Similarities));
					
	
	
					i=0, k=0, cols = 0, row = 0; 
					while(!feof(fp))
					{
						fscanf(fp, "%s ", temp);
						token = temp;
						token = strtok(token, " ");
						
							//check whether the token separated from the system is actually a word
							if(isalpha(*token))
							{
								
								 
								++row; 
								cols = 0;
								strcpy(words[++i], token);
								
							}
							//if it is not a word, then it is a float, which we should put in the array numbers
							else 
							{
								
								numbers[row][++cols]=atof(token);
							}
							
						}
	
	
	
	//Cosine Similarity  
	/*The basic concept is to compare each value with one another. The numbers are in another array, and the words are separately stored
	 *So, to find the similarities between two words, we must incremement through both arrays together and return the two words compared
	 *as well as the similarity between their vectors. The equation is given, we just have to go through the array of numbers, and    
	 * calculate them.
	 */
	
	for(i=0; i <= row; i++)
	{
		
		for(j=i+1; j <= row; j++)
		{
			for( k=0; k <= cols; k++)
			{
				
				num +=  (numbers[i][k]) * (numbers[j][k]) ;
				num1 +=  ((numbers[i][k]) * (numbers[i][k]));
				num2 +=  ((numbers[j][k]) * (numbers[j][k]));            
				
			}
			
			
			denom = (sqrt(num1))*(sqrt(num2));
			similarity = num/denom;
			
			
			if(index == 50000){                        
				index = 0; 
				/*Here we have to sort both structs sim1 and sim2. Sim1 will go first and we will find the top three
				 *similarities, which then we will store in sim2. Then, once again we would have to sort our final array
				 *and submit the final product
				 */
				qsort((void *)(sim1), 50000, sizeof(struct Similarities), compare);
				for (m = 0; m < 3; m++){
					strcpy(sim2[l].words1, sim1[m].words1); 
					strcpy(sim2[l].words2, sim1[m].words2); 
					sim2[l].similarity = sim1[m].similarity;
					l++;
				}
				qsort((void *)sim2, 1000, sizeof(struct Similarities), compare);
			}
			
			strcpy(sim1[index].words1, words[i]); 
			strcpy(sim1[index].words2, words[j]); 
			sim1[index].similarity = similarity;
			index++;
			num1=0, num2=0, num=0;                         
		}
	}
	
	/* To get the final result, we have to store all of the TOP THREE similarities, and store them as ONE INDIVIDUAL STRING. Then
	 * we will return the string to main to be executed
	 */
	
	snprintf(endResult, 60000,  "%s %s %s %f %s %s %f %s %s %f\n", fileName, sim2[0].words1, sim2[0].words2, sim2[0].similarity, 
						sim2[1].words1, sim2[1].words2, sim2[1].similarity, sim2[2].words1, sim2[2].words2, sim2[2].similarity);
					
	
	
	/*reallocate the memory here, so it can be used again for the other files*/
	free(words); 
	free(numbers); 
	free(sim1);
	free(sim2);
	fclose(fp);  
	return endResult;
	
}

int main(int argc, char *argv[]){
   int counter; 
    int i,fd[argc][2];
    int pid, pidnext;
    char endR[argc][1024];
    char *result = (char *)malloc(sizeof(char));
    char *buffer = (char *)malloc(sizeof(char));
    
    /*check if the correct number of arguments have been stored*/
    if (argc <= 1){
        printf("Please enter at least one valid file!\n", argv[0]);
    }
    
    else{
    	    int counter = 1;
    	    i = 0;
    	    //for(i=0; counter< argc; i++){
    	    while(counter < argc)
    	    {
    		
			//initialize a pipe
			pipe(fd[i]);
			pid = fork(); 
			
			if(pid >= 0)
			{
				//either parent or child process 
				if(pid == 0)
				{ 
				
					//child process
					close(fd[i][0]);
					result = findSimilarity(argv[counter]);
					strcpy(buffer, result);	
					write(fd[i][1], buffer, strlen(buffer) * sizeof(char));
					close(fd[i][1]);
					exit(0);
				}
			}
			
			counter++;
			i++;
    		
    		
    	    }
    	
    	    //PARENT PROCESS = PRINT OUT THE FINAL RESULT AFTER READING IT FROM THE CHILDREN PROCESS
    	
    	for(i = 0; i <= argc-2; i++){
    		//wait for all the children process to be complete
    		wait(NULL);
    		
    		//then close the process of writing
    		close(fd[i][1]);
    		//read from children
    		read(fd[i][0], endR[i], sizeof(char)*(1024));
    		
    		//print the result
    		printf("%s",endR[i]);
    		//close reading and terminate
    		close(fd[i][0]);
    	}
    	
    }
    return 0;
}


//compare method for the similarities -- similar to comparator in java
int compare(const void *a, const void *b)
{
	const struct Similarities *sim1 = a;
	const struct Similarities *sim2 = b;
	double result1 = (sim1 -> similarity);
	double result2 = (sim2 -> similarity);
	
	if(result1 > result2)
	{
		return -1;
	}
	else if(result1 < result2)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


