#include "Command/LT1.h"
#include <stdlib.h>
#include  "System/Fat16.h"


#define N  251
//#define N  75
//==========Globol Variable=======

unsigned int n=0;

const int hn[126] = { 40,   -2,   -2,   -2,   -1,   -1,   -1,   -2,   -3,   -3,   -2,   -0,   -0,
   -1,   -3,   -4,   -3,   -1,    0,   -0,   -2,   -5,   -5,   -3,   -0,    1,   -1,
   -5,   -7,   -6,   -3,    0,    0,   -3,   -7,   -9,   -6,   -2,    1,   -1,   -6,
  -10,  -10,   -6,   -1,    1,   -3,  -10,  -13,  -11,   -4,    0,   -1,   -7,  -14,
  -15,  -10,   -2,    0,   -4,  -12,  -17,  -15,   -7,   -1,   -1,   -8,  -17,  -20,
  -14,   -4,    1,   -4,  -14,  -22,  -21,  -11,   -1,    0,   -9,  -21,  -26,  -19,
   -6,    2,   -2,  -17,  -28,  -27,  -14,    1,    4,   -8,  -26,  -35,  -25,   -6,
    8,    2,  -18,  -38,  -38,  -18,    7,   14,   -4,  -34,  -51,  -38,   -3,   26,
   20,  -20,  -62,  -70,  -28,   34,   64,   24,  -70, -146, -122,   39,  290,  518,
  610};
int xn[N] = 0;

struct LT1_CONTEXT *LT1_Context = NULL;
PIPE ADC_Pipe;
BYTE ADCTimerID;
SDFILE FILE1;
UINT sd_buffer[256];
int count;

int step=0;

void ECG_Acquisition()
{
    UINT Sample;
    ADC12CTL0 |= ADC12SC;
    
    if (ADC12MEM0 & 0x0800) Sample = ADC12MEM0 & ~0xf800;
    else Sample = ADC12MEM0 | 0xf800;
//    if (PutWord(LT1_Context->ECG_Pipe_ID,step) == 0) LT1_Context->ECG_Error |= ECG_BUFFER_FULL;  //put low byte
    if (PutWord(LT1_Context->ECG_Pipe_ID,Sample) == 0) LT1_Context->ECG_Error |= ECG_BUFFER_FULL;  //put low byte

    step++;
    if (step > 2047) step = -2048;
    ADC12CTL0 &= ~ADC12SC;
    P6DIR |= BIT6;
    P6OUT ^= BIT6;
}

BOOL StartSample()
{
  LT1_Context->ECG_Timer_ID = (BYTE )TakeTimer(TBADC,TIMER_CLK/SAMPLING_RATE,PERIOD,ECG_Acquisition);
  LT1_Context->ECG_Pipe_ID = Take_Pipe(128);  
  if (LT1_Context->ECG_Timer_ID == NO_SERV || LT1_Context->ECG_Pipe_ID == NO_PIPE){
      LT1_Context->ECG_Error |= ECG_ADC_TAKE_FAIL;  
      return 0;
  }
  StartADC();
  Init_PeakDetection(32);
  return 1;
}

void StopSample()
{
    StopADC();
    Release_Pipe(LT1_Context->ECG_Pipe_ID);
    ReleaseTimer(ADCTimerID);    
    Delete_PeakDetection();
}


BYTE HandleECG()
{ 
    UINT Data;
    if (Waiting_Byte(LT1_Context->ECG_Pipe_ID) > 1){//If ECG pipe have Data 
        Data = GetWord(LT1_Context->ECG_Pipe_ID);  
        Data = fir(Data)>>3;
        P6DIR |= BIT5;
        P6OUT |= BIT5;        
        Insert_Value(Data);
        P6DIR &= ~BIT5;
        P6OUT &= ~BIT5;        
        if (LT1_Context->State & LT1_STATE_SENT_ECG){
            if (Remain_UART_Byte() > 2){        //UART pipe is not full
                PutBytetoUART(Data & 0xff);
                PutBytetoUART(Data >> 8);
            }else return 0;
        }else{
            sd_buffer[count++] = Data;
            if (count == 256){
                P6DIR |= BIT3;
                P6OUT |= BIT3;
                FWrite(512,sd_buffer,&FILE1);
                P6OUT &= ~BIT3;                
                count = 0;
            }
        }
        return 1;
    }else{
        return 0;
    }
}

void LT1_Destroy()
{
   if (LT1_Context->State & LT1_STATE_STORE_ECG){
       while(Waiting_Byte(LT1_Context->ECG_Pipe_ID) > 1){     //If ECG pipe have Data    
          int Data = GetWord(LT1_Context->ECG_Pipe_ID);       
          sd_buffer[count++] = Data;
       }
       FWrite(count<<1,sd_buffer,&FILE1);
       FClose(&FILE1);
   }
   SCH_Delete_Task(LT1_Context->TaskID);
   StopSample();
   free(LT1_Context);
   LT1_Context = 0;
}

void Start_Record(char state)
{
            if (StartSample() != 0){
                if (state == 1){  //RT trans ECG
                    LT1_Context->State = LT1_STATE_SENT_ECG;     
                }else{          //Store ECG
                    LT1_Context->State = LT1_STATE_STORE_ECG;
                    char *temp,x='0',fileName[12];
                    temp = "ECG0    ECG";
                    for(int i=0;i<12;i++) fileName[i] = temp[i];
                    initMMC();                    
                    InitFAT16();
                    do{ 
                        fileName[3] = x;
                        x++;                        
                    }while(FOpen(fileName,&FILE1) == 0);
                    count = 0;
                }            
            }else LT1_Destroy();
}

void LT1_Exe()
{
    if (LT1_Context->State & LT1_STATE_RECEIVE_ARGUMENT){
        if (Command_Receiving_State() == 0) {    //when reveiving argument is time out.
            LT1_Destroy();
        }else if (Command_Argu_amount() >= 5){   //The argument is fully received.
            char state;
            state = Get_Command_Argu();
            ULONG *SendECGInterval;
            SendECGInterval = &LT1_Context->Interval;
            for(int i=0;i<4;i++) ((BYTE *)SendECGInterval)[i] = Get_Command_Argu();
            Reset_Command_State();
            LT1_Context->Interval = *SendECGInterval;
            Start_Record(state);
        }
    }else{
        if ((LT1_Context->Interval) > 0){
           // if (IsConnecting() == 0) LT1_Destroy();
            if (LT1_Context->SentAmount < SAMPLING_RATE){
                while (HandleECG() == 1) LT1_Context->SentAmount++;
            }else{
               LT1_Context->SentAmount = 0;
               LT1_Context->Interval--;                    // send a sample every time
            }
        }else{
            LT1_Destroy();
        }
    }
    
}

BYTE LT1_Creat(UINT interval)  
{
    if (LT1_Context == 0){    //The LT1 had already been executed.
        LT1_Context = (struct LT1_CONTEXT *)malloc(sizeof(struct LT1_CONTEXT));
        if (LT1_Context == 0) return 0;
        LT1_Context->Interval = interval;
        if(interval == 0)  LT1_Context->State = LT1_STATE_RECEIVE_ARGUMENT;
        else{
            Start_Record(2);  //2 is storing ECG
        }
        LT1_Context->SentAmount=0;
        LT1_Context->TaskID = SCH_Add_Task(LT1_Exe,0,20);                 //set interval of task for 4 Schedular tick
        return 1;
    }else return 0;
}

