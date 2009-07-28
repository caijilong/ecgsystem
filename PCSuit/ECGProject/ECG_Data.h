// ECG_Data.h: interface for the ECG_Data class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ECG_DATA_H__01535160_9360_11D7_8A9A_85AFB17F9D7A__INCLUDED_)
#define AFX_ECG_DATA_H__01535160_9360_11D7_8A9A_85AFB17F9D7A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CECG_Data  
{
public:

	void GetPosTime(int pos,BYTE* hh,BYTE* mm);
	CString GetInfoFrom(int pos,int* qrs_count,int * qrs_ms,int* pr_ms,int* qt_qtc_ms,BYTE* hh,BYTE* mm);
	void SetInfoAt(int pos,CString comment,int qrs_count,int qrs_ms,int pr_ms,int qt_qtc_ms,BYTE hh,BYTE mm);
	void SerializeAll(CArchive* ar);
	void RemoveAll();
	void OpenFile(CString File_Name);
	void SaveData(CString File_Name);
	int GetAllDataLenght();
	void GetFrom(double* dest,int start,int lenght);
	void SetDataAt(double* source,int start,int lenght);
	void Message(double msg);
	void SetDataNew(double* source,int lenght,CString commentint,int QRS_count,int QRS_ms,int PR_ms,int QT_QTc_ms,BYTE hh,BYTE mm);
	CWordArray Ecg_Data;//array will store the data
	CWordArray Personal;
	CWordArray Time;
	CWordArray QRS_COUNT;
	CECG_Data();
	virtual ~CECG_Data();

private:
	void GetTime(WORD w_time,BYTE* hh,BYTE* mm);
	WORD SetTime(BYTE hh,BYTE mm);

	
};

#endif // !defined(AFX_ECG_DATA_H__01535160_9360_11D7_8A9A_85AFB17F9D7A__INCLUDED_)
