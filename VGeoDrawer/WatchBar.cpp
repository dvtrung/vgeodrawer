#include "StdAfx.h"
#include "WatchBar.h"
#include "InputDlg.h"

BEGIN_MESSAGE_MAP(CWatchBar, CSizingControlBar)
	//{{AFX_MSG_MAP(CCommandCtrlBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY(NM_RCLICK,ID_WATCH_LIST,CWatchBar::OnListViewRClick)
	ON_NOTIFY(LVN_GETDISPINFO,ID_WATCH_LIST,CWatchBar::OnListGetDispInfo)
	ON_NOTIFY(LVN_ENDLABELEDIT,ID_WATCH_LIST,CWatchBar::OnListEndLabelEdit)
	ON_NOTIFY(LVN_KEYDOWN,ID_WATCH_LIST,CWatchBar::OnListKeyDown)
	ON_NOTIFY(NM_DBLCLK,ID_WATCH_LIST,CWatchBar::OnListLDbClick)
	//}}AFX_MSG_MAP
	//	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

CWatchBar::CWatchBar(void)
{
	m_objCont=NULL;
}

CWatchBar::~CWatchBar(void)
{
}

int CWatchBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CSizingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_List.Create(LVS_EDITLABELS | LVS_REPORT | LVS_NOSORTHEADER | WS_VISIBLE,CRect(0,0,0,0),this,ID_WATCH_LIST);
	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_FLATSB);
	//m_List.SetWorkAreas()
	m_List.InsertColumn(0,L"Biểu thức");
	m_List.InsertColumn(1,L"Giá trị");
	//m_Edit.ModifyStyleEx(0,WS_EX_CLIENTEDGE);

	m_List.InsertItem(0,L"");

	return 0;
}

void CWatchBar::OnListViewRClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	pView->PopupMenu(6,GetMessagePos());
}

void CWatchBar::OnSize(UINT nType, int cx, int cy)
{
	CSizingControlBar::OnSize(nType, cx, cy);
	m_List.MoveWindow(CRect(0,0,cx,cy));
	int w1=m_List.GetColumnWidth(0);
	int w2=m_List.GetColumnWidth(1);
	m_List.SetColumnWidth(0,cx*w1/(w1+w2));
	m_List.SetColumnWidth(1,cx*w2/(w1+w2));
}

void CWatchBar::DoDataExchange(CDataExchange* pDX)
{
	CSizingControlBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCommandWnd)
	DDX_Control(pDX,ID_WATCH_LIST,m_List);
	//}}AFX_DATA_MAP}
}

void CWatchBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHandler)
{
	CSizingControlBar::OnUpdateCmdUI(pTarget, bDisableIfNoHandler);
	UpdateDialogControls(pTarget, bDisableIfNoHandler);
}

CSize CWatchBar::CalcDynamicLayout(int nLength, DWORD dwMode)
{
	CSize size=CSizingControlBar::CalcDynamicLayout(nLength,dwMode);
	//size.cy=30;
	return size;
}

void CWatchBar::Reload()
{
	m_List.DeleteAllItems();	

	for (int i=0;i<m_objCont->m_expWatchArray.GetSize();i++)
		m_List.InsertItem(m_List.GetItemCount(),m_objCont->m_expWatchArray[i]->m_strExp);

	m_List.InsertItem(m_List.GetItemCount(),L"");
}

void CWatchBar::AddWatch()
{
	int i=m_List.InsertItem(m_List.GetItemCount(),m_objCont->m_expWatchArray[m_List.GetItemCount()]->m_strExp);
// 	LVITEM lvI;
// 
// 	lvI.mask = LVIF_TEXT | LVIF_PARAM | LVIF_STATE; 
// 	lvI.state = 0;
// 	lvI.stateMask = 0; 
// 
// 	lvI.iItem = m_List.GetItemCount();
// 	lvI.iImage = -1;
// 	lvI.iSubItem = i;
// 	//lvI.lParam = (LPARAM) &m_objCont->m_expWatchArray[m_List.GetItemCount()]->m_strExp;
// 	lvI.pszText = (LPWSTR)(LPCWSTR)m_objCont->m_expWatchArray[0]->GetStringValue();									  
// 	m_List.InsertItem(&lvI);
}

void CWatchBar::OnListGetDispInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (m_objCont==NULL) return;
	LPNMLVDISPINFOW pTVDispInfo = reinterpret_cast<LPNMLVDISPINFOW>(pNMHDR);

	CString str;

	if (m_objCont->m_expWatchArray.GetSize()!=pTVDispInfo->item.iItem)
	{
		switch (pTVDispInfo->item.iSubItem)
		{
		case 0:
			pTVDispInfo->item.pszText = m_objCont->m_expWatchArray[pTVDispInfo->item.iItem]->m_strExp.GetBuffer(256);
			break;

		case 1:
			//const_cast( static_cast< LPCTSTR >( m_PrinterDeviceName ) )
			str=const_cast<LPTSTR>(static_cast<LPCTSTR>(m_objCont->m_expWatchArray[pTVDispInfo->item.iItem]->GetStringValue()));
			wcsncpy(pTVDispInfo->item.pszText,str.GetBuffer(255),255);// = L"6";//str.GetBuffer(1);
			str.ReleaseBuffer(255);
			break;

		default:
			break;
		}
	}

	*pResult = 0;
}

void CWatchBar::OnListEndLabelEdit( NMHDR *pNMHDR, LRESULT *pResult )
{
	LPNMLVDISPINFOW pLVDispInfo = reinterpret_cast<LPNMLVDISPINFOW>(pNMHDR);
	if (m_objCont->m_expWatchArray.GetSize()==pLVDispInfo->item.iItem)
	{
		if (pLVDispInfo->item.pszText!=L"" && pLVDispInfo->item.pszText!=NULL)
		{
			m_objCont->AddWatch(pLVDispInfo->item.pszText);
			m_List.InsertItem(m_List.GetItemCount(),L"");
		}
	}
	else
	{
		if (CString(pLVDispInfo->item.pszText)==L"")
		{
			if (pLVDispInfo->item.iItem!=m_objCont->m_expWatchArray.GetSize())
			{
				delete m_objCont->m_expWatchArray.GetAt(pLVDispInfo->item.iItem);
				m_objCont->m_expWatchArray.RemoveAt(pLVDispInfo->item.iItem);
				m_List.DeleteItem(pLVDispInfo->item.iItem);
			}
		}
		else
			if (pLVDispInfo->item.pszText!=NULL) m_objCont->m_expWatchArray[pLVDispInfo->item.iItem]->SetExpression(pLVDispInfo->item.pszText,&m_objCont->m_Array);
		//if (pLVDispInfo->item.pszText==L"")
	}
	*pResult = 1;
}

void CWatchBar::OnListKeyDown(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDown = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	if (pLVKeyDown->wVKey==VK_DELETE)
	{
		int count=m_List.GetSelectedCount();
		for (int i=0;i<count;i++)
		{
			POSITION pos = m_List.GetFirstSelectedItemPosition();
			int nItem=m_List.GetNextSelectedItem(pos);
			if (nItem!=m_objCont->m_expWatchArray.GetCount())
			{
				delete m_objCont->m_expWatchArray.GetAt(nItem);
				m_objCont->m_expWatchArray.RemoveAt(nItem);
				m_List.DeleteItem(nItem);
			}
		}
	}
	else if (pLVKeyDown->wVKey==VK_SPACE)
	{
		POSITION pos=m_List.GetFirstSelectedItemPosition();
		if (m_List.GetSelectedCount()==1) m_List.EditLabel(m_List.GetNextSelectedItem(pos));
	}
	*pResult = 0;
}

void CWatchBar::OnListLDbClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHDR pLV = reinterpret_cast<LPNMHDR>(pNMHDR);
	POSITION pos=m_List.GetFirstSelectedItemPosition();
	m_List.EditLabel(m_List.GetNextSelectedItem(pos));
	*pResult = 0;
}