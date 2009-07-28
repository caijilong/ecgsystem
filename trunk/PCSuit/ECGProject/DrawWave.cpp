#include "stdafx.h"
#include "DrawWave.h"
#include "math.h"

CScale::CScale(CScale *scale)
{
	XStart = scale->XStart;
	XEnd = scale->XEnd;
	YStart = scale->YStart;
	YEnd = scale->YEnd;
}
CScale::CScale(float XS,float XE,float YS,float YE)
{
	XStart = XS;
	XEnd = XE;
	YStart = YS;
	YEnd = YE;
	CString x;

}

CDrawWave::CDrawWave(CRect *rect,COLORREF c,UINT type)
{
	DisplayType = type;
	display_count = 0;
	color = c;
	Extern_Frame_rect = *rect;
	Frame_rect.SetRect(rect->left+5,rect->top+5,rect->right-5,rect->bottom-5);
	Scale = 0;

	wave_rect.SetRect(Frame_rect.left+30,Frame_rect.top,Frame_rect.right,Frame_rect.bottom-20);
	display_buffer = new float[wave_rect.Width()];
	for (int i=0;i<wave_rect.Width();i++) display_buffer[i] = 0;
}

CDrawWave::~CDrawWave(void)
{
	delete [] display_buffer;
	display_buffer = 0;
	if (Scale != 0){
		delete Scale;
		Scale = 0;
	}
}

void CDrawWave::DumpValue(float *value,int size)
{
	int k=0;
	for (float i=0;i<size;i+=(float(size)/wave_rect.Width())) 
		display_buffer[k++] = -value[int(i)];
}

void CDrawWave::InsertValueAt(int index,float value)
{
	if (index < wave_rect.Width()){
		display_buffer[index] = -value;	
	}
}

void CDrawWave::InsertValue(float value) //value is 1 to -1
{
	display_buffer[display_count] = -value;
	display_count++;
	if (display_count == wave_rect.Width()){
			display_count = 0;
	}
}

void CDrawWave::ChangeValue(float value,int index)
{
	display_buffer[index] = -value;
}

void CDrawWave::Skip(int num)
{
	display_count+=num;
}

void CDrawWave::SetGrid(CScale scale)
{
	if (Scale == 0) Scale = new CScale(&scale);
	else *Scale = scale;
}
void CDrawWave::SetGrid(CScale scale,float xinterval,float yinterval,COLORREF color,CString x,CString y)
{
	if (Scale == 0){
		Scale = new CScale(&scale);
	}
	else *Scale = scale;
	Gridcolor = color;

	YScaleInterval = yinterval;
	XScaleInterval = xinterval;
}

void CDrawWave::DrawGrid(CDC *dcMem)
{
	float ReScaleX,ReScaleY,Original,v;
	float ReCordX,ReCordY,Shift;
	int d=2;

	ReScaleY = wave_rect.Height() / fabs(Scale->YStart - Scale->YEnd);
	ReScaleX = wave_rect.Width() / fabs(Scale->XStart - Scale->XEnd);

	if (DisplayType == TYPE_SIGN){
		Original = wave_rect.Height()/2 + wave_rect.top;
	}else if (DisplayType == TYPE_UNSIGN){
		Original = wave_rect.bottom;
	}


	CPen lpenR(PS_SOLID,1,Gridcolor);//Select color green
	CPen* pOldpen = dcMem->SelectObject(&lpenR);
	dcMem->SelectObject(&lpenR);

	dcMem->MoveTo(wave_rect.left,wave_rect.bottom);
	dcMem->LineTo(wave_rect.left,wave_rect.top);
	dcMem->MoveTo(wave_rect.right,wave_rect.bottom);
	dcMem->LineTo(wave_rect.left,wave_rect.bottom);
	dcMem->DrawEdge(Extern_Frame_rect,2,BF_RECT);
	//draw horizontal lines
	int pos;
	CString ScaleString;
	CRect rect;
/*	rect.SetRect(0,0,20,10);
	rect.MoveToXY(Extern_Frame_rect.left,Extern_Frame_rect.top);
	ScaleString.Format(XAxisLabel);

	rect.SetRect(0,0,20,10);
	rect.MoveToXY(Frame_rect.right,Frame_rect.bottom);
	ScaleString.Format(YAxisLabel);*/

	rect.SetRect(0,0,50,15);

	rect.MoveToXY(Frame_rect.left,Frame_rect.top);
	ScaleString.Format("%1.1f",Scale->YEnd);
	dcMem->DrawText(ScaleString,ScaleString.GetLength(),&rect,DT_LEFT);
	rect.MoveToXY(Frame_rect.left,Frame_rect.bottom);
	ScaleString.Format("%1.1f",Scale->YStart);
	dcMem->DrawText(ScaleString,ScaleString.GetLength(),&rect,DT_LEFT);

	pos = Original - int(Scale->YEnd*ReScaleY);
	dcMem->MoveTo(Frame_rect.left+15,wave_rect.top);
	dcMem->LineTo(Frame_rect.right,wave_rect.top);
	dcMem->MoveTo(Frame_rect.right,wave_rect.bottom);
	dcMem->LineTo(Frame_rect.right,Frame_rect.top);
	if (DisplayType == TYPE_SIGN){
		for(v=0 ; v <= Scale->YEnd ; v=v+YScaleInterval)
		{
			pos = Original - int(v * ReScaleY);
			if (pos > wave_rect.top){
				dcMem->MoveTo(Frame_rect.left+15,pos);
				dcMem->LineTo(Frame_rect.right,pos);
				rect.MoveToXY(Frame_rect.left,pos);
				ScaleString.Format("%1.1f",v);
				dcMem->DrawText(ScaleString,ScaleString.GetLength(),&rect,DT_LEFT);
			}
		}
		for(v=0 ; v >= Scale->YStart ; v=v-YScaleInterval)
		{
			pos = Original - int(v * ReScaleY);
			if (pos < wave_rect.bottom){
				dcMem->MoveTo(Frame_rect.left+15,pos);
				dcMem->LineTo(Frame_rect.right,pos);
				rect.MoveToXY(Frame_rect.left,pos);
				ScaleString.Format("%1.1f",v);
				dcMem->DrawText(ScaleString,ScaleString.GetLength(),&rect,DT_LEFT);
			}
		}
	}else  if (DisplayType == TYPE_UNSIGN){
		for(v=0 ; v < Scale->YEnd ; v=v+YScaleInterval)
		{
			pos = Original - int(v * ReScaleY);
			if (pos > wave_rect.top && pos < wave_rect.bottom){
				dcMem->MoveTo(Frame_rect.left+15,pos);
				dcMem->LineTo(Frame_rect.right,pos);
				rect.MoveToXY(Frame_rect.left,pos);
				ScaleString.Format("%1.1f",Scale->YStart + v);
				dcMem->DrawText(ScaleString,ScaleString.GetLength(),&rect,DT_LEFT);
			}
		}
	}

	//draw vertical lines each one is equal to 1 sec
	for(v=0; v < (Scale->XEnd - Scale->XStart); v=v+XScaleInterval)
	{
		int pos;
		pos = int(v * ReScaleX)+wave_rect.left;
		dcMem->MoveTo(pos,Frame_rect.top);
		dcMem->LineTo(pos,Frame_rect.bottom-15);

		rect.MoveToXY(pos-3,Frame_rect.bottom-15);
		ScaleString.Format("%1.1f",Scale->XStart + v);
		dcMem->DrawText(ScaleString,ScaleString.GetLength(),&rect,DT_LEFT);
	}
	//draw rect

	dcMem->SelectObject(pOldpen);
}

void CDrawWave::DrawWave(CDC *dcMem) 
{
	int i,NormalYAxis,Original;
	float j,k=0,Shift=0;

	if (Scale){
		DrawGrid(dcMem);
		if (DisplayType == TYPE_SIGN) Shift = (Scale->YStart + Scale->YEnd)/2;
		else if (DisplayType == TYPE_UNSIGN) Shift = Scale->YStart/(Scale->YEnd - Scale->YStart );
	}

	if (DisplayType == TYPE_SIGN){
		NormalYAxis = wave_rect.Height()/2;
		Original = wave_rect.Height()/2 + wave_rect.top;
	}else if (DisplayType == TYPE_UNSIGN){
		NormalYAxis = wave_rect.Height();
		Original = wave_rect.bottom;
	}

 	CPen lpen(PS_SOLID,1,color);//Select color green
	CPen* pOldpen = dcMem->SelectObject(&lpen);

	for (i=0 ; i < wave_rect.Width();i++)
	{
		if (int(i) != display_count)
		{
				j = k;
				k = display_buffer[i]+Shift;
				int y1,y2;
				y1 = int(NormalYAxis * j + Original);
				y2 = int(NormalYAxis * k + Original);
				POINT p1,p2;
				if (y2 < wave_rect.top) y2 = wave_rect.top+1;
				if (y1 < wave_rect.top) y1 = wave_rect.top+1;
				if (y2 > wave_rect.bottom) y2 = wave_rect.bottom-1;
				if (y1 > wave_rect.bottom) y1 = wave_rect.bottom-1;
					if (int((j - k) * NormalYAxis) == 0)
					{
						dcMem->SetPixel(wave_rect.left + i,y2,color);
					}
					else
					{
						dcMem->MoveTo(wave_rect.left+i,y1);
						dcMem->LineTo(wave_rect.left+i,y2);
					}
		}else k = display_buffer[i]+Shift;
	}
	dcMem->SelectObject(pOldpen);
}