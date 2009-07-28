#pragma once

#include "ECGRecorder.h"
#include "afxwin.h"

// CAcquisitionSetting dialog

class CAcquisitionSetting : public CDialog
{
	DECLARE_DYNAMIC(CAcquisitionSetting)

public:
	CAcquisitionSetting(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAcquisitionSetting();


	CECGRecorder ecg_recorder;
	
//Control member
	CString m_RecordFileName;
	CString m_Age;
	int Male;
	int m_SamplingRate;
	CString m_ComPort_Number;
	int m_Resolution;

// Dialog Data
	enum { IDD = IDD_ACQUISITION_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnUartopen();
//	afx_msg void OnUartclose();

public:
	afx_msg void OnBnClickedOk();
public:
	afx_msg void OnBnClickedSexSel();
public:
	afx_msg void OnBnClickedRadio2();
public:
	int m_BaudRate;
public:
	afx_msg void OnBnClickedCancel();
};
