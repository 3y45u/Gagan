#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdbool.h>
#include<pthread.h>

int totalResources;
int totalProcesses;
int availRes[20];
int allocatedRes[20][20];
int maxRequired[20][20];
int need[20][20];
int safeSeq[20];
int processRan=0;

pthread_mutex_t lockavailRes;
pthread_cond_t condition;

void* processCode(void*);
bool SafeSeq();

int main()
{
	printf("\nEnter the Number of Processes : ");
	scanf("%d",&totalProcesses);
	printf("\nEnter the Number of Resources : ");
	scanf("%d",&totalResources);

	printf("\nEnter the Currently Available Resources [A B C...] : ");
	for(int i=0;i<totalResources;i++)
		scanf("%d",&availRes[i]);


	printf("\n");
	for(int i=0;i<totalProcesses;i++)
	{
		printf("\nEnter the Allocated Resources to Process P%d [A B C...] : ",i+1);
		for(int j=0;j<totalResources;j++)
			scanf("%d",&allocatedRes[i][j]);
	}
	printf("\n");

	for(int i=0;i<totalProcesses;i++)
	{
	 printf("\nEnter the Maximum resource required by process P%d [A B C...] : ",i+1);
	 for(int j=0;j<totalResources;j++)
	 	scanf("%d",&maxRequired[i][j]);
	}
	printf("\n");

	for(int i=0;i<totalProcesses;i++)
	{
		for(int j=0;j<totalResources;j++)
		{
			need[i][j]=maxRequired[i][j]-allocatedRes[i][j];
		}
	}

	for(int i=0;i<totalProcesses;i++)
		safeSeq[i]=-1;
	if(!SafeSeq())
	{
		printf("\nUnsafe State.Deadlock Occurs and hence Request is not granted\n\n");
		exit(0);
	}

	printf("\nSafe Sequence Found : ");
	for(int i=0;i<totalProcesses;i++)
	{
		printf("  P%d  ",safeSeq[i]+1);
	}
	printf("\n\n");

	printf("Allocated Resources : \n");
    printf(" A  B  C \n");
	for(int i=0;i<totalProcesses;i++)
    {
        for(int j=0;j<totalResources;j++)
        {
            printf(" %d ",allocatedRes[i][j]);
        }
        printf("\n");
    }
    printf("\n\n");

    printf("Maximum Resources : \n");
    printf(" A  B  C \n");
	for(int i=0;i<totalProcesses;i++)
    {
        for(int j=0;j<totalResources;j++)
        {
            printf(" %d ",maxRequired[i][j]);
        }
        printf("\n");
    }
    printf("\n\n");

    printf("Available Resources : \n");
    printf(" A  B  C \n");
	for(int i=0;i<totalResources;i++)
    {
        printf(" %d ",availRes[i]);
    }
    printf("\n\n");

    printf("Needed Resources : \n");
    printf(" A  B  C \n");
	for(int i=0;i<totalProcesses;i++)
    {
        for(int j=0;j<totalResources;j++)
        {
            printf(" %d ",need[i][j]);
        }
        printf("\n");
    }
    printf("\n\n");

	pthread_t processes[totalProcesses];
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	int processNumber[totalProcesses];

	for(int i=0;i<totalProcesses;i++)
    {
       processNumber[i]=i;
    }

	for(int i=0;i<totalProcesses;i++)
	{
        pthread_create(&processes[i],&attr,processCode,(void*)(&processNumber[i]));
	}

	for(int i=0;i<totalProcesses;i++)
    {
        pthread_join(processes[i],NULL);
    }
}

bool SafeSeq()
{
	int temp[totalResources];
	for(int i=0;i<totalResources;i++)
    {
        temp[i]=availRes[i];
    }

	bool finishedProcess[totalProcesses];
	for(int i=0;i<totalProcesses;i++)
    {
        finishedProcess[i]=false;
    }


	int finishedP_no=0;
	while(finishedP_no<totalProcesses)
	{
		bool safe=false;
		for(int i=0;i<totalProcesses;i++)
		{
		if(!finishedProcess[i])
		{
		bool a=true;
		for(int j=0;j<totalResources;j++)
		{
			if(need[i][j]>temp[j])
			{
				a=false;
				break;
			}
		}

		if(a)
		{
		for(int j=0;j<totalResources;j++)
        {
            temp[j]+=allocatedRes[i][j];
        }
		safeSeq[finishedP_no]=i;
		finishedProcess[i]=true;
		++finishedP_no;
		safe=true;
		}
		}
		}
		if(!safe)
		{
			for(int k=0;k<totalProcesses;k++)
				safeSeq[k]=-1;
			return false;
		}
	}
	return true;
}


void * processCode(void *arg)
{
	int p=*((int *)arg);

	pthread_mutex_lock(&lockavailRes);

	while(p!=safeSeq[processRan])
		pthread_cond_wait(&condition,&lockavailRes);

	printf("\n--> Process P%d",p+1);
	printf("\n\tAllocated : ");
	for(int i=0;i<totalResources;i++)
    {
        printf(" %d ",allocatedRes[p][i]);
    }

	printf("\n\tNeeded : ");
	for(int i=0;i<totalResources;i++)
    {
        printf(" %d ",need[p][i]);
    }

	printf("\n\tAvailable : ");
	for(int i=0;i<totalResources;i++)
    {
        printf(" %d ",availRes[i]);
    }


	for(int i=0;i<totalResources;i++)
    {
      availRes[i]+=allocatedRes[p][i];
    }

	printf("\n\tNow Available: ");

	for(int i=0;i<totalResources;i++)
    {
       printf(" %d ",availRes[i]);
    }
	printf("\n\n");

	processRan++;
	pthread_cond_broadcast(&condition);
	pthread_mutex_unlock(&lockavailRes);
	pthread_exit(NULL);


}


