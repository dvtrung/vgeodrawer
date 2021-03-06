#include "StdAfx.h"
#include "PropertiesBar.h"

CPropertiesBar::CPropertiesBar(void)
{
	
}

CPropertiesBar::~CPropertiesBar(void)
{
}

void CPropertiesBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHandler)
{
	CSizingControlBar::OnUpdateCmdUI(pTarget, bDisableIfNoHandler);
	UpdateDialogControls(pTarget, bDisableIfNoHandler);
}

void CPropertiesBar::DoDataExchange(CDataExchange* pDX)
{
	CSizingControlBar::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPropertiesBar, CSizingControlBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_MESSAGE(WM_PG_ITEMCHANGED, OnItemChanged)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

int CPropertiesBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CSizingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_Font.CreatePointFont(80,L"Tahoma");
	SetFont(&m_Font);

	m_Grid.CreateEx(0, AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW), L"", WS_CHILD|WS_VISIBLE, CRect(0,0,200,100), this, 0x1002);
	m_Grid.SetTrueFalseStrings(L"Có",L"Không");
	m_Grid.SetBoldModified(true);

	secAxis=m_Grid.AddSection(L"Trục tọa độ");
	secGrid=m_Grid.AddSection(L"Lưới");

	secInfo=m_Grid.AddSection(L"Thông tin chung");
	secEvent=m_Grid.AddSection(L"Sự kiện");
	secValue=m_Grid.AddSection(L"Giá trị");
	secStyle=m_Grid.AddSection(L"Kiểu dạng");
	secAnimateTrace=m_Grid.AddSection(L"Chuyển động & Lưu vết");
	secObjWin=m_Grid.AddSection(L"Cửa sổ đối tượng hình học");
	secOther=m_Grid.AddSection(L"Khác");

	m_propertyButtonCmd.m_cmdArray=&pDoc->m_cmdArray;

	return 0;
}

void CPropertiesBar::OnSize(UINT nType, int cx, int cy)
{
	CSizingControlBar::OnSize(nType, cx, cy);
	m_Grid.MoveWindow(CRect(0,0,cx,cy));
}

// Selection contain object type ?
bool CPropertiesBar::CheckSelection(ObjType nType1, ObjType nType2, ObjType nType3, ObjType nType4, ObjType nType5)
{
	for (int i=0;i<m_Selection.GetSize();i++)
	{
		if (!(m_Selection[i]->m_Type==nType1 || m_Selection[i]->m_Type==nType2 || m_Selection[i]->m_Type==nType3 || m_Selection[i]->m_Type==nType4 || m_Selection[i]->m_Type==nType5)) return false;
	}
	return true;
}

bool CPropertiesBar::CheckNoBoundObjSelection()
{
	for (int i=0;i<m_Selection.GetSize();i++)
	{
		if (IsBoundType(m_Selection[i]->m_Type)) return false;
	}
	return true;
}

bool CPropertiesBar::CheckGeoSelection()
{
	for (int i=0;i<m_Selection.GetSize();i++)
	{
		if (!(IsGeoObject(m_Selection[i]->m_Type))) return false;
	}
	return true;
}

bool CPropertiesBar::CheckLineSelection()
{
	for (int i=0;i<m_Selection.GetSize();i++)
	{
		if (!(IsLineType(m_Selection[i]->m_Type))) return false;
	}
	return true;
}

bool CPropertiesBar::CheckTextSelection()
{
	for (int i=0;i<m_Selection.GetSize();i++)
	{
		if (!(IsTextType(m_Selection[i]->m_Type))) return false;
	}
	return true;
}


void CPropertiesBar::RefreshProperties()
{
	if (pDoc->GetObjCont()==NULL) return;

	CArray<CVGObject*> selectionArr;

	if (!IsWindowVisible()) return;
	m_Selection.RemoveAll();

	//CheckObject();

	m_Grid.RemoveAllItems();

	for (int i=0;i<pDoc->GetObjCont()->GetSize();i++)
	{
		if (pDoc->GetObjCont()->GetAt(i)->m_Select)
		{
			m_Selection.Add(pDoc->GetObjCont()->GetAt(i));
		}
	}

	for (int i=0;i<m_Selection.GetSize();i++) selectionArr.Add(m_Selection.GetAt(i));

	if (selectionArr.GetSize()==0)
	{
		m_Grid.AddBoolItemP(secAxis,L"Trục Ox",&pDoc->GetObjCont()->m_graphAxis.m_bOx);
		m_Grid.AddBoolItemP(secAxis,L"Trục Oy",&pDoc->GetObjCont()->m_graphAxis.m_bOy);
		m_Grid.AddBoolItemP(secAxis,L"Hiện số",&pDoc->GetObjCont()->m_graphAxis.m_bShowNumber);
		m_Grid.AddBoolItemP(secGrid,L"Hiện lưới",&pDoc->GetObjCont()->m_graphAxis.m_bShowGrid);
		CStringArray v;
		v.Add(L"1 - Bình thường");
		v.Add(L"2 - Chấm 1");
		v.Add(L"3 - Chấm 2");
		v.Add(L"4 - Gạch 1");
		v.Add(L"5 - Gạch 2");
		m_Grid.AddComboItemP(secGrid,L"Kiểu lưới",v,&pDoc->GetObjCont()->m_graphAxis.m_GridDashStyle);
		return;
	}

	if (selectionArr.GetSize()==1)
	{
		m_Grid.AddStringItem(secInfo,L"Tên",selectionArr[0]->m_Name);

		CString str=selectionArr[0]->m_Definition; str.Remove('~');
		m_Grid.AddStringItem(secInfo,L"Định nghĩa",str,false);
		
		if (selectionArr[0]->m_Type==OBJ_POINT)
		{
			m_Grid.AddDoubleItemP(secValue,L"Tọa độ X",&((CVGPoint*)selectionArr[0])->m_x,L"",((CVGPoint*)selectionArr[0])->m_Mode==POINT_MODE_NUM_NUM);
			m_Grid.AddDoubleItemP(secValue,L"Tọa độ Y",&((CVGPoint*)selectionArr[0])->m_y,L"",((CVGPoint*)selectionArr[0])->m_Mode==POINT_MODE_NUM_NUM);
		}
		else if (selectionArr[0]->m_Type==OBJ_TEXT)
		{
			m_propertyTextItem.m_obj=((CVGText*)selectionArr[0]);
			m_Grid.AddCustomItem(secValue,L"Văn bản",&m_propertyTextItem);
		}
		else if (IsButtonType(selectionArr[0]->m_Type))
		{
			m_propertyButtonCmd.m_obj=((CVGButtonBase*)selectionArr[0]);
			m_Grid.AddCustomItem(secEvent,L"Nhấn chuột",&m_propertyButtonCmd);
		}
		else if (selectionArr[0]->m_Type==OBJ_LINE)
		{
			m_Grid.AddStringItem(secValue,L"Phương trình",((CVGLine*)selectionArr[0])->GetValue(),false);
		}
		else if (selectionArr[0]->m_Type==OBJ_SEGMENT)
		{
			m_Grid.AddStringItem(secValue,L"Độ dài",((CVGSegment*)selectionArr[0])->GetValue(),false);
		}
		else if (selectionArr[0]->m_Type==OBJ_VECTOR)
		{
			m_Grid.AddStringItem(secValue,L"Giá trị",((CVGVector*)selectionArr[0])->GetValue(),false);
		}
		else if (selectionArr[0]->m_Type==OBJ_CIRCLE)
		{
			m_Grid.AddStringItem(secValue,L"Phương trình",((CVGCircle*)selectionArr[0])->GetValue(),false);
			m_Grid.AddStringItem(secValue,L"Tọa độ tâm",L"("+Math::DoubleToString(((CVGCircle*)selectionArr[0])->m_CenterX)+L", "+Math::DoubleToString(((CVGCircle*)selectionArr[0])->m_CenterY)+L")",false);
			m_Grid.AddDoubleItemP(secValue,L"Bán kính",&((CVGCircle*)selectionArr[0])->m_Radius,L"",false/*selectionArr[0]->m_Mode==CIRCLE_MODE_POINT_RADIUS && selectionArr[0]->m_Param[1]->m_Type==OBJ_NUMBER*/);
		}
		else if (selectionArr[0]->m_Type==OBJ_SLIDER)
		{
			m_Grid.AddStringItem(secValue,L"Giá trị",((CVGSlider*)selectionArr[0])->m_NumVariable!=NULL ? ((CVGSlider*)selectionArr[0])->m_NumVariable->m_Name : L"");
		}
	}

	//////////////////////////////////////////////////////////////////////////
	CArray<void*> arr;

	arr.RemoveAll();
	for (int i=0;i<selectionArr.GetSize();i++) arr.Add(&selectionArr[i]->m_Description);
	m_Grid.AddStringItemPs(secInfo,L"Ghi chú",arr);

	arr.RemoveAll();
	for (int i=0;i<selectionArr.GetSize();i++) arr.Add(&selectionArr[i]->m_bVisible);
	m_Grid.AddBoolItemPs(secInfo,L"Hiện",arr);

	arr.RemoveAll();
	for (int i=0;i<selectionArr.GetSize();i++) arr.Add(&selectionArr[i]->m_strCondition);
	m_Grid.AddStringItemPs(secInfo,L"ĐK có nghĩa",arr);

	arr.RemoveAll();
	for (int i=0;i<selectionArr.GetSize();i++) arr.Add(&selectionArr[i]->m_bFixed);
	m_Grid.AddBoolItemPs(secInfo,L"Cố định",arr);

	//////////////////////////////////////////////////////////////////////////

	if (CheckSelection(OBJ_POINT))
	{
		CStringArray v;
		for (int i=1;i<=MAX_OBJECT_SIZE;i++) 
			v.Add(Math::IntToString(i));

		arr.RemoveAll();
		for (int i=0;i<selectionArr.GetSize();i++) arr.Add(&((CVGPoint*)selectionArr[i])->m_Size);

		m_Grid.AddComboItemPs(secStyle,L"Kích thước",v,arr);
	}

	if (CheckNoBoundObjSelection())
	{
		arr.RemoveAll();
		for (int i=0;i<selectionArr.GetSize();i++) arr.Add(&(selectionArr[i])->m_Color);
		m_Grid.AddColorItemPs(secStyle,L"Màu sắc",arr);
	}

	if (CheckSelection(OBJ_LINE, OBJ_SEGMENT, OBJ_VECTOR, OBJ_RAY, OBJ_CIRCLE))
	{
		arr.RemoveAll();
		for (int i=0;i<selectionArr.GetSize();i++) 
			arr.Add(&(selectionArr[i]->m_Type==OBJ_CIRCLE ? ((CVGCircle*)selectionArr[i])->m_DashStyle : ((CVGLineBase*)selectionArr[i])->m_DashStyle));
		CStringArray v;
		v.Add(L"1 - Bình thường");
		v.Add(L"2 - Chấm 1");
		v.Add(L"3 - Chấm 2");
		v.Add(L"4 - Gạch 1");
		v.Add(L"5 - Gạch 2");
		m_Grid.AddComboItemPs(secStyle,L"Kiểu đường",v,arr);

		arr.RemoveAll();

		CStringArray v2;
		for (int i=1;i<=MAX_OBJECT_SIZE;i++) 
			v2.Add(Math::IntToString(i));

		for (int i=0;i<selectionArr.GetSize();i++)
			arr.Add(&(selectionArr[i]->m_Type==OBJ_CIRCLE ? ((CVGCircle*)selectionArr[i])->m_PenWidth : ((CVGLineBase*)selectionArr[i])->m_PenWidth));
		m_Grid.AddComboItemPs(secStyle,L"Độ đậm",v2,arr);
	}

	if (CheckSelection(OBJ_ANGLE))
	{
		arr.RemoveAll();
		for (int i=0;i<selectionArr.GetSize();i++) 
			arr.Add(&((CVGAngle*)selectionArr[i])->m_ArcCount);

		CStringArray v;
		for (int i=1;i<=3;i++) 
			v.Add(Math::IntToString(i));
		m_Grid.AddComboItemPs(secStyle,L"Số vòng",v,arr);
		
		arr.RemoveAll();
		for (int i=0;i<selectionArr.GetSize();i++) 
			arr.Add(&((CVGAngle*)selectionArr[i])->m_Size);
		
		v.RemoveAll();
		for (int i=1;i<=10;i++) 
			v.Add(Math::IntToString(i));
		m_Grid.AddComboItemPs(secStyle,L"Kích thước",v,arr);
	}

	if (CheckTextSelection())
	{
		arr.RemoveAll();
		for (int i=0;i<selectionArr.GetSize();i++) 
			arr.Add(&((CVGText*)selectionArr[0])->m_Font);
		m_Grid.AddFontItemPs(secStyle,L"Font",arr);
	}

	if (CheckSelection(OBJ_POLYGON))
	{
		arr.RemoveAll();
		for (int i=0;i<selectionArr.GetSize();i++) 
			arr.Add(&((CVGPolygon*)selectionArr[i])->m_Opacity);
		m_Grid.AddIntegerItemPs(secStyle,L"Độ trong",arr,L"%d %%");

		arr.RemoveAll();
		for (int i=0;i<selectionArr.GetSize();i++) 
			arr.Add(&((CVGPolygon*)selectionArr[i])->m_bBorder);
		m_Grid.AddBoolItemPs(secStyle,L"Đường viền",arr);
	}

	if (CheckSelection(OBJ_PANEL))
	{
		arr.RemoveAll();
		for (int i=0;i<selectionArr.GetSize();i++)
			arr.Add(&((CVGPanel*)selectionArr[i])->m_bTitle);
		m_Grid.AddBoolItemPs(secValue,L"Có tiêu đề",arr);

		arr.RemoveAll();
		for (int i=0;i<selectionArr.GetSize();i++)
			arr.Add(&((CVGPanel*)selectionArr[i])->m_Text);
		m_Grid.AddStringItemPs(secValue,L"Tiêu đề",arr);

		arr.RemoveAll();
		for (int i=0;i<selectionArr.GetSize();i++)
			arr.Add(&((CVGPanel*)selectionArr[i])->m_Font);
		m_Grid.AddFontItemPs(secStyle,L"Font",arr);

		arr.RemoveAll();
		for (int i=0;i<selectionArr.GetSize();i++)
			arr.Add(&((CVGPanel*)selectionArr[i])->m_Color);
		m_Grid.AddColorItemPs(secStyle,L"Màu tiêu đề",arr);

		arr.RemoveAll();
		CStringArray v;
		v.Add(L"Không");
		v.Add(L"Trái");
		v.Add(L"Phải");
		v.Add(L"Trên");
		v.Add(L"Dưới");
		for (int i=0;i<selectionArr.GetSize();i++)
			arr.Add(&((CVGPanel*)selectionArr[i])->m_Dock);
		m_Grid.AddComboItemPs(secValue,L"Docking",v,arr);

		m_propertyGradient.m_PanelArr.RemoveAll();
		for (int i=0;i<selectionArr.GetSize();i++)
			m_propertyGradient.m_PanelArr.Add((CVGPanel*)selectionArr[i]);
		m_Grid.AddCustomItem(secStyle,L"Kiểu dạng",&m_propertyGradient);

		arr.RemoveAll();
		for (int i=0;i<selectionArr.GetSize();i++)
			arr.Add(&((CVGPanel*)selectionArr[i])->m_nOpacity);
		m_Grid.AddIntegerItemPs(secStyle,L"Độ rõ",arr,L"%d %%");
	}

	if (CheckSelection(OBJ_BUTTON, OBJ_CHECKBOX, OBJ_RADIO_BUTTON))
	{
		arr.RemoveAll();
		for (int i=0;i<selectionArr.GetSize();i++)
			arr.Add(&((CVGButtonBase*)selectionArr[i])->m_Text);
		m_Grid.AddStringItemPs(secValue,L"Tiêu đề",arr);

		arr.RemoveAll();
		for (int i=0;i<selectionArr.GetSize();i++)
			arr.Add(&((CVGButtonBase*)selectionArr[i])->m_Color);
		m_Grid.AddColorItemPs(secStyle,L"Màu nền",arr);

		arr.RemoveAll();
		for (int i=0;i<selectionArr.GetSize();i++)
			arr.Add(&((CVGButtonBase*)selectionArr[i])->m_ColorMouseOver);
		m_Grid.AddColorItemPs(secStyle,L"Mouse Over",arr);
		
		arr.RemoveAll();
		for (int i=0;i<selectionArr.GetSize();i++)
			arr.Add(&((CVGButtonBase*)selectionArr[i])->m_ColorMouseDown);
		m_Grid.AddColorItemPs(secStyle,L"Mouse Click",arr);

		arr.RemoveAll();
		for (int i=0;i<selectionArr.GetSize();i++)
			arr.Add(&((CVGButtonBase*)selectionArr[i])->m_BorderColor);
		m_Grid.AddColorItemPs(secStyle,L"Màu viền",arr);

		arr.RemoveAll();
		for (int i=0;i<selectionArr.GetSize();i++)
			arr.Add(&((CVGButtonBase*)selectionArr[i])->m_Font);
		m_Grid.AddFontItemPs(secStyle,L"Font",arr);
	}

	if (CheckSelection(OBJ_CHECKBOX, OBJ_RADIO_BUTTON))
	{
		arr.RemoveAll();
		for (int i=0;i<selectionArr.GetSize();i++)
		{
			if (selectionArr[i]->m_Type==OBJ_CHECKBOX) arr.Add(&((CVGCheckBox*)selectionArr[i])->m_bState);
			else arr.Add(&((CVGRadioButton*)selectionArr[i])->m_bState);
		}
		m_Grid.AddBoolItemPs(secValue,L"Giá trị",arr);
	}

	if (CheckSelection(OBJ_SLIDER))
	{
		arr.RemoveAll();
		for (int i=0;i<selectionArr.GetSize();i++)
			arr.Add(&((CVGSlider*)selectionArr[i])->m_Text);
		m_Grid.AddStringItemPs(secValue,L"Tiêu đề",arr);

		arr.RemoveAll();
		for (int i=0;i<selectionArr.GetSize();i++)
			arr.Add(&((CVGSlider*)selectionArr[i])->m_nStart);
		m_Grid.AddDoubleItemPs(secValue,L"Giá trị đầu",arr);

		arr.RemoveAll();
		for (int i=0;i<selectionArr.GetSize();i++)
			arr.Add(&((CVGSlider*)selectionArr[i])->m_nEnd);
		m_Grid.AddDoubleItemPs(secValue,L"Giá trị cuối",arr);

		arr.RemoveAll();
		for (int i=0;i<selectionArr.GetSize();i++)
			arr.Add(&((CVGButtonBase*)selectionArr[i])->m_Font);
		m_Grid.AddFontItemPs(secStyle,L"Font",arr);
	}
	
	if (CheckGeoSelection())
	{
		//////////////////////////////////////////////////////////////////////////
		arr.RemoveAll();
		for (int i=0;i<selectionArr.GetSize();i++) arr.Add(&((CVGGeoObject*)selectionArr[i])->m_bAnimate);
		m_Grid.AddBoolItemPs(secAnimateTrace,L"Chuyển động",arr);

		arr.RemoveAll();
		for (int i=0;i<selectionArr.GetSize();i++) arr.Add(&((CVGGeoObject*)selectionArr[i])->m_bTrace);
		m_Grid.AddBoolItemPs(secAnimateTrace,L"Lưu vết",arr);
	}

	//////////////////////////////////////////////////////////////////////////

// 	arr.RemoveAll();
// 	for (int i=0;i<selectionArr.GetSize();i++) arr.Add(&selectionArr[i]->m_bShowValue);
// 	m_Grid.AddBoolItemPs(secObjWin,L"Hiện giá trị",arr);
// 
// 	arr.RemoveAll();
// 	for (int i=0;i<selectionArr.GetSize();i++) arr.Add(&selectionArr[i]->m_bShowDefinition);
// 	m_Grid.AddBoolItemPs(secObjWin,L"Hiện định nghĩa",arr);
// 
// 	arr.RemoveAll();
// 	for (int i=0;i<selectionArr.GetSize();i++) arr.Add(&selectionArr[i]->m_bShowDescription);
// 	m_Grid.AddBoolItemPs(secObjWin,L"Hiện ghi chú",arr);

	if (CheckSelection(OBJ_POINT))
	{
		//////////////////////////////////////////////////////////////////////////
		arr.RemoveAll();
		for (int i=0;i<selectionArr.GetSize();i++)
			arr.Add(&((CVGPoint*)selectionArr[i])->m_bShowPointers);
		m_Grid.AddBoolItemPs(secOther,L"Đường gióng",arr);
	}

	if (CheckSelection(OBJ_LINE))
	{
		arr.RemoveAll();
		for (int i=0;i<selectionArr.GetSize();i++)
			arr.Add(&((CVGLine*)selectionArr[i])->m_bCutSuperfluousPart);
		m_Grid.AddBoolItemPs(secOther,L"Bỏ đoạn thừa",arr);
	}
}

LRESULT CPropertiesBar::OnItemChanged(WPARAM wParam, LPARAM lParam)
{
	CPropertyGrid::CItem* pItem=m_Grid.FindItem((HITEM)wParam);
	CPropertyGrid::CSection* pSection=m_Grid.FindSectionByItem((HITEM)wParam);
	pDoc->GetObjCont()->OnChangeProperties(&m_Selection);

	if (pItem->m_name==L"Tên")
	{
		CString strName=*((CString*)pItem->m_pAllValue[0]);
		pDoc->GetObjCont()->RenameObject(m_Selection[0],strName);
		*((CString*)pItem->m_pAllValue[0])=m_Selection[0]->m_Name;
	}
	
	if (pItem->m_name==L"Văn bản")
	{
		((CVGText*)m_Selection[0])->SetText(((CVGText*)m_Selection[0])->m_Text,&pDoc->GetObjCont()->m_Array,&pDoc->m_ScriptObject,pDoc->GetObjCont()->m_strReplacedNameArr);
	}
	
	if (pItem->m_name==L"ĐK có nghĩa")
	{
		for (int i=0;i<m_Selection.GetSize();i++)
			(m_Selection[i])->RefreshCondition(&pDoc->GetObjCont()->m_Array);
	}

	if (pItem->m_name==L"Docking")
	{
		for (int i=0;i<m_Selection.GetSize();i++)
			((CVGPanel*)m_Selection[i])->RefreshDocking(&pDoc->GetObjCont()->m_AxisInfo);
	}

	if (pItem->m_name==L"Độ rõ" && CheckSelection(OBJ_PANEL))
	{
		for (int i=0;i<m_Selection.GetSize();i++)
			((CVGPanel*)m_Selection[i])->SetOpacity();
	}

	if (pItem->m_name==L"Giá trị" && m_Selection[0]->m_Type==OBJ_SLIDER)
	{
		CString s=*(CString*)pItem->m_pAllValue[0];
		if (s!=L"")
			for (int i=0;i<pDoc->GetObjCont()->GetSize();i++)
				if (s==pDoc->GetObjCont()->GetAt(i)->m_Name)
					((CVGSlider*)m_Selection[0])->m_NumVariable=(CVGExpression*)pDoc->GetObjCont()->GetAt(i);
	}
	
	if (pSection->m_title==L"Lưới" || pSection->m_title==L"Trục tọa độ")
	{
		pDoc->pView->DrawBackground();
	}
	
	if (pItem->m_name==L"Hiện giá trị" || pItem->m_name==L"Hiện định nghĩa" || pItem->m_name==L"Hiện ghi chú")
	{
		//pDoc->GetObjCont()->m_pObjTree->CalcAllItemsSize();
	}

	pDoc->GetObjCont()->Calc();
	pDoc->pView->Draw();
	return 0;
}

void CPropertiesBar::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CSizingControlBar::OnShowWindow(bShow, nStatus);

	if (bShow) RefreshProperties();
}
