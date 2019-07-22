//SJN.c

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct Program
{
	int programName;//program name
	int serviceTime;//service time
	int arrivalTime;//arrival time
	int startTime;//start time 
	int finishTime;//finish time
	int turnAroundTime;//turnaround time
	double normalizedTurnAroundTime;//normalizedturnaround time
	int waitingTime;//waiting time
};

//global variables
struct Program *P;//array of Programs
int *readyQueue;//queue of Program indices
int switches = 0;//total number of switches
int switchTime = 0;//the amount of time program switching takes
int CPUTime = 0;//simulated CPU time
int numProgs = 0;//number of programs left to execute
int validProgs = 0;//hold number of programs whose arrivalTime is valid for current CPUTime
int totalProgs = 0;//hold initial amount of programs, used for print function
char *fileName;//character array to hold inputted fileName
int tempArray[10];//temporary arrays for reading data from file

main(int argc, char *argv[]) {
	if(argc != 1)//if fileName was passed
	{
		fileName = (char *)malloc(sizeof(char) * strlen(argv[1]));
		strcpy(fileName, argv[1]);//copy inputted filename into variable
	}
	else//otherwise set to default values
	{
		printf("Missing command line arguments.\n");
		perror("No input file parameter passed.");
		return EXIT_FAILURE;
	}
	FILE *fp;//pointer to FILE 

	fp = fopen(fileName, "r");//open input file
	//if file opened, get data from file; otherwise, exit program
	if (fp == NULL){
		perror("Failed to open file.");
		return EXIT_FAILURE;
	}
	else
	{
		getData(fp);
	}
	int i = 0;//counter variable	
	totalProgs = numProgs;//store total number of programs
	for (i = 0; i < totalProgs; i++)//store indices of all jobs from job queue into ready queue(Enqueue)
		readyQueue[i] = i;
	SortJobs();//call function to sort jobs in ready queue
	while(numProgs > 0)
		SJN();
	PrintData(totalProgs);//print data
	fclose(fp);//close file buffer
	free(fileName);//release dynamic memory
	free(readyQueue);
	free(P);
}

getData(FILE *fp)//get data from file
{
	int count = 0;//counter to hold number of programs
	char c;//temp character variable for use in program counter
	for (c = getc(fp); c != EOF; c = getc(fp))//read file character by character
        	if (c == '\n') //Increment count if this character is newline
            		count++;
	count = count - 1;//first line of input is irrelevant so remove it from count
	rewind(fp);//reset file pointer to beginning of file
	readyQueue = (int *)malloc(count * sizeof(int));
	P = (struct Program *)malloc(count * sizeof(struct Program));
	fscanf(fp, "%i", tempArray);//get switch time
	switchTime = tempArray[0];//set switchTime
	while (fscanf(fp, "%i", tempArray) != EOF)
	{
		P[numProgs].programName = tempArray[0];//copy program name to struct value of programName

		fscanf(fp, "%i", tempArray);
		P[numProgs].arrivalTime = tempArray[0];//copy arrival time to struct value of arrivalTime

		fscanf(fp, "%i", tempArray);
		P[numProgs].serviceTime = tempArray[0];//copy service time to struct value of serviceTime
		numProgs++;
	}
}

SJN()//start of SJN algorithm
{
	//only jobs in queue can be called on
	int i = 0;
	int shortest;//holds index of shortest program
	validProgs = 0;//reset number of valid

	for (i = 0; i < numProgs; i++)//find out what programs are ready in queue
	{
		if (P[readyQueue[i]].arrivalTime <= CPUTime)
		{
			validProgs++;
		}
	}

	//find shortest job
	shortest = 0;//set value of shortest to first index
	for (i = 0; i < validProgs; i++)
	{
		//if service time of current shortest program is greater than service time of i, change value of shortest to shorter program
		if (P[readyQueue[shortest]].serviceTime > P[readyQueue[i]].serviceTime)
		{
			shortest = i;
		}
	}

	if (validProgs > 0)//if there is a program in queue, run program and dequeue
	{
		RunProg(readyQueue[shortest]);//run shortest program
		Dequeue(shortest);//remove index from queue
        if(numProgs != 0)//if there are still programs to switch to
        {
            switches++;//increment number of switches on program completion
            CPUTime += switchTime;//add switching time to CPUTime
        }
	}
	else//otherwise, increment CPUTime and run algorithm again
	{
		CPUTime++;
	}

}

Dequeue(int jobNum)//dequeue function, which removes program from queue
{
	int i = jobNum;
	for (i; i < totalProgs; i++)
	{
		readyQueue[i] = readyQueue[i + 1];
	}
	numProgs--;
}

RunProg(int jobNum)//run program function, which "runs" the program
{
	int remainingServiceTime = P[jobNum].serviceTime;//hold service time
	P[jobNum].startTime = CPUTime;//record start time of program
	while (remainingServiceTime > 0)//run program
	{
		CPUTime++;//increment CPUTime
		remainingServiceTime--;//decrement remaining service time
	}

	//after program executes start storing and calculations
	P[jobNum].finishTime = CPUTime;//store finish time
	P[jobNum].turnAroundTime = abs(P[jobNum].finishTime - P[jobNum].arrivalTime);//calculate and store turn around time
	P[jobNum].normalizedTurnAroundTime = (double)P[jobNum].turnAroundTime / P[jobNum].serviceTime;//calculate and store normalized turn around time
	P[jobNum].waitingTime = abs(P[jobNum].turnAroundTime - P[jobNum].serviceTime);//calculate and store waiting time
}

//printing function that only needs the number of total programs that have run
PrintData(int i)
{
	FILE *pf;
	pf = fopen("AlgorithmResults.txt", "w");
	int j = 0;
	fprintf(pf,"Program:\tArrival Time:\tService Time:\tStart Time:\tFinish Time:\tTurnaround:\tNTurnaround:\t\tWaiting:\t\n");
	for (j; j < i; j++)
	{
		fprintf(pf,"%i\t\t%i\t\t%i\t\t%i\t\t%i\t\t%i\t\t%f\t\t%i\n", P[j].programName, P[j].arrivalTime, P[j].serviceTime, P[j].startTime, P[j].finishTime, P[j].turnAroundTime, P[j].normalizedTurnAroundTime, P[j].waitingTime);
	}
	fprintf(pf, "CPUTime: %i\n", CPUTime);//total CPUTime
	fprintf(pf, "Number of Switches: %i\n", switches);
	fprintf(pf, "Switch Time: %i\n", switchTime);
	fflush(pf);//force finish writing to file
	fclose(pf);//close file stream
}

SortJobs()//sort function, which sorts the programs in file by arrival time
{
	int i = 0;//counter variable	
	int j = 0;
	for (i = 0; i < totalProgs; i++)//sort programs by arrival time
	{
		for (j = i; j>=1; j--)
		{
			if (P[readyQueue[j-1]].arrivalTime > P[readyQueue[j]].arrivalTime)
			{
				int swap = readyQueue[j-1];
				readyQueue[j-1] = readyQueue[j];
				readyQueue[j] = swap;
			}
		}
	}
}
