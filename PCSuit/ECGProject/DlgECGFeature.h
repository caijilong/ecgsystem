#pragma once


// CDlgECGFeature dialog

class CDlgECGFeature : public CDialog
{
	DECLARE_DYNAMIC(CDlgECGFeature)

public:
	CDlgECGFeature(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgECGFeature();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
