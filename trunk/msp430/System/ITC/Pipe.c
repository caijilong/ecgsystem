#include "Pipe.h"


const UINT Buffer_Size[BUFFER_AMOUNT] = {16,16,16,16,64,64,128,128,256,512};
PIPE rb[BUFFER_AMOUNT];

BYTE Take_Pipe(UINT size)
{
    int i;
    for(i=0;i<BUFFER_AMOUNT;i++){
       if ((rb[i].Flag & AVAILABLE) != 0 && (rb[i].Length >= size)) break;
    }
    if (i < BUFFER_AMOUNT){
        rb[i].Flag &= ~AVAILABLE;
        return i;
    }else return NO_PIPE;
}

void Release_Pipe(BYTE rb_id)
{
   rb[rb_id].Flag |= AVAILABLE;
}
 
BYTE Get_Rear_Index(BYTE rb_id)
{
    return rb[rb_id].r_rear;
}

BYTE Get_Head_Index(BYTE rb_id)
{
    return rb[rb_id].r_front;
}

BYTE *Get_Buf_Ptr(BYTE rb_id)
{
    return rb[rb_id].Buffer;
}
 
BOOL Init_Pipe()
{
    for(int rb_id=0;rb_id<BUFFER_AMOUNT;rb_id++){
       rb[rb_id].Buffer = (BYTE *)malloc(sizeof(BYTE)*Buffer_Size[rb_id]);
       if (rb[rb_id].Buffer != 0){
           rb[rb_id].r_front=0;
           rb[rb_id].r_rear=0;
           rb[rb_id].Remaining_Byte = 0;
           rb[rb_id].Length = Buffer_Size[rb_id];
           rb[rb_id].Flag = AVAILABLE;
       }else return 0;
    }
}

void SkipByte(BYTE rb_id,BYTE skip)
{
    while(skip-- > 0 && rb[rb_id].Remaining_Byte > 0){
        rb[rb_id].r_rear++;
        (rb[rb_id].r_rear == rb[rb_id].Length) ? 0 : rb[rb_id].r_rear;
        rb[rb_id].Remaining_Byte--;
    }
}

BYTE PutByte(BYTE rb_id,BYTE d)
{
    PIPE *p;
    p = &rb[rb_id];
    p->Buffer[p->r_front] = d;
    p->r_front++;
    if (p->r_front == p->Length) p->r_front = 0;//forms a ringed buffer
    if (p->Remaining_Byte == p->Length){
        p->Flag |= FULL;
        p->r_rear = p->r_front+1 & (p->Length - 1);
    }else p->Remaining_Byte++;              //either overflow or increase Remaining_Byte    
}

BYTE PutWord(BYTE rb_id,UINT d)
{
    PIPE *p;
    BYTE *temp;
    int *temp2;
    p = &rb[rb_id];
    temp = &p->Buffer[p->r_front];
    temp2 = (int *)temp;
    *temp2 = d;
    p->r_front+=2;
    if (p->r_front == p->Length) p->r_front = 0;//forms a ringed buffer
    if (p->Remaining_Byte == p->Length){
        p->Flag |= FULL;
        p->r_rear = p->r_front+2 & (p->Length - 1);
    }else p->Remaining_Byte+=2;              //either overflow or increase Remaining_Byte   
}

/*           |<----------------------IN_BUF_LEN--------------------->|
      buffer   + + + + + + + + * * * * * * * * * * * * * - - - - - -
                  ^            ^                       ^     ^
              already read    rear                    front empty
*/

BYTE GetByte(BYTE rb_id)
{
    PIPE *p;
    p = &rb[rb_id];
    BYTE ReturnValue;
    if (p->Remaining_Byte > 0){
        ReturnValue = p->Buffer[p->r_rear++];
        if (p->r_rear == p->Length) p->r_rear = 0;//forms a ringed buffer
        p->Remaining_Byte--;
        return ReturnValue;
    }
}

UINT GetWord(BYTE rb_id)
{
    UINT ReturnValue;
    PIPE *p;
    BYTE *temp;
    int *temp2;
    p = &rb[rb_id];
    
    if (p->Remaining_Byte > 1){
        temp = &p->Buffer[p->r_rear];
        temp2 = (int *)temp;    
        ReturnValue = *temp2;
//        ReturnValue = (p->Buffer[p->r_rear+1] << 8) | p->Buffer[p->r_rear];
        p->r_rear += 2;
        if (p->r_rear == p->Length) p->r_rear = 0;//forms a ringed buffer
        p->Remaining_Byte-=2;
        return ReturnValue;
    }
}

UINT Waiting_Byte(BYTE rb_id)
{
    return rb[rb_id].Remaining_Byte;
}

UINT Remain_Space(BYTE rb_id)
{
    return  (rb[rb_id].Length - rb[rb_id].Remaining_Byte);
}

UINT Is_Full(BYTE rb_id)
{
    if (rb[rb_id].Remaining_Byte == rb[rb_id].Length) return 1;
    else return 0;

}
void SkipALL(BYTE rb_id)
{
    rb[rb_id].r_rear = rb[rb_id].r_front;
    rb[rb_id].Remaining_Byte = 0;
}
