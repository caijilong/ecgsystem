#include "globol.h"
#include "System/TBManager.h"

#define MAX_TASK 5
#define PERIODIC  1
#define SINGLE    0

struct SCHEDULAR{
    void (*pfunc)(void);
    BYTE Delay;
    BYTE Period;
    BYTE Active;
};

#define ERROR_SCH_TASK_ARRAY_FULL   1
