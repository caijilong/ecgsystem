#include <stdlib.h>
#include "TBManager.h"


typedef struct{
    UINT  Period;
    BYTE  Operated_Mode;
    void  (*Timer_Function_Pointer)(void);   //function pointer will be called when timeout
}TIMER_ARRAY;

TIMER_ARRAY TimerB_Array[TIMERB_AMOUNT];
UINT TimerB_Error=0;

void InitTB()
{
    TBCTL = TBCLR + TBSSEL_1 + MC_2 + TBIE;//ACLK, CONTINUE mode
    for(int i=0 ; i<TIMERB_AMOUNT ; i++) TimerB_Array[i].Timer_Function_Pointer = 0;
}

TIMERID TakeTimer(TIMERID RequiredTimer,UINT Interval,BYTE mode,void (*f)())//if RequiredTimer = 0, timer is selected by ID
{
    UINT i,*pTBCCR,*pTBCCTL;
    if (RequiredTimer == RANDOM){
        for (i=TB2 ; i<TIMERB_AMOUNT ; i++){  //TB0 and TB1 is usd for ADC; 
            RequiredTimer = i;
            if (TimerB_Array[i].Timer_Function_Pointer == 0) break;
        }
            TimerB_Error |= ERROR_CREAT_TIMER;
    }else{
        if (TimerB_Array[i].Timer_Function_Pointer != 0){
            TimerB_Error |= ERROR_CREAT_TIMER;
            return NO_SERV;
        }
    }
    pTBCCR = (UINT *)(&TBCCR0 + RequiredTimer);
    pTBCCTL = (UINT *)(&TBCCTL0 + RequiredTimer);

    *pTBCCR = TBR + Interval;
    *pTBCCTL |= CCIE;
    TimerB_Array[RequiredTimer].Period = Interval;
    TimerB_Array[RequiredTimer].Operated_Mode = mode;
    TimerB_Array[RequiredTimer].Timer_Function_Pointer = f;
    return RequiredTimer;
}

BOOL ReleaseTimer(TIMERID RequiredTimer)
{
    UINT *pTBCCTL;
    if (RequiredTimer < TIMERB_AMOUNT){
        _DINT();                                      //enter the critical section for preventing the timer
        pTBCCTL = (UINT *)(&TBCCTL0 + RequiredTimer); //ISR call null function pointer
        *pTBCCTL &= ~CCIE;
        _EINT();
        TimerB_Array[RequiredTimer].Timer_Function_Pointer = 0;
        return 1;
    }else{
        TimerB_Error |= ERROR_DELETE_TIMER;
        return 0;
    }
}

void AdjustTimer(TIMERID num,UINT clk)
{
   UINT *pTBCCR;  
   if (TimerB_Array[num].Timer_Function_Pointer != 0){
      pTBCCR = (UINT *)(&TBCCR0 + num);
      *pTBCCR += clk;
   }
}

void  TimerISRFunction(TIMERID num)
{
   UINT *pTBCCR;
   if ( num < 7 && TimerB_Array[num].Timer_Function_Pointer != 0){
      pTBCCR = (UINT *)(&TBCCR0 + num);
      *pTBCCR += TimerB_Array[num].Period;
      TimerB_Array[num].Timer_Function_Pointer();
      if ((TimerB_Array[num].Operated_Mode & SINGLE) != 0) ReleaseTimer(num);
   }
}


#pragma vector=TIMERB0_VECTOR
__interrupt void Timer_B0(void)
{
//if (!IsConnecting()) LPM3_EXIT; //sorry, this code has to write here...
    TimerISRFunction(0);
}

#pragma vector=TIMERB1_VECTOR
__interrupt void Timer_B1(void)
{
//if (!IsConnecting()) LPM3_EXIT; //sorry, this code has to write here...
    TimerISRFunction(TBIV >> 1);
}
