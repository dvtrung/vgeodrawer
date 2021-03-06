// CommandsViewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VGeoDrawer.h"
#include "CommandsOrganizeDlg.h"
#include "WinUser.h"
#include "Windows.h"
#include "appfunc.h"
#include "AddToolDlg.h"
#include "InputDlg.h"

// CCommandsOrganizeDlg dialog

IMPLEMENT_DYNAMIC(CCommandsOrganizeDlg, CDialog)

CCommandsOrganizeDlg::CCommandsOrganizeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCommandsOrganizeDlg::IDD, pParent)
{

}

CCommandsOrganizeDlg::~CCommandsOrganizeDlg()
{
}

void CCommandsOrganizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NEW, m_NewBtn);
	DDX_Control(pDX, IDC_TREE_COMMAND, m_Tree);
	DDX_Control(pDX, IDC_EDIT, m_Edit);
	//DDX_Control(pDX, IDC_TAB_COMMAND, m_Tab);
	DDX_Control(pDX, IDC_EDIT_INPUT, m_Input);
	DDX_Control(pDX, IDC_EDIT_OUTPUT, m_Output);
	DDX_Control(pDX, IDC_EDIT_DES, m_Description);
	DDX_Control(pDX, IDC_EDIT_NAME, m_Name);
	DDX_Control(pDX, IDC_EDIT_COND, m_Condition);
}


BEGIN_MESSAGE_MAP(CCommandsOrganizeDlg, CDialog)
//	ON_WM_CREATE()
ON_WM_CREATE()
//ON_NOTIFY(NM_CLICK, IDC_TREE, &CCommandsOrganizeDlg::OnNMClickTree)
ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_COMMAND, &CCommandsOrganizeDlg::OnTvnSelchangedTree)
ON_BN_CLICKED(IDC_NEW, &CCommandsOrganizeDlg::OnBnClickedNew)
//ON_NOTIFY(TVN_BEGINLABELEDIT, IDC_TREE_COMMAND, &CCommandsOrganizeDlg::OnTvnBeginlabeleditTree)
ON_NOTIFY(TVN_DELETEITEM, IDC_TREE_COMMAND, &CCommandsOrganizeDlg::OnTvnDeleteitemTree)
ON_NOTIFY(TVN_SELCHANGING, IDC_TREE_COMMAND, &CCommandsOrganizeDlg::OnTvnSelchangingTree)
//ON_NOTIFY(TVN_ENDLABELEDIT, IDC_TREE_COMMAND, &CCommandsOrganizeDlg::OnTvnEndlabeleditTree)
ON_NOTIFY(TVN_KEYDOWN, IDC_TREE_COMMAND, &CCommandsOrganizeDlg::OnTvnKeydownTree)
ON_BN_CLICKED(IDC_DELETE, &CCommandsOrganizeDlg::OnBnClickedDelete)
//ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_COMMAND, &CCommandsOrganizeDlg::OnTcnSelchangeTabCommand)
ON_WM_CTLCOLOR()
ON_EN_CHANGE(IDC_EDIT, &CCommandsOrganizeDlg::OnEnChangeEdit)
ON_BN_CLICKED(ID_BUTTON_OK, &CCommandsOrganizeDlg::OnBnClickedButtonOk)
ON_BN_CLICKED(ID_CANCEL, &CCommandsOrganizeDlg::OnBnClickedCancel)
ON_NOTIFY(TVN_GETDISPINFO, IDC_TREE_COMMAND, &CCommandsOrganizeDlg::OnTvnGetdispinfoTreeCommand)
END_MESSAGE_MAP()


int CCommandsOrganizeDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

BOOL CCommandsOrganizeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
// 	SetIcon(NULL,TRUE);
// 	SetIcon(NULL,FALSE);
	//CString strFileName=AfxGetApp()->GetProfileString("CmdFile","files","\\tools\\std.dat");

	m_Font.CreatePointFont(100,L"Courier New");
	m_Edit.InitCtrl();
	m_Edit.SetFont(&m_Font);
	m_Input.SetFont(&m_Font);
	m_Output.SetFont(&m_Font);
	m_Condition.SetFont(&m_Font);
	m_Edit.m_cmdArray=&m_CommandArray;

	//OnTcnSelchangeTabCommand(NULL,NULL);

	//HTREEITEM item=m_Tree.InsertItem(L"Standard Commands",NULL,NULL);

	m_CommandArray.LoadCommands();

	FillTree();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CCommandsOrganizeDlg::FillTree()
{
	m_Tree.SetRedraw(FALSE);
	m_Tree.DeleteAllItems();
	standardItem=m_Tree.InsertItem(L"Câu lệnh chuẩn");
	expItem=m_Tree.InsertItem(L"Câu lệnh biểu thức");
	geoItem=m_Tree.InsertItem(L"Câu lệnh dựng hình");

	//userItem=m_Tree.InsertItem(L"Câu lệnh người dùng");
	for (int i=0;i<m_CommandArray.GetSize();i++)
	{
		CCommandArray::CCmd* cmd=(CCommandArray::CCmd*)m_CommandArray.m_CmdArray[i];
		if (cmd->GetSize()==1)
		{
			CString strParam=m_CommandArray.GetParam((*cmd)[0]->m_Input);
			m_Tree.InsertItem(cmd->m_CommandName+L" ("+strParam+')',0,0,geoItem);
		}
		else
		{
			HTREEITEM subitem=m_Tree.InsertItem(cmd->m_CommandName,0,0,geoItem);
			for (int j=0;j<cmd->GetSize();j++)
			{
				CString strParam=m_CommandArray.GetParam((*cmd)[j]->m_Input);
				m_Tree.InsertItem(cmd->m_CommandName+L" ("+strParam+')',subitem,NULL);
			}
			m_Tree.SortChildren(subitem);
		}
	}

	for (int i=0;i<m_CommandArray.m_CmdStdArray.GetSize();i++)
	{
		CString str=m_CommandArray.m_CmdStdArray[i].strName+L" ("+m_CommandArray.m_CmdStdArray[i].strInput+L")";
		m_Tree.InsertItem(str,0,0,standardItem);
	}

	for (int i=0;i<m_CommandArray.m_CmdExpArray.GetSize();i++)
	{
		CString str=m_CommandArray.m_CmdExpArray[i].strName+L" ("+m_CommandArray.GetParam(m_CommandArray.m_CmdExpArray[i].strInput)+L")";
		m_Tree.InsertItem(str,0,0,expItem);
	}

	m_Tree.SortChildren(geoItem);
	m_Tree.SortChildren(standardItem);
	m_Tree.SortChildren(expItem);
	m_Tree.Expand(standardItem,TVE_EXPAND);
	m_Tree.Expand(geoItem,TVE_EXPAND);
	m_Tree.SetRedraw(TRUE);
}

void CCommandsOrganizeDlg::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM item=m_Tree.GetSelectedItem();
	if (m_Tree.GetChildItem(item)==NULL && GetTopParent(item)==geoItem)
	{
		CString str=m_Tree.GetItemText(item);
		int pos=str.Find(L" ");
		CString strName=str.Left(pos);
		CString strInput=str.Mid(pos+2, str.GetLength()-pos-3);
		CCommandArray::CCmd* cmd=m_CommandArray[strName];
		int i=cmd->GetIndexByInput(strInput);
		CCommandArray::SubCommand* subCmd=&cmd->m_SubCmdArray[i];

		m_Edit.SetWindowText(subCmd->m_Command);
 		m_Input.SetWindowText(subCmd->m_Input);
 		m_Output.SetWindowText(subCmd->m_Output);
 		m_Name.SetWindowText(cmd->m_CommandName);
 		m_Description.SetWindowText(subCmd->m_Description);
	}
	else if (GetTopParent(item)==standardItem)
	{
		CString str=m_Tree.GetItemText(item);
		int pos=str.Find(L" ");
		CString strName=str.Left(pos);
		int i=m_CommandArray.GetStdIndex(strName);

		m_Edit.SetWindowText(L"");
		m_Input.SetWindowText(m_CommandArray.m_CmdStdArray[i].strInput);
		m_Output.SetWindowText(L"");
		m_Name.SetWindowText(m_CommandArray.m_CmdStdArray[i].strName);
		m_Description.SetWindowText(m_CommandArray.m_CmdStdArray[i].strDescription);
	}
	else if (GetTopParent(item)==expItem)
	{
		CString str=m_Tree.GetItemText(item);
		int pos=str.Find(L" ");
		CString strName=str.Left(pos);
		int i=m_CommandArray.GetExpIndex(strName);

		m_Edit.SetWindowText(L"");
		m_Input.SetWindowText(m_CommandArray.m_CmdExpArray[i].strInput);
		m_Output.SetWindowText(m_CommandArray.m_CmdExpArray[i].strOutput);
		m_Name.SetWindowText(m_CommandArray.m_CmdExpArray[i].strName);
		m_Description.SetWindowText(m_CommandArray.m_CmdExpArray[i].strDescription);
	}
	else
	{
		m_Edit.SetWindowText(L"");
		m_Input.SetWindowText(L"");
		m_Output.SetWindowText(L"");
		m_Name.SetWindowText(L"");
		m_Description.SetWindowText(L"");
	}
	*pResult = 0;
}

HTREEITEM CCommandsOrganizeDlg::GetTopParent(HTREEITEM item)
{
	HTREEITEM parent=m_Tree.GetParentItem(item);
	if (m_Tree.GetParentItem(parent)==NULL) return parent;
	else return m_Tree.GetParentItem(parent);
}

void CCommandsOrganizeDlg::SaveTreeItem(HTREEITEM item)
{
	if (m_Tree.GetChildItem(item)!=NULL || (GetTopParent(item)!=geoItem)) return;
	CString str=m_Tree.GetItemText(item);
	CCommandArray::CCmd* cmd;
	CCommandArray::SubCommand* subCmd;
	m_CommandArray.GetCommandByDeclaration(str,&cmd,&subCmd);

	CString s;
	m_Edit.GetWindowText(s);
	subCmd->m_Command=s;
	m_Input.GetWindowText(s);
	subCmd->m_Input=s;
	// 	m_aCommand.m_aInput[nIndex]=str;
	m_Output.GetWindowText(s);
	subCmd->m_Output=s;
	//m_Name.GetWindowText(s);
	//cmd->m_CommandName=s;
	m_Description.GetWindowText(s);
	subCmd->m_Description=s;

	m_Tree.SetItemText(item,cmd->m_CommandName+L" ("+m_CommandArray.GetParam(subCmd->m_Input)+')');
}

void CCommandsOrganizeDlg::OnTvnSelchangingTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM item=pNMTreeView->itemOld.hItem;
	SaveTreeItem(item);
}

void CCommandsOrganizeDlg::OnBnClickedNew()
{
	HTREEITEM sItem=m_Tree.GetSelectedItem();
	HTREEITEM rItem=m_Tree.GetRootItem();

	CInputDlg dlg(L"Câu lệnh mới");
	dlg.AddStatic(L"Nhập tên câu lệnh");
	CString strName=L"New Command";
	dlg.AddEdit(&strName);

	dlg.AddStatic(L"Nhập Input");
	CString strInput;
	dlg.AddEdit(&strInput);

	dlg.AddStatic(L"Nhập Output");
	CString strOutput;
	dlg.AddEdit(&strOutput);

	if (dlg.DoModal()==IDOK)
	{
		m_CommandArray.AddCommand(strName,L"",L"",strInput,L"",L"",0);
		//m_CommandArray.AddStdCommand(strName,L"",strInput);
		//m_CommandArray.AddExpCommand(strName,L"",strInput,strOutput);
		FillTree();
		//m_Tree.Select(item,TVGN_FIRSTVISIBLE);
	}

// 	CAddToolDlg dlg;
// 	if (dlg.DoModal()==IDOK)
// 	{
// 		if (dlg.m_bAddByCommand)
// 		{
// 			m_CommandArray.AddCommand(dlg.m_CommandName,
// 				dlg.m_Description,dlg.m_Command,
// 				dlg.m_CommandInput,dlg.m_CommandOutput,
// 				dlg.m_ToolName,0);
// 			FillTree();
// 		}
// 	}
}

//void CCommandsOrganizeDlg::OnTvnBeginlabeleditTree(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
//	HTREEITEM item=pTVDispInfo->item.hItem;
//	m_Tree.GetEditControl()->SetWindowText(m_aCommand.m_aCommandName[GetCmd(item)]);
//	*pResult = 0;
//}

void CCommandsOrganizeDlg::OnTvnDeleteitemTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	*pResult = 0;
}

//void CCommandsOrganizeDlg::OnTvnEndlabeleditTree(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
//	if (pTVDispInfo->item.pszText!=NULL)
//	{
//		int i=GetCmd(pTVDispInfo->item.hItem);
//		m_aCommand.m_aCommandName[i]=pTVDispInfo->item.pszText;
//		CString str=pTVDispInfo->item.pszText;
//		str.Append(L" ("+m_aCommand.GetParam(m_aCommand.m_aInput[i])+')');
//		m_Tree.SetItemText(pTVDispInfo->item.hItem,str);
//	}
//	*pResult = 0;
//}


void CCommandsOrganizeDlg::OnOK()
{
	
}

void CCommandsOrganizeDlg::OnTvnKeydownTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVKEYDOWN pTVKeyDown = reinterpret_cast<LPNMTVKEYDOWN>(pNMHDR);
	if (pTVKeyDown->wVKey==VK_DELETE) OnBnClickedDelete();
	*pResult = 0;
}

void CCommandsOrganizeDlg::OnBnClickedDelete()
{
	HTREEITEM item=m_Tree.GetSelectedItem();
	if (m_Tree.GetParentItem(item)==NULL) return;
	if (AfxMessageBox(L"Bạn có muốn xóa câu lệnh "+m_Tree.GetItemText(item)+L" không ?",MB_YESNO)==IDYES)
	{
		CString str=m_Tree.GetItemText(item);
		int pos=str.Find(L" ");
		CString strName=str.Left(pos);
		CString strInput=str.Mid(pos+2, str.GetLength()-pos-3);
		m_CommandArray.Delete(strName,strInput);

		m_Tree.DeleteItem(item);
		FillTree();
	}
}

// void CCommandsOrganizeDlg::OnTcnSelchangeTabCommand(NMHDR *pNMHDR, LRESULT *pResult)
// {
// 	if (m_Tab.GetCurSel()==0)
// 	{
// 		GetDlgItem(IDC_EDIT_OUTPUT)->ShowWindow(FALSE);
// 		GetDlgItem(IDC_EDIT_INPUT)->ShowWindow(FALSE);
// 		GetDlgItem(IDC_EDIT)->ShowWindow(FALSE);
// 		GetDlgItem(IDC_STATIC1)->ShowWindow(FALSE);
// 		GetDlgItem(IDC_STATIC2)->ShowWindow(FALSE);
// 
// 		GetDlgItem(IDC_STATIC3)->ShowWindow(TRUE);
// 		GetDlgItem(IDC_STATIC4)->ShowWindow(TRUE);
// 		GetDlgItem(IDC_EDIT_NAME)->ShowWindow(TRUE);
// 		GetDlgItem(IDC_EDIT_DES)->ShowWindow(TRUE);
// 	}
// 	else
// 	{
// 		GetDlgItem(IDC_EDIT_OUTPUT)->ShowWindow(TRUE);
// 		GetDlgItem(IDC_EDIT_INPUT)->ShowWindow(TRUE);
// 		GetDlgItem(IDC_EDIT)->ShowWindow(TRUE);
// 		GetDlgItem(IDC_STATIC1)->ShowWindow(TRUE);
// 		GetDlgItem(IDC_STATIC2)->ShowWindow(TRUE);
// 
// 		GetDlgItem(IDC_STATIC3)->ShowWindow(FALSE);
// 		GetDlgItem(IDC_STATIC4)->ShowWindow(FALSE);
// 		GetDlgItem(IDC_EDIT_NAME)->ShowWindow(FALSE);
// 		GetDlgItem(IDC_EDIT_DES)->ShowWindow(FALSE);
// 	}
// 	//*pResult = 0;
// }

// HBRUSH CCommandsOrganizeDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
// {
// 	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
// 
// 
// 	if(nCtlColor == CTLCOLOR_STATIC)
// 	{
// 		if (pWnd->GetDlgCtrlID()!=IDC_CHECK_GROUP)
// 		{
// 			hbr=(HBRUSH)(::GetStockObject(HOLLOW_BRUSH));
// 			pDC->SetBkMode(TRANSPARENT);
// 		}
// 	}
// 
// 	// TODO:  Return a different brush if the default is not desired
// 	return hbr;
// }

void CCommandsOrganizeDlg::OnEnChangeEdit()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void CCommandsOrganizeDlg::OnBnClickedButtonOk()
{
	SaveTreeItem(m_Tree.GetSelectedItem());
	
	m_CommandArray.SaveCommands();
	this->OnCancel();
}

void CCommandsOrganizeDlg::OnBnClickedCancel()
{
	this->OnCancel();
}

void CCommandsOrganizeDlg::OnTvnGetdispinfoTreeCommand(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	pTVDispInfo->item.pszText=L"a";

	*pResult = 0;
}
