#include "globol.h"

//為什麼Buffer不用靜態?
//因為需要依照裝置的速度來決定buffer大小,Buffer大小不一的話,就不能在同一個結構中宣告不一樣的大小的陣列

//Why dont define buffer as static?
//I need determinate the size of buffer with speed of device.
//If the size of buffer is not fix, the array with different size doesn't be defined in the same struct.

struct BUFFER{
    BYTE    *Buffer;
    UINT    r_front,r_rear;
    UINT    Remaining_Byte;
    UINT    Length;
    BYTE    Flag;
};


#define AVAILABLE 1
#define FULL      2

#define NO_PIPE  0xff

#define PIPE_FREE 0xff    //no one use this pipe

#define BUFFER_AMOUNT 10

#define PIPE struct BUFFER

extern PIPE rb[BUFFER_AMOUNT];