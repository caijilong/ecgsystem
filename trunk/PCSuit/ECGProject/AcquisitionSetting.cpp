// AcquisitionSetting.cpp : implementation file
//

#include "stdafx.h"
#include "ecg.h"
#include "AcquisitionSetting.h"


// CAcquisitionSetting dialog

IMPLEMENT_DYNAMIC(CAcquisitionSetting, CDialog)

CAcquisitionSetting::CAcquisitionSetting(CWnd* pParent /*=NULL*/)
	: CDialog(CAcquisitionSetting::IDD, pParent)
	, m_ComPort_Number(_T("1"))
	, m_BaudRate(57600)
{

}

CAcquisitionSetting::~CAcquisitionSetting()
{
}

void CAcquisitionSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_ComPort_Number);
	DDX_Text(pDX, IDC_EDIT2, m_BaudRate);
}


BEGIN_MESSAGE_MAP(CAcquisitionSetting, CDialog)
	ON_BN_CLICKED(IDOK, &CAcquisitionSetting::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CAcquisitionSetting::OnBnClickedCancel)
END_MESSAGE_MAP()


// CAcquisitionSetting message handlers

void CAcquisitionSetting::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}
void CAcquisitionSetting::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}
