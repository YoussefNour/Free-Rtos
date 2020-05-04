#include "FreeRTOS.h"
#include "task.h"
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_sysctl.h"
#include "basic_io.h"
#include <stdlib.h>
//#include <time.h>
//#include <stm32f4xx_hal_conf.h>

#define N 5
#define tst 4
#define latest_arrival_time 15
#define max_computation_time 8
#define max_period_multiplier 17

struct task{
	//char name[20];
	int Ta;
	int Tp;
	int Tc;
};

volatile unsigned int n;
struct task tasks[N];
void InitTasks(int);
int admit(void);
void vContinuousProcessingTask( void *pvParameters );

const char *pcTextForTask1 = "Continuous task 1 running\n";

int main( void )
{
	srand(256);
	InitTasks(1);
	vPrintStringAndNumber("schedulability:",admit());
	//xTaskCreate( vContinuousProcessingTask, "Task 1", 240, (void*)pcTextForTask1, 1, NULL );
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
	volatile unsigned long ul;
	for( ul = 0; ul <n; ul++ )
		{
			vPrintStringAndNumber("task has been created",ul);
			tasks[ul].Ta = rand()% latest_arrival_time;
			vPrintStringAndNumber("TA:",tasks[ul].Ta);
			tasks[ul].Tc = (rand()%(max_computation_time-1)+1);
			
			switch(mode){
				case 1://safemode
					tasks[ul].Tp = (rand()%(max_period_multiplier*tasks[ul].Tc-3*tasks[ul].Tc)+3*tasks[ul].Tc)*tst;
					break;
				case 2://no guarantee
					tasks[ul].Tp = (rand()%(10*tasks[ul].Tc-3*tasks[ul].Tc)+3*tasks[ul].Tc)*tst;
					break;
			}
			tasks[ul].Tc = tasks[ul].Tc*tst;
			vPrintStringAndNumber("Tc:",tasks[ul].Tc);
			vPrintStringAndNumber("Tp:",tasks[ul].Tp);
		}
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
