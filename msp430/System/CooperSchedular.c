
#include "CooperSchedular.h"

UINT SCH_Error_Report=0;

struct SCHEDULAR TaskArray[MAX_TASK];

void SCH_Update()
{
    BYTE index;
    for(index = 0 ; index < MAX_TASK ; index++){
        if (TaskArray[index].pfunc){
            if (TaskArray[index].Delay == 0){
                TaskArray[index].Active++;
                if (TaskArray[index].Period){
                    TaskArray[index].Delay = TaskArray[index].Period;
                }
            }else{
                TaskArray[index].Delay--;
            }
        }
    }
}

BYTE ReportError(){


}

BYTE SCH_Add_Task(void (*pfunc)(),const BYTE Delay,const BYTE Period)
{
    BYTE index=0;
    while((TaskArray[index].pfunc != 0) && (index < MAX_TASK)) index++;  //find an empty slot for task

    if (index == MAX_TASK){   //The task array is full

        SCH_Error_Report |= ERROR_SCH_TASK_ARRAY_FULL;
        return 0xff;
    }

    TaskArray[index].pfunc = pfunc;
    TaskArray[index].Delay = Delay;
    TaskArray[index].Active = 0;
    TaskArray[index].Period = Period;

    return index;
}

BYTE SCH_Delete_Task(const BYTE index)
{
    BYTE ret;
    if (TaskArray[index].pfunc == 0){
        ret = 0;

    }else{
        ret = 1;

    }
    TaskArray[index].pfunc = NULL;
    TaskArray[index].Delay = 0;
    TaskArray[index].Active = 0;
    TaskArray[index].Period = 0;
    return ret;
}

BYTE SCH_Init()
{
    if (TakeTimer(TB1,TIMER_CLK/1024,PERIOD,SCH_Update) == 0) return 0;   // A timer taken for schedular tick is 0.0009765625s.
    return 1;
}

void SCH_Sleep()
{
    LPM4;
}

void SCH_Dispatch_Task(void)
{
    BYTE index;
    for(index = 0; index < MAX_TASK; index++){
        if (TaskArray[index].Active > 0){
            (*TaskArray[index].pfunc)();       //run task
            TaskArray[index].Active--;
            if (TaskArray[index].Period == 0){
                SCH_Delete_Task(index);
            }
        }
    }
   // SCH_Sleep();
}

void SCH_Start()
{
    _EINT();
}

