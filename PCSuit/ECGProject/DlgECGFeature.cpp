// DlgECGFeature.cpp : implementation file
//

#include "stdafx.h"
#include "ecg.h"
#include "DlgECGFeature.h"


// CDlgECGFeature dialog

IMPLEMENT_DYNAMIC(CDlgECGFeature, CDialog)

CDlgECGFeature::CDlgECGFeature(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgECGFeature::IDD, pParent)
{

}

CDlgECGFeature::~CDlgECGFeature()
{
}

void CDlgECGFeature::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgECGFeature, CDialog)
END_MESSAGE_MAP()


// CDlgECGFeature message handlers
