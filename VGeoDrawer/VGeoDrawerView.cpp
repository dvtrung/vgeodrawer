// VGeoDrawerView.cpp : implementation of the CVGeoDrawerView class
//

#include "stdafx.h"
#include "VGeoDrawer.h"

#include "VGeoDrawerDoc.h"
#include "VGeoDrawerView.h"

#include "VGMath.h"
#include "InputDlg.h"
#include "appfunc.h"
#include "TextEditorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CVGeoDrawerView

IMPLEMENT_DYNCREATE(CVGeoDrawerView, CScrollView)

BEGIN_MESSAGE_MAP(CVGeoDrawerView, CScrollView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CScrollView::OnFilePrintPreview)
	ON_COMMAND(ID_EDIT_ANIMATE_START, &CVGeoDrawerView::OnStartAnimate)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ANIMATE_START, &CVGeoDrawerView::OnUpdateStartAnimate)
	ON_COMMAND(ID_EDIT_ANIMATE_PAUSE, &CVGeoDrawerView::OnPauseAnimate)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ANIMATE_PAUSE, &CVGeoDrawerView::OnUpdatePauseAnimate)

	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_SETCURSOR()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_COMMAND(ID_EDIT_SELECT_ALL, &CVGeoDrawerView::OnEditSelectAll)
	ON_WM_KEYDOWN()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()

ON_WM_SHOWWINDOW()
ON_WM_CONTEXTMENU()
ON_WM_RBUTTONUP()
ON_WM_MBUTTONDOWN()
ON_WM_MBUTTONUP()

ON_WM_MEASUREITEM()
ON_WM_MENUCHAR()
ON_WM_INITMENUPOPUP()

ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

// CVGeoDrawerView construction/destruction

CVGeoDrawerView::CVGeoDrawerView()
{
	m_bCursor=FALSE;
	m_bAnimating=FALSE;
	m_bMovingObj=FALSE;
	m_bTestPointMoving=FALSE;
	m_btdMode=BTD_MODE_SELECT;

	objMouseOver1=NULL;
	objMouseOver2=NULL;
}

CVGeoDrawerView::~CVGeoDrawerView()
{
	
}

BOOL CVGeoDrawerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	m_Mode=MODE_MOVE;

	return CScrollView::PreCreateWindow(cs);
}

// CVGeoDrawerView printing

BOOL CVGeoDrawerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CVGeoDrawerView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
// 	CRect r; GetClientRect(&r);
// 	BmpView=new Bitmap(m_bHasPadSize ? m_Width : r.Width(),
// 		m_bHasPadSize ? m_Height : r.Height(),
// 		PixelFormat32bppARGB);
// 	DrawToBitmap(BmpView);
// 	hbmpView=new HBITMAP;
// 	BmpView->GetHBITMAP(Color(255,255,255),hbmpView);
// 	bmpView=new CBitmap();
// 	bmpView->FromHandle(*hbmpView);
}

void CVGeoDrawerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
// 	bmpView->DeleteObject();
// 	delete hbmpView;
}


// CVGeoDrawerView diagnostics

#ifdef _DEBUG
void CVGeoDrawerView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CVGeoDrawerView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CVGeoDrawerDoc* CVGeoDrawerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVGeoDrawerDoc)));
	return (CVGeoDrawerDoc*)m_pDocument;
}
#endif //_DEBUG

void CVGeoDrawerView::RefreshScrollBar()
{
	if (GetDocument()->m_bHasPadSize)
	{
		CRect r;
		GetClientRect(r);
		int width=GetDocument()->m_Width*nZoom/100;
		int height=GetDocument()->m_Height*nZoom/100;
		SetScrollSizes(MM_TEXT, CSize(width+2*VIEW_DISTANCE,height+2*VIEW_DISTANCE),CSize(r.Width(),r.Height()));

		int h=GetScrollPos(SB_HORZ);
		int v=GetScrollPos(SB_VERT);
		m_Left=width<r.Width() ? (r.Width()-width)/2 : -h+VIEW_DISTANCE;
		m_Top=height<r.Height() ? (r.Height()-height)/2 : -v+VIEW_DISTANCE;
	}
	else 
	{
		SetScrollSizes(MM_TEXT,CSize(0,0));
		m_Left=0; m_Top=0;
	}
	Draw();
}

void CVGeoDrawerView::SetViewCursor(LPCTSTR lpszCursorName)
{
	HCURSOR lhCursor=AfxGetApp()->LoadStandardCursor(lpszCursorName);
	SetCursor(lhCursor);
	m_currentCursor=lhCursor;
}

void CVGeoDrawerView::DrawBackground()
{
	if (!GetDocument()->m_bInit) return;
	CRect r;
	GetClientRect(&r);
	GetDocument()->GetObjCont()->DrawBackground(GetDocument()->m_bHasPadSize ? GetDocument()->m_Width : r.Width(),
		GetDocument()->m_bHasPadSize ? GetDocument()->m_Height : r.Height());
}

void CVGeoDrawerView::DrawAllBackground()
{
	if (!GetDocument()->m_bInit) return;
	CRect r;
	GetClientRect(&r);
	int width=GetDocument()->m_bHasPadSize ? GetDocument()->m_Width : r.Width();
	int height=GetDocument()->m_bHasPadSize ? GetDocument()->m_Height : r.Height();
	for (int i=0;i<GetDocument()->m_ObjContainerArr.GetSize();i++)
		GetDocument()->m_ObjContainerArr[i]->DrawBackground(width,height);
}

void CVGeoDrawerView::DrawToBitmap(Bitmap* memBitmap)
{
	CVGeoDrawerDoc* pDoc = GetDocument();

	Graphics memGr(memBitmap);
	memGr.SetTextRenderingHint(TextRenderingHintSingleBitPerPixelGridFit);

	pDoc->GetObjCont()->Draw(&memGr);

	//Ve vien hinh chu nhat bao doi tuong chon

	if (!m_SelectionRightBottom.Equals(m_SelectionTopLeft))
	{
		Rect sRect(min(m_SelectionTopLeft.X,m_SelectionRightBottom.X),min(m_SelectionTopLeft.Y,m_SelectionRightBottom.Y),
			abs(m_SelectionRightBottom.X-m_SelectionTopLeft.X),abs(m_SelectionRightBottom.Y-m_SelectionTopLeft.Y));

		if (m_btdMode==BTD_MODE_SELECT)
		{
			SolidBrush br(Color(100,200,200,200));
			memGr.FillRectangle(&br,sRect);

			Pen pen(Color(255,128,128,128));
			memGr.DrawRectangle(&pen,sRect);
		}
		else if (m_btdMode==BTD_MODE_NEW_BOUND_ITEM)
		{
			Pen pen(Color::Black);
			float f[2]={1,4}; pen.SetDashPattern(f,2);
			memGr.DrawRectangle(&pen,sRect);
		}
	}

	FontFamily fontFamily(L"Tahoma");
	Font font(&fontFamily,10);
	SolidBrush br(Color(128,128,128));
	memGr.DrawString(strTextFollowMouse,-1,&font,PointF(m_PrevPt.X+15,m_PrevPt.Y+15),&br);
}

void CVGeoDrawerView::ResizeDrawingPad()
{
	CRect r;
	GetClientRect(&r);
	drPadSize=Size(GetDocument()->m_bHasPadSize ? GetDocument()->m_Width : r.Width(),
		GetDocument()->m_bHasPadSize ? GetDocument()->m_Height : r.Height());
	GetDocument()->GetObjCont()->RefreshDocking();
}

void CVGeoDrawerView::Draw()
{
	//Size s=GetDocument()->GetObjCont()->m_AxisInfo.m_Size;

	if (!GetDocument()->m_bInit) return;
	GetDocument()->m_wndPageList->DrawItemBitmap(GetDocument()->m_nCurrentContainer);

	if (GetDocument()==NULL) return;
	//CPaintDC dc(this);
	//int h=GetScrollPos(SB_HORZ);
	//int v=GetScrollPos(SB_VERT);

	Graphics gr(this->m_hWnd);
	CVGeoDrawerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CRect r;
	GetClientRect(&r);
	
	int width=GetDocument()->m_bHasPadSize ? GetDocument()->m_Width*nZoom/100 : r.Width();
	int height=GetDocument()->m_bHasPadSize ? GetDocument()->m_Height*nZoom/100 : r.Height();

	Bitmap memBitmap(width,height,PixelFormat32bppARGB);
	//Graphics memGr(&memBitmap);
	
	Bitmap memBitmap2(r.Width(),r.Height(),PixelFormat32bppARGB);
	Graphics memGr2(&memBitmap2);
	memGr2.Clear(Color::Gray);

	if (pDoc->m_ObjContainerArr.GetSize()!=0)
	{
		DrawToBitmap(&memBitmap);
		memGr2.DrawImage(&memBitmap,m_Left,m_Top,width,height);
		if (GetDocument()->m_bHasPadSize)
		{
			Pen pen(Color(0,0,0),1);
			memGr2.DrawRectangle(&pen,m_Left,m_Top,width,height);
		}
	}
	gr.DrawImage(&memBitmap2,0,0);
}

// CVGeoDrawerView message handlers

Point CVGeoDrawerView::GetPlotPoint(CPoint point, CVGObject* except)
{
	CPoint pt(point.x*100/nZoom,point.y*100/nZoom);
	
	int x=Math::Round((double)((pt.x-GetDocument()->GetObjCont()->m_AxisInfo.m_originPoint.X))/GetDocument()->GetObjCont()->m_AxisInfo.Pixel)
		*GetDocument()->GetObjCont()->m_AxisInfo.Pixel+GetDocument()->GetObjCont()->m_AxisInfo.m_originPoint.X;
	int y=-(
		Math::Round((double)((-pt.y+GetDocument()->GetObjCont()->m_AxisInfo.m_originPoint.Y))/GetDocument()->GetObjCont()->m_AxisInfo.Pixel)
				*GetDocument()->GetObjCont()->m_AxisInfo.Pixel-GetDocument()->GetObjCont()->m_AxisInfo.m_originPoint.Y);
	
// 	int minX=100000;
// 	int minY=100000;
// 	CVGObjContainer* objCont=GetDocument()->GetObjCont();
// 	for (int i=0;i<objCont->GetSize();i++)
// 	{
// 		if (objCont->GetAt(i)->m_Type==OBJ_POINT && ((CVGPoint*)objCont->GetAt(i))->m_bVisible && ((CVGPoint*)objCont->GetAt(i))->IsVisible() && objCont->GetAt(i)!=except)
// 		{
// 			Point pt=GetDocument()->GetObjCont()->m_AxisInfo.ToClientPoint(((CVGPoint*)objCont->GetAt(i))->m_x, ((CVGPoint*)objCont->GetAt(i))->m_x);
// 			if (abs(pt.X-point.x)<abs(minX-point.x)) minX=pt.X;
// 			if (abs(pt.Y-point.y)<abs(minY-point.y)) minY=pt.Y;
// 		}
// 	}
// 
// 	if (abs(minX-point.x)<abs(x-point.x)) x=minX;
// 	if (abs(minY-point.y)<abs(y-point.y)) y=minY;
// 		
 	return Point(x*nZoom/100,y*nZoom/100);
}

Point CVGeoDrawerView::GetPadPoint(int x, int y)
{
	return Point((x-m_Left)/**100/nZoom*/,(y-m_Top)/**100/nZoom*/);
}

CVGObject* CVGeoDrawerView::GetObject(int nIndex)
{
	return (nIndex<0 || nIndex>=GetDocument()->GetObjCont()->GetSize()) ? 
		NULL : GetDocument()->GetObjCont()->GetAt(nIndex);
}

void CVGeoDrawerView::OnLButtonDown(UINT nFlags, CPoint point)
{
	GetDocument()->SetModifiedFlag();
	Point pt=GetPadPoint(point.x,point.y);
// 	if (GetDocument()->m_currentToolName==L"Biểu thức")
// 	{
// 		GetDocument()->GetObjCont()->AddExpressionD(CPoint(pt.X,pt.Y));
// 		return;
// 	}
// 	if (GetDocument()->m_currentToolName==L"Số thực")
// 	{
// 		GetDocument()->GetObjCont()->AddNumberD(CPoint(pt.X,pt.Y));
// 		return;
// 	}
// 	else if (GetDocument()->m_currentToolName==L"Văn bản")
// 	{
// 		GetDocument()->GetObjCont()->AddTextD(CPoint(pt.X,pt.Y));
// 		return;
// 	}
	if (GetDocument()->m_currentToolName==L"Hàm số")
	{
		GetDocument()->GetObjCont()->AddFunctionD();
		return;
	}
	else if (GetDocument()->m_currentToolName==L"Nút bấm" ||
		GetDocument()->m_currentToolName==L"Chèn ảnh" ||
		GetDocument()->m_currentToolName==L"Check Box" ||
		GetDocument()->m_currentToolName==L"Radio Button" ||
		GetDocument()->m_currentToolName==L"Vùng làm việc" ||
		GetDocument()->m_currentToolName==L"Văn bản" ||
		GetDocument()->m_currentToolName==L"Slider")
	{
		m_btdMode=BTD_MODE_NEW_BOUND_ITEM;
		m_SelectionTopLeft=pt;
		m_SelectionRightBottom=pt;
		if (GetDocument()->m_currentToolName==L"Nút bấm") m_newBoundObjType=OBJ_BUTTON;
		else if (GetDocument()->m_currentToolName==L"Chèn ảnh") m_newBoundObjType=OBJ_IMAGE;
		else if (GetDocument()->m_currentToolName==L"Check Box") m_newBoundObjType=OBJ_CHECKBOX;
		else if (GetDocument()->m_currentToolName==L"Radio Button") m_newBoundObjType=OBJ_RADIO_BUTTON;
		else if (GetDocument()->m_currentToolName==L"Vùng làm việc") m_newBoundObjType=OBJ_PANEL;
		else if (GetDocument()->m_currentToolName==L"Văn bản") m_newBoundObjType=OBJ_TEXT;
		else if (GetDocument()->m_currentToolName==L"Slider") m_newBoundObjType=OBJ_SLIDER;
		SetViewCursor(IDC_CROSS);
	}

	bool bIsLabel;
	SetCapture();

	CVGeoDrawerDoc* pDoc=GetDocument();

	if (pDoc->m_currentToolName==L"Dịch chuyển" || ((nFlags & MK_CONTROL))) // Kéo trục tọa độ
	{
		m_btdMode=BTD_MODE_DRAG_DRAWING_PAD;
		SetViewCursor(IDC_SIZEALL);
		m_lPoint=pt;
		GetDocument()->SetModifiedFlag();
		return;
	}

	if (pDoc->m_currentToolName==L"Con trỏ")
	{
		bool bIsBoundMark;
		bool bIsMouseOverButton;
		int x,y; // vi tri cua chuot luc bat dau click vao bound mark
		pDoc->GetObjCont()->CheckMouseOver(pt,bIsLabel,bIsBoundMark,bIsMouseOverButton,objMouseOver1,objMouseOver2,m_boundState,x,y);

		if (bIsBoundMark)
		{
			((CVGBound*)objMouseOver1)->m_ResizeX=x;
			((CVGBound*)objMouseOver1)->m_ResizeY=y;
			m_objBoundResizing=(CVGBound*)objMouseOver1;
			m_btdMode=BTD_MODE_RESIZE_OBJ;
		}
		else if (bIsMouseOverButton && !pDoc->m_bDesignMode)
		{
			if (IsButtonType(objMouseOver1->m_Type))
			{
				((CVGButtonBase*)objMouseOver1)->m_bButtonMouseDown=TRUE;
				m_btdMode=BTD_MODE_BUTTON_MOUSE_DOWN;
				Draw();
			}
			else if (objMouseOver1->m_Type==OBJ_SLIDER)
			{
				m_btdMode=BTD_MODE_LBUTTON_SLIDER;
				((CVGSlider*)objMouseOver1)->OnLButton(pt);
				pDoc->GetObjCont()->Calc();
				Draw();
			}
		}
		else
		{
			if (objMouseOver1!=NULL)
			{
				if (bIsLabel)
				{
					m_btdMode=BTD_MODE_DRAG_LABEL;
					m_objLabelIndex=objMouseOver1;
				}
				else
				{
					if (!((IsBoundType(objMouseOver1->m_Type)) && !pDoc->m_bDesignMode))
						m_btdMode=BTD_MODE_DRAG_SELECTION;
					else
						goto modeSelect;
				}
				m_beginPoint=pt;
			}
			else
			{
modeSelect:
				// Vẽ hình chữ nhật bao quanh đối tượng chọn
				m_btdMode=BTD_MODE_SELECT;
				m_SelectionTopLeft=pt;
				m_SelectionRightBottom=pt;
			}
		}
	}
	
	else // Chọn đối tượng để thực hiện dựng hình
	{
		pDoc->GetObjCont()->CheckMouseOverGeoObj(pt,bIsLabel,objMouseOver1,objMouseOver2);
		
		if (pDoc->GetObjCont()->m_bIsTesting) // Neu dang o che do dung thu thi tat di
		{
			pDoc->GetObjCont()->DeleteTestObj();
			m_bTestPointMoving=FALSE;
		}

		if (objMouseOver1==NULL) // Điểm mới
		{
KhongDuaChuotLenDoiTuongNao:
			if (pDoc->IsObjTypeAvailable(OBJ_POINT_HIDDEN_CHILD))
			{
				CVGPoint* newPt = (CVGPoint*)pDoc->GetObjCont()->AddObject(OBJ_POINT);
				PointD point=pDoc->GetObjCont()->m_AxisInfo.ToAxisPoint(pt);
				newPt->m_Mode=POINT_MODE_NUM_NUM;
				newPt->m_x=point.X;
				newPt->m_y=point.Y;
				newPt->m_Type=OBJ_POINT_HIDDEN_CHILD;
				newPt->m_bTemp=true;
				newPt->m_bVisible=false;
				pDoc->AddCrObj(newPt);
				GetDocument()->SetModifiedFlag();
			}

			else if (pDoc->IsObjTypeAvailable(OBJ_POINT))
			{
				CVGPoint* newPt = (CVGPoint*)pDoc->GetObjCont()->AddObject(OBJ_POINT);
				CArray<int> arr; arr.Add(pDoc->GetObjCont()->GetSize()-1);
				pDoc->GetObjCont()->OnAddObjs(&arr);
				pDoc->GetObjCont()->cmdCount++;
				newPt->m_cmdIndex=pDoc->GetObjCont()->cmdCount;
				PointD point;
				if (nFlags & MK_SHIFT)
				{
					Point pt1=GetPlotPoint(CPoint(pt.X,pt.Y),newPt);
					point=pDoc->GetObjCont()->m_AxisInfo.ToAxisPoint(pt1);
				}
				else point=pDoc->GetObjCont()->m_AxisInfo.ToAxisPoint(pt);
				newPt->m_Mode=POINT_MODE_NUM_NUM;
				newPt->m_x=point.X;
				newPt->m_y=point.Y;
				newPt->SetName(pDoc->GetObjCont()->GetNextAvailableName(OBJ_POINT),pDoc->GetObjCont()->m_LabelFont,&pDoc->GetObjCont()->m_AxisInfo);
				pDoc->GetObjCont()->AddObjTreeItem(newPt);
				newPt->m_Definition=L"Point ~"+newPt->m_Name+L"~";
				pDoc->m_wndConstructionSteps->AddCommand(newPt->m_Definition);
				pDoc->AddCrObj(newPt);
				GetDocument()->SetModifiedFlag();
			}
			else if (pDoc->IsObjTypeAvailable(OBJ_EXPRESSION))
				pDoc->AddExpression();
		}
		else // Chọn điểm
		{
			CVGObject* obj=objMouseOver1;
			if (pDoc->IsObjTypeAvailable(obj->m_Type) && obj->m_Select==FALSE && pDoc->m_currentToolName!=L"Điểm")
			{
				pDoc->AddCrObj(obj);
			}
			else if (pDoc->IsObjTypeAvailable(OBJ_POINT))
			{
				if (objMouseOver2==NULL) // Điểm nằm trên đối tượng
				{
					CVGPoint* newPt = (CVGPoint*)pDoc->GetObjCont()->AddObject(OBJ_POINT);
					CArray<int> arr; arr.Add(pDoc->GetObjCont()->GetSize()-1);
					pDoc->GetObjCont()->OnAddObjs(&arr);
					pDoc->GetObjCont()->cmdCount++;
					newPt->m_cmdIndex=pDoc->GetObjCont()->cmdCount;
					PointD point=pDoc->GetObjCont()->m_AxisInfo.ToAxisPoint(pt);
					newPt->m_Mode=POINT_MODE_NUM_NUM;
					newPt->m_x=point.X;
					newPt->m_y=point.Y;
					newPt->m_Type=OBJ_POINT;
					newPt->SetChild(objMouseOver1);
					newPt->SetName(pDoc->GetObjCont()->GetNextAvailableName(OBJ_POINT),pDoc->GetObjCont()->m_LabelFont,&pDoc->GetObjCont()->m_AxisInfo);
					pDoc->GetObjCont()->AddObjTreeItem(newPt);
					newPt->m_Definition=L"Point ~"+newPt->m_Name+L"~"+L" : Point("+L"~"+objMouseOver1->m_Name+L"~"+L")";
					pDoc->m_wndConstructionSteps->AddCommand(newPt->m_Definition);
					pDoc->AddCrObj(newPt);
				}
				else // Giao điểm
				{
					CVGPoint* newPt = (CVGPoint*)pDoc->GetObjCont()->AddObject(OBJ_POINT);
					CArray<int> arr; arr.Add(pDoc->GetObjCont()->GetSize()-1);
					pDoc->GetObjCont()->OnAddObjs(&arr);
					pDoc->GetObjCont()->cmdCount++;
					newPt->m_cmdIndex=pDoc->GetObjCont()->cmdCount;
					PointD point=pDoc->GetObjCont()->m_AxisInfo.ToAxisPoint(pt);
					newPt->m_x=point.X;
					newPt->m_y=point.Y;
					newPt->m_Type=OBJ_POINT;
					newPt->SetName(pDoc->GetObjCont()->GetNextAvailableName(OBJ_POINT),pDoc->GetObjCont()->m_LabelFont,&pDoc->GetObjCont()->m_AxisInfo);
					pDoc->GetObjCont()->SetIntersection((CVGGeoObject*)objMouseOver1,
						(CVGGeoObject*)objMouseOver2,newPt);
					pDoc->GetObjCont()->AddObjTreeItem(newPt);
					newPt->m_Definition=L"Point ~"+newPt->m_Name+L"~"+L" : Intersection("+L"~"+((CVGGeoObject*)objMouseOver1)->m_Name+L"~"+L", "+L"~"+
						((CVGGeoObject*)objMouseOver2)->m_Name+L"~"+L")";
					pDoc->m_wndConstructionSteps->AddCommand(newPt->m_Definition);
					pDoc->AddCrObj(newPt);
				}
			}
			else goto KhongDuaChuotLenDoiTuongNao;
		}
	}

	CScrollView::OnLButtonDown(nFlags, point);
}

void CVGeoDrawerView::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bMovingObj=FALSE;
	Point pt=GetPadPoint(point.x,point.y);
	
	if (GetCapture()==this)
	{
		ReleaseCapture();

		CVGeoDrawerDoc* pDoc=GetDocument();
		bool bIsLabel;
		pDoc->GetObjCont()->CheckMouseOverObj(pt,bIsLabel,objMouseOver1,objMouseOver2);

		if (GetDocument()->m_currentToolName==L"Con trỏ" && objMouseOver1!=NULL && nFlags & MK_CONTROL) 
			// Chọn thêm đối tượng (giữ Ctrl)
		{
			m_btdMode=BTD_MODE_DRAG_SELECTION;
			GetDocument()->GetObjCont()->Select(objMouseOver1,FALSE);
			RefreshProperties();
		}
		else if (m_btdMode==BTD_MODE_SELECT)
		{
			CVGeoDrawerDoc* pDoc=GetDocument();
			GetDocument()->GetObjCont()->SelectAll(FALSE);
			Rect rect;
			RectFromTwoPoints(m_SelectionTopLeft,m_SelectionRightBottom,rect);			
			GetDocument()->GetObjCont()->SelectInRect(rect);
			GetDocument()->GetObjCont()->SelectInRect(rect);
			m_SelectionTopLeft=m_SelectionRightBottom;
			RefreshProperties();
		}
		else if (m_btdMode==BTD_MODE_DRAG_LABEL || m_btdMode==BTD_MODE_DRAG_SELECTION)
		{
			if (m_beginPoint.X==pt.X && m_beginPoint.Y==pt.Y)
			{
				if ((objMouseOver1!=NULL && (objMouseOver1)->m_Select==FALSE) && 
					!(objMouseOver1!=NULL && (IsBoundType(objMouseOver1->m_Type)) && !pDoc->m_bDesignMode))
				{
					pDoc->GetObjCont()->SelectAll(FALSE);
					pDoc->GetObjCont()->Select(objMouseOver1,FALSE);
				}
				RefreshProperties();
			}
			else
			{
				pDoc->GetObjCont()->ResetButtonState();
				// BoundObj trong panel
// 				if (nMouseOver1!=-1 && IsBoundType(objMouseOver1->m_Type))
// 				{
// 					CVGBound* obj=(CVGBound*)objMouseOver1;
// 					Rect rect=obj->GetRect();
// 					int i=pDoc->GetObjCont()->CheckPtInPanel(pt,obj);
// 					if (i!=-1)
// 					{
// 						if (obj->m_Param.GetSize()!=0)
// 						{
// 							CVGPanel* panel=(CVGPanel*)obj->m_Param[0]; obj->m_Param.RemoveAll();
// 							for (int i=0;i<panel->m_objChild.GetSize();i++)
// 								if (panel->m_objChild[i]==obj) panel->m_objChild.RemoveAt(i);
// 						}
// 						obj->m_Param.Add((CVGPanel*)pDoc->GetObjCont()->GetAt(i));
// 						((CVGPanel*)pDoc->GetObjCont()->GetAt(i))->m_objChild.Add(obj);
// 
// 					}
// 				}
			}
		}
		else if (m_btdMode==BTD_MODE_BUTTON_MOUSE_DOWN)
		{
			int i=pDoc->GetObjCont()->ClickButton();
			pDoc->GetObjCont()->ResetButtonState();
			if (i!=-1)
			{
				if (((CVGButtonBase*)pDoc->GetObjCont()->GetAt(i))->CheckMouseOverButton(pt))
				{
					((CVGButtonBase*)pDoc->GetObjCont()->GetAt(i))->m_bButtonMouseOver=true;
				}
			}
		}
		else if (m_btdMode==BTD_MODE_NEW_BOUND_ITEM)
		{
			Rect rc;
			RectFromTwoPoints(m_SelectionTopLeft,m_SelectionRightBottom,rc);
			Rect rect(rc.GetLeft()*100/nZoom, rc.GetTop()*100/nZoom,rc.Width*100/nZoom,rc.Height*100/nZoom);
			CVGBound* obj=(CVGBound*)GetDocument()->GetObjCont()->AddObject(m_newBoundObjType);
			if (obj->m_Type==OBJ_BUTTON) ((CVGButtonBase*)obj)->m_Text=L"Nút bấm";
			if (obj->m_Type==OBJ_CHECKBOX) ((CVGButtonBase*)obj)->m_Text=L"Check Box";
			if (obj->m_Type==OBJ_RADIO_BUTTON) ((CVGButtonBase*)obj)->m_Text=L"Radio Button";
			if (obj->m_Type==OBJ_SLIDER) ((CVGSlider*)obj)->m_Text=L"Slider";
			if (obj->m_Type==OBJ_TEXT)
			{
				CTextEditorDlg dlg(this);
				if (dlg.DoModal()==IDOK)
				{
					((CVGText*)obj)->SetText(dlg.m_Text,&GetDocument()->GetObjCont()->m_Array,GetDocument()->GetObjCont()->m_ScriptObject,GetDocument()->GetObjCont()->m_strReplacedNameArr);
				}
			}
			if (obj->m_Type==OBJ_IMAGE)
			{
				CFileDialog dlg(TRUE,L".bmp",NULL,OFN_FILEMUSTEXIST,
					L"All Pictures (*.emf;*.jpg;*.jpeg;*.gif;*.bmp;*.png;*.tiff)|*.jpg;*.jpeg;*.gif;*.bmp;*.png;*.tiff;*.emf|All Files (*.*)|*.*|", NULL,0);
				if (dlg.DoModal()==IDOK)
				{
					Bitmap* bmp;
					bmp=Bitmap::FromFile(dlg.GetOFN().lpstrFile);
					((CVGImage*)obj)->SetBitmap(bmp);
					delete bmp;
				}
			}
			obj->SetRect(rect);
			
			int k=GetDocument()->GetObjCont()->CheckRectInPanel(rect,obj->m_Type==OBJ_PANEL ? (CVGPanel*)obj:NULL);
			if (k!=-1) 
				GetDocument()->GetObjCont()->SetObjParent(obj,(CVGPanel*)GetObject(k));

			m_SelectionTopLeft=m_SelectionRightBottom;

			GetDocument()->GetObjCont()->AddObjTreeItem(obj);

			CArray<int> arr; arr.Add(GetDocument()->GetObjCont()->GetSize()-1);
			GetDocument()->GetObjCont()->OnAddObjs(&arr);

			SetViewCursor(IDC_ARROW);
			m_btdMode=BTD_MODE_NONE;
		}
		Draw();
	}

	CScrollView::OnLButtonUp(nFlags, point);
}

void CVGeoDrawerView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (GetDocument()->m_ObjContainerArr.GetSize()==0) return;
	bool bDraw=false; if (strTextFollowMouse!=L"") bDraw=true;
	CVGeoDrawerDoc* pDoc=GetDocument();
	bool bIsLabel;
	Point pt=GetPadPoint(point.x,point.y);
	if (pDoc->GetObjCont()->ResetButtonState()) bDraw=true;

	// Neu Button dang dc nhan thi dung moi hoat dong
	if ((nFlags & MK_LBUTTON) && m_btdMode==BTD_MODE_BUTTON_MOUSE_DOWN) return;

	if ((((nFlags & MK_LBUTTON) || (nFlags & MK_MBUTTON)) && (GetCapture()==this)) || m_bTestPointMoving)
	{
		if ((nFlags & MK_CONTROL) || (nFlags & MK_MBUTTON) || (pDoc->m_currentToolName==L"Dịch chuyển")) 
			// Di chuyen truc toa do
		{
			if (m_btdMode==BTD_MODE_DRAG_DRAWING_PAD)
			{
				pDoc->GetObjCont()->m_AxisInfo.m_originPoint=Point(
					pDoc->GetObjCont()->m_AxisInfo.m_originPoint.X+pt.X-m_lPoint.X,
					pDoc->GetObjCont()->m_AxisInfo.m_originPoint.Y+pt.Y-m_lPoint.Y);
				m_lPoint=pt;
				DrawBackground();
				pDoc->GetObjCont()->Calc_Resize();
				bDraw=true;
			}
		}

		else if (m_bTestPointMoving)
		{
			CVGPoint* vgPoint=(CVGPoint*)pDoc->GetObjCont()->GetAt(pDoc->GetObjCont()->m_nTestStartPos);
			PointD point;
			if (nFlags & MK_SHIFT)
			{
				Point pt1=GetPlotPoint(CPoint(pt.X,pt.Y),vgPoint);
				point=pDoc->GetObjCont()->m_AxisInfo.ToAxisPoint(pt1);
			}
			else point=pDoc->GetObjCont()->m_AxisInfo.ToAxisPoint(pt);
			vgPoint->m_x=point.X;
			vgPoint->m_y=point.Y;
			pDoc->GetObjCont()->Calc(vgPoint);
			bDraw=true;
		}

		else if (m_btdMode==BTD_MODE_NEW_BOUND_ITEM)
		{
			m_SelectionRightBottom=pt;
			bDraw=true;
		}

		else if (m_btdMode==BTD_MODE_LBUTTON_SLIDER)
		{
			((CVGSlider*)objMouseOver1)->OnLButton(pt);
			pDoc->GetObjCont()->Calc(((CVGSlider*)objMouseOver1)->m_NumVariable);
			bDraw=true;
		}
		
		else if (pDoc->m_currentToolName==L"Con trỏ")
		{
			if (m_btdMode==BTD_MODE_SELECT)
			{
				m_SelectionRightBottom=pt;
				bDraw=true;
			}
			if (m_btdMode==BTD_MODE_DRAG_SELECTION)
			{
				CVGObject* obj=objMouseOver1;
				if (!m_bMovingObj) 
				{
					CArray<CVGObject*> arr;
					arr.Add(obj);
					if (((IsLineType(obj->m_Type)) && (obj->m_Mode==LINE_MODE_THROUGH_POINT || obj->m_Mode==LINE_MODE_THROUGH_TWO_POINTS))
						|| (obj->m_Type==OBJ_CIRCLE && obj->m_Mode==CIRCLE_MODE_POINT_POINT))
					{
						arr.Add(obj->m_Param[0]);
						arr.Add(obj->m_Param[1]);
					}
					for (int i=0;i<arr.GetSize();i++)
						if (arr[i]->m_Type==OBJ_POINT)
						{
							for (int j=0;j<((CVGPoint*)arr[i])->m_objMove.GetSize();j++)
								arr.Add(((CVGPoint*)arr[i])->m_objMove.GetAt(j));
						}

					pDoc->GetObjCont()->OnChangeProperties(&arr);
					m_bMovingObj=TRUE;
				}

				pDoc->GetObjCont()->ResetObjMoved();
				//Shift ?
				if (obj->m_Type==OBJ_POINT && obj->m_Mode==POINT_MODE_NUM_NUM && 
					(nFlags & MK_SHIFT))
				{
					Point pnt=GetPlotPoint(CPoint(pt.X,pt.Y),obj);
					PointD pntD=pDoc->GetObjCont()->m_AxisInfo.ToAxisPoint(pnt);
					CVGPoint* vgPt=(CVGPoint*)objMouseOver1;

					for (int i=0;i<vgPt->m_objMove.GetSize();i++)
					{
						vgPt->m_objMove[i]->m_moved=TRUE;
						vgPt->m_objMove[i]->m_x+=pntD.X-vgPt->m_x;
						vgPt->m_objMove[i]->m_y+=pntD.Y-vgPt->m_y;
						vgPt->m_objMove[i]->Move((pnt.X-pt.X)*100/nZoom,
							(pnt.Y-pt.Y)*100/nZoom,CPoint(0,0),&pDoc->GetObjCont()->m_AxisInfo);
					}
					vgPt->m_x=pntD.X;
					vgPt->m_y=pntD.Y;

					vgPt->m_moved=TRUE;
				}
				else if ((IsLineType(obj->m_Type)) && 
					obj->m_Mode==LINE_MODE_THROUGH_TWO_POINTS && 
					(obj->m_Param[1])->m_Type==OBJ_POINT_HIDDEN_CHILD &&
					(nFlags & MK_SHIFT))
				{
					Point pnt=GetPlotPoint(CPoint(pt.X,pt.Y),obj);
					PointD pntD=pDoc->GetObjCont()->m_AxisInfo.ToAxisPoint(pnt);
					CVGPoint* vgPt=(CVGPoint*)obj->m_Param[1];

					vgPt->m_x=pntD.X;
					vgPt->m_y=pntD.Y;
					vgPt->m_moved=TRUE;

					pDoc->GetObjCont()->Calc(obj);
				}
				else
				{
					obj->Move((pt.X-m_PrevPt.X)*100/nZoom,(pt.Y-m_PrevPt.Y)*100/nZoom,CPoint(m_PrevPt.X,m_PrevPt.Y),&pDoc->GetObjCont()->m_AxisInfo);
					if (IsBoundType(obj->m_Type))
					{
						int i=GetDocument()->GetObjCont()->CheckPtInPanel(pt,(CVGBound*)obj);
					
						pDoc->GetObjCont()->SetObjParent((CVGBound*)obj,(CVGPanel*)GetObject(i));
					}
				}
				
				if (IsGeoObject(obj->m_Type)) 
				{
					pDoc->GetObjCont()->Calc(obj);
					pDoc->m_wndProperties->RefreshProperties();
				}
				bDraw=true;
			}
			else if (m_btdMode==BTD_MODE_DRAG_LABEL)
			{
				pDoc->GetObjCont()->DragLabel(m_objLabelIndex,pt.X-m_PrevPt.X,pt.Y-m_PrevPt.Y);
				bDraw=true;
			}
			else if (m_btdMode==BTD_MODE_RESIZE_OBJ)
			{
				CVGBound* obj=m_objBoundResizing;
				SetViewCursor(obj->GetCursor(m_boundState));
				obj->Resize(m_boundState,pt.X*100/nZoom,pt.Y*100/nZoom);
				bDraw=true;
			}
		}
	}

	BOOL bFinal; // Dùng phía sau

	if (!((nFlags & MK_LBUTTON) || (nFlags & MK_RBUTTON))) // Di chuyển chuột, không nhấn
	{

		if (strTextFollowMouse!=strFixedTextFollowMouse) { strTextFollowMouse=strFixedTextFollowMouse; bDraw=true; }
		if (pDoc->GetObjCont()->HighLightObj(NULL)) bDraw=true;

		if (pDoc->m_currentToolName==L"Con trỏ")
		{
			bool bIsMouseOverButton;
			bool bIsBoundMark;
			int x,y;
			int st;
			pDoc->GetObjCont()->CheckMouseOver(pt, bIsLabel, bIsBoundMark, bIsMouseOverButton, objMouseOver1, objMouseOver2, st, x, y);

			if (bIsBoundMark)
				SetViewCursor(((CVGBound*)objMouseOver1)->GetCursor(st));
			else if (bIsMouseOverButton && !pDoc->m_bDesignMode)
			{
				if (IsButtonType(objMouseOver1->m_Type))
				{
					SetViewCursor(IDC_HAND);
					((CVGButtonBase*)objMouseOver1)->m_bButtonMouseOver=TRUE;
					bDraw=true;
				}
				else if (objMouseOver1->m_Type==OBJ_SLIDER)
				{
					SetViewCursor(IDC_HAND);
					((CVGSlider*)objMouseOver1)->m_bMouseOver=TRUE;
					bDraw=true;
				}
			}
			else
			{
				if (objMouseOver1!=NULL)
				{
					//SetViewCursor(IDC_HAND);
					SetViewCursor(IDC_ARROW);
					pDoc->GetObjCont()->HighLightObj(objMouseOver1);
					bDraw=true;
				}
				else
				{
					SetViewCursor(IDC_ARROW);
					if (pDoc->GetObjCont()->HighLightObj(NULL)) bDraw=true;
					if (strTextFollowMouse!=strFixedTextFollowMouse) { strFixedTextFollowMouse=L""; bDraw=true; }
				}
			}
		}
		else // Chon dt de dung hinh
		{
			pDoc->GetObjCont()->CheckMouseOverObj(pt, bIsLabel, objMouseOver1, objMouseOver2);
			// Cho phép chọn đối tượng
			if (objMouseOver1!=NULL && !objMouseOver1->m_Select && (pDoc->IsObjTypeAvailable(objMouseOver1->m_Type,&bFinal)))
			{
				if (bFinal && /*!pDoc->GetObjCont()->m_bIsTestingPoint &&*/ (objMouseOver1)->m_Select==FALSE)
				{
					pDoc->GetObjCont()->DeleteTestObj();
					pDoc->GetObjCont()->StartTestObj();
					pDoc->GetObjCont()->HighLightObj(objMouseOver1);
					pDoc->AddCrObj(objMouseOver1,TRUE);
					bDraw=true;
				}
				else
				{
					SetViewCursor(IDC_HAND);
					pDoc->GetObjCont()->HighLightObj(objMouseOver1);
					bDraw=true;
				}
			}
			// Khi có thể chọn đt là điểm và đưa chuột lên một đt khác. Diem nam tren doi tuong do dc tao
			else if (objMouseOver1!=NULL &&
				objMouseOver1->m_Type!=OBJ_POINT && 
				pDoc->IsObjTypeAvailable(OBJ_POINT) &&
				!pDoc->IsObjTypeAvailable(objMouseOver1->m_Type) &&
				pDoc->m_currentToolName!=L"Con trỏ")
			{
				if (objMouseOver2==NULL)
				{
					if (IsLineType(objMouseOver1->m_Type) || 
						objMouseOver1->m_Type==OBJ_CIRCLE ||
						objMouseOver1->m_Type==OBJ_FUNCTION ||
						objMouseOver1->m_Type==OBJ_CONIC)
					{
						SetViewCursor(IDC_HAND);
						strTextFollowMouse=L"Điểm trên đối tượng";
					}
					else 
					{
						if (strTextFollowMouse!=strFixedTextFollowMouse) { strTextFollowMouse=strFixedTextFollowMouse; bDraw=true; }
						goto KhongDuaChuotLenObjNao;
						//goto ChoPhepChonDoiTuong;
					}
				}
				else
				{
					SetViewCursor(IDC_HAND);
					strTextFollowMouse=L"Giao điểm";
				}
				bDraw=true;
			}
			// Không đưa chuột lên đt nào
			else
			{			
			KhongDuaChuotLenObjNao:
				SetViewCursor(IDC_ARROW);
				BOOL bIsFinal;
				if (pDoc->IsObjTypeAvailable(OBJ_POINT, &bIsFinal) && bIsFinal==TRUE) // Thử dựng trước đt
				{
					if (pDoc->GetObjCont()->m_bIsTesting && !pDoc->GetObjCont()->m_bIsTestingPoint) pDoc->GetObjCont()->DeleteTestObj();
					if (!pDoc->GetObjCont()->m_bIsTesting)
					{
						if (pDoc->IsObjTypeAvailable(OBJ_POINT))
						{
							SetCapture();
							pDoc->GetObjCont()->StartTestObj();

							CVGPoint* newPt = (CVGPoint*)pDoc->GetObjCont()->AddObject(OBJ_POINT);
							PointD point;
							if (nFlags & MK_SHIFT)
							{
								Point pt1=GetPlotPoint(CPoint(pt.X,pt.Y),newPt);
								point=pDoc->GetObjCont()->m_AxisInfo.ToAxisPoint(pt1);
							}
							else point=pDoc->GetObjCont()->m_AxisInfo.ToAxisPoint(pt);
							newPt->m_Mode=POINT_MODE_NUM_NUM;
							newPt->m_x=point.X;
							newPt->m_y=point.Y;
							newPt->m_bVisible=FALSE;
							newPt->m_bTemp=true;
							pDoc->AddCrObj(newPt,TRUE);
							m_bTestPointMoving=TRUE;
							pDoc->GetObjCont()->m_bIsTestingPoint=TRUE;
						}
					}
				}
				else // Khong thu dung dc
				{
					pDoc->GetObjCont()->DeleteTestObj();

					if (pDoc->GetObjCont()->HighLightObj(NULL)) bDraw=true;
					if (strTextFollowMouse!=strFixedTextFollowMouse) { strTextFollowMouse=strFixedTextFollowMouse; bDraw=true; }
				}
			}
		}
	}

	m_PrevPt=pt;
	if (bDraw) Draw();

	CScrollView::OnMouseMove(nFlags, point);
}

BOOL CVGeoDrawerView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	GetDocument()->GetObjCont()->m_AxisInfo.IncZoom(zDelta/120,pt);
	GetDocument()->GetObjCont()->Calc_Resize();

	DrawBackground();
	Draw();

	return TRUE;
	//return CScrollView::OnMouseWheel(nFlags, zDelta, pt);
}

BOOL CVGeoDrawerView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	SetCursor(m_currentCursor);
 	
	//return CScrollView::OnSetCursor(pWnd, nHitTest, message);
	return 1;
}

void CVGeoDrawerView::OnEditSelectAll()
{
	GetDocument()->GetObjCont()->SelectAll(TRUE);
	Draw();
}

void CVGeoDrawerView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar==VK_LEFT || nChar==VK_RIGHT || nChar==VK_UP || nChar==VK_DOWN)
	{
		int x=0;int y=0;
		int n=1;
		BOOL b=FALSE;
		if ((GetKeyState(VK_CONTROL))<0) n=10;
		switch (nChar)
		{
		case VK_LEFT: x=-n;b=TRUE;break;
		case VK_RIGHT: x=n;b=TRUE;break;
		case VK_UP: y=-n;b=TRUE;break;
		case VK_DOWN: y=n;b=TRUE;break;
		}
		GetDocument()->GetObjCont()->MoveSelection(x,y,CPoint(0,0));
		if (b) Draw();
	}
	else if (nChar==VK_ADD || nChar==VK_SUBTRACT)
	{
		CRect rect; GetClientRect(&rect);
		
		GetDocument()->GetObjCont()->m_AxisInfo.IncZoom(nChar==VK_ADD ? 1 : -1,rect.CenterPoint());
		GetDocument()->GetObjCont()->Calc_Resize();

		DrawBackground();
		Draw();
	}
	else if ((nChar>='a' && nChar <='z') || (nChar>='A' && nChar<='Z'))
	{
		CVGObjContainer* objCont=GetDocument()->GetObjCont();
		int i=objCont->GetSelection();
		if (i!=-1 && objCont->GetSelection(i+1)==-1) // Chỉ có 1 đt đc chọn
		{
// 			if (IsButtonType(objCont->m_Array[i]->m_Type))
// 			{
// 				((CVGButton*)objCont->m_Array[i])->m_Text=((CVGButton*)objCont->m_Array[i])->m_Text+(char)nChar;
// 				Draw();
// 			}
// 			else
// 			{
				CVGObject* obj=objCont->GetAt(i);
				CInputDlg dlg(L"Name",this);
				CString strName=(CString)(char)nChar;
				if ((GetKeyState(VK_SHIFT))>0) strName.MakeLower();
				dlg.AddStatic(L"Nhập tên đối tượng");
				dlg.AddEdit(&strName);
				if (dlg.DoModal()==IDOK)
				{
					if (!GetDocument()->GetObjCont()->CheckRegularName(strName))
					{
						AfxGetApp()->DoMessageBox(L"Tên không hợp lệ", MB_OK | MB_ICONERROR,-1);
					}
					else
					{
						GetDocument()->GetObjCont()->RenameObject(obj,strName);
						// 					if (IsGeoObject(obj->m_Type))
						// 						((CVGGeoObject*)obj)->SetName(strName,&GetDocument()->m_AxisInfo);
						// 					else obj->m_Name=strName;
						GetDocument()->GetObjCont()->Calc();
						Draw();
					}
				}
//			}
		}
	}
	else if (nChar==VK_DELETE)
	{
		GetDocument()->OnEditDelete();
	}
	else if (nChar==VK_RETURN)
	{
		GetDocument()->EndSelectObject();
	}

	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}
void CVGeoDrawerView::OnDraw(CDC* pDC)
{
	if (pDC->IsPrinting())
	{
// 		CRect r; GetClientRect(&r);
// 		Bitmap memBitmap(m_bHasPadSize ? m_Width : r.Width(),
// 			m_bHasPadSize ? m_Height : r.Height(),
// 			PixelFormat32bppARGB);
// 		DrawToBitmap(&memBitmap);
// 		HBITMAP hbmp;
// 		memBitmap.GetHBITMAP(Color(255,2552,255),&hbmp);
// 		CBitmap bmp;
// 		bmp.FromHandle(hbmp);
		//pDC->SelectObject(bmpView);
	}
	else 
	{
		Draw();
		GetDocument()->m_wndPageList->DrawItemBitmap(GetDocument()->m_nCurrentContainer);
		GetDocument()->m_wndPageList->Invalidate();
	}
}

BOOL CVGeoDrawerView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	return CScrollView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void CVGeoDrawerView::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CScrollView::OnShowWindow(bShow, nStatus);
	
	//DrawBackground();
	//Draw();
	//GetDocument()->GetObjCont()->pView=this;
	GetDocument()->pView=this;
}

void CVGeoDrawerView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	
}

void CVGeoDrawerView::OnInitialUpdate()
{
	DrawAllBackground();
	RefreshScrollBar();
	CScrollView::OnInitialUpdate();
}

CAxisInfo* CVGeoDrawerView::GetAxisInfo()
{
	return &GetDocument()->GetObjCont()->m_AxisInfo;
}

void CVGeoDrawerView::RefreshSize()
{
	if (GetDocument()->m_bFitScreen && GetDocument()->m_bHasPadSize)
	{
		CRect rc;
		GetClientRect(rc);
		nZoom=100*(min((double)(rc.Width()-2*VIEW_DISTANCE)/GetDocument()->m_Width,(double)(rc.Height()-2*VIEW_DISTANCE)/GetDocument()->m_Height));
		DrawAllBackground();
		
		int width=GetDocument()->m_Width*nZoom/100;
		int height=GetDocument()->m_Height*nZoom/100;
		
		m_Left=(rc.Width()-width)/2;
		m_Top=(rc.Height()-height)/2;
	}
}

void CVGeoDrawerView::OnSize(UINT nType, int cx, int cy)
{
	if (!GetDocument()->m_bInit) return;

	RefreshSize();

	ResizeDrawingPad();
	
	if (!GetDocument()->m_bHasPadSize) DrawBackground();
	RefreshScrollBar();
}

void CVGeoDrawerView::OnTimer(UINT_PTR nIDEvent)
{
	GetDocument()->GetObjCont()->NextAnimatePos();
	GetDocument()->GetObjCont()->CalcAnimate();
	Draw();

	CScrollView::OnTimer(nIDEvent);
}

void CVGeoDrawerView::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
	CRect rect;
	GetClientRect(&rect);
	int i=0;
	int pos1=GetScrollPos(SB_HORZ);
	switch (nSBCode)
	{
	case SB_LINEDOWN : i=1; break;
	case SB_LINEUP : i=-1; break;
	case SB_PAGEDOWN : i=rect.Height();break;
	case SB_PAGEUP : i=-rect.Height();break;
	case SB_THUMBTRACK : i=nPos-pos1;
	}
	SetScrollPos(SB_HORZ,pos1+i);
	int pos2=GetScrollPos(SB_HORZ);
	m_Left+=-(pos2-pos1);

	int h=GetScrollPos(SB_HORZ);
	m_Left=GetDocument()->m_Width<rect.Width() ? (rect.Width()-GetDocument()->m_Width)/2 : -pos2+VIEW_DISTANCE;
	Draw();
}

void CVGeoDrawerView::OnVScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
	CRect rect;
	GetClientRect(&rect);
	int i=0;
	int pos1=GetScrollPos(SB_VERT);
	switch (nSBCode)
	{
	case SB_LINEDOWN : i=1; break;
	case SB_LINEUP : i=-1; break;
	case SB_PAGEDOWN : i=rect.Height();break;
	case SB_PAGEUP : i=-rect.Height();break;
	case SB_THUMBTRACK : i=nPos-pos1;
	}
	SetScrollPos(SB_VERT,pos1+i);
	int pos2=GetScrollPos(SB_VERT);
	m_Top+=-(pos2-pos1);

	m_Top=GetDocument()->m_Height<rect.Height() ? (rect.Height()-GetDocument()->m_Height)/2 : -pos2+VIEW_DISTANCE;
	Draw();
}

void CVGeoDrawerView::OnPrint( CDC* pDC, CPrintInfo* pInfo )
{

}

void CVGeoDrawerView::AddToCommandList(CString str)
{
	GetDocument()->m_wndConstructionSteps->AddCommand(str);
}

void CVGeoDrawerView::OnStartAnimate()
{
	CArray<CVGObject*> arr;
	for (int i=0;i<GetDocument()->GetObjCont()->GetSize();i++)
		if (IsGeoObject(GetDocument()->GetObjCont()->GetAt(i)->m_Type))
		{
			if (((CVGGeoObject*)GetDocument()->GetObjCont()->GetAt(i))->m_bAnimate)
				arr.Add(GetDocument()->GetObjCont()->GetAt(i));
		}

	GetDocument()->GetObjCont()->OnChangeProperties(&arr);
	SetTimer(0,100,NULL);
	m_bAnimating=TRUE;
}

void CVGeoDrawerView::OnUpdateStartAnimate( CCmdUI *pCmdUI )
{
	pCmdUI->Enable(!m_bAnimating);
}

void CVGeoDrawerView::OnPauseAnimate()
{
	KillTimer(0);
	m_bAnimating=FALSE;
}

void CVGeoDrawerView::OnUpdatePauseAnimate( CCmdUI *pCmdUI )
{
	pCmdUI->Enable(m_bAnimating);
}

void CVGeoDrawerView::RefreshProperties()
{
	GetDocument()->m_wndProperties->RefreshProperties();
}

void CVGeoDrawerView::RefreshConstructionSteps()
{
	GetDocument()->m_wndConstructionSteps->Reload();
}

void CVGeoDrawerView::PopupMenu(int nIndex, CPoint point)
{
	SetViewCursor(IDC_ARROW);
	m_menu.LoadMenu(IDR_POPUP);
	HMENU hMenu=m_menu.Detach();
	CMenu *pPopup = CMenu::FromHandle( hMenu )->GetSubMenu(nIndex);
	//**************   Uncomment these codes if you want to response CmdUI ****************
     CCmdUI      CmdUI;
     CmdUI.m_nIndexMax = pPopup->GetMenuItemCount();
     for( unsigned int nIndex = 0; nIndex < CmdUI.m_nIndexMax; ++nIndex )
     {
          CmdUI.m_nIndex = nIndex;
          CmdUI.m_nID = pPopup->GetMenuItemID(nIndex);
          CmdUI.m_pMenu = pPopup;
          CmdUI.DoUpdate( this, TRUE );
     }
	//*************************************************************************************/
	::SetMenuDefaultItem(pPopup->m_hMenu,0,TRUE);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x,point.y,this);
	PostMessage(WM_NULL);     
}

void CVGeoDrawerView::OnRButtonUp(UINT nFlags, CPoint point)
{
	Point pt=GetPadPoint(point.x,point.y);
	ClientToScreen(&point);
	bool bIsLabel;
	CVGObject* mouse1; CVGObject* mouse2;
	GetDocument()->GetObjCont()->CheckMouseOverObj(pt,bIsLabel,mouse1,mouse2);
	GetDocument()->GetObjCont()->SelectAll(FALSE);
	if (mouse1!=NULL && mouse1->m_Select==FALSE) 
	{
		GetDocument()->GetObjCont()->Select(mouse1,FALSE);
	}
	Draw();

	if (mouse1==NULL)
	{
		PopupMenu(0,point);
	}
	else
	{
		CMenu menu;
		VERIFY(menu.LoadMenu(IDR_POPUP));
		if (IsGeoObject(mouse1->m_Type))
		{
			PopupMenu(1,point);
		}
		else
		{
			PopupMenu(2,point);
		}
	}

	CScrollView::OnRButtonUp(nFlags, point);
}

void CVGeoDrawerView::OnMButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	Point pt=GetPadPoint(point.x,point.y);

	m_btdMode=BTD_MODE_DRAG_DRAWING_PAD;
	SetViewCursor(IDC_SIZEALL);
	m_lPoint=pt;
	GetDocument()->SetModifiedFlag();

	CScrollView::OnMButtonDown(nFlags, point);
}

void CVGeoDrawerView::OnMButtonUp(UINT nFlags, CPoint point)
{
	if (GetCapture()==this) ReleaseCapture();

	CScrollView::OnMButtonUp(nFlags, point);
}

CScriptObject* CVGeoDrawerView::GetScriptObject()
{
	return &GetDocument()->m_ScriptObject;
}

void CVGeoDrawerView::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	BOOL setflag=FALSE;
	if(lpMeasureItemStruct->CtlType==ODT_MENU){
		if(IsMenu((HMENU)lpMeasureItemStruct->itemID)){
			CMenu* cmenu = 
				CMenu::FromHandle((HMENU)lpMeasureItemStruct->itemID);

			if(m_menu.IsMenu(cmenu)){
				m_menu.MeasureItem(lpMeasureItemStruct);
				setflag=TRUE;
			}

			if (m_ToolMenu.IsMenu(cmenu))
			{
				m_ToolMenu.MeasureItem(lpMeasureItemStruct);
				setflag=TRUE;
			}
		}
	}


	CScrollView::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

LRESULT CVGeoDrawerView::OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu)
{
	LRESULT lresult;
	if(m_menu.IsMenu(pMenu))
		lresult=BCMenu::FindKeyboardShortcut(nChar, nFlags, pMenu);
	else
		lresult=CScrollView::OnMenuChar(nChar, nFlags, pMenu);
	return(lresult);

	return CScrollView::OnMenuChar(nChar, nFlags, pMenu);
}

void CVGeoDrawerView::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CScrollView::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	if(!bSysMenu)
	{
		if(m_menu.IsMenu(pPopupMenu))
			BCMenu::UpdateMenu(pPopupMenu);
	}
}

void CVGeoDrawerView::RefreshWatch()
{
	GetDocument()->m_wndWatchBar->m_List.RedrawItems(0,GetDocument()->m_wndWatchBar->m_List.GetItemCount()-1);
}

void CVGeoDrawerView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CVGObjContainer* objCont=GetDocument()->GetObjCont();
	int i=objCont->GetSelection();
	if (i!=-1 && objCont->GetSelection(i+1)==-1) // Chỉ có 1 đt đc chọn
	{
		if (objCont->GetAt(i)->m_Type==OBJ_TEXT)
		{
			CVGText* m_obj=(CVGText*)objCont->GetAt(i);
			CTextEditorDlg dlg;
			dlg.m_Text=m_obj->m_Text;
			if (dlg.DoModal()==IDOK);
			{
				m_obj->m_Text=dlg.m_Text;
			}
			m_obj->DrawText();
		}
	}

	CScrollView::OnLButtonDblClk(nFlags, point);
}
