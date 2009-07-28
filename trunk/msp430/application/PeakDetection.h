#include "globol.h"

#define SAMPLING_RATE         360
#define SMOOTH_FILTER_LEN     8
#define WINDOW	              SAMPLING_RATE/3
#define THRES_VAR     	      150
#define GROUP_DELAY	      6


struct PD{
	UINT Piezo_Window_Skip,SlidingWindowThreshold,max_count,TimeFactor,DiffFrontAdd_Lag1,ShiftWindowCount;
	BYTE BufID,FlashTimerID;
        int Shift_buffer[SMOOTH_FILTER_LEN],Window_buffer[WINDOW];
};