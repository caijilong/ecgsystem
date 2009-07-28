#include "System/TBManager.h"
#include "System/IO/UART.h"
#include "globol.h"
#include "System/ITC/Pipe.h"
#include "System/IO/PortManager.h"

BYTE t_Pipe_ID;
BYTE r_Pipe_ID;
BYTE UART_Flag=0;
UINT UART0_Error=0;

#pragma vector=UART1RX_VECTOR
__interrupt void usart1_rx(void)
{
    PutByte(r_Pipe_ID,RXBUF1);
}

#pragma vector=UART1TX_VECTOR
__interrupt void usart0_tx(void)
{
    if (Waiting_Byte(t_Pipe_ID) > 0){
        TXBUF1 = GetByte(t_Pipe_ID);
    }else UART_Flag &= ~TRANSMETTING; //Exit critical section as datas in buffer are transmitted fully
}

BYTE UART_Receive_Amount()
{
    return Waiting_Byte(r_Pipe_ID);
}

BOOL Init_UART()
{
    U1TCTL = SSEL1;
    U1MCTL = 0x6b;
    U1BR0  = 0xD0;
    U1BR1  = 0x00;
    ME2    |= URXE1 + UTXE1;
    U1RCTL = 0x00;
    U1CTL  = CHAR;
    r_Pipe_ID = Take_Pipe(IN_BUF_LEN);
    t_Pipe_ID = Take_Pipe(OUT_BUF_LEN);
    IE2    |= (URXIE1 + UTXIE1);                            // Enable USART0 RX interrupt
    return 1;
}


void Task_UART_Send()
{

/*    UINT i=1;
    while(Waiting_Byte(t_Pipe_ID) > 0){
        while ((IFG1 & UTXIFG0) == 0 && i != 0) i++;      //The estimated value of delay counter is i = 128
        if (i == 0){
            UART0_Error |= TX_TIME_OUT;
        }else i = 1;
        TXBUF0 = GetByte(t_Pipe_ID);
    }*/
//      if (UART_Tx_Buffer->RemainByte && (IFG1 & UTXIFG0) != 0) TXBUF0 = GetByte(t_Pipe_ID);
    if ((UART_Flag & TRANSMETTING) == 0 && Waiting_Byte(t_Pipe_ID) != 0){  //If UART is in critical section of transmittion, return
          UART_Flag |= TRANSMETTING;                                          //Enter critical section during transmitting data with ISR
          TXBUF1 = GetByte(t_Pipe_ID);
    }
}
UINT Remain_UART_Byte(void)
{
    return Remain_Space(t_Pipe_ID);
}

void SendTimeOut()
{
    UART_Flag |= TIME_OUT;
}

BYTE PutBytetoUART(char b)
{
BYTE chk,timer_id;

chk = PutByte(t_Pipe_ID,b);
if (chk == 0){
    timer_id = TakeTimer(RANDOM,TIMER_CLK*0.5,SINGLE,SendTimeOut);//Set a timer for detecting wrong command
    Task_UART_Send();           //if buffer is full, send all data in buffer immediately         
    UART_Flag &= ~TIME_OUT;
    while(Is_Full(t_Pipe_ID) == 1 && UART_Flag & TIME_OUT);
    ReleaseTimer(timer_id);
    if (UART_Flag & TIME_OUT){
        UART0_Error |= TX_BUFFER_FULL;
        return 0;
    }
    PutByte(t_Pipe_ID,b);    
}
if ((UART_Flag & TRANSMETTING) == 0){
    Task_UART_Send();           //if buffer is full, send all data in buffer immediately                
}               
return 1;

}

BYTE GetByteFromUART()
{
  return GetByte(r_Pipe_ID);
}

void ClearUARTBuffer()
{
    SkipALL(r_Pipe_ID);
}

/*
BYTE IsConnecting()
{
    if (UART_Rx_Buffer) return 1;
    else return 0;
}

BYTE IsOpening()
{
    if (P2OUT & BT_POWER) return 1;
    else return 0;
}




BYTE CheckBT()
{
    if ((P2IN & BT_ISCON) != 0){
        //Connecting)
      if (UART_Rx_Buffer == 0){
          CreatRangBuffer(r_Pipe_ID,IN_BUF_LEN);
      }
    }else{
        //Disconnecting
      if (UART_Rx_Buffer != 0) ReleaseRangBuffer(UART_Rx_Buffer);
      UART_Rx_Buffer = 0;
    }
}
#pragma vector = PORT2_VECTOR
__interrupt void P2ISR(void)
{
    int i=5000;
    if (P2IFG & BT_ISCON){
        TakeTimer(RANDOM,TIMER_CLK/2,SINGAL,CheckBT);
        P2IFG &= ~BT_ISCON;
    }
    if (P2IFG & BT_SW){
        while(i--);
        while((P2IN & BT_SW) == 0);
        P2OUT ^= BT_POWER;
        if ((P2OUT & BT_POWER) == 0){
            LPM1_EXIT;                          //If BT turned off,enter LPM3
            if (IsConnecting()){                //if turn off BT with connecting
                if (UART_Rx_Buffer != 0) ReleaseRangBuffer(UART_Rx_Buffer);
                UART_Rx_Buffer = 0;
            }
        }
        i=65535;
        int j = 10;
        while(j--)
          while(i--);
        P2IFG &= ~BT_SW;
        P2IFG &= ~BT_ISCON;
    }
}*/