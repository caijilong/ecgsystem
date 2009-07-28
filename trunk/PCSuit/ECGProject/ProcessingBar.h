#pragma once
#include "afxcmn.h"


// CProcessingBar dialog

class CProcessingBar : public CDialog
{
	DECLARE_DYNAMIC(CProcessingBar)

public:
	CProcessingBar(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProcessingBar();
	
// Dialog Data
	enum { IDD = IDD_PROCESSING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_Progress;
};
