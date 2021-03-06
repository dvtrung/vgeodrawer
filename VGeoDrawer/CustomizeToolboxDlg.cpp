// CustomizeToolboxDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VGeoDrawer.h"
#include "CustomizeToolboxDlg.h"
#include "appfunc.h"

IMPLEMENT_DYNAMIC(CCustomizeToolboxDlg, CDialog)

CCustomizeToolboxDlg::CCustomizeToolboxDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCustomizeToolboxDlg::IDD, pParent)
{
	
}

CCustomizeToolboxDlg::~CCustomizeToolboxDlg()
{
}

void CCustomizeToolboxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE, m_TreeCtrl);
	DDX_Control(pDX, IDC_ITEMLIST, m_TreeItemCtrl);
	DDX_Control(pDX, IDC_COMMAND_LIST, m_CommandList);
}


BEGIN_MESSAGE_MAP(CCustomizeToolboxDlg, CDialog)
	ON_WM_CREATE()
//	ON_BN_CLICKED(IDC_BUTTON5, &CCustomizeToolboxDlg::OnBnClickedButton5)
ON_BN_CLICKED(IDC_BTN_CHANGEICON, &CCustomizeToolboxDlg::OnBnClickedBtnChangeIcon)
ON_BN_CLICKED(IDOK, &CCustomizeToolboxDlg::OnBnClickedOk)
ON_BN_CLICKED(IDC_BTNDOWN, &CCustomizeToolboxDlg::OnBnClickedBtndown)
ON_BN_CLICKED(IDC_ADD, &CCustomizeToolboxDlg::OnBnClickedAdd)
ON_NOTIFY(NM_DBLCLK, IDC_ITEMLIST, &CCustomizeToolboxDlg::OnNMDblclkItemlist)
ON_BN_CLICKED(IDC_BTNINS, &CCustomizeToolboxDlg::OnBnClickedButton1)
ON_BN_CLICKED(IDC_BTNREMOVE, &CCustomizeToolboxDlg::OnBnClickedBtnremove)
ON_BN_CLICKED(IDC_BTNUP, &CCustomizeToolboxDlg::OnBnClickedBtnup)
//ON_NOTIFY(TVN_SELCHANGED, IDC_ITEMLIST, &CCustomizeToolboxDlg::OnTvnSelchangedItemlist)
ON_BN_CLICKED(IDC_REMOVE, &CCustomizeToolboxDlg::OnBnClickedRemove)
ON_NOTIFY(TVN_SELCHANGED, IDC_ITEMLIST, &CCustomizeToolboxDlg::OnTvnSelchangedItemlist)
ON_BN_CLICKED(IDC_BTN_ADD_ITEM_COMMAND, &CCustomizeToolboxDlg::OnBnClickedBtnAddItemCommand)
//ON_NOTIFY(TVN_KEYDOWN, IDC_COMMAND_LIST, &CCustomizeToolboxDlg::OnTvnKeydownCommandList)
//ON_NOTIFY(TVN_SELCHANGED, IDC_COMMAND_LIST, &CCustomizeToolboxDlg::OnTvnSelchangedCommandList)
//ON_NOTIFY(TVN_BEGINLABELEDIT, IDC_ITEMLIST, &CCustomizeToolboxDlg::OnTvnBeginlabeleditItemlist)
ON_NOTIFY(TVN_ENDLABELEDIT, IDC_ITEMLIST, &CCustomizeToolboxDlg::OnTvnEndlabeleditItemlist)
ON_NOTIFY(TVN_BEGINLABELEDIT, IDC_ITEMLIST, &CCustomizeToolboxDlg::OnTvnBeginlabeleditItemlist)
ON_NOTIFY(TVN_KEYDOWN, IDC_ITEMLIST, &CCustomizeToolboxDlg::OnTvnKeydownItemlist)
ON_NOTIFY(TVN_KEYDOWN, IDC_COMMAND_LIST, &CCustomizeToolboxDlg::OnTvnKeydownCommandList)
ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, &CCustomizeToolboxDlg::OnTvnSelchangedTree)
END_MESSAGE_MAP()


// CCustomizeToolboxDlg message handlers

int CCustomizeToolboxDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	//m_ListView.SetImageList(&m_ImageList,LVSIL_NORMAL);

	return 0;
}


void CCustomizeToolboxDlg::OnBnClickedBtnChangeIcon()
{
	// Vị trí sửa
	CCommandArray::CCmd* cmd;
	CCommandArray::SubCommand* subCmd;

	CInputDlg m_InputDlg(L"Add Item");
	CString strItemName,strCmd;

	m_InputDlg.AddStatic(L"Tool name :");
	m_InputDlg.AddEdit(&strItemName);
	m_InputDlg.AddStatic(L"Command :");

	CStringArray strArray;

	for (int i=0;i<m_cmdArray.GetSize();i++)
	{
		cmd=m_cmdArray[i];
		for (int j=0;j<cmd->GetSize();j++)
		{
			if ((*cmd)[j]->m_ToolName==L"")
			{
				CString strParam=m_cmdArray.GetParam((*cmd)[j]->m_Input);
				strArray.Add(cmd->m_CommandName+L" ("+strParam+')');
			}
		}
	}
	m_InputDlg.AddComboBox(&strCmd,&strArray,WS_VISIBLE | WS_BORDER | CBS_SORT | CBS_DROPDOWNLIST);

	if (m_InputDlg.DoModal()==IDOK)
	{
		for (int i=0;i<m_cmdArray.GetSize();i++)
		{
			for (int j=0;j<m_cmdArray[i]->GetSize();j++)
			{
				if (m_cmdArray[i]->m_SubCmdArray[j].m_ToolName==strItemName)
				{
					AfxGetApp()->DoMessageBox(L"There exist an item whose name is '"+strItemName+L"'",MB_ICONERROR | MB_OK,-1);
					return;
				}
			}
		}

		int pos=strCmd.Find(L" ");
		cmd=m_cmdArray[strCmd.Left(pos)];
		subCmd=&cmd->m_SubCmdArray[cmd->GetIndexByInput(strCmd.Mid(pos+2,strCmd.GetLength()-pos-3))];
	
		//Edit Icon

		CEditIconDlg dlg;

		// 	int nIndex=m_ListCtrl.GetNextSelectedItem(pos); // Vị trí item được chọn
		// 	CString str=m_ListCtrl.GetItemText(nIndex,0); // Item Text
		// 	cmd=(*m_cmdArray)[str];
		//HICON icon=m_cmdArray.m_ImageList.ExtractIcon(subCmd->m_ImageIndex);
		Bitmap bmp(32,32,PixelFormat32bppARGB);
		//if (subCmd->m_ImageIndex==-1)
		//{
		//bmp.FromHBITMAP();
		//}
		dlg.bmp=&bmp;

		if (dlg.DoModal()==IDOK)
		{
			subCmd->m_ToolName=strItemName;

			HICON hIcon;
			dlg.m_IconEditor.m_bmp->GetHICON(&hIcon);

			m_cmdArray.m_ImageList.Add(hIcon);
			subCmd->m_ImageIndex=m_cmdArray.m_ImageList.GetImageCount()-1;
			m_TreeItemCtrl.InsertItem(subCmd->m_ToolName,subCmd->m_ImageIndex,subCmd->m_ImageIndex,TVI_ROOT,TVI_SORT);
		}
	}
}

BOOL CCustomizeToolboxDlg::Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd)
{
	return CDialog::Create(lpszTemplateName, pParentWnd);
}

BOOL CCustomizeToolboxDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CFile file;
	//Nạp item trong TreeItemCtrl

	m_cmdArray.LoadCommands();

	CCommandArray::CCmd* cmd;
	CStringArray toolName; // Array kiểm tra trùng tên ToolName không ?
	for (int i=0;i<m_cmdArray.GetSize();i++)
	{
		cmd=m_cmdArray[i];
		for (int j=0;j<cmd->GetSize();j++)
		{
			if ((*cmd)[j]->m_ToolName!=L"")
			{
				int k=0;
				for (k;k<toolName.GetSize();k++)
				{
					if ((*cmd)[j]->m_ToolName==toolName[k]) break;
				}
				if (k==toolName.GetSize())
				{
					toolName.Add((*cmd)[j]->m_ToolName);
					CString strParam=m_cmdArray.GetParam((*cmd)[j]->m_Input);
					m_TreeItemCtrl.InsertItem(/*cmd->m_CommandName+L" ("+strParam+')'*/(*cmd)[j]->m_ToolName,
						(*cmd)[j]->m_ImageIndex,(*cmd)[j]->m_ImageIndex);
				}
			}
		}
	}

	// Nạp m_TreeCtrl

	file.Open(GetAppDirectory()+L"\\data\\tbitem.dat",CFile::modeRead);
	CArchive ar(&file,CArchive::load);
	CString str;

	BOOL b;
	HTREEITEM rItem;

	int count;
	ar >> count;
	int imgIndex;
	for (int i=0;i<count;i++)
	{
		ar >> str;
		ar >> b;
		imgIndex=m_cmdArray.GetToolImageIndex(str);
		if (b)
			rItem=m_TreeCtrl.InsertItem(str,imgIndex,imgIndex,TVI_ROOT,TVI_LAST);
		else 
			m_TreeCtrl.InsertItem(str,imgIndex,imgIndex,rItem);
	}

	ar.Close();
	file.Close();

	//
	
	m_TreeItemCtrl.SortChildren(TVI_ROOT);
	m_TreeCtrl.SetImageList(&m_cmdArray.m_ImageList,TVSIL_NORMAL);
	m_TreeItemCtrl.SetImageList(&m_cmdArray.m_ImageList,TVSIL_NORMAL);

	//((CButton*)GetDlgItem(IDC_BTNDOWN))->SetIcon(AfxGetApp()->LoadIcon(IDI_UP));

	return TRUE;
}

void CCustomizeToolboxDlg::OnOK()
{
	CFile file;
	file.Open(GetAppDirectory()+L"\\data\\tbitem.dat",CFile::modeWrite);
	CArchive ar(&file,CArchive::store);
	CString str; BOOL b;

	HTREEITEM item = m_TreeCtrl.GetChildItem(TVI_ROOT);

	ar << m_TreeCtrl.GetCount();

	while (item!=NULL)
	{
		CString str=m_TreeCtrl.GetItemText(item);
		ar << str;
		b=(m_TreeCtrl.GetParentItem(item)==NULL);
		ar << b;

		if (m_TreeCtrl.GetChildItem(item)==NULL)
		{
			HTREEITEM item2=m_TreeCtrl.GetNextSiblingItem(item);
			if (item2==NULL) item2=m_TreeCtrl.GetNextSiblingItem(m_TreeCtrl.GetParentItem(item));
			item=item2;
		}
		else
			item=m_TreeCtrl.GetChildItem(item);
	}	

// 	for (int i=0;i<m_ListCtrl.GetItemCount();i++)
// 	{
// 		ar << m_ListCtrl.GetItemText(i,0);
// 		TCHAR szBuffer[1024];
// 		DWORD cchBuf(1024);
// 		lvi.iItem = i;
// 		lvi.iSubItem = 0;
// 		lvi.mask = LVIF_TEXT | LVIF_IMAGE;
// 		lvi.pszText = szBuffer;
// 		lvi.cchTextMax = cchBuf;
// 		m_ListCtrl.GetItem(&lvi);
// 		
// 		m_ImageList.GetImageInfo(lvi.iImage,&ImgInfo);
// 		Bitmap bmp(ImgInfo.hbmImage,NULL);
// 
// 		CLSID clsid;
// 		GetEncoderClsid(L"image/bmp",&clsid);
// 		CString path=CAppFunc::GetAppDirectory()+L"\\data\\bmp\\"+m_ListCtrl.GetItemText(i,0)+L".bmp";
// 		bmp.Save(path,&clsid,NULL);
//	}
	
// 	for (int i=0;i<m_ImageList.GetImageCount();i++)
// 	{
// 		archive << m_ImageList.GetImageInfo(i,&pImgInfo);
// 		Bitmap bmp(pImgInfo.hbmImage,NULL);
// 		bmp.Save("")
// 	}

	ar.Close();
	file.Close();
	this->OnCancel();

	CDialog::OnOK();
}


void CCustomizeToolboxDlg::OnBnClickedOk()
{
	m_cmdArray.SaveCommands();

	OnOK();
}

void CCustomizeToolboxDlg::OnBnClickedAdd()
{
	m_TreeCtrl.InsertItem(L"<Please insert items>",-1,-1);
}

void CCustomizeToolboxDlg::OnNMDblclkItemlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	HTREEITEM item=m_TreeItemCtrl.GetSelectedItem();
	CString strToolName=m_TreeItemCtrl.GetItemText(item);
	// Vị trí sửa
	CCommandArray::SubCommand* subCmd=NULL;

	for (int i=0;i<m_cmdArray.GetSize();i++)
	{
		if (subCmd!=NULL) break;
		for (int j=0;j<m_cmdArray[i]->GetSize();j++)
		{
			if (m_cmdArray[i]->m_SubCmdArray[j].m_ToolName==strToolName)
			{
				subCmd=&m_cmdArray[i]->m_SubCmdArray[j];
				break;
			}
		}
	}


	//Edit Icon

	CEditIconDlg dlg;

	HICON icon=m_cmdArray.m_ImageList.ExtractIcon(subCmd->m_ImageIndex);
	//Bitmap bmp(icon);
	
	dlg.bmp=Bitmap::FromHICON(icon);

	if (dlg.DoModal()==IDOK)
	{
		HICON hIcon;
		dlg.m_IconEditor.m_bmp->GetHICON(&hIcon);
// 		HBITMAP hBitmap;
// 		dlg.m_IconEditor.m_bmp->GetHBITMAP(Color(255,255,255),&hBitmap);
// 
// 		CBitmap bmp;
// 		bmp.Attach(hBitmap);
		m_cmdArray.m_ImageList.Replace(subCmd->m_ImageIndex,hIcon);
		//m_ListCtrl.RedrawItems(0,m_ListCtrl.GetItemCount()-1);
		//m_ListCtrl.SetImageList(&m_cmdArray->m_ImageList,LVSIL_NORMAL);

		/*bmp.DeleteObject();*/
	}
	delete dlg.bmp;

	m_TreeCtrl.RedrawWindow();
	m_TreeItemCtrl.RedrawWindow();
}

void CCustomizeToolboxDlg::OnBnClickedButton1()
{
	HTREEITEM item1=m_TreeCtrl.GetSelectedItem();
	HTREEITEM item2=m_TreeItemCtrl.GetSelectedItem();
	if (item1==NULL || item2==NULL) return;
	int i,j;
	m_TreeItemCtrl.GetItemImage(item2,i,j);
	if (m_TreeCtrl.GetChildItem(item1)==NULL && m_TreeCtrl.GetParentItem(item1)==NULL)
	{
		m_TreeCtrl.SetItemImage(item1,i,j);
		m_TreeCtrl.SetItemText(item1,m_TreeItemCtrl.GetItemText(item2));
	}

	if (m_TreeCtrl.GetParentItem(item1)==NULL)
		m_TreeCtrl.InsertItem(m_TreeItemCtrl.GetItemText(item2),i,j,item1);
	else
		m_TreeCtrl.InsertItem(m_TreeItemCtrl.GetItemText(item2),i,j,m_TreeCtrl.GetParentItem(item1),item1);
}

void CCustomizeToolboxDlg::OnBnClickedBtnremove()
{
	HTREEITEM item=m_TreeCtrl.GetSelectedItem();
	if (m_TreeCtrl.GetParentItem(item)==NULL) return;
	m_TreeCtrl.DeleteItem(item);
}

void CCustomizeToolboxDlg::OnBnClickedBtnup()
{
	m_TreeCtrl.SetRedraw(FALSE);
	HTREEITEM item=m_TreeCtrl.GetSelectedItem();
	if (item==NULL) return;
	if (m_TreeCtrl.GetPrevSiblingItem(item)!=NULL)
	{
		if (m_TreeCtrl.GetParentItem(item)!=NULL && 
			m_TreeCtrl.GetPrevSiblingItem((m_TreeCtrl.GetPrevSiblingItem(item)))==NULL)
		{	// Đổi tên category
			HTREEITEM parent=m_TreeCtrl.GetParentItem(item);
			m_TreeCtrl.SetItemText(parent,m_TreeCtrl.GetItemText(item));
			int i;
			m_TreeCtrl.GetItemImage(item,i,i);
			m_TreeCtrl.SetItemImage(parent,i,i);
		}
		MoveTreeItem(item,m_TreeCtrl.GetParentItem(item),m_TreeCtrl.GetPrevSiblingItem(m_TreeCtrl.GetPrevSiblingItem(item)));
	}
	m_TreeCtrl.SetRedraw(TRUE);
}

void CCustomizeToolboxDlg::OnBnClickedBtndown()
{
	m_TreeCtrl.SetRedraw(FALSE);
	HTREEITEM item=m_TreeCtrl.GetSelectedItem();
	if (item==NULL) return;
	if (m_TreeCtrl.GetNextSiblingItem(item)!=NULL)
	{
		if (m_TreeCtrl.GetParentItem(item)!=NULL && 
			m_TreeCtrl.GetPrevSiblingItem(item)==NULL)
		{	// Đổi tên category
			HTREEITEM parent=m_TreeCtrl.GetParentItem(item);
			HTREEITEM newFirstItem=m_TreeCtrl.GetNextSiblingItem(item);
			m_TreeCtrl.SetItemText(parent,m_TreeCtrl.GetItemText(newFirstItem));
			int i;
			m_TreeCtrl.GetItemImage(newFirstItem,i,i);
			m_TreeCtrl.SetItemImage(parent,i,i);
		}
		MoveTreeItem(item,m_TreeCtrl.GetParentItem(item),m_TreeCtrl.GetNextSiblingItem(item));
	}
	m_TreeCtrl.SetRedraw(TRUE);
}

void CCustomizeToolboxDlg::MoveTreeItem(HTREEITEM hItem, HTREEITEM hNewParent, HTREEITEM hInsertAfter)
{
	CString str=m_TreeCtrl.GetItemText(hItem);
	int imageIndex;
	m_TreeCtrl.GetItemImage(hItem,imageIndex,imageIndex);
	HTREEITEM newItem=m_TreeCtrl.InsertItem(str,imageIndex,imageIndex,hNewParent,hInsertAfter==NULL ? TVI_FIRST : hInsertAfter);
	HTREEITEM childItem=m_TreeCtrl.GetChildItem(hItem);
	while (childItem!=NULL)
	{
		HTREEITEM newChildItem=m_TreeCtrl.GetNextSiblingItem(childItem);
		MoveTreeItem(childItem,newItem,TVI_LAST);
		childItem=newChildItem;
	}
	m_TreeCtrl.DeleteItem(hItem);
	m_TreeCtrl.SelectItem(newItem);
	m_TreeCtrl.Expand(newItem,TVE_COLLAPSE);
}
void CCustomizeToolboxDlg::OnBnClickedRemove()
{
	HTREEITEM item=m_TreeCtrl.GetSelectedItem();
	if (m_TreeCtrl.GetParentItem(item)==NULL)
	{
		if (MessageBox(L"Do you want to delete this category ?",AfxGetApp()->m_pszAppName,MB_YESNO)==IDYES)
			m_TreeCtrl.DeleteItem(item);
	}
}

void CCustomizeToolboxDlg::OnTvnSelchangedItemlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	HTREEITEM item=m_TreeItemCtrl.GetSelectedItem();
	if (item==NULL) return;
	CString strText=m_TreeItemCtrl.GetItemText(item);
	m_CommandList.SetRedraw(FALSE);
	m_CommandList.DeleteAllItems();

	for (int i=0;i<m_cmdArray.GetSize();i++)
	{
		for (int j=0;j<m_cmdArray[i]->GetSize();j++)
		{
			if (m_cmdArray[i]->m_SubCmdArray[j].m_ToolName==strText)
			{
				m_CommandList.InsertItem(m_cmdArray[i]->m_CommandName+L" ("+m_cmdArray.GetParam(m_cmdArray[i]->m_SubCmdArray[j].m_Input)+L")",-1,-1);
			}
		}
	}
	m_CommandList.SetRedraw(TRUE);
}

void CCustomizeToolboxDlg::OnBnClickedBtnAddItemCommand()
{
	HTREEITEM item=m_TreeItemCtrl.GetSelectedItem();
	if (item==NULL) return;
	CString strToolName=m_TreeItemCtrl.GetItemText(item);
	CCommandArray::CCmd* cmd;
	CCommandArray::SubCommand* subCmd;

	CInputDlg m_InputDlg(L"Add Item");
	CString strCmd;

	m_InputDlg.AddStatic(L"Command :");

	CStringArray strArray;

	for (int i=0;i<m_cmdArray.GetSize();i++)
	{
		cmd=m_cmdArray[i];
		for (int j=0;j<cmd->GetSize();j++)
		{
			if ((*cmd)[j]->m_ToolName==L"")
			{
				CString strParam=m_cmdArray.GetParam((*cmd)[j]->m_Input);
				strArray.Add(cmd->m_CommandName+L" ("+strParam+')');
			}
		}
	}
	m_InputDlg.AddComboBox(&strCmd,&strArray,WS_VISIBLE | WS_BORDER | CBS_SORT | CBS_DROPDOWNLIST);

	if (m_InputDlg.DoModal()==IDOK)
	{
		m_CommandList.InsertItem(strCmd,-1,-1);
		int image=m_cmdArray.GetToolImageIndex(strToolName);
		m_cmdArray.GetCommandByDeclaration(strCmd,&cmd,&subCmd);
		subCmd->m_ToolName=strToolName;
		subCmd->m_ImageIndex=image;
	}
}
void CCustomizeToolboxDlg::OnTvnKeydownCommandList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVKEYDOWN pTVKeyDown = reinterpret_cast<LPNMTVKEYDOWN>(pNMHDR);
	
	HTREEITEM item=m_CommandList.GetSelectedItem();
	if (item==NULL) return;
	if (pTVKeyDown->wVKey==VK_DELETE)
	{
		if (AfxGetApp()->DoMessageBox(L"Do you want to remove this command ?",MB_YESNO | MB_ICONASTERISK,-1)==IDYES)
		{
			CString str=m_CommandList.GetItemText(item);
			CCommandArray::CCmd* cmd;
			CCommandArray::SubCommand* subCmd;
			m_cmdArray.GetCommandByDeclaration(str,&cmd,&subCmd);
			subCmd->m_ToolName=L"";
			m_CommandList.DeleteItem(item);
		}
	}

	*pResult = 0;
}

void CCustomizeToolboxDlg::OnTvnBeginlabeleditItemlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	strOldText=pTVDispInfo->item.pszText;
	*pResult = 0;
}

void CCustomizeToolboxDlg::OnTvnEndlabeleditItemlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);

	if (pTVDispInfo->item.pszText==NULL) return;
	for (int i=0;i<m_cmdArray.GetSize();i++)
	{
		for (int j=0;j<m_cmdArray[i]->GetSize();j++)
		{
			if (m_cmdArray[i]->m_SubCmdArray[j].m_ToolName==pTVDispInfo->item.pszText)
			{
				AfxGetApp()->DoMessageBox(L"A tool with the name you specified already exist",MB_OK | MB_ICONERROR, -1);
				return;
			}
		}
	}

	HTREEITEM item=m_TreeItemCtrl.GetSelectedItem();
	m_TreeItemCtrl.SetItemText(item,pTVDispInfo->item.pszText);
	for (int i=0;i<m_cmdArray.GetSize();i++)
	{
		for (int j=0;j<m_cmdArray[i]->GetSize();j++)
		{
			if (m_cmdArray[i]->m_SubCmdArray[j].m_ToolName==strOldText)
			{
				m_cmdArray[i]->m_SubCmdArray[j].m_ToolName=pTVDispInfo->item.pszText;
			}
		}
	}
	// Đổi tên trong TreeCtrl
	CString str;

	HTREEITEM tvitem = m_TreeCtrl.GetChildItem(TVI_ROOT);
	while (tvitem!=NULL)
	{
		if (m_TreeCtrl.GetItemText(tvitem)==strOldText)
		{
			m_TreeCtrl.SetItemText(tvitem,pTVDispInfo->item.pszText);
		}
		if (m_TreeCtrl.GetChildItem(tvitem)==NULL)
		{
			HTREEITEM item2=m_TreeCtrl.GetNextSiblingItem(tvitem);
			if (item2==NULL) item2=m_TreeCtrl.GetNextSiblingItem(m_TreeCtrl.GetParentItem(tvitem));
			tvitem=item2;
		}
		else
			tvitem=m_TreeCtrl.GetChildItem(tvitem);
	}	

	*pResult = 0;
}

void CCustomizeToolboxDlg::OnTvnKeydownItemlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVKEYDOWN pTVKeyDown = reinterpret_cast<LPNMTVKEYDOWN>(pNMHDR);
	if (pTVKeyDown->wVKey==VK_DELETE)
	{
		if (AfxGetApp()->DoMessageBox(L"Do you want to delete this toolbox item ?", MB_ICONASTERISK | MB_YESNO,-1)==IDYES)
		{
			HTREEITEM item=m_TreeItemCtrl.GetSelectedItem();
			CString str=m_TreeItemCtrl.GetItemText(item);
			m_TreeItemCtrl.DeleteItem(item);
			for (int i=0;i<m_cmdArray.GetSize();i++)
			{
				for (int j=0;j<m_cmdArray[i]->GetSize();j++)
				{
					if (m_cmdArray[i]->m_SubCmdArray[j].m_ToolName==str)
					{
						m_cmdArray[i]->m_SubCmdArray[j].m_ToolName="";
						m_cmdArray[i]->m_SubCmdArray[j].m_ImageIndex=-1;
					}
				}
			}
		}
	}
	*pResult = 0;
}

void CCustomizeToolboxDlg::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}
