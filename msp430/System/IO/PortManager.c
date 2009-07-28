#include "System/IO/PortManager.h"


typedef struct{
    BYTE PortStatus;
    TIMERID TimerID;
    UINT count;
}PS;//PortStatus
extern int f,c;

PS Port[2] = {{0,0xff},{0,0xff}};

BYTE PortRead(BYTE port,BYTE pin){
  return ((Port[port].PortStatus & pin) != 0) ? 1 : 0;
}
void InitialPort()    //set the stationary port
{
    P1SEL = 0;
    P2SEL = 0;
    P3SEL = SPI0 | UART1;
    P4SEL = 0;
    P5SEL = 0;
    P6SEL = ECG;
    
    P1DIR = 0;
    P2DIR = ~(BUTTON | BT_ISCON);
    P3DIR  |= (SD_CS | BT_POWER | UART1 | BT_POWER);
    P4DIR = 0;
    P5DIR = 0;
    P6DIR = 0;

    P1IE = 0;
    P2IE = BUTTON;
    
    P1IES = 0;
    P2IES = BUTTON;

    P1OUT = 0;
    P2OUT = 0;
    P3OUT  |= BT_POWER;
    P4OUT = 0;
    P5OUT = 0;
    P6OUT = 0;

    Port[0].PortStatus = (P1IN & P1DIR);
    Port[1].PortStatus = (P2IN & P2DIR);
}


void CheckPort()//under ISR,dont spend time too long
{
    Port[1].count++;   
    if((Port[1].PortStatus & P2IN) & P2IE != 0){   //Free button
        if (Port[1].PortStatus & BUTTON) LT1_Creat(300);   //Staring
        Port[1].PortStatus = 0;        
        ReleaseTimer(Port[1].TimerID);
        Port[1].TimerID = 0xff;
    }else if(Port[1].count == 200){   //press button continually till 2s
        if (Port[1].PortStatus & BUTTON) P3OUT ^= BT_POWER;
        Port[1].PortStatus = 0;        
        ReleaseTimer(Port[1].TimerID);
        Port[1].TimerID = 0xff;
    }
}


#pragma vector = PORT2_VECTOR
__interrupt void P2ISR(void)
{
    if (((Port[1].PortStatus & P2IFG) & P2IE) == LOW){  //(PortStatus[0] & ~P2DIR) : only consider the input pin
        Port[1].count = 0;
        if (Port[1].TimerID == 0xff){
            Port[1].TimerID = TakeTimer(RANDOM,TIMER_CLK/100,PERIOD,CheckPort);
            if (Port[1].TimerID == 0xff) Port[1].TimerID = 0xff;
        }
        Port[1].PortStatus |= (P2IFG & P2IE); //trans port stats to postive
    }
    P2IFG = 0;
}

#pragma vector = PORT1_VECTOR
__interrupt void P1ISR(void)
{
  P1IFG = 0;
}