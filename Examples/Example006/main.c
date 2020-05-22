#include "FreeRTOS.h"
#include "task.h"
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_sysctl.h"
#include "basic_io.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#define N 5
#define tst 1
#define latest_arrival_time 15
#define max_computation_time 8
#define max_period_multiplier 17

struct task{
	char name[20];
	int Ta;
	int Tp;
	int Tc;
	int P;
	xTaskHandle handler;
	int running;
};

volatile unsigned int n;
struct task tasks[N];
struct task *dTasks;
void InitTasks(int);
int admit(struct task tasks[],int n);
void swap(struct task* a, struct task* b);
int partition (struct task arr[], int low, int high);
void quickSort(struct task arr[], int low, int high);
void printTasks(struct task tasks[],int n);
void prioritize(struct task tasks[],int n);
void CreateTasks(struct task tasks[],int n);
void DynamicScheduler(struct task tasks[]);
static void VTask(struct task* p);
struct task* ptr;
xTaskHandle xTaskDynamicHandle;

int main( void )
{
	srand(256); 
	InitTasks(1);
	vPrintStringAndNumber("\n\nschedulability:",admit(tasks,n));
	quickSort(tasks,0,n-1);
	printf("\nfinished sorting");
	printTasks(tasks,n);
	prioritize(tasks,n);
	printTasks(tasks,n);
	switch(1){
		case 1:
			CreateTasks(tasks,n);
			break;
		case 2:
			xTaskCreate(DynamicScheduler,"D-Scheduler",100,tasks,N,&xTaskDynamicHandle);
			break;
		default:
			break;
	}
	vTaskStartScheduler();
	free(ptr); 
	for( ;; );
}

void vApplicationIdleHook(){
	for(;;);
}


void InitTasks(int mode){
	n = rand()%N+2;
	char arr[2] = "4";
	xTaskHandle xTaskHandleArr[N];
	volatile unsigned int ul;
	for( ul = 0; ul <n; ul++ )
		{
			tasks[ul].handler = xTaskHandleArr[ul];
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
			tasks[ul].running = 0;
			printf("Tp: %d ",tasks[ul].Tp);
		}
}


int admit(struct task tasks[],int n)
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

void printTasks(struct task tasks[],int n){
	for(int ul =0;ul<n;ul++){
		printf("\n%s \n",tasks[ul].name);
		vPrintStringAndNumber("TA:",tasks[ul].Ta);
		vPrintStringAndNumber("Tc:",tasks[ul].Tc);
		printf("Tp: %d ",tasks[ul].Tp);
		printf("\nPriority: %d\n",tasks[ul].P);
	}
}

void prioritize(struct task tasks[],int n){
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
	portTickType xLastWakeTime= xTaskGetTickCount();
	char *Taskname = (char *)p->name;
	volatile unsigned long ul;
	volatile unsigned long i;
	portTickType temp;
	int oldtick;
	
	if(p->running == 0){
		xLastWakeTime = 0;
		p->running =1;
	}
	for(;;){
		vPrintString(Taskname);
		vPrintStringAndNumber(" starts running at ",xTaskGetTickCount());
		vPrintString("\n");
		oldtick = 0;
    while(oldtick < p->Tc){
          oldtick++;
          temp = xTaskGetTickCount();
          while(temp==xTaskGetTickCount());
        }
		vPrintString(Taskname);
		vPrintStringAndNumber(" is done at ",xTaskGetTickCount());
		vPrintString("\n");
		vTaskDelayUntil(&xLastWakeTime,(p->Tp));
	}
}

void CreateTasks(struct task tasks[],int n){
	//xTaskCreate(DynamicScheduler,"D-Scheduler",100,tasks,1,&xTaskDynamicHandle);
	for(int ul=n-1;ul>=0;ul--){
		xTaskCreate(VTask,tasks[ul].name,50,&tasks[ul],tasks[ul].P+1,&tasks[ul].handler);
	}
	
}

static void DynamicScheduler(struct task tasks[]){
	
	//looping on static array
	volatile unsigned int ul;
	volatile unsigned int activeTasks=0;
	portTickType xLastWakeTime;
	volatile unsigned int delay=0;	
		
	for(;;){
		// creation for loop
		for( ul = 0; ul <n; ul++ )
		{
			if(tasks[ul].running==0 && tasks[ul].Ta <= xTaskGetTickCount()){
				if(ptr == NULL){
					ptr = (struct task*)malloc((++activeTasks)*sizeof(struct task));
					tasks[ul].running=1;
					xTaskCreate(VTask,tasks[ul].name,100,&tasks[ul],tasks[ul].P,&tasks[ul].handler);
					ptr[activeTasks-1] = tasks[ul];
					delay = tasks[ul].Tc;
					vPrintStringAndNumber("\n\nschedulability:",admit(ptr,activeTasks));
					quickSort(ptr,0,activeTasks-1);
					prioritize(ptr,activeTasks);
					//printTasks(ptr,activeTasks);
				  
				} 
				else{
					ptr = realloc(ptr, (++activeTasks) * sizeof(struct task)); 
					tasks[ul].running=1;
					xTaskCreate(VTask,tasks[ul].name,100,&tasks[ul],tasks[ul].P,&tasks[ul].handler);
					ptr[activeTasks-1] = tasks[ul];
					delay = tasks[ul].Tc;
					vPrintStringAndNumber("\n\nschedulability:",admit(ptr,activeTasks));
					quickSort(ptr,0,activeTasks-1);
					prioritize(ptr,activeTasks);
					//printTasks(ptr,activeTasks);
				} 
		}
		}
		vTaskDelayUntil(&xLastWakeTime,delay);
	}
	
}

