// DlgPersonal.cpp : implementation file
//

#include "stdafx.h"
#include "DlgPersonal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgPersonal dialog


CDlgPersonal::CDlgPersonal(CWnd* pParent /*=NULL*/)
: CDialog(CDlgPersonal::IDD, pParent)
	, m_RecordName(_T("1"))
	, m_Age(_T("30"))
{
	//{{AFX_DATA_INIT(CDlgPersonal)
	m_RecordName.Insert(1,"Male");
	m_RecordName.Insert(2,"Female");
	//}}AFX_DATA_INIT
}


void CDlgPersonal::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPersonal)
	DDX_Text(pDX, IDC_EDIT1, m_RecordName);
	DDX_Text(pDX, IDC_EDIT3, m_Age);
	//}}AFX_DATA_MAP

	DDX_Control(pDX, IDC_COMBOBOXEX1, m_Sex);
}


BEGIN_MESSAGE_MAP(CDlgPersonal, CDialog)
	//{{AFX_MSG_MAP(CDlgPersonal)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CDlgPersonal::OnBnClickedOk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPersonal message handlers

void CDlgPersonal::SetPersonalData()
{
	UpdateData(true);

//	UpdateData(false);
}

void CDlgPersonal::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	SetPersonalData();
	CDialog::OnOK();
}
