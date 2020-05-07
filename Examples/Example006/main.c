#include "FreeRTOS.h"
#include "task.h"
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_sysctl.h"
#include "basic_io.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
//#include <time.h>
//#include <stm32f4xx_hal_conf.h>

#define N 5
#define tst 4
#define latest_arrival_time 15
#define max_computation_time 8
#define max_period_multiplier 17

struct task{
	char name[20];
	int Ta;
	int Tp;
	int Tc;
	int P;
};

volatile unsigned int n;
struct task tasks[N];
void InitTasks(int);
int admit(void);
void swap(struct task* a, struct task* b);
int partition (struct task arr[], int low, int high);
void quickSort(struct task arr[], int low, int high);
void printTasks(struct task tasks[]);
void prioritize(struct task tasks[]);
void CreateTasks(struct task tasks[]);
static void VTask(struct task* p);
void vApplicationIdleHook(void);

const char *pcTextForTask1 = "Continuous task 1 running\n";

int main( void )
{
	srand(256); //time(null)
	InitTasks(1);
	vPrintStringAndNumber("\n\nschedulability:",admit());
	quickSort(tasks,0,n-1);
	printf("\nfinished sorting");
	printTasks(tasks);
	prioritize(tasks);
	printTasks(tasks);
	CreateTasks(tasks);
	vTaskStartScheduler();
	for( ;; );
}


void vContinuousProcessingTask( void *pvParameters )
{
char *pcTaskName;
volatile unsigned long ul;

	pcTaskName = ( char * ) pvParameters;

	for( ;; )
	{
		vPrintString( pcTaskName );

		for( ul = 0; ul < 0xf; ul++ )
		{
			__nop();
		}
	}
}

void InitTasks(int mode){
	n = rand()%N+2;
	char arr[2] = "4";
	volatile unsigned int ul;
	for( ul = 0; ul <n; ul++ )
		{
			sprintf(tasks[ul].name,"Task %d",ul+1);
			printf("\n\n%s \n",tasks[ul].name);
			tasks[ul].Ta = rand()% latest_arrival_time;
			vPrintStringAndNumber("TA:",tasks[ul].Ta);
			tasks[ul].Tc = (rand()%(max_computation_time-1)+1);
			
			switch(mode){
				case 1://safemode
					tasks[ul].Tp = (rand()%(max_period_multiplier*tasks[ul].Tc-3*tasks[ul].Tc)+3*tasks[ul].Tc)*tst; //problem 2 tst problem
					break;
				case 2://no guarantee
					tasks[ul].Tp = (rand()%(10*tasks[ul].Tc-3*tasks[ul].Tc)+3*tasks[ul].Tc)*tst;
					break;
			}
			tasks[ul].Tc = tasks[ul].Tc*tst;
			vPrintStringAndNumber("Tc:",tasks[ul].Tc);
			printf("Tp: %d ",tasks[ul].Tp);
		}
}

void vApplicationIdleHook(){
	vPrintString("idle\n");
	for(int i =0 ;i<1000;i++ );
}

int admit()
{
	float UCPU=0.0;
	volatile unsigned long ul;
	for(ul = 0; ul <n; ul++ )
	{
		UCPU+=tasks[ul].Tc/tasks[ul].Tp;
	}
	if(UCPU<0.7){
		return 1;
	}else return 0;
}

void swap(struct task* a, struct task* b) 
{ 
    struct task t = *a; 
    *a = *b; 
    *b = t; 
} 

int partition (struct task arr[], int low, int high) 
{ 
    int pivot = arr[high].Tp;    // pivot 
    int i = (low - 1);  // Index of smaller element 
  
    for (int j = low; j <= high- 1; j++) 
    { 
        // If current element is smaller than the pivot 
        if (arr[j].Tp < pivot) 
        { 
            i++;    // increment index of smaller element 
            swap(&arr[i], &arr[j]); 
        } 
    } 
    swap(&arr[i + 1], &arr[high]); 
    return (i + 1); 
} 
  
void quickSort(struct task arr[], int low, int high) 
{ 
    if (low < high) 
    { 
        /* pi is partitioning index, arr[p] is now 
           at right place */
        int pi = partition(arr, low, high); 
  
        // Separately sort elements before 
        // partition and after partition 
        quickSort(arr, low, pi - 1); 
        quickSort(arr, pi + 1, high); 
    }
} 

void printTasks(struct task tasks[]){
	for(int ul =0;ul<n;ul++){
		printf("\n%s \n",tasks[ul].name);
		vPrintStringAndNumber("TA:",tasks[ul].Ta);
		vPrintStringAndNumber("Tc:",tasks[ul].Tc);
		printf("Tp: %d ",tasks[ul].Tp);
		printf("\nPriority: %d\n",tasks[ul].P);
	}
}

void prioritize(struct task tasks[]){
	int count=1;
	for(int ul=n-1;ul>0;ul--){
		if(tasks[ul].Tp>tasks[ul-1].Tp){
			tasks[ul].P=count++;
		}else{
			tasks[ul].P=count;
		}
	}
	tasks[0].P = count;
	printf("\n\nTasks have been prioritized\n");
}

static void VTask(struct task* p){
	char *Taskname = (char *)p->name;
	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	for(;;){
		vPrintString(Taskname);
		vPrintString(" is running\n");
		vTaskDelayUntil(&xLastWakeTime,p->Tp/ portTICK_RATE_MS);
	}
}

void CreateTasks(struct task tasks[]){
	for(int ul=n-1;ul>=0;ul--){
		xTaskCreate(VTask,tasks[ul].name,50,&tasks[ul],tasks[ul].P,NULL);
	}
}

