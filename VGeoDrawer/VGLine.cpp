#include "stdafx.h"
#include "VGObject.h"
#include "VGMath.h"

CVGLine::CVGLine()
{
	m_PenWidth=1;
	m_Type=OBJ_LINE;
	m_bCutSuperfluousPart=FALSE;
	m_pt1=NULL; m_pt2=NULL;
}

CVGLine::~CVGLine(void)
{
}

void CVGLine::Draw(Graphics *gr, CAxisInfo* m_AxisInfo, bool bTrace)
{
	if (!m_bVisible || !m_bAvailable || m_bTemp) return;

	Point pt1,pt2;
	GetScreenStartEndPoint(pt1,pt2,m_AxisInfo);

	if (m_bCutSuperfluousPart && m_pt1!=NULL)
	{
		pt1=m_AxisInfo->ToClientPoint(m_pt1->m_x,m_pt1->m_y);
		pt2=m_AxisInfo->ToClientPoint(m_pt2->m_x,m_pt2->m_y);
		DrawLine(pt1,pt2,gr,bTrace);

		if (m_Select)
		{
			GetScreenStartEndPoint(pt1,pt2,m_AxisInfo);
			Pen pen(Color(75,m_Color.GetR(),m_Color.GetG(),m_Color.GetB()));
			gr->DrawLine(&pen,pt1,pt2);
		}

	}
	else
	{
		GetScreenStartEndPoint(pt1,pt2,m_AxisInfo);
		DrawLine(pt1,pt2,gr,bTrace);
	}
}

bool CVGLine::CheckMouseOver(Point point, CAxisInfo* m_AxisInfo)
{
	if (!m_bVisible || !m_bAvailable || m_bTemp) return FALSE;

	PointD pt=m_AxisInfo->ToAxisPoint(point); // Toa do chuot tren truc
	double y=(-m_c-m_a*pt.X)/m_b;
	double x=(-m_c-m_b*pt.Y)/m_a;
	Point pt1=m_AxisInfo->ToClientPoint(x,pt.Y);
	Point pt2=m_AxisInfo->ToClientPoint(pt.X,y);
	if ((point.X>pt1.X-6 && point.X<pt1.X+6)||(point.Y>pt2.Y-6 && point.Y<pt2.Y+6)) 
	{
		if (m_bCutSuperfluousPart && m_pt1!=NULL && !m_Select)
		{
			pt1=m_AxisInfo->ToClientPoint(m_pt1->m_x,m_pt1->m_y);
			pt2=m_AxisInfo->ToClientPoint(m_pt2->m_x,m_pt2->m_y);
			return (point.X>=min(pt1.X,pt2.X)-6 && point.X<=max(pt1.X,pt2.X)+6 && 
				point.Y>=min(pt1.Y,pt2.Y)-6 && point.Y<=max(pt1.Y,pt2.Y)+6);
		}
		else
			return TRUE;
	}
		
	return FALSE;
}

void CVGLine::Move(int x,int y, CPoint startPt, CAxisInfo* m_AxisInfo)
{
	if (m_bFixed) return;
	
	if (m_Mode==LINE_MODE_THROUGH_TWO_POINTS && m_Param[1]->m_Type==OBJ_POINT_HIDDEN_CHILD)
	{
		((CVGPoint*)m_Param[1])->MoveTo(startPt.x+x,startPt.y+y,m_AxisInfo);
		return;
	}

	if ((m_Mode==LINE_MODE_PARALLEL || m_Mode==LINE_MODE_PERPENDICULAR) && m_Param[0]->m_Type==OBJ_POINT_HIDDEN_CHILD)
	{
		((CVGPoint*)m_Param[0])->MoveTo(startPt.x+x,startPt.y+y,m_AxisInfo);
		return;
	}

	if (m_Mode==LINE_MODE_THROUGH_TWO_POINTS)
	{
		CVGPoint* m_Point1=(CVGPoint*)m_Param[0];
		CVGPoint* m_Point2=(CVGPoint*)m_Param[1];
		m_Point1->Move(x,y,startPt,m_AxisInfo);
		m_Point2->Move(x,y,startPt,m_AxisInfo);
	}
}

void CVGLine::GetABCFr(CValueFractionEx& A, CValueFractionEx& B, CValueFractionEx& C)
{
	switch (m_Mode)
	{
	case LINE_MODE_DEFINED:
		{
			CValue* vl;
			vl=((CVGExpression*)m_Param[0])->m_Exp.CalcFr();
			A=*(CValueFractionEx*)vl; delete vl;
			vl=((CVGExpression*)m_Param[1])->m_Exp.CalcFr();
			B=*(CValueFractionEx*)vl; delete vl;
			vl=((CVGExpression*)m_Param[2])->m_Exp.CalcFr();
			C=*(CValueFractionEx*)vl; delete vl;
			break;
		}
	case LINE_MODE_THROUGH_TWO_POINTS:
		{
			CVGPoint* pt1=(CVGPoint*)m_Param[0];
			CVGPoint* pt2=(CVGPoint*)m_Param[1];
			CValueFractionEx x1,y1; pt1->CaclFr(x1,y1);
			CValueFractionEx x2,y2; pt2->CaclFr(x2,y2);
			Math::LineThroughTwoPointsFr(x1,y1,x2,y2,A,B,C);
			break;
		}
	case LINE_MODE_PARALLEL:
	case LINE_MODE_PERPENDICULAR:
		{
			CVGPoint* pt=(CVGPoint*)m_Param[0];
			CVGLineBase* l=(CVGLine*)m_Param[1];
			CValueFractionEx a,b,c;
			l->GetABCFr(a,b,c);
			CValueFractionEx x,y; pt->CaclFr(x,y);
			if (m_Mode==LINE_MODE_PARALLEL) Math::GetParallelLineFr(x,y,a,b,c,A,B,C);
			else Math::GetPerpendicularLineFr(x,y,a,b,c,A,B,C);
			break;
		}
		case LINE_MODE_ANGLE_BISECTOR:
		{
			CVGPoint* pt1=(CVGPoint*)m_Param[0];
			CVGPoint* pt2=(CVGPoint*)m_Param[1];
			CVGPoint* pt3=(CVGPoint*)m_Param[2];

			CValueFractionEx a1,a2,b1,b2,c1,c2;
			CValueFractionEx x1,x2,x3,y1,y2,y3;
			pt1->CaclFr(x1,y1); pt2->CaclFr(x2,y2); pt3->CaclFr(x3,y3);
			Math::LineThroughTwoPointsFr(x1,y1,x2,y2,a1,b1,c1);
			Math::LineThroughTwoPointsFr(x3,y3,x2,y2,a2,b2,c2);
			CValueFractionEx a3,a4,b3,b4,c3,c4;
			Math::AngleBisectorFr(a1,b1,c1,a2,b2,c2,a3,b3,c3,a4,b4,c4);
			double m1=a3.ToDouble()*pt1->m_x+b3.ToDouble()*pt1->m_y+c3.ToDouble();
			double m2=a3.ToDouble()*pt3->m_x+b3.ToDouble()*pt3->m_y+c3.ToDouble();
			if (Math::Equal(m1,0) || Math::Equal(m2,0))
			{
				A=a1; B=b1; C=c1;
			}
			else if (((m1 > 0) && (m2 > 0)) || ((m1 < 0) && (m2 < 0)))
			{
				A=a4; B=b4; C=c4;
			}
			else
			{
				A=a3; B=b3; C=c3;
			}
			break;
		}
		case LINE_MODE_TANGENT_CONIC:
		{
			CVGPoint* pt=(CVGPoint*)m_Param[0];
			CVGConic* cn=(CVGConic*)m_Param[1];
			CValueFractionEx la,lb,lc;
			CValueFractionEx x,y; pt->CaclFr(x,y);
			CValueFractionEx a,b,c,d,e,f; cn->CalcFr(a,b,c,d,e,f);
			if (tmp==1)
			{
				int num=Math::TangentLineFr(x,y,a,b,c,d,e,f,A,B,C,la,lb,lc);
				if (num<1) m_bAvailable=false;
			}
			else
			{
				int num=Math::TangentLineFr(x,y,a,b,c,d,e,f,la,lb,lc,A,B,C);
				if (num<2) m_bAvailable=false;
			}
			break;
		}/*
			case LINE_MODE_ANGLE_BISECTOR_WITH_POINT:
				{
					CVGLineBase* l1=(CVGLineBase*)m_Param[0];
					CVGLineBase* l2=(CVGLineBase*)m_Param[1];
					CVGPoint* pt=(CVGPoint*)m_Param[2];
					double a1,a2,b1,b2,c1,c2;
					l1->GetABC(a1,b1,c1);
					l2->GetABC(a2,b2,c2);
					double a3,a4,b3,b4,c3,c4;
					Math::AngleBisector(a1,b1,c1,a2,b2,c2,a3,b3,c3,a4,b4,c4);
					bool k1=(a1*pt->m_x+b1*pt->m_y+c1>0) ^ (a2*pt->m_x+b2*pt->m_y+c2>0);
					double x,y; // Điểm bất kì trên pg
					if (!Math::Equal(b3,0)) { x=0; y=-c3/b3; }
					else { y=0; x=-c3/a3; }
					bool k2=(a1*x+b1*y+c1>0) ^ (a2*x+b2*y+c2>0);
					if (k1==k2) { m_a=a3,m_b=b3,m_c=c3; }
					else { m_a=a4,m_b=b4,m_c=c4; }
					break;
				}
			case LINE_MODE_EXTERIOR_ANGLE_BISECTOR:
				{
					CVGPoint* pt1=(CVGPoint*)m_Param[0];
					CVGPoint* pt2=(CVGPoint*)m_Param[1];
					CVGPoint* pt3=(CVGPoint*)m_Param[2];
		
					double a1,a2,b1,b2,c1,c2;
					Math::LineThroughTwoPoints(pt1->m_x,pt1->m_y,pt2->m_x,pt2->m_y,a1,b1,c1);
					Math::LineThroughTwoPoints(pt3->m_x,pt3->m_y,pt2->m_x,pt2->m_y,a2,b2,c2);
					double a3,a4,b3,b4,c3,c4;
					Math::AngleBisector(a1,b1,c1,a2,b2,c2,a3,b3,c3,a4,b4,c4);
					double m1=a3*pt1->m_x+b3*pt1->m_y+c3;
					double m2=a3*pt3->m_x+b3*pt3->m_y+c3;
					if (Math::Equal(m1,0) || Math::Equal(m2,0))
					{
						m_a=a1; m_b=b1; m_c=c1;
					}
					else if (((m1 > 0) && (m2 > 0)) || ((m1 < 0) && (m2 < 0)))
					{
						m_a=a3; m_b=b3; m_c=c3;
					}
					else
					{
						m_a=a4; m_b=b4; m_c=c4;
					}
					break;
				}
			case GEO_MODE_TRANSLATE:
				{
					CVGVector* v=(CVGVector*) m_Param[1];
					CVGLine* l=(CVGLine*) m_Param[0];
					if (l->m_b!=0)
					{
						m_a=l->m_a;
						m_b=l->m_b;
						m_c = - l->m_a * v->x + l->m_c - l->m_b* v->y;
					}
					else
					{
						m_a=l->m_a;
						m_b=0;
						m_c = l->m_c - l->m_a* v->x;
					}
					break;
				}
			case GEO_MODE_REFLECT_POINT:
				{
					CVGPoint* pt=(CVGPoint*) m_Param[1];
					CVGLine* l=(CVGLine*) m_Param[0];
					Math::GetReflectLine(l->m_a,l->m_b,l->m_c,pt->m_x,pt->m_y,m_a,m_b,m_c);
					break;
				}
			case  GEO_MODE_REFLECT_LINE:
				{
					CVGLine* l1=(CVGLine*)m_Param[0];
					CVGLine* l2=(CVGLine*)m_Param[1];
					Math::GetReflectLine(l1->m_a,l1->m_b,l1->m_c,l2->m_a,l2->m_b,l2->m_c,m_a,m_b,m_c);
					break;
				}
			case GEO_MODE_DILATE:
				{
					CVGLine* l=(CVGLine*)m_Param[0];
					CVGPoint* ptCenter=(CVGPoint*)m_Param[1];
					double k=((CVGExpression*)m_Param[2])->GetDoubleValue();
					Math::GetDilateLine(l->m_a,l->m_b,l->m_c,ptCenter->m_x,ptCenter->m_y,k,m_a,m_b,m_c);
				}
			case GEO_MODE_ROTATO:
				{
					CVGLine* l=(CVGLine*)m_Param[0];
					CVGPoint* ptCenter=(CVGPoint*)m_Param[1];
					double k=((CVGExpression*)m_Param[2])->GetDoubleValue();
					Math::GetRotateLine(l->m_a,l->m_b,l->m_c,ptCenter->m_x,ptCenter->m_y,k,m_a,m_b,m_c);
				}*/
		
	}
}

void CVGLine::Calc(CAxisInfo* m_AxisInfo)
{
	if (!CheckCanCalc()) return;

	switch (m_Mode)
	{
	case LINE_MODE_DEFINED:
		{
			m_a=((CVGExpression*)m_Param[0])->GetDoubleValue();
			m_b=((CVGExpression*)m_Param[1])->GetDoubleValue();
			m_c=((CVGExpression*)m_Param[2])->GetDoubleValue();
			break;
		}
	case LINE_MODE_THROUGH_TWO_POINTS:
		{
			CVGPoint* pt1=(CVGPoint*)m_Param[0];
			CVGPoint* pt2=(CVGPoint*)m_Param[1];
			Math::LineThroughTwoPoints(pt1->m_x,pt1->m_y,pt2->m_x,pt2->m_y,m_a,m_b,m_c);
			break;
		}
	case LINE_MODE_PARALLEL:
	case LINE_MODE_PERPENDICULAR:
		{
			CVGPoint* pt=(CVGPoint*)m_Param[0];
			CVGLineBase* l=(CVGLine*)m_Param[1];
			double A,B,C;
			l->GetABC(A,B,C);
			if (m_Mode==LINE_MODE_PARALLEL) Math::GetParallelLine(pt->m_x,pt->m_y,A,B,C,m_a,m_b,m_c);
			else Math::GetPerpendicularLine(pt->m_x,pt->m_y,A,B,C,m_a,m_b,m_c);
			break;
		}
	case LINE_MODE_ANGLE_BISECTOR:
		{
			CVGPoint* pt1=(CVGPoint*)m_Param[0];
			CVGPoint* pt2=(CVGPoint*)m_Param[1];
			CVGPoint* pt3=(CVGPoint*)m_Param[2];

			double a1,a2,b1,b2,c1,c2;
			Math::LineThroughTwoPoints(pt1->m_x,pt1->m_y,pt2->m_x,pt2->m_y,a1,b1,c1);
			Math::LineThroughTwoPoints(pt3->m_x,pt3->m_y,pt2->m_x,pt2->m_y,a2,b2,c2);
			double a3,a4,b3,b4,c3,c4;
			Math::AngleBisector(a1,b1,c1,a2,b2,c2,a3,b3,c3,a4,b4,c4);
			double m1=a3*pt1->m_x+b3*pt1->m_y+c3;
			double m2=a3*pt3->m_x+b3*pt3->m_y+c3;
			if (Math::Equal(m1,0) || Math::Equal(m2,0))
			{
				m_a=a1; m_b=b1; m_c=c1;
			}
			else if (((m1 > 0) && (m2 > 0)) || ((m1 < 0) && (m2 < 0)))
			{
				m_a=a4; m_b=b4; m_c=c4;
			}
			else
			{
				m_a=a3; m_b=b3; m_c=c3;
			}
			break;
		}
	case LINE_MODE_ANGLE_BISECTOR_WITH_POINT:
		{
			CVGLineBase* l1=(CVGLineBase*)m_Param[0];
			CVGLineBase* l2=(CVGLineBase*)m_Param[1];
			CVGPoint* pt=(CVGPoint*)m_Param[2];
			double a1,a2,b1,b2,c1,c2;
			l1->GetABC(a1,b1,c1);
			l2->GetABC(a2,b2,c2);
			double a3,a4,b3,b4,c3,c4;
			Math::AngleBisector(a1,b1,c1,a2,b2,c2,a3,b3,c3,a4,b4,c4);
			bool k1=(a1*pt->m_x+b1*pt->m_y+c1>0) ^ (a2*pt->m_x+b2*pt->m_y+c2>0);
			double x,y; // Điểm bất kì trên pg
			if (!Math::Equal(b3,0)) { x=0; y=-c3/b3; }
			else { y=0; x=-c3/a3; }
			bool k2=(a1*x+b1*y+c1>0) ^ (a2*x+b2*y+c2>0);
			if (k1==k2) { m_a=a3,m_b=b3,m_c=c3; }
			else { m_a=a4,m_b=b4,m_c=c4; }
			break;
		}
	case LINE_MODE_EXTERIOR_ANGLE_BISECTOR:
		{
			CVGPoint* pt1=(CVGPoint*)m_Param[0];
			CVGPoint* pt2=(CVGPoint*)m_Param[1];
			CVGPoint* pt3=(CVGPoint*)m_Param[2];

			double a1,a2,b1,b2,c1,c2;
			Math::LineThroughTwoPoints(pt1->m_x,pt1->m_y,pt2->m_x,pt2->m_y,a1,b1,c1);
			Math::LineThroughTwoPoints(pt3->m_x,pt3->m_y,pt2->m_x,pt2->m_y,a2,b2,c2);
			double a3,a4,b3,b4,c3,c4;
			Math::AngleBisector(a1,b1,c1,a2,b2,c2,a3,b3,c3,a4,b4,c4);
			double m1=a3*pt1->m_x+b3*pt1->m_y+c3;
			double m2=a3*pt3->m_x+b3*pt3->m_y+c3;
			if (Math::Equal(m1,0) || Math::Equal(m2,0))
			{
				m_a=a1; m_b=b1; m_c=c1;
			}
			else if (((m1 > 0) && (m2 > 0)) || ((m1 < 0) && (m2 < 0)))
			{
				m_a=a3; m_b=b3; m_c=c3;
			}
			else
			{
				m_a=a4; m_b=b4; m_c=c4;
			}
			break;
		}
	case LINE_MODE_TANGENT_CONIC:
		{
			CVGPoint* pt=(CVGPoint*)m_Param[0];
			CVGConic* c=(CVGConic*)m_Param[1];
			double la,lb,lc;
			if (tmp==1)
			{
				int num=Math::TangentLine(pt->m_x,pt->m_y,c->m_a,c->m_b,c->m_c,c->m_d,c->m_e,c->m_f,m_a,m_b,m_c,la,lb,lc);
				if (num<1) m_bAvailable=false;
			}
			else
			{
				int num=Math::TangentLine(pt->m_x,pt->m_y,c->m_a,c->m_b,c->m_c,c->m_d,c->m_e,c->m_f,la,lb,lc,m_a,m_b,m_c);
				if (num<2) m_bAvailable=false;
			}
			break;
		}
	case GEO_MODE_TRANSLATE:
		{
			CVGVector* v=(CVGVector*) m_Param[1];
			CVGLine* l=(CVGLine*) m_Param[0];
			if (l->m_b!=0)
			{
				m_a=l->m_a;
				m_b=l->m_b;
				m_c = - l->m_a * v->x + l->m_c - l->m_b* v->y;
			}
			else
			{
				m_a=l->m_a;
				m_b=0;
				m_c = l->m_c - l->m_a* v->x;
			}
			break;
		}
	case GEO_MODE_REFLECT_POINT:
		{
			CVGPoint* pt=(CVGPoint*) m_Param[1];
			CVGLine* l=(CVGLine*) m_Param[0];
			Math::GetReflectLine(l->m_a,l->m_b,l->m_c,pt->m_x,pt->m_y,m_a,m_b,m_c);
			break;
		}
	case  GEO_MODE_REFLECT_LINE:
		{
			CVGLine* l1=(CVGLine*)m_Param[0];
			CVGLine* l2=(CVGLine*)m_Param[1];
			Math::GetReflectLine(l1->m_a,l1->m_b,l1->m_c,l2->m_a,l2->m_b,l2->m_c,m_a,m_b,m_c);
			break;
		}
	case GEO_MODE_DILATE:
		{
			CVGLine* l=(CVGLine*)m_Param[0];
			CVGPoint* ptCenter=(CVGPoint*)m_Param[1];
			double k=((CVGExpression*)m_Param[2])->GetDoubleValue();
			Math::GetDilateLine(l->m_a,l->m_b,l->m_c,ptCenter->m_x,ptCenter->m_y,k,m_a,m_b,m_c);
			break;
		}
	case GEO_MODE_ROTATO:
		{
			CVGLine* l=(CVGLine*)m_Param[0];
			CVGPoint* ptCenter=(CVGPoint*)m_Param[1];
			double k=((CVGExpression*)m_Param[2])->GetDoubleValue();
			Math::GetRotateLine(l->m_a,l->m_b,l->m_c,ptCenter->m_x,ptCenter->m_y,k,m_a,m_b,m_c);
			break;
		}
	}
}

void CVGLine::SetPerpendicular(CVGPoint *pt, CVGLineBase *l)
{
	m_Mode=LINE_MODE_PERPENDICULAR;
	m_Param.RemoveAll();
	m_Param.Add(pt);
	m_Param.Add(l);
}

void CVGLine::SetParallel(CVGPoint *pt, CVGLineBase *l)
{
	m_Mode=LINE_MODE_PARALLEL;
	m_Param.RemoveAll();
	m_Param.Add(pt);
	m_Param.Add(l);
}

void CVGLine::SetExteriorAngleBisector(CVGPoint* pt1, CVGPoint* pt2, CVGPoint* pt3)
{
	m_Mode=LINE_MODE_EXTERIOR_ANGLE_BISECTOR;
	m_Param.RemoveAll();
	m_Param.Add(pt1);
	m_Param.Add(pt2);
	m_Param.Add(pt3);
}

void CVGLine::SetAngleBisector(CVGPoint* pt1, CVGPoint* pt2, CVGPoint* pt3)
{
	m_Mode=LINE_MODE_ANGLE_BISECTOR;
	m_Param.RemoveAll();
	m_Param.Add(pt1);
	m_Param.Add(pt2);
	m_Param.Add(pt3);
}

void CVGLine::SetAngleBisector(CVGLineBase* d1, CVGLineBase* d2, CVGPoint* pt)
{
	m_Mode=LINE_MODE_ANGLE_BISECTOR_WITH_POINT;
	m_Param.RemoveAll();
	m_Param.Add(d1);
	m_Param.Add(d2);
	m_Param.Add(pt);
}

void CVGLine::GetABC(double& A, double& B, double& C)
{
	A=m_a;
	B=m_b;
	C=m_c;
}

CString CVGLine::GetValue(bool bName)
{
	CString str;
	if (bName) str=L"("+m_Name+L"):";
	CValueFractionEx a,b,c;
	GetABCFr(a,b,c);
	if (Math::Equal(m_a,0) && Math::Equal(m_c,0) && !Math::Equal(m_b,0)) str+=L"y=0";
	else if (Math::Equal(m_b,0) && Math::Equal(m_c,0) && !Math::Equal(m_a,0)) str+=L"x=0";
	else if (Math::Equal(m_a,0) && !Math::Equal(m_b,0) && !Math::Equal(m_c,0)) str+=L"y="+(c/b).ToString();
	else if (Math::Equal(m_b,0) && !Math::Equal(m_a,0) && !Math::Equal(m_c,0)) str+=L"x="+(c/a).ToString();
	else if (Math::Equal(m_c,0) && !Math::Equal(m_a,0) && !Math::Equal(m_b,0)) 
	{
		CString s=(-a/b).ToString();
		str+=L"y=";
		if (s=="-1") str+=L"-";
		else if (s!="1") str+=s;
		str+=L"x";
	}
	else if (!Math::Equal(m_c,0) && !Math::Equal(m_a,0) && !Math::Equal(m_b,0)) 
	{
		CString s=a.ToString();
		if (s=="-1") str+=L"-";
		else if (s!="1") str+=s;
		str+=L"x";
		s=b.ToString();
		if (s[0]!='-') str+=L"+";
		if (s=="-1") str+=L"-";
		else if (s!="1") str+=s;
		str+=L"y";
		s=c.ToString();
		if (s[0]!='-') str+=L"+";
		str+=s; str+=L"=0";
	}
	else str="Invalid";
	return str;
}

void CVGLine::GetScreenStartEndPoint(Point& pt1, Point& pt2 , CAxisInfo* m_AxisInfo)
{
	double x,y;
	bool bflag=FALSE;
	Point pt;

	if (!Math::Equal(m_b,0))
	{
		x=m_AxisInfo->GetMinX(); y=(-m_c-m_a*x)/m_b;
		pt=m_AxisInfo->ToClientPoint(x,y);
		if (pt.Y>=0 && pt.Y<=drPadSize.Height*nZoom/100)
		{ pt1=pt ;bflag=TRUE;	}
	}

	if (!Math::Equal(m_a,0))
	{
		y=m_AxisInfo->GetMinY(); x=(-m_c-m_b*y)/m_a;
		pt=m_AxisInfo->ToClientPoint(x,y);
		if (pt.X>=0 && pt.X<=drPadSize.Width*nZoom/100)
		{
			if (!bflag) { pt1=pt; bflag=TRUE; }
			else { pt2=pt; return; }
		}
	}

	if (!Math::Equal(m_b,0))
	{
		x=m_AxisInfo->GetMaxX(); y=(-m_c-m_a*x)/m_b;
		pt=m_AxisInfo->ToClientPoint(x,y);
		if (pt.Y>=0 && pt.Y<=drPadSize.Height*nZoom/100)
		{
			if (!bflag) { pt1=pt; bflag=TRUE; }
			else { pt2=pt; return; }
		}
	}

	if (!Math::Equal(m_a,0))
	{
		y=m_AxisInfo->GetMaxY(); x=(-m_c-m_b*y)/m_a;
		pt=m_AxisInfo->ToClientPoint((-m_c-m_b*y)/m_a,y);
		if (pt.X>=0 && pt.X<=drPadSize.Width*nZoom/100)
		{
			if (!bflag) { pt1=pt; bflag=TRUE; }
			else { pt2=pt; return; }
		}
	}
}

void CVGLine::Serialize( CArchive& ar, CArray<CVGObject*>* objArr )
{
	CVGLineBase::Serialize(ar,objArr);

	if (ar.IsStoring())
	{
		ar << m_a;
		ar << m_b;
		ar << m_c;
		ar << m_bCutSuperfluousPart;
	}
	else
	{
		ar >> m_a;
		ar >> m_b;
		ar >> m_c;
		ar >> m_bCutSuperfluousPart;
	}
}

void CVGLine::OnAnimate(int Speed, CAxisInfo* m_AxisInfo)
{
	if (m_Mode==LINE_MODE_THROUGH_TWO_POINTS && (m_Param[1])->m_Type==OBJ_POINT_HIDDEN_CHILD)
	{
		CVGPoint* pt1=(CVGPoint*)m_Param[0];
		CVGPoint* pt2=(CVGPoint*)m_Param[1];
		Math::GetRotatePoint(pt2->m_x,pt2->m_y,pt1->m_x,pt2->m_x,180/(20*Speed),pt2->m_x,pt2->m_y);
	}
}

void CVGLine::Clone( CVGObject* obj )
{
	CVGLineBase::Clone(obj);
	CVGLine* l=(CVGLine*)obj;
	m_a=l->m_a; m_b=l->m_b; m_c=l->m_c;
	m_bDirection=l->m_bDirection;
	m_bCutSuperfluousPart=l->m_bCutSuperfluousPart;
}