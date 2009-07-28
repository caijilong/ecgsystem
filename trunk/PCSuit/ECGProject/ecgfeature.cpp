#include "stdafx.h"
#include "ECGFeature.h"
#include "math.h"

CECGFeature::CECGFeature(CRect *rect,COLORREF color,UINT type,int sampling_rate) : CDrawWave(rect,color,type)
{
	location.R.x = 0;
	location.S.x = 0;
	location.Q.x = 0;
	location.P.x = 0;
	location.T.x = 0;
	Sampling_Rate = sampling_rate;
}

void CECGFeature::Convolution(float *xn,float *yn,float *hn,int hn_len,int xn_len)
{
	int i,j,k;
	float y;
	for(j=0;j<xn_len;j++){
		y = 0;
		for(i=0;i<hn_len;i++){
			k = j-i;
			if (k >= 0 && k < xn_len) y += (hn[i]*xn[k]);
		}
		yn[j] = fabsf(y);
	}
}



int CECGFeature::GetRPos(float *buf,int len)
{
	int i,j,k;
	float h1[5] = {-1, -2, 0, 2, 1},h2[8] = {1.0/8 ,1.0/8 ,1.0/8 ,1.0/8 ,1.0/8 ,1.0/8 ,1.0/8 ,1.0/8};
	float y,*yn,*yn2,Max;
	yn = new float[len];
	yn2 = new float[len];	

	for(i = 0 ; i < len ; i++){
		yn[i] = 0;
		yn2[i] = 0;
	}
	Convolution(buf,yn,h1,5,len);
	Convolution(yn,yn2,h2,8,len);

	Max = 0;
	for(i = 0 ; i < len ; i++){
		Max = yn2[i] > Max ? yn2[i]:Max;
	}
	for(i = 0 ; i < len ; i++){
		if (Max == yn2[i]) break;
	}
//	i -= (4 + 3);	//4: moving average的group delay, 1: derivate的group delay
	Max = -2;
	for(j = i-50 ; j < i ; j++){
		Max = buf[j] > Max ? buf[j]:Max;
	}
	for(j = i-50 ; j < i ; j++){
		if (Max == buf[j]) break;
	}
	delete [] yn;
	delete [] yn2;
	return j;
}

void CECGFeature::DetectECGFeature(float *detect_buf,int DetectLen)
{
	int templete_start;
	long i;

	float Org = wave_rect.Height()/2 + wave_rect.top;
	location.R.x = GetRPos(detect_buf,DetectLen);
	location.R.y = (detect_buf[location.R.x]*wave_rect.Height()/2) + Org;

	for(i=location.R.x-5;i>1;i--){
		if (detect_buf[i-1] > detect_buf[i]) break;
	}
	location.Q.x = i;
	location.Q.y = (detect_buf[location.Q.x]*wave_rect.Height()/2) + Org;
	if (location.Q.x > -1){
		for(i=location.R.x;i<DetectLen-1;i++){
			if (detect_buf[i+1] > detect_buf[i]) break;
		}
		location.S.x = i;
		location.S.y = (detect_buf[location.S.x]*wave_rect.Height()/2) + Org;

		float Max = -2;
		int interval = Sampling_Rate*0.2;
		if (location.Q.x - interval>0){
			for(i = location.Q.x ; i > (location.Q.x - interval) ; i--){
				Max = detect_buf[i] > Max ? detect_buf[i]:Max;
			}
			for(i = location.Q.x ; i > (location.Q.x - interval) ; i--){
				if (Max == detect_buf[i]) break;
			}
		}
		location.P.x = i;
		location.P.y = (detect_buf[location.P.x]*wave_rect.Height()/2) + Org;

		Max = -2;
		for(i = location.S.x ; i < DetectLen ; i++){
			Max = detect_buf[i] > Max ? detect_buf[i]:Max;
		}
		for(i = location.S.x ; i < DetectLen ; i++){
			if (Max == detect_buf[i]) break;
		}
		location.T.x = i;
		location.T.y = (detect_buf[location.T.x]*wave_rect.Height()/2) + Org;
	}
	if (location.R.x > 1) {
		QTInterval = float(location.T.x - location.Q.x) / Sampling_Rate;
		STInterval= float(location.T.x - location.S.x) / Sampling_Rate;
		QRSInterval= float(location.S.x - location.Q.x) / Sampling_Rate;
		PSInterval= float(location.S.x - location.P.x) / Sampling_Rate;
		PRInterval= float(location.R.x - location.P.x) / Sampling_Rate;

		location.R.x = (float(location.R.x)/DetectLen)*this->wave_rect.Width();
					//(location.R.x/DetectLen)結果為x在0.8秒中的比利,乘上顯示區域的寬度,就對應到顯示區域的秒數
		location.S.x = (float(location.S.x)/DetectLen)*this->wave_rect.Width();
		location.Q.x = (float(location.Q.x)/DetectLen)*this->wave_rect.Width();
		location.P.x = (float(location.P.x)/DetectLen)*this->wave_rect.Width();
		location.T.x = (float(location.T.x)/DetectLen)*this->wave_rect.Width();

		location.R.y = (display_buffer[location.R.x]*wave_rect.Height()/2) + Org;
		location.S.y = (display_buffer[location.S.x]*wave_rect.Height()/2) + Org;
		location.Q.y = (display_buffer[location.Q.x]*wave_rect.Height()/2) + Org;
		location.P.y = (display_buffer[location.P.x]*wave_rect.Height()/2) + Org;
		location.T.y = (display_buffer[location.T.x]*wave_rect.Height()/2) + Org;

		location.R.x += (wave_rect.left+1);
					//(location.R.x/DetectLen)結果為x在0.8秒中的比利,乘上顯示區域的寬度,就對應到顯示區域的秒數
		location.S.x += (wave_rect.left+1);
		location.Q.x += (wave_rect.left+1);
		location.P.x += (wave_rect.left+1);
		location.T.x += (wave_rect.left+1);
	}else{
		location.R.x =0;
		location.S.x =0;
		location.Q.x =0;
		location.P.x =0;
		location.T.x =0;
		location.R.y =0;
		location.S.y =0;
		location.Q.y =0;
		location.P.y =0;
		location.T.y =0;
	}

}


void CECGFeature::DrawFeature(CDC *dcMem) 
{
	CRect rect;
	if(location.Q.x != 0){
		
		rect.SetRect(location.Q.x-15,this->wave_rect.bottom-15,location.Q.x-5,this->wave_rect.bottom);
		
		dcMem->DrawText(_T("Q"),1,&rect,DT_CENTER);
		dcMem->MoveTo(rect.left,rect.bottom);
		dcMem->LineTo(location.Q);
		dcMem->DrawEdge(&rect,1,BF_RECT);
	}

	if(location.R.x != 0){
		
		rect.SetRect(location.R.x,this->wave_rect.top,location.R.x+10,this->wave_rect.top+15);
		
		dcMem->DrawText(_T("R"),1,&rect,DT_CENTER);
		dcMem->MoveTo(rect.left,rect.top);
		dcMem->LineTo(location.R);
		dcMem->DrawEdge(&rect,1,BF_RECT);
	}
	if(location.S.x != 0){
		
		rect.SetRect(location.S.x+15,this->wave_rect.bottom-15,location.S.x+25,this->wave_rect.bottom);
		
		dcMem->DrawText(_T("S"),1,&rect,DT_CENTER);
		dcMem->MoveTo(rect.left,rect.top);
		dcMem->LineTo(location.S);
		dcMem->DrawEdge(&rect,1,BF_RECT);
	}
	if(location.P.x != 0){
		
		rect.SetRect(location.P.x,this->wave_rect.top+20,location.P.x+10,this->wave_rect.top+35);
		
		dcMem->DrawText(_T("P"),1,&rect,DT_CENTER);
		dcMem->MoveTo(rect.left,rect.top);
		dcMem->LineTo(location.P);
		dcMem->DrawEdge(&rect,1,BF_RECT);
	}
	if(location.T.x != 0){
		
		rect.SetRect(location.T.x,this->wave_rect.top,location.T.x+10,this->wave_rect.top+15);
		
		dcMem->DrawText(_T("T"),1,&rect,DT_CENTER);
		dcMem->MoveTo(rect.left,rect.top);
		dcMem->LineTo(location.T);
		dcMem->DrawEdge(&rect,1,BF_RECT);
	}
}

CECGFeature::~CECGFeature(void)
{

}


