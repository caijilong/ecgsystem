#include "globol.h"

#define TIMERB_AMOUNT  7
#define TIMERA_AMOUNT  3

#define TIMERID SCHAR
#define TB_FLAG BYTE
#define TIMER_ERROR BYTE

#define TIMERB_AMOUNT  7


#define RESET_ALL_TIMER 0xff

#define PERIOD      1
#define SINGLE      2

#define TIMER_CLK   32768

#define TBADC     0x00
#define TB0       0x00
#define TB1       0x01
#define TB2       0x02
#define TB3       0x03
#define TB4       0x04
#define TB5       0x05
#define TB6       0x06
#define RANDOM    0xff

#define NO_SERV  0xff
#define TSEM_B0  BIT0
#define TSEM_B1  BIT1
#define TSEM_B2  BIT2
#define TSEM_B3  BIT3
#define TSEM_B4  BIT4
#define TSEM_B5  BIT5
#define TSEM_B6  BIT6

#define ADC_INTERVAL  89    //32768/360

#define ERROR_NO_ERROR      0
#define ERROR_INITIAL       1
#define ERROR_CREAT_TIMER   2
#define ERROR_DELETE_TIMER  4


extern UINT TimerB_Error;
