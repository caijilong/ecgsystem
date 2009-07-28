#include "stdafx.h"
#include "Filter.h"
#include "math.h"

CFilter::CFilter(int SamplingRate)
{
	//LoadFilterCoeff();
	sampling_rate = SamplingRate;

	Filter_Buffer_40 = new float[BUFFER_SIZE];
	Filter_Buffer_60 = new float[BUFFER_SIZE];
	Filter_Buffer_0_05 = new float[BUFFER_SIZE];

	for (int i=0;i<BUFFER_SIZE;i++) Filter_Buffer_40[i] = 0;
	for (int i=0;i<BUFFER_SIZE;i++) Filter_Buffer_60[i] = 0;
	for (int i=0;i<BUFFER_SIZE;i++) Filter_Buffer_0_05[i] = 0;
}

CFilter::~CFilter(void)
{
	if (Filter_Buffer_40 != 0){
		delete Filter_Buffer_40;
		delete Filter_Buffer_60;
		delete Filter_Buffer_0_05;
	}
}
float CFilter::FixFilter(int samp)
{
	double coeff[251] = {0.019573,-0.000485,0.000190,0.000529,-0.000401,0.002838,-0.000750,-0.000550,0.001138,0.000070,0.002687,-0.001686,-0.001029,
0.002653,0.000193,0.001208,-0.002447,-0.000144,0.004206,-0.001384,-0.000856,-0.001354,0.001518,0.003792,-0.004342,-0.001247,0.001642,
0.001407,0.000896,-0.006011,0.001030,0.003825,-0.001943,-0.001853,-0.004201,0.003550,0.002454,-0.006492,-0.001253,-0.000543,0.002621,
-0.001372,-0.008187,0.002208,0.000765,-0.001923,-0.003409,-0.006166,0.004322,-0.001716,-0.006085,-0.001650,-0.004097,0.002596,-0.004621,
-0.007022,0.000940,-0.004993,-0.000400,-0.004768,-0.006991,0.001114,-0.006792,-0.001450,-0.004037,-0.009176,0.000752,-0.006173,-0.002577,
-0.005833,-0.011358,0.003552,-0.005518,-0.007979,-0.007502,-0.008616,0.006726,-0.010560,-0.014953,-0.002137,-0.003346,0.002106,-0.019414,
-0.014052,0.009436,-0.006298,-0.010899,-0.019996,-0.002926,0.014289,-0.022074,-0.018758,-0.004960,0.004636,0.002331,-0.037352,-0.008358,
0.013696,-0.007788,-0.015976,-0.032885,0.012349,0.014418,-0.035932,-0.016456,-0.009321,0.017617,-0.005887,-0.054363,0.009824,0.007516,
-0.006616,-0.021109,-0.046761,0.041946,-0.005078,-0.043679,0.000824,-0.035279,0.056060,-0.037059,-0.075572,0.095819,-0.133451,0.183610,
0.745942,0.183610,-0.133451,0.095819,-0.075572,-0.037059,0.056060,-0.035279,0.000824,-0.043679,-0.005078,0.041946,-0.046761,-0.021109,
-0.006616,0.007516,0.009824,-0.054363,-0.005887,0.017617,-0.009321,-0.016456,-0.035932,0.014418,0.012349,-0.032885,-0.015976,-0.007788,
0.013696,-0.008358,-0.037352,0.002331,0.004636,-0.004960,-0.018758,-0.022074,0.014289,-0.002926,-0.019996,-0.010899,-0.006298,0.009436,
-0.014052,-0.019414,0.002106,-0.003346,-0.002137,-0.014953,-0.010560,0.006726,-0.008616,-0.007502,-0.007979,-0.005518,0.003552,-0.011358,
-0.005833,-0.002577,-0.006173,0.000752,-0.009176,-0.004037,-0.001450,-0.006792,0.001114,-0.006991,-0.004768,-0.000400,-0.004993,0.000940,
-0.007022,-0.004621,0.002596,-0.004097,-0.001650,-0.006085,-0.001716,0.004322,-0.006166,-0.003409,-0.001923,0.000765,0.002208,-0.008187,
-0.001372,0.002621,-0.000543,-0.001253,-0.006492,0.002454,0.003550,-0.004201,-0.001853,-0.001943,0.003825,0.001030,-0.006011,0.000896,
0.001407,0.001642,-0.001247,-0.004342,0.003792,0.001518,-0.001354,-0.000856,-0.001384,0.004206,-0.000144,-0.002447,0.001208,0.000193,
0.002653,-0.001029,-0.001686,0.002687,0.000070,0.001138,-0.000550,-0.000750,0.002838,-0.000401,0.000529,0.000190,-0.000485,0.019573};
	float ret;
	for (int i=BUFFER_SIZE-1;i>0;i--) Filter_Buffer_40[i] = Filter_Buffer_40[i-1];
	Filter_Buffer_40[0] = samp;
	ret = RTConvolution(Filter_Buffer_40,(float *)coeff,251);
	return ret;
}
void CFilter::LoadFilterCoeff(int sampling_rate)
{
	Coeff_40_LP = new float[101];
	Coeff_60Notch = new float[301];
	Coeff_0_05_HP = new float[371];
	CreatFIRCoeff(Coeff_40_LP,50,sampling_rate,101,LP);
	CreatFIRCoeff(Coeff_60Notch,60,sampling_rate,301,NOTCH);
	CreatFIRCoeff(Coeff_0_05_HP,1.0f,sampling_rate,371,HP);
}

float CFilter::Sinc(float FC,int n,int FilterLen)
{	
	float s;
		if((n - FilterLen/2)==0)
			s = 2*PI*FC;
		else if(0 > (n-FilterLen/2) || 0 < (n-FilterLen/2))
			s = sin(2*PI*FC*(n-FilterLen/2))/(n-FilterLen/2);
		s = s*(0.54f-0.46f*cos(2*PI*n/FilterLen));
	return s;
}

void CFilter::CreatFIRCoeff(float *Coeff,float CutFreq,int SamplingRate,int FilterLen,int FilterType) 
{
	int i;
	float sum = 0,FC;
	
	if (FilterType == NOTCH) FC = float(CutFreq-2) / SamplingRate;
	else if (FilterType == HP) FC = float((SamplingRate / 2) - CutFreq) / SamplingRate;
	else FC = float(CutFreq) / SamplingRate;
	for(i=0;i<FilterLen;i++)
	{
		Coeff[i] = Sinc(FC,i,FilterLen);
	}
	//Calculate unity gain at dc
	sum = 0;
	for(i=0;i<FilterLen;i++)
	{
		sum = sum+Coeff[i];
	}
	//Normalize for unity gain
	for(i=0;i<FilterLen;i++)
	{
		Coeff[i] = Coeff[i]/sum;
	}
	if (FilterType == HP){
		for(i=1;i<FilterLen;i+=2)
		{
			Coeff[i] = -Coeff[i];
		}
	}else if (FilterType == NOTCH){
		float *Coeff2;
		Coeff2 = new float[FilterLen];
		FC = float(CutFreq+2) / SamplingRate;
		for(i=0;i<FilterLen;i++)
		{
			Coeff2[i] = Sinc(FC,i,FilterLen);
		}

		//Calculate unity gain at dc
		sum = 0;
		for(i=0;i<FilterLen;i++)
		{
			sum = sum+Coeff2[i];
		}
		//Normalize for unity gain
		for(i=0;i<FilterLen;i++)
		{
			Coeff2[i] = Coeff2[i]/sum;
		}
		for(i=1;i<FilterLen;i+=2)
		{
			Coeff2[i] = -Coeff2[i];
		}
		//Calculate summ kernel
		for(i=0;i<FilterLen;i++)
		{
			Coeff[i] = Coeff[i]+Coeff2[i];
		}
		delete Coeff2;
	}

}

float CFilter::FIR_40_LP(float samp)
{
	int i;
	float ret;
	for (i=BUFFER_SIZE-1;i>0;i--) Filter_Buffer_40[i] = Filter_Buffer_40[i-1];
	Filter_Buffer_40[0] = samp;
	ret = RTConvolution(Filter_Buffer_40,Coeff_40_LP,101);
	return ret;
}

float CFilter::FIR_60Notch(float samp)
{
	int i;
	float ret;
	for (i=BUFFER_SIZE-1;i>0;i--) Filter_Buffer_60[i] = Filter_Buffer_60[i-1];
	Filter_Buffer_60[0] = samp;
	ret = RTConvolution(Filter_Buffer_60,Coeff_60Notch,301);
	return ret;
}

float CFilter::FIR_0_05_HP(float samp)
{
    int i;
	float ret;
	for (i=BUFFER_SIZE-1;i>0;i--) Filter_Buffer_0_05[i] = Filter_Buffer_0_05[i-1];
	Filter_Buffer_0_05[0] = samp;
	ret = RTConvolution(Filter_Buffer_0_05,Coeff_0_05_HP,371);
	return ret;
}


float CFilter::RTConvolution(float *xn,float *hn,int hn_len)
{
	int i,j,k;
	float y;
	y = 0;

	for(i=0;i<hn_len;i++){
		y += (hn[i]*xn[i]);
	}
	return y;
}


void CFilter::Convolution(float *xn,float *yn,float *hn,int hn_len,int xn_len)
{
	int i,j,k;
	float y;
	for(j=0;j<xn_len;j++){
		y = 0;
		for(i=0;i<hn_len;i++){
			k = j-i;
			if (k >= 0 && k < xn_len) y += (hn[i]*xn[k]);
		}
		yn[j] = y;
	}
}
