// ScriptDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VGeoDrawer.h"
#include "ScriptDlg.h"
#include "ScriptDlgRun.h"
#include "appfunc.h"
#include "InputDlg.h"


// CScriptDlg dialog

IMPLEMENT_DYNAMIC(CScriptDlg, CDialog)

CScriptDlg::CScriptDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CScriptDlg::IDD, pParent)
{

}

CScriptDlg::~CScriptDlg()
{
}

void CScriptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SCRIPT, m_Edit);
	DDX_CBString(pDX, IDC_EDIT_SCRIPT, m_strCode);
}


BEGIN_MESSAGE_MAP(CScriptDlg, CDialog)
	ON_BN_CLICKED(ID_SELECT_FILE, &CScriptDlg::OnBnClickedSelectFile)
	ON_BN_CLICKED(ID_CHECK_ERROR, &CScriptDlg::OnBnClickedCheckError)
	ON_BN_CLICKED(ID_RUN, &CScriptDlg::OnBnClickedRun)
	ON_BN_CLICKED(ID_SET_DEFAULT, &CScriptDlg::OnBnClickedSetDefault)
	ON_BN_CLICKED(ID_ADD_FUNCTION, &CScriptDlg::OnBnClickedAddFunction)
END_MESSAGE_MAP()


// CScriptDlg message handlers

void CScriptDlg::OnBnClickedSelectFile()
{
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("Script Files (*.js; *.vbs)|*.js;*.vbs|Text Files (*.txt)|*.txt|All Files (*.*)|*.*||"));
	if (dlg.DoModal() == IDOK)
	{
		m_ScriptObj.Reset();
		m_ScriptObj.LoadScript( dlg.GetPathName() );
	}
}

BOOL CScriptDlg::UpdateScript()
{
	CString strCode;
	m_Edit.GetWindowText( strCode );
	m_ScriptObj.Reset();
	return m_ScriptObj.AddScript(strCode);
}

void CScriptDlg::OnBnClickedCheckError()
{
	CString strCode;
	m_Edit.GetWindowText( strCode );
	m_ScriptObj.Reset();
	if (!UpdateScript())
	{
		int line;
		CString strError = m_ScriptObj.GetErrorString(line);
		line--;
		m_Edit.SetSel(m_Edit.LineIndex(line),
			m_Edit.LineIndex(line)+m_Edit.LineLength(line));
		m_Edit.SetFocus();
		AfxMessageBox(strError, MB_OK | MB_ICONERROR);
	}
	else
		AfxMessageBox(L"Không có lỗi", MB_OK | MB_ICONINFORMATION);
}

BOOL CScriptDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_Edit.InitCtrl();

	m_ScriptObj.Reset();
	m_ScriptObj.AddScript(m_DocScriptObj->m_strCode);
	m_Edit.SetWindowText(m_DocScriptObj->m_strCode);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CScriptDlg::OnBnClickedRun()
{
	if (!UpdateScript())
	{
		AfxMessageBox(L"Không thực hiện được đoạn mã. \r\nChọn Kiểm tra lỗi để soát lỗi",MB_OK | MB_ICONERROR);
	}
	else
	{
		CScriptDlgRun dlg;
		dlg.m_ScriptObj=&m_ScriptObj;
		dlg.DoModal();
	}
}

void CScriptDlg::OnBnClickedSetDefault()
{
	CStdioFile file;
	CFileException feError;
	if (file.Open(GetAppDirectory()+L"\\data\\defaultscript.dat",CStdioFile::modeWrite | CStdioFile::modeCreate,&feError))
	{
		CString strScript;
		m_Edit.GetWindowText(strScript);
		strScript.Replace(L"\r\n",L"\n");
		file.WriteString(strScript);
		file.Close();
	}
	else
	{
		TCHAR	errBuff[256];
		feError.GetErrorMessage( errBuff, 256 );
		AfxMessageBox(errBuff,MB_OK | MB_ICONERROR);
	}

// 	CFile file(GetAppDirectory()+L"\\data\\defaultscript.dat",CFile::modeWrite | CFile::modeCreate);
// 	CString str;
// 	m_Edit.GetWindowText(str);
// 	file.Write(str,str.GetLength());
// 	file.Close();
}

void CScriptDlg::OnBnClickedAddFunction()
{
	CInputDlg dlg(L"Add Function",this);
	dlg.AddStatic(L"Tên hàm số :");
	CString strFuncName;
	dlg.AddEdit(&strFuncName);

	dlg.AddStatic(L"Tham số :");
	CString strParam;
	dlg.AddEdit(&strParam);

	if (dlg.DoModal()==IDOK)
	{
		CString str;
		m_Edit.GetWindowText(str);
		str.Append(L"\r\n\r\nfunction "+strFuncName+L"("+strParam+L")"+"\r\n{\r\n  \r\n}");
		m_Edit.SetWindowText(str);
		m_Edit.LineScroll(m_Edit.GetLineCount()-2);
	}
}
