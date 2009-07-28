#pragma once
#include "drawwave.h"


class CECGFeature :	public CDrawWave
{
public:
	//CDrawWave(CRect *rect,COLORREF color,UINT type);
	CECGFeature(CRect *rect,COLORREF color,UINT type,int sampling_rate);
	void DetectECGFeature(float *detect_buf,int DetectLen);
	void DrawFeature(CDC *dcMem);
	int GetRPos(float *buf,int len);
	int GetMaxPos(float *yn2);
	int DisplaySlot,DisplaySlotNum;
	class FeatureLoc{
	public :
		CPoint P;
		CPoint Q;
		CPoint R;
		CPoint S;
		CPoint T;
	};
	float QTInterval,STInterval,QRSInterval,PSInterval,PRInterval;
	FeatureLoc location;
private:
	void Convolution(float *xn,float *yn,float *hn,int hn_len,int xn_len);
	int Sampling_Rate;
public:
	~CECGFeature(void);
};
