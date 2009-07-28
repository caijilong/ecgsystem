#include "Command.h"
#include "System/ITC/Signal.h"

struct COM_CONTEXT *Com_Context = NULL;

/*------------------------------------------------------------------------------------
*    For avoiding the uncompletely command being recceived, we take a timer. And
*   each LT command function must invoke Reset_Command_State() when the
*   command word and argument are received correctly.
*------------------------------------------------------------------------------------*/
void Reset_Command_State()
{
   ReleaseTimer(Com_Context->TimerID);
   Com_Context->CommandState = COMMAND_STATE_NOTHING;
}

BYTE Command_Receiving_State()
{
  return Com_Context->CommandState;
}


BOOL Delete_Com_Context()     //Delete command context as BT is disconncting
{
    if (Com_Context != NULL){
        free(Com_Context);
        return 1;
    }else return 0;
}

//You may add some command in this CommandSwitchList
//The Com_Context->ReceiveDataAmount setup the received byte after command words

BYTE CommandSwitchList(BYTE com)
{
        switch (com){
            case LT_GET_ECG:
                  if (LT1_Creat(0) == 0) return 0;
                  Com_Context->ReceiveDataAmount = 5;    //There will be 4 byte to be received
                  break;
            case LT_BREAK:
                  LT1_Destroy();
                  Reset_Command_State();                  
                  break;
            case LT_GET_ID:
 //                 LT3_Creat();
                  break;
            case LT_SET_TIME:            //set timer
                  Com_Context->ReceiveDataAmount = 6;    //There will be 6 byte to receive                  
                  while(Com_Context->ReceiveDataAmount > 0){
                      PutByte(Com_Context->Arg_buf_id,GetByteFromUART());
                      Com_Context->ReceiveDataAmount--;
                  }                  
                  Send_Signal(SIG_DATE_TIME_ADJ);                  
                  Reset_Command_State();
                  break;
            case LT_RESERVATION:
 //                 LT5_Creat();
                  Com_Context->ReceiveDataAmount = 10;   //There will be 10 byte to receive
                  break;
            default:
                return 0;
        }
        return 1;
}

void CommandTimeUP()
{
   Com_Context->CommandState = COMMAND_STATE_NOTHING;          //present the instance condition of command char   
}


// The completed command order is GOT_L -> GOT_T -> NUM
// if num is less than 0x7f, host -> recorder
// if num is bigger than 0x80 recorder -> host after the completed command order is finished.
// This function cant wait a number of time for receiving command, so that wont delay for wait command characteristic
BYTE LeadTekCommand()
{
    BYTE CommandType;
        if (Com_Context->CommandState == COMMAND_STATE_NOTHING){                //if there is not any state of receiving command
            if (GetByteFromUART() == 'T'){                //The first char is L
//                Com_Context->TimerID = TakeTimer(RANDOM,0.005*Crystal,SINGLE,CommandTimeUP);//Set a timer for detecting wrong command
                Com_Context->TimerID = TakeTimer(RANDOM,16384,SINGLE,CommandTimeUP);//Set a timer for detecting wrong command
                Com_Context->CommandState = GOT_L;          //set the state as 'GOT_L'
            }
        }else if (Com_Context->CommandState == GOT_L){      //if there is GOT_L and timer is not run out
            if (GetByteFromUART() == 'K') Com_Context->CommandState = GOT_T;
        }else if (Com_Context->CommandState == GOT_T){      //if it is GOT_T for instance, than receive the command number
            CommandType = GetByteFromUART();
            if (CommandType != 0){
                Com_Context->CommandState = COMMAND_STATE_RECEIVE_ARGUMENT;
                return CommandType;
            }
        }
    return NOCOMMAND;
}


void Task_Command_Schedular()
{
    BYTE com;
    if (Com_Context->CommandState == COMMAND_STATE_NOTHING)
    {
        while(UART_Receive_Amount()){
            com = LeadTekCommand();
            if (com != 0){
                if (CommandSwitchList(com) == 0) Reset_Command_State();
                break;
            }
        }
    }else if (Com_Context->CommandState == COMMAND_STATE_RECEIVE_ARGUMENT){
        if (UART_Receive_Amount() >= Com_Context->ReceiveDataAmount){
            while(Com_Context->ReceiveDataAmount > 0){
                PutByte(Com_Context->Arg_buf_id,GetByteFromUART());
                Com_Context->ReceiveDataAmount--;
            }
            Com_Context->CommandState = COMMAND_STATE_ARGUMENT_PERPARED;
        }
    }
}

BYTE Command_Argu_amount()
{
    return Waiting_Byte(Com_Context->Arg_buf_id);
}
BYTE Get_Command_Argu()
{
    return GetByte(Com_Context->Arg_buf_id);
}

BOOL Creat_Command_Task()      //Creat command context as BT is connecting
{
    Com_Context = (struct COM_CONTEXT *)malloc(sizeof(struct COM_CONTEXT));
    if (Com_Context == NULL) return 0;
    Com_Context->InstanceCommand=0;
    Com_Context->CommandState=0;
    Com_Context->TimerID=0;
    Com_Context->ReceiveDataAmount=0;
    Com_Context->Arg_buf_id = Take_Pipe(16);
    if (Com_Context->Arg_buf_id == NO_PIPE){
        Delete_Com_Context();
        return 0;
    }
    Com_Context->TaskID = SCH_Add_Task(Task_Command_Schedular,0,100);
    if (Com_Context->TaskID == 0xff){
        Release_Pipe(Com_Context->Arg_buf_id);
        Delete_Com_Context();
        return 0;
    }
    return 1;
}
