#include <stdio.h>
#include <stdlib.h>
 
int main ()
{
	FILE *fp;
	char fileName[100];
	char output[6];
	int i;
	int randomServiceTime;
	int randomArrivalTime;
	printf("Enter a file name: ");//get fileName from user
	gets (fileName);
	srand(time(NULL));
	/* open the file for writing*/
	fp = fopen (fileName,"w");
	for(i = 0; i < 1000; i++){
		randomServiceTime = (rand() % 800) + 1;//service time is between 1 and 800
		randomArrivalTime = (rand() % 500000);//arrival time is between 0 and 149
		sprintf(output, "%i %i %i\n", i, randomArrivalTime, randomServiceTime);
		fprintf (fp, output, i + 1);
 	}
	/* close the file*/
	fclose (fp);
	return 0;
}
