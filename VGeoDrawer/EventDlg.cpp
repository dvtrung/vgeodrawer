// EventDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VGeoDrawer.h"
#include "EventDlg.h"
#include "InputDlg.h"


// CEventDlg dialog

IMPLEMENT_DYNAMIC(CEventDlg, CDialog)

CEventDlg::CEventDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEventDlg::IDD, pParent)
{

}

CEventDlg::~CEventDlg()
{
}

void CEventDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEventDlg, CDialog)
	ON_BN_CLICKED(ID_ADDEVENT, &CEventDlg::OnBnClickedAddevent)
END_MESSAGE_MAP()


// CEventDlg message handlers

void CEventDlg::OnBnClickedAddevent()
{
	CInputDlg dlg(L"Add Event");
	dlg.AddStatic(L"Sự kiện");
	CStringArray arr;
	CString str;
	
	for (int i=0;i<m_objCont->GetSize();i++)
	{
		if (IsButtonType(m_objCont->GetAt(i)->m_Type))
		{
			arr.Add(L"Nhấn "+m_objCont->GetAt(i)->m_Name);
		}
	}
	arr.Add(L"Biểu thức Boolean");
	dlg.AddComboBox(&str,&arr,WS_VISIBLE | WS_BORDER | CBS_DROPDOWNLIST);
	if (dlg.DoModal()==IDOK)
	{

	}
}

BOOL CEventDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
