// DlgAnylsisSel.cpp : implementation file
//

#include "stdafx.h"
#include "ecg.h"
#include "DlgAnylsisSel.h"


// CDlgAnylsisSel dialog

IMPLEMENT_DYNAMIC(CDlgAnylsisSel, CDialog)

CDlgAnylsisSel::CDlgAnylsisSel(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAnylsisSel::IDD, pParent)
	, m_HRV(FALSE)
	, m_FeaturePoint(FALSE)
	, m_ReadSD(FALSE)
{

}

CDlgAnylsisSel::~CDlgAnylsisSel()
{
}

void CDlgAnylsisSel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK1, m_HRV);
	DDX_Check(pDX, IDC_CHECK2, m_FeaturePoint);
	DDX_Check(pDX, IDC_CHECK3, m_ReadSD);
}


BEGIN_MESSAGE_MAP(CDlgAnylsisSel, CDialog)
	ON_BN_CLICKED(IDC_CHECK1, &CDlgAnylsisSel::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_CHECK2, &CDlgAnylsisSel::OnBnClickedCheck2)
END_MESSAGE_MAP()


// CDlgAnylsisSel message handlers

void CDlgAnylsisSel::OnBnClickedCheck1()
{
	this->UpdateData(true);
	this->UpdateData(false);
}

void CDlgAnylsisSel::OnBnClickedCheck2()
{
	this->UpdateData(true);
	this->UpdateData(false);
}
