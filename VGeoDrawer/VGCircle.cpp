#include "StdAfx.h"
#include "VGObject.h"
#include <math.h>

CVGCircle::CVGCircle(void)
{
	m_PenWidth=1;
	m_DashStyle=0;
	m_bDirection=0;
	m_Type=OBJ_CIRCLE;
}

CVGCircle::~CVGCircle(void)
{
}

void CVGCircle::Draw(Graphics *gr, CAxisInfo* m_AxisInfo, bool bTrace)
{
	if (!m_bVisible || !m_bAvailable || m_bTemp) return;

	Pen pen(m_Color,(REAL)m_PenWidth+1);
	GetPen(m_DashStyle,&pen);

	int d=(int)(2*m_AxisInfo->ToClientLength(m_Radius));
	Point ptTopLeft=m_AxisInfo->ToClientPoint(m_CenterX-m_Radius,m_CenterY+m_Radius);
	Rect rect(ptTopLeft.X,ptTopLeft.Y,d,d);


	if ((m_Select || m_HighLight) && (!bTrace))
	{
		Pen pen2(Color(100,m_Color.GetRed(),m_Color.GetGreen(),m_Color.GetBlue()),m_PenWidth+1+2);
		gr->DrawEllipse(&pen2,rect);
	}

	gr->DrawEllipse(&pen,rect);
}

bool CVGCircle::CheckMouseOver(Point point, CAxisInfo* m_AxisInfo)
{
	if (!m_bVisible || !m_bAvailable || m_bTemp) return FALSE;
	Point pt=m_AxisInfo->ToClientPoint(m_CenterX,m_CenterY); // Tâm đường tròn
	double radius=m_AxisInfo->ToClientLength(m_Radius);
	double dis=sqrt((double)((point.X-pt.X)*(point.X-pt.X)+(point.Y-pt.Y)*(point.Y-pt.Y)));
	if (dis>radius-4 && dis<radius+4) return TRUE;
	return FALSE;
}

void CVGCircle::Move(int x,int y, CPoint startPt, CAxisInfo* m_AxisInfo)
{
	if (m_bFixed) return;
	if (m_Mode==CIRCLE_MODE_POINT_POINT && m_Param[1]->m_Type==OBJ_POINT_HIDDEN_CHILD)
	{
		((CVGPoint*)m_Param[1])->MoveTo(startPt.x+x,startPt.y+y,m_AxisInfo);
		return;
	}

	if (m_Mode==CIRCLE_MODE_POINT_POINT)
	{
		CVGPoint* center=(CVGPoint*)m_Param[0];
		CVGPoint* pt=(CVGPoint*)m_Param[1];
		center->Move(x,y,startPt,m_AxisInfo);
		center->Move(x,y,startPt,m_AxisInfo);
	}
}

void CVGCircle::CalcFr(CValueFractionEx& centerX, CValueFractionEx& centerY, CValueFractionEx& radius)
{
	switch (m_Mode)
	{
	case CIRCLE_MODE_POINT_POINT:
		{
			CVGPoint* center=(CVGPoint*)m_Param[0];
			CVGPoint* pt=(CVGPoint*)m_Param[1];
			center->CaclFr(centerX,centerY);
			CValueFractionEx ptX,ptY; pt->CaclFr(ptX,ptY);
			radius=(centerX-ptX)*(centerX-ptX)+
				(centerY-ptY)*(centerY-ptY);
			radius=radius.Sqrt();
			break;
		}
	case CIRCLE_MODE_POINT_RADIUS:
		{
			CVGPoint* center=(CVGPoint*)m_Param[0];
			CVGExpression* r=(CVGExpression*)m_Param[1];
			center->CaclFr(centerX,centerY);
			CValue* vl=r->m_Exp.CalcFr();
			radius=*(CValueFractionEx*)vl;
			delete vl;
			break;
		}
	/*
	case GEO_MODE_TRANSLATE:
			{
				CVGCircle* c=(CVGCircle*)m_Param[0];
				CVGVector* v=(CVGVector*)m_Param[1];
				m_CenterX=c->m_CenterX+v->x;
				m_CenterY=c->m_CenterY+v->y;
				m_Radius=c->m_Radius;
				break;
			}
		case GEO_MODE_REFLECT_POINT:
			{
				CVGCircle* c=(CVGCircle*)m_Param[0];
				CVGPoint* pt=(CVGPoint*)m_Param[1];
				Math::GetReflectPoint(c->m_CenterX,c->m_CenterY,pt->m_x,pt->m_y,m_CenterX,m_CenterY);
				m_Radius=c->m_Radius;
			}
		case GEO_MODE_REFLECT_LINE:
			{
				CVGCircle* cir=(CVGCircle*)m_Param[0];
				CVGLineBase* l=(CVGLineBase*)m_Param[1];
				double a,b,c;
				l->GetABC(a,b,c);
				Math::GetReflectPoint(cir->m_CenterX,cir->m_CenterY,a,b,c,m_CenterX,m_CenterY);
				m_Radius=cir->m_Radius;
			}
		case GEO_MODE_DILATE:
			{
				CVGCircle* c=(CVGCircle*)m_Param[0];
				CVGPoint* pt=(CVGPoint*)m_Param[1];
				double k=((CVGExpression*)m_Param[2])->GetDoubleValue();
				Math::GetDilatePoint(c->m_CenterX,c->m_CenterY,pt->m_x,pt->m_y,k,m_CenterX,m_CenterY);
				m_Radius=c->m_Radius*k;
			}*/
	}	
}

void CVGCircle::Calc(CAxisInfo* m_AxisInfo)
{
	if (!CheckCanCalc()) return;

	switch (m_Mode)
	{
	case CIRCLE_MODE_POINT_POINT:
		{
			CVGPoint* center=(CVGPoint*)m_Param[0];
			CVGPoint* pt=(CVGPoint*)m_Param[1];
			m_CenterX=center->m_x;
			m_CenterY=center->m_y;
			m_Radius=sqrt((center->m_x-pt->m_x)*(center->m_x-pt->m_x)+
				(center->m_y-pt->m_y)*(center->m_y-pt->m_y));
			if (m_Radius>500) m_bAvailable=false;
			break;
		}
	case CIRCLE_MODE_POINT_RADIUS:
		{
			CVGPoint* center=(CVGPoint*)m_Param[0];
			CVGExpression* radius=(CVGExpression*)m_Param[1];
			m_CenterX=center->m_x;
			m_CenterY=center->m_y;
			m_Radius=radius->GetDoubleValue();
			if (m_Radius<0) m_bAvailable=FALSE;
			break;
		}
	case GEO_MODE_TRANSLATE:
		{
			CVGCircle* c=(CVGCircle*)m_Param[0];
			CVGVector* v=(CVGVector*)m_Param[1];
			m_CenterX=c->m_CenterX+v->x;
			m_CenterY=c->m_CenterY+v->y;
			m_Radius=c->m_Radius;
			break;
		}
	case GEO_MODE_REFLECT_POINT:
		{
			CVGCircle* c=(CVGCircle*)m_Param[0];
			CVGPoint* pt=(CVGPoint*)m_Param[1];
			Math::GetReflectPoint(c->m_CenterX,c->m_CenterY,pt->m_x,pt->m_y,m_CenterX,m_CenterY);
			m_Radius=c->m_Radius;
		}
	case GEO_MODE_REFLECT_LINE:
		{
			CVGCircle* cir=(CVGCircle*)m_Param[0];
			CVGLineBase* l=(CVGLineBase*)m_Param[1];
			double a,b,c;
			l->GetABC(a,b,c);
			Math::GetReflectPoint(cir->m_CenterX,cir->m_CenterY,a,b,c,m_CenterX,m_CenterY);
			m_Radius=cir->m_Radius;
		}
	case GEO_MODE_DILATE:
		{
			CVGCircle* c=(CVGCircle*)m_Param[0];
			CVGPoint* pt=(CVGPoint*)m_Param[1];
			double k=((CVGExpression*)m_Param[2])->GetDoubleValue();
			Math::GetDilatePoint(c->m_CenterX,c->m_CenterY,pt->m_x,pt->m_y,k,m_CenterX,m_CenterY);
			m_Radius=c->m_Radius*k;
		}
	}
}

void CVGCircle::Serialize( CArchive& ar, CArray<CVGObject*>* objArr )
{
	CVGGeoObject::Serialize(ar,objArr);

	if (ar.IsStoring())
	{
		ar << m_PenWidth;
		ar << m_DashStyle;
		ar << m_CenterX;
		ar << m_CenterY;
		ar << m_Radius;
	}
	else
	{
		ar >> m_PenWidth;
		ar >> m_DashStyle;
		ar >> m_CenterX;
		ar >> m_CenterY;
		ar >> m_Radius;
	}
}

bool CVGCircle::CheckObjInRect(Rect rect, CAxisInfo* m_AxisInfo)
{
	int r=(int)m_AxisInfo->ToClientLength(m_Radius);
	Point ct=m_AxisInfo->ToClientPoint(m_CenterX,m_CenterY);
	CRect rc(rect.X+r,rect.Y+r,rect.GetRight()-2*r,rect.GetBottom()-2*r);
	return rc.PtInRect(CPoint(ct.X,ct.Y));
}

void CVGCircle::OnAnimate(int Speed, CAxisInfo* m_AxisInfo)
{
	if (m_Mode==CIRCLE_MODE_POINT_POINT && (m_Param[1])->m_Type==OBJ_POINT_HIDDEN_CHILD)
	{
		CVGPoint* pt1=(CVGPoint*)m_Param[0];
		CVGPoint* pt2=(CVGPoint*)m_Param[1];
		double a,b,c;
		Math::LineThroughTwoPoints(pt1->m_x,pt1->m_y,pt2->m_x,pt2->m_y,a,b,c);
		double d=m_AxisInfo->ToAxisLength(Speed);

		double x1,y1,x2,y2;
		Math::GetIntersectionLineCircle(pt2->m_x,pt2->m_y,d,a,b,c,x1,y1,x2,y2);
		if (m_bDirection) { pt2->m_x=x1; pt2->m_y=y1; }
		else { pt2->m_x=x2; pt2->m_y=y2; }
		double kc=m_AxisInfo->ToClientLength(Math::GetDistance(pt2->m_x,pt2->m_y,pt1->m_x,pt1->m_y));
		Point pt=m_AxisInfo->ToClientPoint(pt1->m_x,pt1->m_y);
		if (kc>min(pt.X,min(pt.Y,min(drPadSize.Width-pt.X,drPadSize.Height-pt.Y)))) m_bDirection=!m_bDirection;
	}
}

CString CVGCircle::GetValue(bool bName)
{
	CString str;
	CValueFractionEx x,y,c;
	CalcFr(x,y,c);
	if (bName) str=L"("+m_Name+L") : ";

	if (x.a.GetSize()==1 && x.a[0]<0)
		str+=L"(x+"+x.Abs().ToString()+L")^2";
	else
		str+=L"(x-"+x.ToString()+L")^2";

	str+=" + ";

	if (y.a.GetSize()==1 && y.a[0]<0)
		str+=L"(x+"+y.Abs().ToString()+L")^2";
	else
		str+=L"(x-"+y.ToString()+L")^2";

	str+=L"="+(c*c).ToString();
	return str;
}


void CVGCircle::Clone( CVGObject* obj )
{
	CVGGeoObject::Clone(obj);
	CVGCircle* c=(CVGCircle*)obj;
	m_PenWidth=c->m_PenWidth;
	m_DashStyle=c->m_DashStyle;
	m_bDirection=c->m_bDirection;
	m_CenterX=c->m_CenterX;
	m_CenterY=c->m_CenterY;
	m_Radius=c->m_Radius;
}