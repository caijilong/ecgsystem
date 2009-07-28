#pragma once


// CDlgFeatureDect dialog

class CDlgFeatureDect : public CDialog
{
	DECLARE_DYNAMIC(CDlgFeatureDect)

public:
	CDlgFeatureDect(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgFeatureDect();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
