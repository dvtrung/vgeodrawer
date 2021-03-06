#include "StdAfx.h"
#include "VGObject.h"
#include <math.h>

#define NON_DEFINED_POINT -4254

CVGConic::CVGConic(void)
{
	m_PenWidth=1;
	m_DashStyle=0;
	m_Type=OBJ_CONIC;

	m_PixelVlX1=m_PixelVlX2=m_PixelVlY1=m_PixelVlY2=NULL;
}

CVGConic::~CVGConic(void)
{
}

void CVGConic::Draw(Graphics *gr, CAxisInfo* m_AxisInfo, bool bTrace)
{
	if (!m_bVisible || !m_bAvailable || m_bTemp) return;
	if (m_PixelVlX1==NULL) return;

	Pen pen(m_Color,m_HighLight || m_Select ? (REAL)m_PenWidth+1 : (REAL)m_PenWidth);
	GetPen(m_DashStyle,&pen);

	Point pt1, prevPt1;
	Point pt2, prevPt2;

	
	for (int i=0;i<drPadSize.Width;i++)
	{
		if (i==0)
		{
			prevPt1.X=i*nZoom/100; prevPt1.Y=m_PixelVlX1[i]*nZoom/100;
			prevPt2.X=i*nZoom/100; prevPt2.Y=m_PixelVlX2[i]*nZoom/100;
		}
		else
		{
			pt1.X=i*nZoom/100;
			pt1.Y=m_PixelVlX1[i]*nZoom/100;
			if (m_PixelVlX1[i]!=NON_DEFINED_POINT && m_PixelVlX1[i-1]!=NON_DEFINED_POINT)
				gr->DrawLine(&pen,prevPt1,pt1);
			prevPt1=pt1;

			pt2.X=i*nZoom/100;
			pt2.Y=m_PixelVlX2[i]*nZoom/100;
			if (m_PixelVlX2[i]!=NON_DEFINED_POINT && m_PixelVlX2[i-1]!=NON_DEFINED_POINT)
				gr->DrawLine(&pen,prevPt2,pt2);
			prevPt2=pt2;
		}
	}
	

	for (int i=0;i<drPadSize.Height;i++)
	{
		if (i==0)
		{
			prevPt1.Y=i*nZoom/100; prevPt1.X=m_PixelVlY1[i]*nZoom/100;
			prevPt2.Y=i*nZoom/100; prevPt2.X=m_PixelVlY2[i]*nZoom/100;
		}
		else
		{
			pt1.Y=i*nZoom/100;
			pt1.X=m_PixelVlY1[i]*nZoom/100;
			if (m_PixelVlY1[i]!=NON_DEFINED_POINT && m_PixelVlY1[i-1]!=NON_DEFINED_POINT)
				gr->DrawLine(&pen,prevPt1,pt1);
			prevPt1=pt1;
			
			pt2.Y=i*nZoom/100;
			pt2.X=m_PixelVlY2[i]*nZoom/100;
			if (m_PixelVlY2[i]!=NON_DEFINED_POINT && m_PixelVlY2[i-1]!=NON_DEFINED_POINT)
				gr->DrawLine(&pen,prevPt2,pt2);
			prevPt2=pt2;
			
		}
	}
}

void CVGConic::PtCalc(CAxisInfo* m_AxisInfo)
{
	if (m_PixelVlX1!=NULL) delete[] m_PixelVlX1;
	if (m_PixelVlX2!=NULL) delete[] m_PixelVlX2;
	if (m_PixelVlY1!=NULL) delete[] m_PixelVlY1;
	if (m_PixelVlY2!=NULL) delete[] m_PixelVlY2;
	m_PixelVlX1=new int[drPadSize.Width];
	m_PixelVlX2=new int[drPadSize.Width];
	m_PixelVlY1=new int[drPadSize.Height];
	m_PixelVlY2=new int[drPadSize.Height];
	
	for (int i=0;i<drPadSize.Width;i++)
	{
		double x=m_AxisInfo->ToAxisX(i);
		double y1,y2; int num = GetY(x,y1,y2);
		if (num==0) { m_PixelVlX1[i]=m_PixelVlX2[i]=NON_DEFINED_POINT; }
		else if (num==1)
		{
			m_PixelVlX1[i]=m_AxisInfo->ToClientY(y1);
			m_PixelVlX2[i]=NON_DEFINED_POINT;
		}
		else
		{
			m_PixelVlX1[i]=m_AxisInfo->ToClientY(y1);
			m_PixelVlX2[i]=m_AxisInfo->ToClientY(y2);
		}
	}
	for (int i=0;i<drPadSize.Height;i++)
	{
		double y=m_AxisInfo->ToAxisY(i);
		double x1,x2; int num = GetX(y,x1,x2);
		if (num==0) { m_PixelVlY1[i]=m_PixelVlY2[i]=NON_DEFINED_POINT; }
		else if (num==1)
		{
			m_PixelVlY1[i]=m_AxisInfo->ToClientX(x1);
			m_PixelVlY2[i]=NON_DEFINED_POINT;
		}
		else
		{
			m_PixelVlY1[i]=m_AxisInfo->ToClientX(x1);
			m_PixelVlY2[i]=m_AxisInfo->ToClientX(x2);
		}
	}
}

//ax^2+by^2+cxy+dx+ey+f

int CVGConic::GetY(double x, double& y1, double& y2)
{
	return Math::SolveQuadricFunction(m_b,m_c*x+m_e,m_a*x*x+m_d*x+m_f,y1,y2);
}

int CVGConic::GetX(double y, double& x1, double& x2)
{
	return Math::SolveQuadricFunction(m_a,m_c*y+m_d,m_b*y*y+m_e*y+m_f,x1,x2);
}

bool CVGConic::CheckMouseOver(Point point, CAxisInfo* m_AxisInfo)
{
	if (m_PixelVlX1==NULL) return false;

//  	if (!IsVisible()) return false;
// 	// Tim toa do y ung voi point.x
// 	PointD pt=m_AxisInfo->ToAxisPoint(point);
// 	double y=(1-pt.X*pt.X/(m_a*m_a))*(m_b*m_b);
// 	int y1=m_AxisInfo->ToClientPoint(0,sqrt(y)).Y;
// 	int y2=m_AxisInfo->ToClientPoint(0,-sqrt(y)).Y;
// 	if (abs(point.Y-y1)<5 || abs(point.Y-y2)<5) return true;
// 
// 	double x=(1-pt.Y*pt.Y/(m_b*m_b))*(m_a*m_a);
// 	int x1=m_AxisInfo->ToClientPoint(sqrt(x),0).X;
// 	int x2=m_AxisInfo->ToClientPoint(-sqrt(x),0).X;
// 	if (abs(point.X-x1)<5 || abs(point.X-x2)<5) return true;
// 
// 	return false;

	if (!IsVisible()) return FALSE;
	if (point.X<0 || point.Y<0) return FALSE;

	return (m_PixelVlX1[int(point.X*100/nZoom)]*nZoom/100>point.Y-5 && m_PixelVlX1[int(point.X*100/nZoom)]*nZoom/100<point.Y+5) ||
		(m_PixelVlX2[int(point.X*100/nZoom)]*nZoom/100>point.Y-5 && m_PixelVlX2[int(point.X*100/nZoom)]*nZoom/100<point.Y+5) ||
		(m_PixelVlY1[int(point.Y*100/nZoom)]*nZoom/100>point.X-5 && m_PixelVlY1[int(point.Y*100/nZoom)]*nZoom/100<point.X+5) ||
		(m_PixelVlY2[int(point.Y*100/nZoom)]*nZoom/100>point.X-5 && m_PixelVlY2[int(point.Y*100/nZoom)]*nZoom/100<point.X+5);
}

void CVGConic::CalcFr(CValueFractionEx& a, CValueFractionEx& b, CValueFractionEx& c, CValueFractionEx& d, CValueFractionEx& e, CValueFractionEx& f)
{
	switch (m_Mode)
	{
	case ELLIPSE_MODE_A_B:
		CValue* vl;
		vl=((CVGExpression*)m_Param[0])->m_Exp.CalcFr();
		a=*(CValueFractionEx*)vl; delete vl;
		vl=((CVGExpression*)m_Param[1])->m_Exp.CalcFr();
		b=*(CValueFractionEx*)vl; delete vl;
		vl=((CVGExpression*)m_Param[2])->m_Exp.CalcFr();
		c=*(CValueFractionEx*)vl; delete vl;
		vl=((CVGExpression*)m_Param[3])->m_Exp.CalcFr();
		d=*(CValueFractionEx*)vl; delete vl;
		vl=((CVGExpression*)m_Param[4])->m_Exp.CalcFr();
		e=*(CValueFractionEx*)vl; delete vl;
		vl=((CVGExpression*)m_Param[5])->m_Exp.CalcFr();
		f=*(CValueFractionEx*)vl; delete vl;
	}
}

void CVGConic::Calc(CAxisInfo* m_AxisInfo)
{
	if (!CheckCanCalc()) return;

	switch (m_Mode)
	{
	case ELLIPSE_MODE_A_B:
		m_a=((CVGExpression*)m_Param[0])->GetDoubleValue();
		m_b=((CVGExpression*)m_Param[1])->GetDoubleValue();
		m_c=((CVGExpression*)m_Param[2])->GetDoubleValue();
		m_d=((CVGExpression*)m_Param[3])->GetDoubleValue();
		m_e=((CVGExpression*)m_Param[4])->GetDoubleValue();
		m_f=((CVGExpression*)m_Param[5])->GetDoubleValue();
		PtCalc(m_AxisInfo);
		break;
	}
}

void CVGConic::Serialize( CArchive& ar, CArray<CVGObject*>* objArr )
{
	CVGGeoObject::Serialize(ar,objArr);

	if (ar.IsStoring())
	{
		ar << m_PenWidth;
		ar << m_DashStyle;
	}
	else
	{
		ar >> m_PenWidth;
		ar >> m_DashStyle;
	}
}

bool CVGConic::CheckObjInRect(Rect rect, CAxisInfo* m_AxisInfo)
{
	return false;
}

CString CVGConic::GetValue(bool bName)
{
// 	CValueFractionEx a,b; CalcFr(a,b);
// 	CString str;
// 	a=a*a; b=b*b;
// 	if (bName) str=L"("+m_Name+L") : ";
// 	str+=L"(x^2)"; if (a.ToDouble()!=1) { str+=L"/("+a.ToString();str+=L")"; }
// 	str+=L"+";
// 	str+=L"(y^2)"; if (b.ToDouble()!=1) { str+=L"/("+b.ToString();str+=L")"; }
// 	str+=L"=1";
// 
// 	return str;

	return L"";
}

void CVGConic::Clone( CVGObject* obj )
{
	CVGGeoObject::Clone(obj);
	CVGConic* c=(CVGConic*)obj;
	m_PenWidth=c->m_PenWidth;
	m_DashStyle=c->m_DashStyle;
	m_a=c->m_a;
	m_b=c->m_b;
}