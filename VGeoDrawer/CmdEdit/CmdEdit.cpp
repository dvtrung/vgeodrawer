// mdEdit.cpp : implementation file
//

#include "stdafx.h"
#include "CmdEdit.h"
#include "..\VGObject.h"

// CCmdEdit

IMPLEMENT_DYNAMIC(CCmdEdit, CEdit)

CCmdEdit::CCmdEdit()
{
	m_objArr=NULL;	
}

CCmdEdit::~CCmdEdit()
{
}


BEGIN_MESSAGE_MAP(CCmdEdit, CEdit)
	ON_WM_KEYDOWN()
	ON_CONTROL_REFLECT(EN_CHANGE, &CCmdEdit::OnEnChange)
	ON_WM_CREATE()
	ON_WM_CHAR()
	ON_WM_KILLFOCUS()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()



// CCmdEdit message handlers



void CCmdEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar==VK_UP)
	{
		if (m_List.GetDroppedState())
		{
			if (m_List.GetCurSel()>0) m_List.SetCurSel(m_List.GetCurSel()-1);
			return;
		}
		if (m_wndTT.IsWindowVisible() && m_wndTT.m_setMethods.size()!=1)
		{
			m_wndTT.ShowPrevMethod();
			return;
		}
	}
	else if (nChar==VK_DOWN)
	{
		if (m_List.GetDroppedState())
		{
			m_List.SetCurSel(m_List.GetCurSel()+1);
			return;
		}
		if (m_wndTT.IsWindowVisible() && m_wndTT.m_setMethods.size()!=1)
		{
			m_wndTT.ShowNextMethod();
			return;
		}
	}
	else if (nChar==VK_LEFT || nChar==VK_RIGHT)
	{
		m_List.ShowDropDown(FALSE);
	}

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
	ShowParamToolTip();
}

void CCmdEdit::ShowParamToolTip()
{
	CString strText, str;
	GetWindowText(strText);
	int s,e;
	GetSel(s,e);
	int i;
	int paramPos=0;
	if (s==e)
	{
		int d=1;
		int j;
		for (i=s-1;i>=0;i--)
		{
			char c=strText[i];
			if (strText[i]=='(') d--;
			else if (strText[i]==')') d++;
			else if (strText[i]==',' && d==1) paramPos++;
			if (d==0) break;
		}

		for (j=i-1;j>=0;j--)
			if (!((strText[j]>='a' && strText[j]<='z') || (strText[j]>='A' && strText[j]<='Z'))) break;
		str=strText.Mid(j+1,i-j-1);

	}

	CCommandArray::CCmd* cmd=m_cmdArray->GetCommandByString(str);
	if (cmd!=NULL)
	{
		m_wndTT.FlushMethods();
		for (int i=0;i<cmd->GetSize();i++)
		{
			vector<CString> vec;
			m_cmdArray->GetParamText(cmd->m_SubCmdArray[i].m_Input,vec);
			m_wndTT.AddMethod(cmd->m_CommandName,vec,cmd->m_SubCmdArray[i].m_Description);
		}
		m_wndTT.SetCurMethod(0);
		m_wndTT.SetCurParam(paramPos);
		CPoint pt=PosFromChar(i);
		//ClientToScreen(&pt);
		m_wndTT.ShowTooltip(CPoint(pt.x,pt.y+20));
		m_List.ShowDropDown(FALSE);
	}
	else m_wndTT.ShowWindow(SW_HIDE);
}

void CCmdEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar==VK_RETURN)
	{
		if (m_List.GetDroppedState())
		{
			CString str;
			m_List.GetWindowText(str);
			ReplaceLastWord(str);
			ShowParamToolTip();
			m_List.ShowDropDown(FALSE);
			return;
		}
		else GetParent()->SendMessage(WM_EDIT_ENTER);
	}
	else if (nChar=='(')
	{
		ShowParamToolTip();
	}
	else if (nChar==')')
	{
		m_wndTT.ShowWindow(SW_HIDE);
	}
	else if (nChar==':')
	{
		CEdit::OnChar(nChar, nRepCnt, nFlags);
		FillList();
		ShowDropDown();
		return;
	}
	else if (nChar=='=')
	{
		int s,e;
		GetSel(s,e);
		if (s==e);
		{
			CString str;
			GetWindowText(str);
			char c=str[s-1];
			if (c=='>')
			{
				this->SetSel(s-1,s);
				ReplaceSel(L"≥");
				return;
			}
			if (c=='<')
			{
				this->SetSel(s-1,s);
				ReplaceSel(L"≤");
				return;
			}
		}
	}

	CEdit::OnChar(nChar, nRepCnt, nFlags);

	ShowParamToolTip();
	if (m_wndTT.IsWindowVisible()) return;
	int s,e;
	GetSel(s,e);
	if (s==e)
	{
		m_List.ResetContent();
		CString str=GetLastWord();
		BOOL bExact=FALSE;
		if (str!=L"")
		{
			for (int i=0;i<m_cmdArray->GetSize();i++)
				if (m_cmdArray->m_CmdArray[i]->m_CommandName.Left(str.GetLength())==str)
				{
					m_List.AddString(m_cmdArray->m_CmdArray[i]->m_CommandName);
					if (m_cmdArray->m_CmdArray[i]->m_CommandName.GetLength()==str.GetLength())
						bExact=TRUE;
				}
			for (int i=0;i<m_cmdArray->m_CmdStdArray.GetSize();i++)
				if (m_cmdArray->m_CmdStdArray[i].strName.Left(str.GetLength())==str)
				{
					m_List.AddString(m_cmdArray->m_CmdStdArray[i].strName);
					if (m_cmdArray->m_CmdStdArray[i].strName.GetLength()==str.GetLength())
						bExact=TRUE;
				}
			for (int i=0;i<m_cmdArray->m_CmdExpArray.GetSize();i++)
				if (m_cmdArray->m_CmdExpArray[i].strName.Left(str.GetLength())==str)
				{
					m_List.AddString(m_cmdArray->m_CmdExpArray[i].strName);
					if (m_cmdArray->m_CmdExpArray[i].strName.GetLength()==str.GetLength())
						bExact=TRUE;
				}
			if (m_objArr!=NULL)
			{
				for (int i=0;i<m_objArr->GetSize();i++)
				{
					if (!m_objArr->GetAt(i)->m_bTemp && m_objArr->GetAt(i)->m_Name.Left(str.GetLength())==str)
					{
						m_List.AddString(m_objArr->GetAt(i)->m_Name);
						if (m_objArr->GetAt(i)->m_Name.GetLength()==str.GetLength())
							bExact=TRUE;
					}
				}
			}
			for (int i=0;i<m_List.GetCount();i++)
			{
				m_List.SetItemHeight(i,18);
			}

		}

		if (m_List.GetCount()!=0 && str!=L"" && !(/*m_List.GetCount()==1 &&*/ bExact))
		{
			m_List.SetCurSel(0);
			//m_List.ShowDropDown(FALSE);
			//ShowDropDown();
			ShowDropDown();
		}
		else
			m_List.ShowDropDown(FALSE);
	}
}

CString CCmdEdit::GetLastWord()
{
	int s,e;
	GetSel(s,e);
	if (s==e)
	{
		CString str;
		GetWindowText(str);
		int i;
		for (i=s-1;i>=0;i--)
			if (!((str[i]>='a' && str[i]<='z') || (str[i]>='A' && str[i]<='Z'))) break;
		return str.Mid(i+1,s-i-1);
	}
	return NULL;
}

void CCmdEdit::ReplaceLastWord(CString strText)
{
	int s,e;
	GetSel(s,e);
	if (s==e)
	{
		CString str;
		GetWindowText(str);
		int i;
		for (i=s-1;i>=0;i--)
			if (!((str[i]>='a' && str[i]<='z') || (str[i]>='A' && str[i]<='Z'))) break;

		ReplaceSel(strText.Right(strText.GetLength()-(s-i-1))+L"()",TRUE);
		SendMessage(WM_KEYDOWN, VK_LEFT, 0);
	}
}

void CCmdEdit::OnEnChange()
{
	
}

void CCmdEdit::ShowDropDown()
{
	CPoint pt=GetCaretPos();
	m_List.MoveWindow(CRect(CPoint(pt.x+5,pt.y),
		CPoint(pt.x+200,pt.y+100)));
	m_wndTT.ShowWindow(SW_HIDE);
	m_List.ShowDropDown(TRUE);
	SendMessage(WM_SETCURSOR,0,0);
}

int CCmdEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

	InitCtrl();

	return 0;
}

void CCmdEdit::FillList()
{
	m_List.ResetContent();
	m_List.SetRedraw(FALSE);
	for (int i=0;i<m_cmdArray->GetSize();i++)
	{
		m_List.AddString(m_cmdArray->m_CmdArray[i]->m_CommandName);
	}
	for (int i=0;i<m_cmdArray->m_CmdStdArray.GetSize();i++)
	{
		m_List.AddString(m_cmdArray->m_CmdStdArray[i].strName);
	}
	for (int i=0;i<m_cmdArray->m_CmdExpArray.GetSize();i++)
	{
		m_List.AddString(m_cmdArray->m_CmdExpArray[i].strName);
	}
	if (m_objArr!=NULL)
	{ 
		for (int i=0;i<m_objArr->GetSize();i++)
		{
			if (!m_objArr->GetAt(i)->m_bTemp)
				m_List.AddString(m_objArr->GetAt(i)->m_Name);
		}
	}
	for (int i=0;i<m_List.GetCount();i++)
	{
		m_List.SetItemHeight(i,18);
	}
	m_List.SetRedraw(TRUE);
}

BOOL CCmdEdit::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	return CEdit::Create(dwStyle | ES_WANTRETURN | ES_MULTILINE | ES_AUTOHSCROLL, rect, pParentWnd, nID);
}

void CCmdEdit::InitCtrl()
{
	m_List.Create(CBS_DROPDOWNLIST | WS_VSCROLL | WS_CHILD | CBS_HASSTRINGS | CBS_DISABLENOSCROLL | CBS_NOINTEGRALHEIGHT,
		CRect(0,0,100,300),this,1000);
	//m_List.Set

	fontTahoma.CreatePointFont(80,L"Tahoma");
	m_List.SetFont(&fontTahoma);

	fontCourierNew.CreatePointFont(100,L"Courier New");
	this->SetFont(&fontCourierNew);

	m_wndTT.Create(this);
	m_wndTT.EnableWindow(FALSE);

}

void CCmdEdit::OnKillFocus(CWnd* pNewWnd)
{
	m_wndTT.ShowWindow(SW_HIDE);

	CEdit::OnKillFocus(pNewWnd);
}
void CCmdEdit::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CEdit::OnShowWindow(bShow, nStatus);

	m_wndTT.ShowWindow(SW_HIDE);
}
