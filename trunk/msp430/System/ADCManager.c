#include "ADCManager.h"
#include "System/ITC/Pipe.h"

UINT ADC_Error=0;




BOOL InitADC()
{
  ADC12CTL0 = ADC12ON + SHT0_0 + REF2_5V + REFON;// Setup ADC12, ref., sampling time
  ADC12CTL1 = CONSEQ_1+SHS_0;           // Timer triggers sampling,ADC12OSC=ADC12CLK= 8Mhz
  ADC12MCTL0 = SREF_1 + EOS;                  //Vr+ = Veref+ = 2.5v
}

void StartADC()
{
  ADC12CTL0 |= ADC12ON;
  ADC12CTL0 |= ENC;                           // Start conversion 
}

BOOL StopADC()
{
  ADC12CTL0 &= ~ENC;                           // Stop conversion
  ADC12CTL0 &= ~ADC12ON; 
}