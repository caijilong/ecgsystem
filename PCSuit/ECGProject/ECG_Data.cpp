// ECG_Data.cpp: implementation of the ECG_Data class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ECG_Data.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CECG_Data::CECG_Data()
{

}

CECG_Data::~CECG_Data()
{

}

void CECG_Data::SetDataNew(double *source, int lenght,CString comment,int QRS_count,int QRS_ms,int PR_ms,int QT_QTc_ms,BYTE hh,BYTE mm)
{
	for(int i = 0;i<lenght;i++)
	{
		Ecg_Data.Add(short(*(source+i)));
	}
	
	TIME.Add(SetTime(hh,mm));
}

void CECG_Data::SetDataAt(double *source,int start, int lenght)
{
	//Set data at specific position in start
	//Chect for overloading DATA array
	if(start+lenght<=GetAllDataLenght())
	{
		for(int i=0;i<lenght;i++)
		{
			Ecg_Data.SetAt(start+i,short(*(source+i)));
		}
	}
}

void CECG_Data::GetFrom(double *dest,int start, int lenght)
{
	//gets data from specific position start
	//Chect for overloading DATA array
	if(start+lenght<=GetAllDataLenght())
	{
		for(int i=0;i<lenght;i++)
		{
			*(dest+i) = double(short(Ecg_Data.GetAt(start+i)));
		}
	}
}

void CECG_Data::Message(double msg)
{
	short i = -32000;
	Ecg_Data.Add(i);
	CString str;
	str.Format("%d",short(Ecg_Data.GetAt(0)));
	str.Format("%d",GetAllDataLenght());
	MessageBox(NULL,str,"Message",MB_OK);
}

int CECG_Data::GetAllDataLenght()
{
	return Ecg_Data.GetSize();
}

void CECG_Data::SaveData(CString File_Name)
{
	CFile sFile(File_Name,CFile::modeCreate|CFile::modeWrite);
	CArchive dArchive(&sFile,CArchive::store);
	
	//Serialize the arrays
	SerializeAll(&dArchive);
}

void CECG_Data::OpenFile(CString File_Name)
{
	RemoveAll();
	CFile oFile(File_Name,CFile::modeRead);
	//Move file pointer to the file begining
	oFile.Seek(0,CFile::begin);
	DWORD f_lenght = oFile.GetLength();
	CArchive dArchive(&oFile,CArchive::load);
	
	//Serialize the arrays
	SerializeAll(&dArchive);
}

void CECG_Data::RemoveAll()
{
	//Free the memory
	Ecg_Data.RemoveAll();
//	COMMENT.RemoveAll();
//	TIME.RemoveAll();
}

void CECG_Data::SerializeAll(CArchive* ar)
{
	//Serialize the arrays
	DATA.Serialize(*ar);
//	COMMENT.Serialize(*ar);
//	TIME.Serialize(*ar);
}

void CECG_Data::SetInfoAt(int pos, CString comment, int qrs_count, int qrs_ms, int pr_ms, int qt_qtc_ms,BYTE hh,BYTE mm)
{
	if(pos<=QRS_COUNT.GetSize())
	{
		QRS_COUNT.SetAt(pos,qrs_count);
		COMMENT.SetAt(pos,comment);
		TIME.SetAt(pos,SetTime(hh,mm));
	}
}

CString CECG_Data::GetInfoFrom(int pos, int *qrs_count, int *qrs_ms, int *pr_ms, int *qt_qtc_ms,BYTE* hh,BYTE* mm)
{
	if(pos<=QRS_COUNT.GetSize())
	{
		GetTime(TIME.GetAt(pos),hh,mm);
	}
	return COMMENT.GetAt(pos);
}


WORD CECG_Data::SetTime(BYTE hh, BYTE mm)
{
	//converts hh & mm in to one word variable
	WORD w_time;
	w_time = hh;
	w_time = w_time << 8;
	w_time = w_time|mm;

	return w_time;

}

void CECG_Data::GetTime(WORD w_time, BYTE *hh, BYTE *mm)
{
	*mm = w_time & 0xFF;
	w_time = w_time >> 8;
	*hh = w_time & 0xFF;
}
/*
int CECG_Data::GetQRSCount(int pos)
{
	//returns the QRS count for a specific record
	int count=0;
	if(pos<=QRS_COUNT.GetSize())
		count = QRS_COUNT.GetAt(pos);

	return count;
}
*/
void CECG_Data::GetPosTime(int pos, BYTE *hh, BYTE *mm)
{
	if(pos<=TIME.GetSize())
	{
		if(pos<=TIME.GetSize())
		{
			GetTime(TIME.GetAt(pos),hh,mm);
		}
	}
}
