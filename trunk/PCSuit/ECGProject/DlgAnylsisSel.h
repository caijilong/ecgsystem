#pragma once


// CDlgAnylsisSel dialog

class CDlgAnylsisSel : public CDialog
{
	DECLARE_DYNAMIC(CDlgAnylsisSel)

public:
	CDlgAnylsisSel(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAnylsisSel();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_HRV;
	BOOL m_FeaturePoint;
public:
	afx_msg void OnBnClickedCheck1();
public:
	afx_msg void OnBnClickedCheck2();
public:
	BOOL m_ReadSD;
};
