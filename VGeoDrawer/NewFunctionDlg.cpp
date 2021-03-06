// NewFunctionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VGeoDrawer.h"
#include "NewFunctionDlg.h"


// CNewFunctionDlg dialog

IMPLEMENT_DYNAMIC(CNewFunctionDlg, CDialog)

CNewFunctionDlg::CNewFunctionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewFunctionDlg::IDD, pParent)
{

}

CNewFunctionDlg::~CNewFunctionDlg()
{
}

void CNewFunctionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CNewFunctionDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_FUNCTYPE, &CNewFunctionDlg::OnCbnSelchangeComboFunctype)
	ON_BN_CLICKED(IDOK, &CNewFunctionDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CNewFunctionDlg message handlers

BOOL CNewFunctionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	((CComboBox*)GetDlgItem(IDC_COMBO_FUNCTYPE))->AddString(L"Phương trình y=f(x)");
	((CComboBox*)GetDlgItem(IDC_COMBO_FUNCTYPE))->AddString(L"Phương trình x=f(y)");
	((CComboBox*)GetDlgItem(IDC_COMBO_FUNCTYPE))->AddString(L"Phương trình tham số x=X(t),y=Y(t)");
	((CComboBox*)GetDlgItem(IDC_COMBO_FUNCTYPE))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CNewFunctionDlg::OnCbnSelchangeComboFunctype()
{
	// TODO: Add your control notification handler code here
}

void CNewFunctionDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}
