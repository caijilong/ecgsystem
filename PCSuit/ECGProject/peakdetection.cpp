#include "stdafx.h"
#include "peakdetection.h"
#include "math.h"

/*
CMeanDeviation::CMeanDeviation(float *buffer,int MaxBuffer)
{
	max_buffer = MaxBuffer;
	if (buffer != 0) MAbuffer = buffer;
	else MAbuffer = new float[max_buffer];
}

void CMeanDeviation::MeanCalculation()
{
	mean = 0;
	for (int i=0;i<max_buffer;i++)
		mean += MAbuffer[i];
	mean /= max_buffer;
}

void CMeanDeviation::InsertValue(float value)
{
	MAbuffer[Index++] = value;
	if (Index == max_buffer) Index = 0;
}

bool CMeanDeviation::IsOverMean(float value)
{
	if (value > mean*3) return 1;
	else return 0;
}

CMeanDeviation::~CMeanDeviation()
{
	
}
*/

CPeakDetection::CPeakDetection(int s,int dis_buffer_size,int BAT,int Res)
{
	UINT i;
		Resolution = Res;
		BeatPerMin = 0;
		BeatInterval = 0;
		BeatNumber = 0;
		Piezo_Window_Skip=0;
		Crossing_Down = 0;
		Crossing_Up = 0;
		sample_count = 0;
		Sampling_Rate = s;
		BeatAverageTimes = BAT;
		Window_Size = int(Sampling_Rate * 0.3);
		Shift_Window_Size = Window_Size + 8;
		MaxMinThreshold = 100;
//		Group_Delay = 	Window_Size/2+1;
		Group_Delay = 	6;
		max_count = dis_buffer_size;
		Templet_max_count = Sampling_Rate*0.8;
		TempleteIndex = 0;
		BeatInterval_average = 0;
		Rangbuffer = new int[max_count];
		Shift_buffer = new int[Shift_Window_Size];
		Window_buffer = new int[Window_Size];
		BeatQueue_f=0;BeatQueue_r=0;BeatNum=0;

		Peak_Shape = new int* [BeatAverageTimes];
		Beat_Average = new float[Templet_max_count];
		TempleteBuffer = new float[Templet_max_count];

		DiffMeanBuffer = new int[Sampling_Rate];
		for(i=0;i<BeatAverageTimes;i++)
			Peak_Shape[i] = new int[Templet_max_count];
		Decision_Window_buffer = new float[Window_Size];
		DecisionMeanBuffer = new float[Sampling_Rate];
	//	HFComponentMean = new CMeanDeviation((float *)Window_buffer,Window_Size);
	//	TempleteMean = new CMeanDeviation(0,Sampling_Rate/2);
		TraceWindowSlot = new int[dis_buffer_size];

		for (i=0;i<max_count;i++) Rangbuffer[i] = 0;
		for (i=0;i<Window_Size;i++){
			Window_buffer[i] = 0;
			Decision_Window_buffer[i] = 0;
		}
		for (i=0;i<Shift_Window_Size;i++) Shift_buffer[i] = 0;
		for(int j=0;j<BeatAverageTimes;j++)
			for (i=0;i<Templet_max_count;i++) Peak_Shape[j][i] = 0;
		for (i=0;i<Templet_max_count;i++){
			Beat_Average[i] = 0;
			TempleteBuffer[i] = 0;
		}
		for (i=0;i<Sampling_Rate;i++){
			DiffMeanBuffer[i]=0;
			DecisionMeanBuffer[i] = 0;
		}
		for (i=0;i<dis_buffer_size;i++){
			TraceWindowSlot[i] = 0;
		}
		for(i=0;i<5;i++) TimeMeanBuf[i]=0;
		for(i=0;i<512;i++) RRInterval[i] = 0;
		RRInterval_Index = 0;
		TempleteStable=0;
		DiffMeanIndex = 0;
		TimeStable = 1;
		TimeMeanIndex = 0;
		Decision_Crossing_Up=0;Decision_Crossing_Down=0,DecisionMeanIndex=0;
		PeakPosIndex = 0;
		PeakAmount = 0;
		TimeStableCount = 0;
		sample_counter=0;
		TimeMean = 0;
		MaxMinDiffThreshold = 1000;
		TimeFactor = 0;
		SlidingWindowThreshold = 500;
		DiffFrontAdd_Lag1 = 0;
}

CPeakDetection::~CPeakDetection(void)
{
		delete [] Rangbuffer;
		delete [] Shift_buffer;
		delete [] Window_buffer;
		for(int i=0;i<BeatAverageTimes;i++)
			delete [] Peak_Shape[i];
		delete [] Peak_Shape;
		delete [] Beat_Average;
		delete [] Decision_Window_buffer;
		delete [] DecisionMeanBuffer;
		delete [] TempleteBuffer;
		delete [] DiffMeanBuffer;
		delete [] TraceWindowSlot;
}

float *CPeakDetection::BeatAverage()
{
	return Beat_Average;
}

void CPeakDetection::ChangeThreshold(float thres)
{
	this->MaxMinThreshold = thres;
}


int  CPeakDetection::Is_Templet_Finish()
{
	int i = UpdatePeakShapFinish;
	UpdatePeakShapFinish = 0;
	return i;
}

int CPeakDetection::GetPeakDeviation()
{
	return PeakDeviation;
}

void CPeakDetection::FindTemplete(int BeatPos)		//Find 0.8s around R peak and saves templete to Peak_Shape
{
int interval,index,i;
	interval = (0.8 * Sampling_Rate);
	index = BeatPos - interval*2/5;//Pop the beat position from queue
												//and estimate the p wave position
	if (index < 0) index += max_count;

	for(i=0 ; i<interval ; i++){
		if (i == Templet_max_count) break;
		Peak_Shape[TempleteIndex][i] = Rangbuffer[index];
		index++;
		if (index == max_count) index = 0;
	}
}


void CPeakDetection::InsertValue(int sample)
{
	Rangbuffer[sample_count++] = sample;
	if (sample_count == max_count) sample_count = 0;
	sample_counter++;
	BeatPosition = DetectionRoutine();

	if (BeatPosition > -1){
		BeatPresent = 1;

		PeakDeviation = sample_count - BeatPosition;
		if (PeakDeviation < 0) PeakDeviation += max_count;

		if (BeatNum > 1){

				FindTemplete(BeatQueue[BeatQueue_r]);

				BeatQueue_r++;
				if (BeatQueue_r == 3) BeatQueue_r = 0;
				BeatNum--;
				TempleteIndex++;
				if (TempleteIndex == BeatAverageTimes) TempleteIndex = 0;
					UpdatePeakShapFinish = 1;
		}
	}
}

int CPeakDetection::IsBeatOccur()
{
	if (BeatPresent == 1){
		BeatPresent = 0;
		return BeatPosition;
	}else return -1;
}

bool CPeakDetection::IsTempleteStable()
{
	int i = TempleteStable;
		//TempleteStable = 0;
	return i;
}

float CPeakDetection::DiffMean(int diff)
{
	float mean;
	int MeanLen;
	mean = 0;
	MeanLen = Sampling_Rate/2;
	DiffMeanBuffer[DiffMeanIndex++] = diff;
	if (DiffMeanIndex == MeanLen) DiffMeanIndex = 0;
	for (int i=0;i<MeanLen;i++) mean += DiffMeanBuffer[i];
	mean /= MeanLen;
	return mean;
}

int CPeakDetection::FindingPosition(int RPos)
{
	int index,interval,i,j,Origin_Max = 0;
	int Max_Pos;


	interval = float(Sampling_Rate)*0.3;

	index = RPos - interval;
	if (index < 0) index += max_count;
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
}

int CPeakDetection::FindPeakPosition(int stable)
{
	int Max_Pos,j,i;
	int RPos;

	if (stable){
		j = sample_count - Templet_max_count/2 - Group_Delay;
		if (j < 0) j +=	max_count;//j is the position of the highest diferential
		RPos = j;
	}else{
		RPos = sample_count;
	}

	Max_Pos = FindingPosition(RPos);

//	Piezo_Window_Skip = this->Window_Size*0.2;
	Piezo_Window_Skip = this->Sampling_Rate*0.1;
//	Piezo_Window_Skip = (this->Sampling_Rate * this->TimeMean)/3;

		if (BeatNumber % 5 == 0) {
			BeatPerMin = BeatInterval_average / 5;
			BeatInterval_average = 0;
		}
		BeatInterval_ms = float(sample_counter) / Sampling_Rate;
		BeatInterval = BeatInterval_ms;
		BeatInterval_average += (60 / BeatInterval);
		sample_counter = 0;
/*	if (BeatInterval > 1){//use for templete check
		if (IsTempleteStable() && IsTempleteMatch()){
			int index = sample_count - Templet_max_count - Sampling_Rate;
			if (index < 0) index += max_count;
			int MaxPosition = FindingPosition(index);
			BeatNum++;
			BeatQueue[BeatQueue_f++] = MaxPosition;		//Push the beat position in queue
			if (BeatQueue_f == 3) BeatQueue_f = 0;

		}
	}*/
	SystemTick = GetTickCount();

	TimeMean = 0;
	float e;
	int index = TimeMeanIndex - 1;
	if (index < 0) index+=5;
	//e = TimeMeanBuf[TimeMeanIndex] - TimeMeanBuf[index];
//	e = TimeMeanBuf[TimeMeanIndex] - 80.0f/60;
//	MaxMinDiffThreshold -= (500*e);
	TimeFactor = 200;
	for(i=0;i<5;i++) TimeMean += TimeMeanBuf[i];
	TimeMean /= 5;
	if (abs(BeatInterval - TimeMean) > 0.5){
		TimeStable = 0;
		TimeStableCount = 0;
	}else {
//		TimeStable = 1;
		if (TimeStableCount < 5) TimeStableCount++;
	}

	TimeMeanBuf[TimeMeanIndex++] = BeatInterval;

	RRInterval[RRInterval_Index++] = BeatInterval;

	if (TimeMeanIndex == 5) TimeMeanIndex = 0;
	//TimeStable = 1;
	if (TimeStableCount == 5){
		BeatNumber++;
		BeatNum++;
		BeatQueue[BeatQueue_f++] = Max_Pos;		//Push the beat position in queue
		if (BeatQueue_f == 3) BeatQueue_f = 0;
		return Max_Pos;
	}else return -1;

}

int CPeakDetection::GetPeakPosition()
{

	if (PeakPosIndex == BeatQueue_f){
		 return -1;
	}else {
		int ret = BeatQueue[PeakPosIndex];
		PeakPosIndex++;
		if (PeakPosIndex == 3) PeakPosIndex = 0;
		return ret;
	}
}

void CPeakDetection::AverageTemplete()
{
	float f;
	for (int i=0;i<Templet_max_count; i++) Beat_Average[i] = 0;
	for(int j=0;j<BeatAverageTimes;j++){
		for (int i=0;i<Templet_max_count; i++){
			f = float(Peak_Shape[j][i])/Resolution;
			Beat_Average[i] += (f/BeatAverageTimes);
		}
	}

	float Variability=0;

	for (int i=0;i<Templet_max_count; i++){
		if (TempleteBuffer > 0){
			Variability += abs(TempleteBuffer[i] - Beat_Average[i]);
			TempleteBuffer[i] = Beat_Average[i];
		}
	}
	if (Variability < 2.5 && BeatNumber > 30) 
		TempleteStable = 1;
	else 
		TempleteStable = 0;
}


bool CPeakDetection::IsQRS(int mean)
{
				int DecisionMaxPos;

				for(DecisionMaxPos =  0 ; DecisionMaxPos < Window_Size ; DecisionMaxPos++){  //MaxPos is sweep from window_buffer[0]
					if (Window_buffer[DecisionMaxPos] == DecisionMax) break;
				}

				if ((DecisionMax) > mean*2/* && DecisionMaxPos == 0*/){
						DecisionThreshold = DecisionMax*0.8;

						if (Decision_Window_buffer[0] > DecisionThreshold){		//The up position
							if (Decision_Crossing_Down == 0) Decision_Crossing_Up = sample_count; 
							Decision_Crossing_Down = sample_count;				
						}else{									//find the down position
							if (Decision_Crossing_Up != 0){

								Decision_Crossing_Up = 0;
								Decision_Crossing_Down = 0;
								
								return 1;
							}
						}
				}else{
					Decision_Crossing_Up = 0;
					Decision_Crossing_Down = 0;
				}
		return 0;
}

float CPeakDetection::DecisionMean(int diff)
{
	float mean;
	int MeanLen;
	mean = 0;
	MeanLen = Sampling_Rate/2;
	DecisionMeanBuffer[DecisionMeanIndex++] = diff;
	if (DecisionMeanIndex == MeanLen) DecisionMeanIndex = 0;
	for (int i=0;i<MeanLen;i++) mean += DecisionMeanBuffer[i];
	mean /= MeanLen;
	return mean;
}


bool CPeakDetection::IsTempleteMatch()
{

	int interval,index,i;

	Decision = 0;
	index = sample_count - Templet_max_count - Sampling_Rate;
	if (index < 0) index += max_count;

	for(i=0;i<Templet_max_count;i++){
		Decision += (Beat_Average[i] * (float(Rangbuffer[index]) / Resolution));
		index++;
		if (index == max_count) index = 0;
	}

		UINT Max=0,Min=65536;
		DecisionMax=0;
		DecisionMin=65536;
		for(i =  Window_Size-1 ; i > 0 ; i--){
			Decision_Window_buffer[i] = Decision_Window_buffer[i-1];
			DecisionMax = Decision_Window_buffer[i-1] > DecisionMax ? Decision_Window_buffer[i-1]:DecisionMax;
			DecisionMin = Decision_Window_buffer[i-1] < DecisionMin ? Decision_Window_buffer[i-1]:DecisionMin;
		}
		Decision_Window_buffer[0] = Decision;

		DecisionMax = Decision_Window_buffer[i-1] > DecisionMax ? Decision_Window_buffer[i-1]:DecisionMax;
		DecisionMin = Decision_Window_buffer[i-1] < DecisionMin ? Decision_Window_buffer[i-1]:DecisionMin;

		float DMean = DecisionMean(DecisionMax - DecisionMin);
		if (IsQRS(DMean))
			return 1;
		else return 0;
}

bool CPeakDetection::IsTimeStable()
{
	if (TimeStableCount == 8) return 1;
	else return 0;
//	return 1;
//	return TimeStable;
}

int CPeakDetection::DetectionRoutine()		//return the peak position of display
{
	    int i,j;
		int DiffBuf[5];
		if (sample_count == 0) j = max_count-1;
		else j = sample_count-1;

		for(i=4;i>=0;i--){
			DiffBuf[i] = Rangbuffer[j];
			j--;
			if (j < 0) j = max_count-1;
		}
		for(i = SMOOTH_FILTER_LEN-1 ; i > 0 ; i--){
			Shift_buffer[i] = Shift_buffer[i-1];
		}

//		Shift_buffer[0] = abs(-0.25*DiffBuf[0] + 0.5*DiffBuf[1] + 1.5*DiffBuf[2] + 0.5*DiffBuf[3] - 0.25*DiffBuf[4]);
		Shift_buffer[0] = abs(-DiffBuf[0] - 2*DiffBuf[1] + 2*DiffBuf[3] + DiffBuf[4]);

		UINT Max=0,Min=65536;
		HFComponentMax=0;
		Max0_2=0;
		MaxLeast0_2=0;
		MaxFuture0_2 = 0;
		HFComponentMin=65536;
		for(i =  Window_Size-1 ; i > 0 ; i--){
			Window_buffer[i] = Window_buffer[i-1];

			HFComponentMax = Window_buffer[i-1] > HFComponentMax ? Window_buffer[i-1]:HFComponentMax;
			HFComponentMin = Window_buffer[i-1] < HFComponentMin ? Window_buffer[i-1]:HFComponentMin;
		}

		Window_buffer[0] = 0;
		for (j = 0 ; j < SMOOTH_FILTER_LEN ; j++) Window_buffer[0] += Shift_buffer[j];				//Sum the value in window				Window_buffer[i] +=  Shift_buffer[j];
		Window_buffer[0] >>= 3;							//Divided the sum by 8

		HighFreqComp = Window_buffer[0];

				int Interval;
				Interval = float(this->Sampling_Rate)*0.1;
				for(i=0;i<Interval;i++){		//Find max in first 0.2s in window
					MaxFuture0_2 = Window_buffer[i] > MaxFuture0_2 ? Window_buffer[i]:MaxFuture0_2;
				}
				while( i++ < Interval*2){		//Find max in first 0.2s in window
					Max0_2 = Window_buffer[i] > Max0_2 ? Window_buffer[i]:Max0_2;
				}
				while(i++ < Window_Size){		//Find max in first 0.2s in window
					MaxLeast0_2 = Window_buffer[i] > MaxLeast0_2 ? Window_buffer[i]:MaxLeast0_2;
				}



		DecisionThreshold = 0;
		HFmean=0;

		//for (i=0;i<Window_Size;i++) HFmean += Window_buffer[i];
		//HFmean /= Window_Size;
	//	HFMaxMinDiff = HFComponentMax - HFComponentMin;
	//	float MaxMinmean = DiffMean(HFMaxMinDiff);
		TraceWindowSlot[sample_count] = 0;

		HFMaxDiffBack = Max0_2 - MaxLeast0_2;
		HFMaxDiffFront = Max0_2 - MaxFuture0_2;

		DiffFrontAdd = HFMaxDiffFront + HFMaxDiffBack;

		if (Piezo_Window_Skip > 0 && DiffFrontAdd_Lag1 < DiffFrontAdd && DiffFrontAdd > 500)
			SlidingWindowThreshold = DiffFrontAdd;
		else
			if (SlidingWindowThreshold > 500) SlidingWindowThreshold -= (SlidingWindowThreshold/(TimeFactor++));
		
		DiffFrontAdd_Lag1 = DiffFrontAdd;

		if (DiffFrontAdd > 0){
//		DiffMean(HFMaxDiffBack);
//		SlidingWindowThreshold = DiffMean(HFMaxDiffFront);

			if (Piezo_Window_Skip == 0){

					if (MayBePeak(SlidingWindowThreshold) && SlidingWindowThreshold < 7000){
						int Max_Pos;
						Max_Pos = FindPeakPosition(0);
					
//						SlidingWindowThreshold = (HFMaxDiffBack + HFMaxDiffFront)/2 > 7000? 4000:(HFMaxDiffBack + HFMaxDiffFront)/2;
						//if (!TempleteStable) 
						AverageTemplete();

						return Max_Pos;

					}
			}else{
				Piezo_Window_Skip--;
				//Decision = 0;
			}
		}
		return -1;
}


bool CPeakDetection::MayBePeak(int mean)
{

				int i,HFComponentMaxPos;


//				for(HFComponentMaxPos =  0 ; HFComponentMaxPos < Window_Size ; HFComponentMaxPos++){  //MaxPos is sweep from window_buffer[0]
//					if (Window_buffer[HFComponentMaxPos] == HFComponentMax) break;
//				}


					int interval = Sampling_Rate*0.1;
					int pos;

					if (DiffFrontAdd > mean){

						pos = sample_count;
						TraceWindowSlot[pos] = -1;
							
						pos = sample_count - interval;
						if (pos < 0) pos += this->max_count;
						TraceWindowSlot[pos] = -1;

						pos = sample_count - interval*2;
						if (pos < 0) pos += this->max_count;
						TraceWindowSlot[pos] = -1;

						pos = sample_count - interval*3;
						if (pos < 0) pos += this->max_count;
						TraceWindowSlot[pos] = -1;

						Crossing_Up = 0;

						return 1;
					}

					/*
						Threshold = HFComponentMax*0.8;
						int delay_count = sample_count - Sampling_Rate*0.1;
						if (delay_count < 0) delay_count = this->max_count;

						if (Window_buffer[(int)(Sampling_Rate*0.1)] > Threshold){		//The up position
							if (Crossing_Down == 0) Crossing_Up = delay_count; 
							Crossing_Down = delay_count;				
						}else{									//find the down position
							if (Crossing_Up != 0){

								Crossing_Up = 0;
								Crossing_Down = 0;
								
								return 1;
							}
						}*/
			//  }else{
			//		Crossing_Up = 0;
			//		Crossing_Down = 0;
			//	}
		return 0;
}
