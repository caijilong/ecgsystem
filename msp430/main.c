#include "System/TBManager.h"
#include "System/ITC/Pipe.h"
#include "System/IO/PortManager.h"
#include "System/ITC/Signal.h"
#include "System/FAT16.h"
#include "System/TBManager.h"
extern void Task_UART_Send();
extern void Task_Command_Schedular();

UINT Init_Error;
int cardSize;
int main( void )
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
    BCSCTL1=0x00; //&= ~XT2OFF; // XT2= HF XTAL
    do{
      IFG1 &= ~OFIFG; // Clear OSCFault flag
      for (int i = 0xFF; i > 0; i--); // Time for flag to set
    }while ((IFG1 & OFIFG)); // OSCFault flag still set?
    BCSCTL2 |= SELM_2+SELS; // MCLK=SMCLK=XT2 (safe)

//Init System Service    
    InitialPort();
    InitFileSystem();
    Init_Pipe();
    InitTB();
//Init Device    
    InitialRTC();        
    Init_UART();
    InitADC();
    Creat_Command_Task();                   //After the bluetooth has beed connected
    SCH_Init();
   SCH_Start();

//    SCH_Add_Task(Task_UART_Send,0,5);       //creat a task for sending data via uart,after bluetooth connect
//   SCH_Add_Task(Acquire_ECG,0,4);      //32768/4*32 = 256hz
//    CommandSwitchList(1);

    while(1){
        SCH_Dispatch_Task();
    }
}
