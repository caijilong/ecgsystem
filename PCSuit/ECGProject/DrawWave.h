#pragma once

#define TYPE_SIGN	0
#define TYPE_UNSIGN	1

class CScale{
public:
	CScale(CScale *scale);
	CScale(float XS,float XE,float YS,float YE);
	float XStart,XEnd,YStart,YEnd;

};

class CDrawWave
{
public:

	CDrawWave(CRect *rect,COLORREF color,UINT type);
	void InsertValue(float value);
	void DrawWave(CDC *dcMem);
	void InsertValueAt(int index,float value);
	void ChangeValue(float value,int index);
	void Skip(int num);
	void DumpValue(float *value,int size);
	void SetGrid(CScale scale);
	void SetGrid(CScale scale,float xinterval,float yinterval,COLORREF color,CString x,CString y);
	int display_count;
	CRect Frame_rect,wave_rect,Extern_Frame_rect;
    float *display_buffer;
	~CDrawWave(void);
		char *xx;
private:
	char *XAxisLabel;
	char *YAxisLabel;
	CPoint VScale,HScale;
	CScale *Scale;
	float XScaleInterval,YScaleInterval;
	UINT DisplayType;

	COLORREF color,Gridcolor;


	void DrawGrid(CDC *dcMem);
};
