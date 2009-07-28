
#include "Filter.h"

#define SMOOTH_FILTER_LEN	8
#define RANG_BUFFER_SIZE	512

#define ANA_TASK_BEAT_DETECT	1
#define ANA_TASK_TEMPLETE		2
#define ANA_TASK_DEBUG_VIEW		4
#define ANA_TASK_BEAT_DETECT_NODIS		8
#define ANA_TASK_FFT			16
#define ANA_TASK_HRV			32
/*
class CMeanDeviation
{
public:

	CMeanDeviation(float *buffer,int MaxBuffer);
	void InsertValue(float value);  //return true of false for present value is over mean or not
	bool IsOverMean(float value);
	int Max,Min;
	int Index;
	float *MAbuffer;  //Moving average buffer
	float mean;
	void MeanCalculation();
	~CMeanDeviation();

private:
	int max_buffer;
};*/


class CPeakDetection
{
public:

//Peak Detection
	~CPeakDetection(void);
	CPeakDetection(int s,int dis_buffer_size,int BeatAverageTimes,int Resolution);
								//S: 取樣頻率
								//dis_buffer_size: 顯示心電圖畫面的rect.width()大小
								//BeatAverageTimes: 心電圖的平均次數,設1即可

	int		BeatNumber;			//累計心跳
	float	BeatPerMin;			//每分鐘心跳
	float	BeatInterval;		//心跳的間隔時間(秒) 
	int		IsBeatOccur();		//呼叫 InsertValue之後若有心跳發生則會傳回心跳位置 
	void	InsertValue(int sample);			//將每次取樣數值丟入並使用IsBeatOccur();取樣數值丟t
												//查看是否有心跳
	UINT	HighFreqComp;				//取得高頻成分
	void 	ChangeThreshold(float thres);	//設定筏值,
	UINT	GetThreshold();				//取得目前的筏值
	int  	Is_Templet_Finish();		//傳回是否已將QRS波丟入Peak_Shape
	bool	IsTimeStable();

	UINT HFComponentMax,HFComponentMin; //高頻成分的最大值與最小值
	int HFMaxMinDiff;
	int  Templet_max_count;				//每個心電圖的取樣個數,為取樣頻率*0.8
	UINT BeatAverageTimes;				//儲存心電圖的個數
	UINT TempleteIndex;					//目前抓取到的心電圖的儲存位置
										//Peak_Shape[TempleteIndex][i]
	float Decision;
	int 	**Peak_Shape;					//儲存心電圖的矩陣
	float		*BeatAverage();					//平均目前的心跳樣板
	int HFmean;
	float BeatInterval_ms;
	float DecisionThreshold;
	int PeakPosIndex,PeakAmount;
	int FindingPosition(int RPos);
	int GetPeakPosition();
	bool	IsTempleteStable();
	int GetPeakDeviation();				//若有心跳則會傳回與目前偵測到R波最高點與目前收到sample位置的差
	int PeakDeviation;
	int *TraceWindowSlot;
	int HFMaxDiffBack,HFMaxDiffFront,DiffFrontAdd,DiffFrontAdd_Lag1;
	float SlidingWindowThreshold;
	float MaxMinDiffThreshold;
	int TimeFactor,max_count;
private:

//Peak Detection
	int sample_counter;
	int Resolution;
	int BeatPosition;
	float MaxMinThreshold;
	int *Rangbuffer;
	int Piezo_sb_Count,Piezo_Window_Skip,Crossing_Up,Crossing_Down,*Shift_buffer,*Window_buffer;
	UINT Window_Size,Shift_Window_Size,Group_Delay;
	UINT Threshold;
	float BeatInterval_average;
	int sample_count,SystemTick;
	bool BeatPresent;
	int DetectionRoutine();
	int Sampling_Rate;
	int Normalize,UpDownCount;
	int FindPeakPosition(int stable);
	bool MayBePeak(int mean);
//	CMeanDeviation *HFComponentMean;
//	CMeanDeviation *TempleteMean;
//Templete
	int Shape_Count,UpdatePeakShapFinish;
	UINT BeatQueue[3],BeatQueue_f,BeatQueue_r,BeatNum;
	float *Beat_Average;
	void AverageTemplete();
	bool IsTempleteMatch();
	bool TempleteStable;
	void FindTemplete(int BeatPos);
//Time mean
	bool TimeStable;
	int TimeMeanIndex,TimeStableCount;
	float TimeMeanBuf[5],TimeMean;
	int Max0_2,MaxLeast0_2,MaxFuture0_2;
	
	int RRInterval_Index,RRInterval[512];

//Adjestable threshold

	float	*TempleteBuffer;
	float   *Decision_Window_buffer,DecisionMax,DecisionMin;
	float	DecisionMean(int diff);
	bool	IsQRS(int mean);
	float	*DecisionMeanBuffer;
	int		Decision_Crossing_Up,Decision_Crossing_Down,DecisionMeanIndex;
//Mean of MaxMin diff
	float	DiffMean(int diff);
	int		*DiffMeanBuffer,DiffMeanIndex;
//Filter


};

