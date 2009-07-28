#include "ADCManager.h"

typedef struct ADC{
    UINT Present_ADC_Clock = 0; //Clock type, either 32768/8192 or 32768/512 hz
    BYTE rb_id[ADC_AMOUNT] = {0xff};
    UINT SuspendMask = 0;       //If bit is set, the adc is suspend
};

ADC ADC_Context;
UINT ADC_Error=0;

void Set_CSTARTADD(UINT Address)
{
    ADC12CTL1 &= 0x0fff;
    ADC12CTL1 |= Address;
}

void Temp_Ecquisition()
{
    Set_CSTARTADD(CSTARTADD1);
}

void Temp_Ecquisition()
{
    Set_CSTARTADD(CSTARTADD1);
}

void Voice_Ecquisition()
{
    Set_CSTARTADD(CSTARTADD2);
}

void Pedometer_Ecquisition()
{
    Set_CSTARTADD(CSTARTADD3);
}

void InitADC()
{
  ADC12CTL0 = ADC12ON + SHT0_0 + REF2_5V + REFON + ENC; // Setup ADC12, ref.2.5, no frequence divided
  ADC12CTL1 = CONSEQ_0 + SHS_0;           // Sequence-of-channels, ADC12SC triggers sampling , ADC12OSC=ADC12CLK=MCLK
  TakeTimer(0,Sampling_Freq,PERIOD,ADC_func);
  ADC_Sleep();
}

void ADC_Sleep()          //for power consumation
{
  TBCCTL0 &= ~CCIE;
  ADC12CTL0 &= ~ADC12ON;
}

void ADC_WakeUp()         //for power consumation
{
  TBCCTL0 |= CCIE;  
  ADC12CTL0 |= ADC12ON;
}

BYTE Is_ADC_Idle(UINT *pADC12MCTLx){
    pADC12MCTLx = (UINT *)(&ADC12MCTL0 + RequiredADC);  
    for (int i=0;i<ADC_AMOUNT;i++){
        pADC12MCTLx = (UINT *)(&ADC12MCTL0 + i);
        if (*pADC12MCTLx & EOS != 0) break;
    }
    if (i == ADC_AMOUNT) return 1;      //The ADC is idle
    else return 0;                      //At least one adc actives
}

BOOL Take_ADC(UINT ADC_Service,UINT Sampling_Freq)
{
    BYTE ReturnBool,Timer_ID;
    UINT *pADC12MCTLx;
    if (ADC_Context.rb_id[VOICE] == RANG_BUFFER_FREE){  //if voice is active, return false
        pADC12MCTLx = (UINT *)(&ADC12MCTL0 + ADC_Service);

        if (ADC_Service == VOICE){
            ADC_Context.Present_ADC_Clock = Crystal/VOICE_FREQ;
            ADC_Context.SuspendMask |= RequiredADC;      
            Set_CSTARTADD(CSTARTADD0);            
            *pADC12MCTLx |= EOS | SREF_1 | ADC_Service;   // Jest Voice will be converted
        }else{
            ADC_Context.Present_ADC_Clock = Crystal/ECG_FREQ;  
            Set_CSTARTADD(CSTARTADD1);                    //Start the adc next voice            
            pADC12MCTLx = (UINT *)(&ADC12MCTL0 + TEMP);   //
            *pADC12MCTLx |= EOS | SREF_1 | ADC_Service;   //End of converted is the last adc
        }
        AdjustTimer(0,ADC_Context.Present_ADC_Clock);
    }return 0;
}


void ReleaseADC(UINT RequiredADC)
{
    UINT *pADC12MCTLx;
    pADC12MCTLx = (UINT *)(&ADC12MCTL0 + RequiredADC);
    *pADC12MCTLx &= ~EOS;
    if (Is_ADC_Idle(pADC12MCTLx)){
        ReleaseTimer(0);
        free(ADC_Context);
    }
}
                                //Since i cant find the method that ADC triggered source only using the TBCCR0 without TBCCR1,so i use the TB
#pragma vector=TIMERB0_VECTOR   //In order to reduce the latancy for ADC, 
__interrupt void ADC_func(void) //I write this isr in adc module directly.
{
    TBCCR0 += ADC_Context->Present_ADC_Clock;
    if (ADC_Context.rb_id[VOICE] == RANG_BUFFER_FREE)  //if the void is active
    {
        ADC12CTL0 |= ADC12SC;
        if (ADC12MEM0 & 0x0800) Sample = ADC12MEM0 & ~0xf800; //transfer to signed integer
        else Sample = ADC12MEM0 | 0xf800;
        ADC12CTL0 &= ~ADC12SC;    
    }else{
        
    
    }
    ADC12CTL0 |= ADC12SC;
//    step++;
  //  if (step == 4096) step = 0;

    if (ADC12MEM0 & 0x0800) Sample = ADC12MEM0 & ~0xf800; //transfer to signed integer
    else Sample = ADC12MEM0 | 0xf800;
    ADC12CTL0 &= ~ADC12SC;    
 //   Sample++;
    for(int i=0;i<ADC_AMOUNT;i++)
        if (ADC_Context->element[i].Factor == 0) PutWord(ADC_Context->element[i].rb_id,Sample);

}