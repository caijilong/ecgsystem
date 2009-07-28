#include "globol.h"
#include "System/IO/UART.h"
#include "System/TBManager.h"
#include "System/ITC/Pipe.h"

#define TOTAL_LT_COMMAND  6
#define NOCOMMAND 0

struct COM_CONTEXT{
    BYTE InstanceCommand;
    BYTE CommandState;
    BYTE TimerID;
    BYTE ReceiveDataAmount;
    BYTE Arg_buf_id;
    BYTE TaskID;
};



//==================State of Command====================
#define COMMAND_STATE_NOTHING   0
#define GOT_L         1
#define GOT_T         2
#define GOT_COM_NUM   3
#define DATA          4
#define COMMAND_STATE_RECEIVE_ARGUMENT   6
#define COMMAND_STATE_ARGUMENT_PERPARED   7



//===========================================

#define COMMAND_NUMBER BYTE
#define COMMAND_ERROR BYTE

#define COMMAND_ERROR_INITIAL    1
#define COMMAND_ERROR_NODE       2
#define COMMAND_NO_ERROR        4
#define COMMAND_DELETE_ERROR    8

#define NOCOMMAND       0
#define LT_GET_ECG      1
#define LT_BREAK        2
#define LT_GET_ID       3
#define LT_SET_TIME     4
#define LT_RESERVATION  5




extern BYTE LT1_Creat();
//extern void LT2_Creat();
//extern void LT3_Creat();
//extern void LT4_Creat();
//extern void LT5_Creat();

