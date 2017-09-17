/*
 *Family Name: Wahi
 *Given Name: Gauri
 *Section: Z
 *Student Number: 212489126
 *CSE Login: gauri9
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

struct Similarities
	{
		char words1[1024];
		char words2[1024];
		double similarity;
	};
int compare(const void * x, const void * y)
{
	const struct Similarities *s1 = x; 
	const struct Similarities *s2 = y; 
	double result1 = (s1 -> similarity);
	double result2 = (s2 -> similarity);
	if ( result1 > result2 )
	{
		return -1;
	}
	else if (result1 < result2)
	{
		return 1;
	}
	else
	{
		return 0;
	}
	
}


int main(int argc, char **argv)
{
	
	
	struct Similarities *sim1;
	struct Similarities *sim2;
	
	char *token;
	char temp[1024];
	
	FILE *fp;
	
	int i, j, k, l = 0, m = 0, x;
	
	int r, c;
	int count = 0;
	if(argc <= 1)
	{
		printf("Please enter a file name.\n");
		return -1;
	}
	else
	{
		
			
			fp = fopen(argv[1], "r");
			if(fp == NULL)
			{
				printf("Please enter a valid file\n");
				return 1;
			}
			else
			{
				//make a for loop
			
				//variables
				
				double num, denom, num1, num2, similarity;
				
				char **words;
				words = (char **)malloc(sizeof(char *) * 1024);
				for(i = 0; i < 1024; i++)
				{
					words[i] = (char *)malloc(500);
				}
				
				double **numbers;
				numbers = (double **)malloc(sizeof(double *) * 1024);
				for(i = 0; i < 1024; i++)
				{
					numbers[i] = malloc(sizeof(double) * 105);
				}
				
				sim1 = malloc(sizeof(struct Similarities) * 50000);
				sim2 = malloc(sizeof(struct Similarities) * 50000);
				
				i = 0;
				r = 0;
				c = 0;
				
				while(!feof(fp))
					{
						fscanf(fp, "%s", temp);
						//printf(" %s ", temp);
						
						
						token = temp;
						token = strtok(token, " ");
						//printf(" %s ", token);
						
						if(isalpha(*token))
						{
							//printf(" %s ", token);
						++i;
							++r;
							c = 0;
							strcpy(words[i], token);
							
						}
						else
						{
							//printf(" %s ", token);
							//isWord = 0;
							++c;
							numbers[r][c] = atof(token);
							
						}
						
						
					}//end of while
					
				
					
					//cosine similarity
					for(i = 0; i <= r; i++)
					{
						for(j = i+1; j <= r; j++)
						{
							for(k = 0; k < c; k++)
							{
								num += ((numbers[i][k]) * (numbers[j][k]));
								num1 += ((numbers[i][k]) * (numbers[i][k]));
								num2 += ((numbers[j][k]) * (numbers[j][k]));
							}
							
							denom = (sqrt(num1) * sqrt(num2));
							similarity = num/denom;
							
							if(count == 50000)
							{
								count = 0;
								//sort sim1 in order
								qsort((void *)sim1, 50000, sizeof(struct Similarities), compare);
								for(l = 0; l < 3; l++)
								{
									strcpy(sim2[m].words1, sim1[l].words1);
									strcpy(sim2[m].words2, sim1[l].words2);
									sim2[m].similarity = sim1[l].similarity;
									m++;
									
								}
								//sort sim2 for final output
								qsort((void *)sim2, 1000, sizeof(struct Similarities), compare);
								
							}
							strcpy(sim1[count].words1, words[i]);
							strcpy(sim1[count].words2, words[j]);
							sim1[count].similarity = similarity;
							
							count++;
							num = 0;
							num1 = 0;
							num2 = 0;
							
						}
					}
					
					
					printf("%s", argv[1]);
					for(i = 0; i < 3; i++)
					{
						//printf("Words1: %s, Words2: %s, Sim: %f\n", sim2[i].words1, sim2[i].words2, sim2[i].similarity);
						printf(" %s %s %f ", sim2[i].words1, sim2[i].words2, sim2[i].similarity);
					}
					printf("\n");
					
					fclose(fp);
				}
					
					
					
			
		
	}
	
	
}
 
 
