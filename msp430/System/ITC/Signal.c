#include "Signal.h"

void (*Dest_fp[SIG_NUM])(void)={0};   // There are 5 signal for using


BYTE Take_Signal(BYTE SigID,void (*f)(void))
{
  if (Dest_fp[SigID] == 0){
    Dest_fp[SigID] = f;
    return 1;
  }else{
    return 0;
  }
}

void Send_Signal(UINT SigID)
{
  if (Dest_fp[SigID] != 0){
    Dest_fp[SigID]();
  }
}

BYTE Release_Signal(BYTE SigID)
{
  if (Dest_fp[SigID] == 0){
    return 0;
  }else{
    Dest_fp[SigID] = 0;
    return 1;
  }
}
