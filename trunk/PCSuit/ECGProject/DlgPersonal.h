#if !defined(AFX_DLGPERSONAL_H__C5839945_9613_11D7_8A9A_BBF336018E78__INCLUDED_)
#define AFX_DLGPERSONAL_H__C5839945_9613_11D7_8A9A_BBF336018E78__INCLUDED_

#include "ECG_Data.h"	// Added by ClassView
#include "afxcmn.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgPersonal.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgPersonal dialog

class CDlgPersonal : public CDialog
{
// Construction
public:
	void SetPersonalData();
	CDlgPersonal(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgPersonal)
	enum { IDD = IDD_PERSONAL };

	CString m_RecordName;

	CString m_Age;

	CComboBoxEx m_Sex;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPersonal)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgPersonal)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:

		afx_msg void OnBnClickedOk();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPERSONAL_H__C5839945_9613_11D7_8A9A_BBF336018E78__INCLUDED_)
