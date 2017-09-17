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

void *findSimilarity(char *);
int compareSim(const void *, const void *);

struct Similarities
{
	char word[1024];
	char next_word[1024];
	double similarity;
};

//global variables -- so the function findSimilarity can return the address of the final string we are outputting, instead of
//initializing it inside the function (this is so it does not return the address of a local variable)
char endResult[100];

void *findSimilarity(char *fileName){
	
	 
	int row = 1005, cols = 105; //Had to count the number of words in the file, and give an approximate estimate
	int i=0, j=0, k=0,l=0, m=0, isWord, index = 0; 
	char temp[100];
	//variables required for calculating the cosine similarity
	double num, denom, num1, num2, similarity;
	
	struct Similarities *sim1;
	struct Similarities *sim2;
	
	char *token, *token2;
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
					char **words=malloc(sizeof(char *) * 1024); 
	
	
					for(i = 0; i< 1005; i++)
					{
						words[i]=(char *)malloc(500);        
		  
					}
					
					double **numbers =malloc(1005 *sizeof(double *)); 
					
					
					for(i = 0; i < 1005; i++)
					{
						numbers[i]= malloc(105 *sizeof(double)); 
						
					}
					
					sim1 = malloc(50000 * sizeof(struct Similarities));
					sim2 = malloc(50000 * sizeof(struct Similarities));
					
	
					i = 0;
					cols = 0; //position of column
					row = -1; //position of row
					
			/*For reading the input I had to change the way the input was read, so instead of using tokens, like I did for rvw.c and process.c 
			 * I now had to use a way to go through the array temp, and find the end of each word, which is the '\0' character. Once that was found
			 * I have to then check if the input read is a word or if it was a float, and designate it to the right spot. Flags (isWord) is used so
			 * when adding them to the array there is no "Segmentation Fault" error, which was the case when isWord was not used. The indexes (i, c, r) where
			 * c and r is columns, and rows respectively have to start from "-1" because of the way we are incrementing it before adding it so they
			 * begin from 0*/
			 
				while(!feof(fp))
				{
					fscanf(fp, "%s ", temp);
					
					while(temp[index] != '\0' && !isspace(temp[index]))
					{
			
						if(isalpha(temp[index]) && !isspace(temp[index])) //this checks if we are checking at least one word
						{
							isWord = 1; //true -- it is a word
							//TESTER -- printf(" %s ", temp);
							
						}
						else 
						{
							isWord = 0; //false -- it is a float
						}
						index++;
					}
			
					if(isWord == 1) //if true
					{    
						
						strcpy(words[i++], temp);
						row++; 
						cols = 0;
					}
					else if (isWord == 0) //if false
					{
						numbers[row][cols++] = atof(temp);
					}
					
					index=0;
					
					
					
					
				}//end of while
				
				index=0;
	
	
	
	//Cosine Similarity  
	/*The basic concept is to compare each value with one another. The numbers are in another array, and the words are separately stored
	 *So, to find the similarities between two words, we must incremement through both arrays together and return the two words compared
	 *as well as the similarity between their vectors. The equation is given, we just have to go through the array of numbers, and    
	 * calculate them.
	 */
	
	 			for(i = 0; i <= row; i++)
					{
						for(j = i+1; j <= row; j++)
						{
							for(k = 0; k < cols; k++)
							{
								num += ((numbers[i][k]) * (numbers[j][k]));
								num1 += ((numbers[i][k]) * (numbers[i][k]));
								num2 += ((numbers[j][k]) * (numbers[j][k]));
							}
							
							denom = (sqrt(num1) * sqrt(num2));
							similarity = num/denom;
							
							if(index == 50000)
							{
								index = 0;
								//sort sim1 in order
								qsort((void *)sim1, 50000, sizeof(struct Similarities), compareSim);
								for(l = 0; l < 3; l++)
								{
									strcpy(sim2[m].word, sim1[l].word);
									strcpy(sim2[m].next_word, sim1[l].next_word);
									sim2[m].similarity = sim1[l].similarity;
									m++;
									
								}
								//sort sim2 for final output
								qsort((void *)sim2, 1000, sizeof(struct Similarities), compareSim);
								
							}
							strcpy(sim1[index].word, words[i]);
							strcpy(sim1[index].next_word, words[j]);
							sim1[index].similarity = similarity;
							
							index++;
							num = 0;
							num1 = 0;
							num2 = 0;
							
						}
					}
	
	/* To get the final result, we have to store all of the TOP THREE similarities, and store them as ONE INDIVIDUAL STRING. Then
	 * we will return the string to main to be executed
	 */
	
	snprintf(endResult,60000,  "%s %s %s %f %s %s %f %s %s %f\n", fileName, sim2[0].word, sim2[0].next_word, sim2[0].similarity, 
						sim2[1].word, sim2[1].next_word, sim2[1].similarity, sim2[2].word, sim2[2].next_word, sim2[2].similarity);
					
	printf("%s", endResult);
	
	/*reallocate the memory here, so it can be used again for the other files*/
	free(words); 
	free(numbers); 
	free(sim1);
	free(sim2);
	fclose(fp); 
	
	//return endResult;
	
}

int main(int argc, char *argv[]){
	
	if(argc <= 1)
	{
		printf("Please enter a valid file!\n");
		exit(1);
	}
	
	/*IF THERE ARE CORRECT NUMBER OF ARGUMENTS IN THE COMMAND LINE, DO THE MULTITHREAD PROCESS*/
		/*Declare the threads*/
		pthread_t tid[argc-1];
		pthread_attr_t tAttr;
		pthread_attr_init(&tAttr);
		
		//index
		
		int count = 1;
		
		while(count < argc)
		{
			char *filename = (char*)malloc(sizeof(char));
			filename = argv[count];
			pthread_create(&tid[count], &tAttr, (void*) findSimilarity, (void*) filename);
			
			//join the threads so they all come out together in the same order
			pthread_join(tid[count], NULL);
			
			count++;
			
		}
	
		
	return 0;
 
}



//THIS IS THE COMPARE METHOD WE NEED TO USE TO COMPARE THE SIMILARITIES -- similar to the comparator in java
int compareSim(const void *a, const void *b)
{
	const struct Similarities *sim1 = a;
	const struct Similarities *sim2 = b;
	//get the similarity of both the structs
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
		return 0; //this implies both similarities are equal
	}
}


