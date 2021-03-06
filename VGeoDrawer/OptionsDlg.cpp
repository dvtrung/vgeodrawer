// OptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VGeoDrawer.h"
#include "OptionsDlg.h"


// COptionsDlg dialog

IMPLEMENT_DYNAMIC(COptionsDlg, CDialog)

extern int numRound;
extern bool bRadian;

COptionsDlg::COptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COptionsDlg::IDD, pParent)
{

}

COptionsDlg::~COptionsDlg()
{
}

void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(COptionsDlg, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_CATEGORY_TREE, &COptionsDlg::OnTvnSelchangedTree)
	ON_BN_CLICKED(IDC_APPLY, &COptionsDlg::OnBnClickedApply)
	ON_BN_CLICKED(IDOK, &COptionsDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_SAVE_AS_DEFAULT, &COptionsDlg::OnBnClickedSaveAsDefault)
END_MESSAGE_MAP()


// COptionsDlg message handlers

BOOL COptionsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_Grid.CreateEx(0, AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW), L"", WS_BORDER|WS_CHILD|WS_VISIBLE, CRect(200,15,500,300), this, 0x1002);
	m_Grid.SetTrueFalseStrings(L"Có",L"Không");
	m_Grid.SetBoldModified(TRUE);
	m_Grid.SetBoldModified(FALSE);

	m_Tree.Create(WS_BORDER | WS_VISIBLE | TVS_FULLROWSELECT | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS | TVS_TRACKSELECT, CRect(15,15,195,300), this, IDC_CATEGORY_TREE);
	
	HTREEITEM itemOpt=m_Tree.InsertItem(L"Tùy chọn chung",-1,-1);
	HTREEITEM itemPad=m_Tree.InsertItem(L"Vùng làm việc",-1,-1);
	HTREEITEM itemProperties=m_Tree.InsertItem(L"Thuộc tính đối tượng",-1,-1);
	HTREEITEM itemGridAxis=m_Tree.InsertItem(L"Trục tọa độ & Lưới",-1,-1);
	//HTREEITEM itemPoint=m_Tree.InsertItem(L"Điểm",-1,-1,itemProperties);
	//HTREEITEM itemLine=m_Tree.InsertItem(L"Đường thẳng",-1,-1,itemProperties);

	LoadOptions();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsDlg::OnTvnSelchangedTree( NMHDR *pNMHDR, LRESULT *pResult )
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM item=m_Tree.GetSelectedItem();
	CString str=m_Tree.GetItemText(item);
	
	if (str==L"Tùy chọn chung")
	{
		m_Grid.ResetContents();
		
		HSECTION secOpt=m_Grid.AddSection(L"Tùy chọn");
		CStringArray v;
		v.Add(L"Tiếng Việt");
		m_Grid.AddComboItem(secOpt,L"Language",v,0);

		HSECTION secOther=m_Grid.AddSection(L"Khác");
		m_Grid.AddIntegerItemP(secOther,L"Làm tròn (1 - 10 chữ số)",&m_NumRound);		
		CStringArray v2;
		v2.Add(L"Độ");
		v2.Add(L"Radian");
		m_Grid.AddComboItemP(secOther,L"Đơn vị góc",v2,(int*)&m_Radian);
	}
	else if (str==L"Thuộc tính đối tượng")
	{
		CStringArray v;
		v.Add(L"1 - Bình thường");
		v.Add(L"2 - Chấm 1");
		v.Add(L"3 - Chấm 2");
		v.Add(L"4 - Gạch 1");
		v.Add(L"5 - Gạch 2");

		m_Grid.ResetContents();
		HSECTION secPoint=m_Grid.AddSection(L"Điểm");
		m_Grid.AddColorItemP(secPoint,L"Màu sắc",&m_defaultValue.m_clrDefaultPoint);

		CStringArray v3;
		for (int i=1;i<=MAX_OBJECT_SIZE;i++) 
			v3.Add(Math::IntToString(i));
		m_Grid.AddComboItemP(secPoint,L"Kích thước",v3,&m_defaultValue.m_nPointSize);

		m_Grid.AddBoolItemP(secPoint,L"Đường gióng",&m_defaultValue.m_bPointer);
		CStringArray v2;
		v2.Add(L"Cartesian");
		v2.Add(L"Polar");
		v2.Add(L"Complex");
		m_Grid.AddComboItem(secPoint,L"Kiểu tọa độ",v2,0);
		
		HSECTION secLine=m_Grid.AddSection(L"Đường thẳng & Tia");
		m_Grid.AddColorItemP(secLine,L"Màu sắc",&m_defaultValue.m_clrDefaultLine);
		m_Grid.AddComboItemP(secLine,L"Kiểu đường",v,&m_defaultValue.m_nLineStyle);
		m_Grid.AddComboItemP(secLine,L"Độ đậm",v3,&m_defaultValue.m_nLineWidth);
		m_Grid.AddBoolItemP(secLine,L"Bỏ đoạn thừa",&m_defaultValue.m_bLineShowSeg);
		v2.RemoveAll();
		v2.Add(L"ax + by = c");
		v2.Add(L"y = mx + b");
		m_Grid.AddComboItem(secLine,L"Kiểu phương trình",v2,0);

		HSECTION secSegment=m_Grid.AddSection(L"Đoạn thẳng");
		m_Grid.AddColorItemP(secSegment,L"Màu sắc",&m_defaultValue.m_clrDefaultSeg);
		m_Grid.AddComboItemP(secSegment,L"Kiểu đường",v,&m_defaultValue.m_nSegStyle);
		m_Grid.AddComboItemP(secSegment,L"Độ đậm",v3,&m_defaultValue.m_nSegWidth);

		HSECTION secVector=m_Grid.AddSection(L"Véc-tơ");
		m_Grid.AddColorItemP(secVector,L"Màu sắc",&m_defaultValue.m_clrDefaultVector);
		m_Grid.AddComboItemP(secVector,L"Kiểu đường",v,&m_defaultValue.m_nVecStyle);
		m_Grid.AddComboItemP(secVector,L"Độ đậm",v3,&m_defaultValue.m_nVecWidth);

		HSECTION secCircle=m_Grid.AddSection(L"Đường tròn");
		m_Grid.AddColorItemP(secCircle,L"Màu sắc",&m_defaultValue.m_clrDefaultCircle);
		m_Grid.AddComboItemP(secCircle,L"Kiểu đường",v,&m_defaultValue.m_nCirStyle);
		m_Grid.AddComboItemP(secCircle,L"Độ đậm",v3,&m_defaultValue.m_nCirWidth);
		v2.RemoveAll();
		v2.Add(L"(x - a)² + (y - b)² = R²");
		v2.Add(L"ax² + by² + cx + dy + e = 0");
		m_Grid.AddComboItem(secCircle,L"Kiểu phương trình",v2,0);
	}
	else if (str==L"Trục tọa độ & Lưới")
	{
		CStringArray v;
		v.Add(L"1 - Bình thường");
		v.Add(L"2 - Chấm 1");
		v.Add(L"3 - Chấm 2");
		v.Add(L"4 - Gạch 1");
		v.Add(L"5 - Gạch 2");

		m_Grid.ResetContents();
		HSECTION secAxis=m_Grid.AddSection(L"Trục tọa độ");
		m_Grid.AddBoolItemP(secAxis,L"Trục Ox",&m_Ox);
		m_Grid.AddBoolItemP(secAxis,L"Trục Oy",&m_Oy);
		m_Grid.AddBoolItemP(secAxis,L"Hiện số",&m_ShowNumber);

		HSECTION secGrid=m_Grid.AddSection(L"Lưới");
		m_Grid.AddBoolItemP(secGrid,L"Hiện lưới",&m_ShowGrid);
		m_Grid.AddComboItemP(secGrid,L"Kiểu lưới",v,&m_GridDashStyle);
	}
	else if (str==L"Vùng làm việc")
	{
		m_Grid.ResetContents();
		HSECTION secPad=m_Grid.AddSection(L"Vùng làm việc");
		m_Grid.AddBoolItemP(secPad,L"Giới hạn vùng làm việc",&m_HasPadSize);
		m_Grid.AddIntegerItemP(secPad,L"Chiều ngang",&m_PadWidth);
		m_Grid.AddIntegerItemP(secPad,L"Chiều dọc",&m_PadHeight);
	}
	*pResult = 0;
}

void COptionsDlg::LoadOptions()
{
	m_NumRound=numRound;
	m_Radian=bRadian;
	m_Ox=m_ObjContainer->m_graphAxis.m_bOx;
	m_Oy=m_ObjContainer->m_graphAxis.m_bOy;
	m_ShowGrid=m_ObjContainer->m_graphAxis.m_bShowGrid;
	m_ShowNumber=m_ObjContainer->m_graphAxis.m_bShowNumber;
	m_GridDashStyle=m_ObjContainer->m_graphAxis.m_GridDashStyle;
	m_HasPadSize=m_pDoc->m_bHasPadSize;
	m_PadHeight=m_pDoc->m_Height;
	m_PadWidth=m_pDoc->m_Width;

	m_defaultValue=m_pDoc->m_defaultValue;
}

void COptionsDlg::SaveOptions()
{
	numRound=m_NumRound;
	bRadian=m_Radian;
	m_ObjContainer->m_graphAxis.m_bOx=m_Ox;
	m_ObjContainer->m_graphAxis.m_bOy=m_Oy;
	m_ObjContainer->m_graphAxis.m_bShowGrid=m_ShowGrid;
	m_ObjContainer->m_graphAxis.m_bShowNumber=m_ShowNumber;
	m_ObjContainer->m_graphAxis.m_GridDashStyle=m_GridDashStyle;
	m_pDoc->m_bHasPadSize=m_HasPadSize; if (!m_HasPadSize) nZoom=100;
	m_pDoc->m_Height=m_PadHeight;
	m_pDoc->m_Width=m_PadWidth;

	m_pDoc->m_defaultValue=m_defaultValue;
}

void COptionsDlg::SaveOptionsAsDefault()
{
	
}

void COptionsDlg::OnBnClickedApply()
{
	//m_pDoc->m_defaultValue=m_defaultValue;
	SaveOptions();
	m_pDoc->GetObjCont()->Calc();
	m_pDoc->pView->RefreshScrollBar();
	m_pDoc->m_wndPageList->RefreshItemSize();
	m_pDoc->pView->DrawBackground();
	m_pDoc->pView->Draw();
}

void COptionsDlg::OnBnClickedOk()
{
	OnBnClickedApply();
	OnOK();
}

void COptionsDlg::OnBnClickedSaveAsDefault()
{
	OnBnClickedApply();
	m_pDoc->SaveDefault();
}
