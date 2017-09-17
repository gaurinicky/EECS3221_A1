/*
 *Family Name: Wahi
 *Given Name: Gauri
 *Section: Z
 *Student Number: 212489126
 *CSE Login: gauri9
 */

 float cosineSimilarity(float one[], float two[], int length);
 #include <stdio.h>
 #include <string.h>
 #include <stdlib.h>
 #include <ctype.h>
 #include <stddef.h>
 #include <math.h>
 

 
 
int main(int argc, char **argv)
{
 
	typedef struct
	{
		char words[1024];
		char numbers[1024];
	} fileReader;
	
	fileReader record[1024];
	char line[1024];
	FILE *fp;
	
	float arr[1024][1024];
	
	char *item;
	float something;
	int count = 0;
	int k, j, i;
	float temp[1024], temp2[1024];
	int x=0, y=0, a=0, b=0;
	
	//double result, num = 0, denom = 0, x=0;
	float result, result2;
	//int count_words = 2;
	
	//open the file
	fp = fopen(argv[1], "r");
	
	while(fgets(line, 1024, fp) != NULL)
	{
		// prints out everything in the file 
		//printf("%s", line);
		//item = line;
		item = strtok(line, " ");
		
		//printf("Token = %s\n", item); //prints out only the words, now put this into an array
		strcpy(record[count].words, item);
		
		//printf("Token = %s\n", item);
		item = strtok(NULL, " ");
		//something = atof(item);
		strcpy(record[count].numbers, item);
		
		//printf("Numbers = %f\n", something);
		//record[count].numbers = something;
		//strcpy(record[count]->numbers, atof(item));
		 
		 
		 count++;
		 
		
		
	}
	
	for(k = 0; k < (count-1); k++)
	{
		//printf(" %s ", record[k].numbers);
		for(j = 0; j < 100; j++)
		{
			something = atof(record[k].numbers);
			arr[k][j] = something;
			
			
		}
		
		
		//printf(" %f ", *arr[k]);
	}
	
	for(i = 0; i < (count); i++)
	{
		for(j = 0; j < (count-1); j++)
		{
			temp[i] = arr[i][j];
			temp2[i+1] = arr[i+1][j];
			
			 result = cosineSimilarity(&temp[i], &temp2[i+1], 100);
			 int x = i;
			 int y = i+1;
			
			
		}
	}
	
	for(i = 2; i < count; i++)
	{
		for(j = 0; j < count-1; j++)
		{
			temp[i] = arr[i][j];
			temp2[i+1] = arr[i+1][j];
			
			result2 = cosineSimilarity(&temp[i], &temp2[i+1], 100);
			
			a = i;
			b = i + 1;
		}
	}
	
	if(result > result2)
			{
				printf("%s %s %f", record[x].words, record[y].words, result);
			}
			else
			{
				printf("%s %s %f", record[a].words, record[b].words, result2);
			}
	
	

	
	fclose(fp);
	return 0;
	
			

	
	
}
 
//the function to calculate cosine similarity
float cosineSimilarity(float one[], float two[], int length)
{
	double dotProduct = 0.0; //initialize the value for all
	double denominator = 0.0; 
	double denom2 = 0.0;
	int i;
	
	for(i = 0; i < length; i++)
	{
			
		
		dotProduct = dotProduct + (one[i] * two[i]); // the dot product in the numerator
		denominator = denominator + (one[i] * one[i]); //the first square sum in the denominator
		denom2 = denom2 + (two[i] * two[i]); //the second square sum in the denominator
		
	}
	
	//return numerator/denominator
	return dotProduct/(sqrt(denominator) * sqrt(denom2));
}



