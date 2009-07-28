#include "PeakDetection.h"
#include "System/IO/PortManager.h"

struct PD *PD_Context;

const int diff[5] = {-1,-2,0,2,1};
//public
BOOL Init_PeakDetection()        //Before peak detection, this initial process must invoked
{
    PD_Context =  (struct PD *)malloc(sizeof(struct PD));
    if (PD_Context == 0) return 0;
    else{
        PD_Context->Piezo_Window_Skip=0;
        PD_Context->SlidingWindowThreshold = 500;
        PD_Context->max_count = 8;                  //max_count is the size of pipe for insert_value
        PD_Context->TimeFactor = 200;
        PD_Context->DiffFrontAdd_Lag1 = 0;
        PD_Context->BufID = Take_Pipe(16);
        PD_Context->ShiftWindowCount = 0;
        
        int i;
        for(i = 0;i < WINDOW;i++) PD_Context->Window_buffer[i] = 0;
        for(i = 0;i < SMOOTH_FILTER_LEN ;i++) PD_Context->Shift_buffer[i] = 0;

        return 1;
    }
}

//public
BOOL Delete_PeakDetection()
{
    if (PD_Context != 0){
        free(PD_Context);
        Release_Pipe(PD_Context->BufID);
        PD_Context = 0;
        return 1;
    }else return 0;
}

/*
int FindingPosition(int RPos)
{
	int index,interval,i,j,Origin_Max = 0;
	int Max_Pos;

	interval = float(Sampling_Rate)*0.3;

	index = RPos - interval;
	if (index < 0) index += PD_Context->max_count;
	for(i = 0 ; i < interval ; i++){			//Smooth the signal of diff. in window length
		if (index == RPos) break;
		Origin_Max = abs(Rangbuffer[index]) > Origin_Max ? abs(Rangbuffer[index]) : Origin_Max;
		index++;
		if (index == max_count) index = 0;
	}

	index = RPos - interval;
	if (index < 0) index += max_count;

	for(i = 0 ; i < interval ; i++){			//Smooth the signal of diff. in window length
		if (index == RPos) break;
		if (abs(Rangbuffer[index]) == Origin_Max){
			Max_Pos = index;
			break;
		}
		index++;
		if (index == max_count) index = 0;
	}
	return Max_Pos;
}*/

void LEDFlash()
{
P4OUT &= ~PEAD_DETECTION;
P4DIR &= ~PEAD_DETECTION;
}

/*
*   RangSample is the sample saved in Rangbuffer from ADC
*/
//public
BOOL DetectionRoutine()
{
    int i,j,*pShift_buffer,*pWindow_buffer,*pPipe,sample_count;

    if (PD_Context == 0) return 0;
    sample_count = Get_Head_Index(PD_Context->BufID) >> 1;
    pPipe = (int *)Get_Buf_Ptr(PD_Context->BufID);
    pShift_buffer = PD_Context->Shift_buffer;
    pWindow_buffer = PD_Context->Window_buffer;
    int Diff_Value;
    if (sample_count == 0) j = PD_Context->max_count-1;
    else j = sample_count-1;
    for(i=4;i>=0;i--){
        Diff_Value = pPipe[j] * diff[i];
        j--;
        if (j < 0) j = PD_Context->max_count-1;
    }
//    for(i = SMOOTH_FILTER_LEN-1 ; i > 0 ; i--) pShift_buffer[i] = pShift_buffer[i-1];
        pShift_buffer[PD_Context->ShiftWindowCount++] = abs(Diff_Value);
        if (PD_Context->ShiftWindowCount == SMOOTH_FILTER_LEN) PD_Context->ShiftWindowCount = 0;
	int Max0_2=0;
	int MaxLeast0_2 = 0;
	int MaxFuture0_2 = 0;

	for(i =  WINDOW-1 ; i > 0 ; i--) pWindow_buffer[i] = pWindow_buffer[i-1];

	pWindow_buffer[0] = 0;
	i=PD_Context->ShiftWindowCount-1;
	int SmoothValue=0;
	for (j = 0 ; j < SMOOTH_FILTER_LEN ; j++) SmoothValue += pShift_buffer[j];

        pWindow_buffer[0] = SmoothValue >> 3;	//Divided the sum by 8
		
	int Interval;
	Interval = SAMPLING_RATE*0.1;
	for(i=0;i<Interval;i++){		//Find max in first 0.2s in window
		MaxFuture0_2 = pWindow_buffer[i] > MaxFuture0_2 ? pWindow_buffer[i]:MaxFuture0_2;
	}
	while(i < Interval*2){		//Find max in first 0.2s in window
	  Max0_2 = pWindow_buffer[i] > Max0_2 ? pWindow_buffer[i]:Max0_2;
	  i++;
	}
	while(i < WINDOW){		//Find max in first 0.2s in window
	  MaxLeast0_2 = pWindow_buffer[i] > MaxLeast0_2 ? pWindow_buffer[i]:MaxLeast0_2;
	  i++;
        }
        int DiffFrontAdd;
	DiffFrontAdd = (Max0_2 << 1) - (MaxLeast0_2 - MaxFuture0_2);
        if (PD_Context->Piezo_Window_Skip > 0 && PD_Context->DiffFrontAdd_Lag1 < DiffFrontAdd && DiffFrontAdd > 800)
	    PD_Context->SlidingWindowThreshold = DiffFrontAdd;
	else
	    if (PD_Context->SlidingWindowThreshold > 800) PD_Context->SlidingWindowThreshold -= (PD_Context->SlidingWindowThreshold/(PD_Context->TimeFactor++));
		
        PD_Context->DiffFrontAdd_Lag1 = DiffFrontAdd;
        if (DiffFrontAdd > 0){
    	    if (PD_Context->Piezo_Window_Skip == 0){       	    
	        if (DiffFrontAdd > PD_Context->SlidingWindowThreshold && PD_Context->SlidingWindowThreshold < 7000){
//		    int Max_Pos;
//		    Max_Pos = FindPeakPosition(0);
                    PD_Context->TimeFactor = 200;
                    PD_Context->Piezo_Window_Skip = SAMPLING_RATE*0.3;
                    PD_Context->FlashTimerID = TakeTimer(RANDOM,327,SINGLE,LEDFlash);
                    P4DIR |= PEAD_DETECTION;
                    P4OUT |= PEAD_DETECTION;

    		    return 1;
    		}
            }else{
                PD_Context->Piezo_Window_Skip--;
            }
        }
	return -1;
}


BOOL Insert_Value(int sample)
{
    PutWord(PD_Context->BufID,sample);
    return DetectionRoutine();
}