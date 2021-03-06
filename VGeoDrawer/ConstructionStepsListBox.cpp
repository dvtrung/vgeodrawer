// CommandListBox.cpp : implementation file
//

#include "stdafx.h"
#include "VGeoDrawer.h"
#include "ConstructionStepsListBox.h"
#include "VGMath.h"


// CConstructionStepsListBox

IMPLEMENT_DYNAMIC(CConstructionStepsListBox, CListBox)

CConstructionStepsListBox::CConstructionStepsListBox()
{

}

CConstructionStepsListBox::~CConstructionStepsListBox()
{
}


BEGIN_MESSAGE_MAP(CConstructionStepsListBox, CListBox)
	ON_WM_DRAWITEM_REFLECT()
	ON_WM_MEASUREITEM_REFLECT()
	ON_WM_COMPAREITEM_REFLECT()
	ON_WM_DELETEITEM_REFLECT()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CConstructionStepsListBox message handlers



void CConstructionStepsListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (GetCount()==0) return;

	int dis=strNumSize;
	Graphics gr(lpDrawItemStruct->hDC);
	Rect rect(dis,lpDrawItemStruct->rcItem.top,lpDrawItemStruct->rcItem.right-dis,lpDrawItemStruct->rcItem.bottom-lpDrawItemStruct->rcItem.top);
	
	CString strText;
	GetText(lpDrawItemStruct->itemID,strText);

	if ((lpDrawItemStruct->itemState & ODS_SELECTED))// &&
	{
		Color clr;
		clr.SetFromCOLORREF(GetSysColor(COLOR_HIGHLIGHT));
		LinearGradientBrush br(rect,Color(242,249,252),Color(216,236,246),LinearGradientModeVertical);

		gr.FillRectangle(&br,rect);
		Pen pen(Color(150,217,249));
		gr.DrawRectangle(&pen,dis,rect.GetTop(),rect.Width-1,rect.Height-1);
		pen.SetColor(Color(192,192,192));
		gr.DrawLine(&pen, Point(dis,rect.GetTop()),Point(dis,rect.GetBottom()));
	}
	else
	{
		Pen pen(Color(192,192,192));
		SolidBrush brush(Color(255,255,255));
		gr.FillRectangle(&brush,rect);
		gr.DrawLine(&pen, Point(dis,rect.GetTop()),Point(dis,rect.GetBottom()));
	}

	FontFamily fontFamily(L"Tahoma");
	Font font(&fontFamily,8,FontStyleRegular);
	SolidBrush brush(Color(0,0,0));
	gr.DrawString(strText,-1,&font,PointF(dis+5,rect.GetTop()+5),&brush);
	gr.DrawString(Math::IntToString(lpDrawItemStruct->itemID+1),-1,&font,PointF(1,rect.GetTop()+5),&brush);
}

void CConstructionStepsListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight=20;
}

int CConstructionStepsListBox::CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct)
{
	return 0;
}

void CConstructionStepsListBox::DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct)
{

}

// Kt Chuỗi số
void CConstructionStepsListBox::GetMaxStringSize()
{
	int i=GetCount()+1;
	CString str;
	while (i>=1) { i/=10; str+=L"5"; }
	Graphics gr(this->m_hWnd);
	FontFamily fontFamily(L"Tahoma");
	Font font(&fontFamily,8,FontStyleRegular);
	RectF rect;
	gr.MeasureString(str,-1,&font,PointF(0,0),&rect);
	strNumSize=rect.Width+3;
}
void CConstructionStepsListBox::OnPaint()
{
	CListBox::OnPaint();
	CRect rect;
	GetClientRect(&rect);
	Graphics gr(this->m_hWnd);
	Pen pen(Color(192,192,192));
	gr.DrawLine(&pen, Point(strNumSize,0),Point(strNumSize,rect.Height()));
}
