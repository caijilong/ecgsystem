// ProcessingBar.cpp : implementation file
//

#include "stdafx.h"
#include "ecg.h"
#include "ProcessingBar.h"


// CProcessingBar dialog

IMPLEMENT_DYNAMIC(CProcessingBar, CDialog)

CProcessingBar::CProcessingBar(CWnd* pParent /*=NULL*/)
	: CDialog(CProcessingBar::IDD, pParent)
{

}

CProcessingBar::~CProcessingBar()
{
}

void CProcessingBar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_Progress);
}


BEGIN_MESSAGE_MAP(CProcessingBar, CDialog)
END_MESSAGE_MAP()


// CProcessingBar message handlers
