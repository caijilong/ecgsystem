#include "globol.h"
#include "System/ITC/Pipe.h"
#include "System/ADCManager.h"

//==================State of LT1====================
#define LT1_STATE_RECEIVE_ARGUMENT  1
#define LT1_STATE_SENT_ECG          2
#define LT1_STATE_SAVE_ECG          3
#define LT1_STATE_STORE_ECG         4
/*
struct LT{                  //This struct is applied in CmmandSwitchList as polymorphism of LT command
    BYTE Command_ID;
    BYTE (*Creat)(void);
    BYTE (*Destroy)(void);
    BYTE (*Execute)(void);
    BYTE (*Suspend)(void);
};
*/

struct LT1_CONTEXT{
    BYTE  TaskID;
    BYTE  State;
    UINT  SentAmount;
    ULONG Interval;
    BYTE  ECG_Pipe_ID;
    BYTE  ECG_Timer_ID;    
    UINT  ECG_Error;
};


#define FLAG_SAMPLE_PREPARED  1   //1 present the sample is completely convert
                                  //0 present converting
#define FLAG_START_CONVERT    2   //1 present start to convert

#define SAMPLING_RATE 360

#define ECG_SAMPLING_OK 1


#define ECG_PORT_TAKE_FAIL    1
#define ECG_ADC_TAKE_FAIL     2
#define ECG_BUFFER_TAKE_FAIL  4
#define ECG_BUFFER_FULL       8
