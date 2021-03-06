#include "StdAfx.h"
#include "VGObject.h"
#include "VGMath.h"
#include "AxisInfo.h"

CVGPoint::CVGPoint(void)
{
	m_Size=3;
	m_Coordinates=POINT_COORDINATES_CARTESIAN;
	m_x=0;m_y=0;
	m_bShowPointers=FALSE;
	m_bDirection=0;
}

CVGPoint::~CVGPoint(void)
{
}

void CVGPoint::Clone( CVGObject* obj )
{
	CVGGeoObject::Clone(obj);
	
	CVGPoint* pt=(CVGPoint*)obj;
	m_moved=pt->m_moved;
	m_Size=pt->m_Size;
	m_x=pt->m_x;
	m_y=pt->m_y;
	m_Coordinates=pt->m_Coordinates;
	m_objMove.RemoveAll();
	for (int i=0;i<pt->m_objMove.GetSize();i++) m_objMove.Add(pt->m_objMove[i]);
	tmp1=pt->tmp1;
	tmp2=pt->tmp2;
	m_bShowPointers=pt->m_bShowPointers;
	m_bDirection=pt->m_bDirection;
}

void CVGPoint::Draw(Graphics* gr, CAxisInfo* m_AxisInfo, bool bTrace)
{
	int size=Math::Round((double)(m_Size+1)*nZoom/100); if (size==0) size=1;
	double d=m_AxisInfo->ToAxisLength(size+1);
	Point pos=m_AxisInfo->ToClientPoint(m_x-d,m_y+d);
	Rect rect(pos.X,pos.Y,2*size+2,2*size+1);

	if ((m_Select || m_HighLight) && !bTrace)
	{
		Pen pen(m_Color,1);
		gr->DrawEllipse(&pen,rect);
	}
	if (!bTrace && m_bShowPointers)
	{
		Pen pen(Color(128,128,128),1);
		float f[2]={1,2}; pen.SetDashPattern(f,2);
		Point pt=m_AxisInfo->ToClientPoint(m_x,m_y);
		Point pt1=m_AxisInfo->ToClientPoint(0,m_y);
		Point pt2=m_AxisInfo->ToClientPoint(m_x,0);
		gr->DrawLine(&pen,pt1,pt);
		gr->DrawLine(&pen,pt2,pt);
	}
	
	SolidBrush br(m_Color);
	rect.Inflate(-1,-1);
	if (!bTrace) gr->FillEllipse(&br,rect);
	else gr->FillEllipse(&br,rect);
	//AfxGetApp()->DoMessageBox(Math::IntToString(m_Size)+'\n'+Math::IntToString(pos.X)+'\n'+Math::IntToString(pos.Y),MB_OK,-1);
	
	CVGGeoObject::Draw(gr,m_AxisInfo,bTrace);
}

bool CVGPoint::CheckMouseOver(Point point, CAxisInfo* m_AxisInfo)
{
	if (!m_bVisible || !m_bAvailable || m_bTemp) return FALSE;
	Point pos=m_AxisInfo->ToClientPoint(m_x,m_y);
	if (PtInRect(CRect(pos.X-m_Size-POINT_CHECK_MOUSEOVER_DISTANCE,
		pos.Y-m_Size-POINT_CHECK_MOUSEOVER_DISTANCE,
		pos.X+m_Size+POINT_CHECK_MOUSEOVER_DISTANCE,
		pos.Y+m_Size+POINT_CHECK_MOUSEOVER_DISTANCE),CPoint(point.X,point.Y)))
		return TRUE;
	return FALSE;
}

//Chuyen toa do tren mat phang ve toa do tren man hinh

void CVGPoint::Move(int x,int y, CPoint startPt, CAxisInfo* m_AxisInfo)
{
	if (m_bFixed) return;
	if (m_Mode==POINT_MODE_NUM_NUM || m_Mode==POINT_MODE_NUM_EXP || m_Mode==POINT_MODE_EXP_NUM && !m_moved)
	{
		double tl=m_AxisInfo->Pixel/m_AxisInfo->Unit;
		if (m_Mode==POINT_MODE_NUM_NUM || m_Mode==POINT_MODE_NUM_EXP) m_x+=(double)x/tl;
		if (m_Mode==POINT_MODE_NUM_NUM || m_Mode==POINT_MODE_EXP_NUM) m_y-=(double)y/tl;
		for (int i=0;i<m_objMove.GetSize();i++)
			m_objMove[i]->Move(x,y,startPt,m_AxisInfo);
		m_moved=TRUE;
	}
	else if ((m_Mode==POINT_MODE_CHILD_LINE && !m_moved) || m_Mode==POINT_MODE_CHILD_CIRCLE || m_Mode==POINT_MODE_CHILD_FUNCTION || m_Mode==POINT_MODE_CHILD_CONIC)
	{
		Point pt1; if (m_objMove.GetSize()!=0) pt1=m_AxisInfo->ToClientPoint(m_x,m_y);
		PointD mouse=m_AxisInfo->ToAxisPoint(Point(x+startPt.x,y+startPt.y));
		m_x=mouse.X;
		m_y=mouse.Y;
		CorrectPointPos(TRUE);
		
		Point pt2; if (m_objMove.GetSize()!=0) pt2=m_AxisInfo->ToClientPoint(m_x,m_y);
		for (int i=0;i<m_objMove.GetSize();i++)
			m_objMove[i]->Move(pt2.X-pt1.X,pt2.Y-pt1.Y,startPt,m_AxisInfo);
	}
}

// Sử dụng cho điểm đặt chế độ HiddenChild
void CVGPoint::MoveTo(int x, int y, CAxisInfo* m_AxisInfo)
{
	if (m_Type==OBJ_POINT_HIDDEN_CHILD && !m_moved)
	{
		PointD pt=m_AxisInfo->ToAxisPoint(Point(x,y));
		m_x=pt.X;
		m_y=pt.Y;
		m_moved=TRUE;
	}
}

void CVGPoint::CaclFr(CValueFractionEx& x, CValueFractionEx& y)
{
	switch (m_Mode)
	{
	case POINT_MODE_NUM_NUM:
		{
			x.FromNumber(m_x);
			y.FromNumber(m_y);
			break;
		}
	case POINT_MODE_EXP_EXP:
		{
			CVGExpression* n1=(CVGExpression*)m_Param[0];
			CVGExpression* n2=(CVGExpression*)m_Param[1];
			CValue* vl;
			vl=n1->m_Exp.CalcFr();
			x=*(CValueFractionEx*)vl; delete vl;
			vl=n2->m_Exp.CalcFr();
			y=*(CValueFractionEx*)vl; delete vl;
			break;
		}
	case POINT_MODE_INTERSECTION_LINE_LINE:
		{
			CVGLineBase* l1=(CVGLineBase*)m_Param[0];
			CVGLineBase* l2=(CVGLineBase*)m_Param[1];
			CValueFractionEx a1,b1,c1,a2,b2,c2;
			l1->GetABCFr(a1,b1,c1);
			l2->GetABCFr(a2,b2,c2);
			Math::GetIntersectionLineLineFr(a1,b1,c1,a2,b2,c2,x,y);
			break;
		}
		
	/*
	case POINT_MODE_EXP_NUM:
			{
				CVGExpression* n=(CVGExpression*)m_Param[0];
				m_x=n->GetDoubleValue();
				break;
			}
		case POINT_MODE_NUM_EXP:
			{
				CVGExpression* n=(CVGExpression*)m_Param[0];
				m_y=n->GetDoubleValue();
				break;
			}
		case POINT_MODE_DIVIDED:
			{
				CVGPoint* pt1=(CVGPoint*)m_Param[0];
				CVGPoint* pt2=(CVGPoint*)m_Param[1];
				Math::GetPointByRatio(pt1->m_x,pt1->m_y,pt2->m_x,pt2->m_y,((CVGExpression*)m_Param[2])->GetDoubleValue(),m_x,m_y);
				break;
			}
		case POINT_MODE_ON_LINE_WITH_DISTANCE_TO_OTHER_POINT:
			{
				CVGLineBase* l=(CVGLineBase*)m_Param[0];
				double a,b,c; l->GetABC(a,b,c);
				CVGPoint* pt=(CVGPoint*)m_Param[1];
				double k=((CVGExpression*)m_Param[2])->GetDoubleValue();
				Math::PointOnLineDS(pt->m_x,pt->m_y,a,b,c,k,m_x,m_y);
				break;
			}*/
		case POINT_MODE_INTERSECTION_LINE_CIRCLE:
			{
				CVGLineBase* l=(CVGLine*)m_Param[0];
				CVGCircle* cc=(CVGCircle*)m_Param[1];
				CValueFractionEx x1,x2,y1,y2;
				CValueFractionEx a,b,c;
				l->GetABCFr(a,b,c);
				CValueFractionEx centerX,centerY,radius;
				cc->CalcFr(centerX,centerY,radius);
				int numRoot=Math::GetIntersectionLineCircleFr(centerX,centerY,radius,a,b,c,x1,y1,x2,y2);
	
				if (x1.ToDouble()==m_x) { x=x1; y=y1; }
				else { x=x2; y=y2; }
				break;
			}
		case POINT_MODE_INTERSECTION_CIRCLE_CIRCLE:
			{
				CVGCircle* c1=(CVGCircle*)m_Param[0];
				CVGCircle* c2=(CVGCircle*)m_Param[1];
				CValueFractionEx cX1,cY1,r1; c1->CalcFr(cX1,cY1,r1);
				CValueFractionEx cX2,cY2,r2; c2->CalcFr(cX2,cY2,r2);
				CValueFractionEx x1,x2,y1,y2;
				int numRoot=Math::GetIntersectionCircleCircleFr(cX1,cY1,r1,
					cX2,cY2,r2,x1,y1,x2,y2);
	
				if (x1.ToDouble()==m_x) { x=x1; y=y1; }
				else { x=x2; y=y2; }
				break;
			}
		case POINT_MODE_INTERSECTION_LINE_CONIC:
			{
				CVGLineBase* l=(CVGLine*)m_Param[0];
				CVGConic* conic=(CVGConic*)m_Param[1];
				CValueFractionEx x1,x2,y1,y2;
				CValueFractionEx la,lb,lc;
				l->GetABCFr(la,lb,lc);
				CValueFractionEx a,b,c,d,e,f;
				conic->CalcFr(a,b,c,d,e,f);
				int numRoot=Math::GetIntersectionLineConicFr(a,b,c,d,e,f,la,lb,lc,x1,y1,x2,y2);

				if (Math::Equal(x1.ToDouble(),m_x)) { x=x1; y=y1; }
				else { x=x2; y=y2; }
				break;
			}
		case POINT_MODE_MIDPOINT:
			{
				CVGPoint* pt1=(CVGPoint*)m_Param[0];
				CVGPoint* pt2=(CVGPoint*)m_Param[1];
				CValueFractionEx x1,y1; pt1->CaclFr(x1,y1);
				CValueFractionEx x2,y2; pt2->CaclFr(x2,y2);
				x = ( x1 + x2 ) / 2;
				y = ( y1 + y2 ) / 2;
				break;
			}
		case POINT_MODE_CENTROID:
			{
				CVGPoint* pt;
				CValueFractionEx x,y; x.FromNumber(0); y.FromNumber(0);
				for (int i=0;i<m_Param.GetSize();i++)
				{
					pt=(CVGPoint*)m_Param[i];
					CValueFractionEx ptX,ptY; pt->CaclFr(ptX,ptY);

					x = x+ptX;
					y = y+ptY;
				}
				x=x/m_Param.GetSize();
				y=y/m_Param.GetSize();
				break;
			}
		case POINT_MODE_CENTER:
			{
				CVGCircle* c=(CVGCircle*)m_Param[0];
				CValueFractionEx r;
				c->CalcFr(x,y,r);
				break;
			}
		case GEO_MODE_TRANSLATE:
			{
				CVGPoint* pt=(CVGPoint*)m_Param[0];
				CValueFractionEx xp,yp; pt->CaclFr(xp,yp);
				CVGLine* l=(CVGLine*)m_Param[1];
				if (l->m_Type=OBJ_VECTOR)
				{
					CVGPoint* pt1=(CVGPoint*)l->m_Param[0];
					CVGPoint* pt2=(CVGPoint*)l->m_Param[1];
					CValueFractionEx x1,y1; pt1->CaclFr(x1,y1);
					CValueFractionEx x2,y2; pt2->CaclFr(x2,y2);
					x=xp+(x2-x1);
					y=yp+(y2-y1);
				}
				break;
			}
		case GEO_MODE_REFLECT_POINT:
			{
				CVGPoint* pt1=(CVGPoint*)m_Param[0];
				CVGPoint* pt2=(CVGPoint*)m_Param[1];
				CValueFractionEx x1,y1; pt1->CaclFr(x1,y1);
				CValueFractionEx x2,y2; pt2->CaclFr(x2,y2);
				Math::GetReflectPointFr(x1,y1,x2,y2,x,y);
				break;
			}
		case GEO_MODE_REFLECT_LINE:
			{
				CVGPoint* pt=(CVGPoint*)m_Param[0];
				CValueFractionEx xp,yp; pt->CaclFr(xp,yp);
				CVGLine* l=(CVGLine*)m_Param[1];
				CValueFractionEx A,B,C;
				l->GetABCFr(A,B,C);
				Math::GetReflectPointFr(xp,yp,A,B,C,x,y);
// 				Math::GetIntersectionLineLineFr(A,B,C,-B,A,B * xp - A * yp,x,y);
// 				m_x=2*m_x-pt->m_x;
// 				m_y=2*m_y-pt->m_y;
				break;
			}
		case GEO_MODE_DILATE:
			{
				CVGPoint* pt=(CVGPoint*)m_Param[0];
				CValueFractionEx xp,yp; pt->CaclFr(xp,yp);
				CVGPoint* ptCenter=(CVGPoint*)m_Param[1];
				CValueFractionEx xc,yc; ptCenter->CaclFr(xc,yc);
				CValue* vl=((CVGExpression*)m_Param[2])->m_Exp.CalcFr();
				CValueFractionEx k=*(CValueFractionEx*)vl; delete vl;
				Math::GetDilatePointFr(xp,yp,xc,yc,k,x,y);
				break;
			}
		case GEO_MODE_INVERSE:
			{
				CVGPoint* pt=(CVGPoint*)m_Param[0];
				CValueFractionEx xp,yp; pt->CaclFr(xp,yp);
				CVGPoint* ptCenter=(CVGPoint*)m_Param[1];
				CValueFractionEx xc,yc; ptCenter->CaclFr(xc,yc);
				CValue* vl=((CVGExpression*)m_Param[2])->m_Exp.CalcFr();
				CValueFractionEx k=*(CValueFractionEx*)vl; delete vl;
				CValueFractionEx a,b,c;
				Math::LineThroughTwoPointsFr(xp,yp,xc,yc,a,b,c);
				Math::PointOnLineDSFr(xp,yp,a,b,c,k/Math::GetDistanceDSFr(xc,yc,xp,yp),x,y);
				break;
			}
		case GEO_MODE_ROTATO:
			{
				CVGPoint* pt=(CVGPoint*)m_Param[0];
				CValueFractionEx xp,yp; pt->CaclFr(xp,yp);
				CVGPoint* ptCenter=(CVGPoint*)m_Param[1];
				CValueFractionEx xc,yc; ptCenter->CaclFr(xc,yc);
				CValue* vl=((CVGExpression*)m_Param[2])->m_Exp.CalcFr();
				CValueFractionEx k=*(CValueFractionEx*)vl; delete vl;
				Math::GetRotatePointFr(xp,yp,xc,yc,k,x,y);
				break;
			}
		}
	
}

void CVGPoint::Calc(CAxisInfo* m_AxisInfo)
{
	if (!CheckCanCalc()) return;
	bool bAvail=TRUE; // Điểm hợp lệ ?
	switch (m_Mode)
	{
	case POINT_MODE_EXP_EXP:
		{
			CVGExpression* n1=(CVGExpression*)m_Param[0];
			CVGExpression* n2=(CVGExpression*)m_Param[1];
			m_x=n1->GetDoubleValue();
			m_y=n2->GetDoubleValue();
			break;
		}
	case POINT_MODE_EXP_NUM:
		{
			CVGExpression* n=(CVGExpression*)m_Param[0];
			m_x=n->GetDoubleValue();
			break;
		}
	case POINT_MODE_NUM_EXP:
		{
			CVGExpression* n=(CVGExpression*)m_Param[0];
			m_y=n->GetDoubleValue();
			break;
		}
	case POINT_MODE_CHILD_LINE:
	case POINT_MODE_CHILD_CONIC:
	case POINT_MODE_CHILD_FUNCTION:
	case POINT_MODE_CHILD_CIRCLE:
		{
			CorrectPointPos(FALSE);
			break;
		}
	case POINT_MODE_DIVIDED:
		{
			CVGPoint* pt1=(CVGPoint*)m_Param[0];
			CVGPoint* pt2=(CVGPoint*)m_Param[1];
			Math::GetPointByRatio(pt1->m_x,pt1->m_y,pt2->m_x,pt2->m_y,((CVGExpression*)m_Param[2])->GetDoubleValue(),m_x,m_y);
			break;
		}
	case POINT_MODE_ON_LINE_WITH_DISTANCE_TO_OTHER_POINT:
		{
			CVGLineBase* l=(CVGLineBase*)m_Param[0];
			double a,b,c; l->GetABC(a,b,c);
			CVGPoint* pt=(CVGPoint*)m_Param[1];
			double k=((CVGExpression*)m_Param[2])->GetDoubleValue();
			Math::PointOnLineDS(pt->m_x,pt->m_y,a,b,c,k,m_x,m_y);
			break;
		}
	case POINT_MODE_INTERSECTION_LINE_LINE:
		{
			CVGLineBase* l1=(CVGLineBase*)m_Param[0];
			CVGLineBase* l2=(CVGLineBase*)m_Param[1];
			double a1,b1,c1,a2,b2,c2;
			l1->GetABC(a1,b1,c1);
			l2->GetABC(a2,b2,c2);
			bool b=Math::GetIntersectionLineLine(a1,b1,c1,a2,b2,c2,m_x,m_y);
			if (!b || !l1->CheckPtIn(m_x,m_y) || !l2->CheckPtIn(m_x,m_y)) bAvail=FALSE;
			break;
		}
	case POINT_MODE_INTERSECTION_LINE_CIRCLE:
		{
			CVGLineBase* l=(CVGLine*)m_Param[0];
			CVGCircle* cc=(CVGCircle*)m_Param[1];
			double x1,x2,y1,y2;
			double a,b,c;
			l->GetABC(a,b,c);
			int numRoot=Math::GetIntersectionLineCircle(cc->m_CenterX,cc->m_CenterY,cc->m_Radius,a,b,c,x1,y1,x2,y2);
			
			if (numRoot==0)
			{
				bAvail=FALSE;
			}
			else
			{
				if ((l->m_Param[0])->m_Type==OBJ_POINT)
				{
					CVGPoint* pt=(CVGPoint*)l->m_Param[0];
					if (Math::Equal(Math::GetDistance(pt->m_x,pt->m_y,cc->m_CenterX,cc->m_CenterY),cc->m_Radius))
					{	// Đường thẳng đi qua một điểm và điểm đó nằm trên đường tròn -> lấy giao điểm còn lại
						if (Math::Equal(x1,pt->m_x) && Math::Equal(y1,pt->m_y))
						{
							m_x=x2;m_y=y2;
						}
						else {m_x=x1;m_y=y1;}
						break;
					}
					else if ((l->m_Param[1])->m_Type==OBJ_POINT)
					{
						CVGPoint* pt=(CVGPoint*)l->m_Param[1];
						if (Math::Equal(Math::GetDistance(pt->m_x,pt->m_y,cc->m_CenterX,cc->m_CenterY),cc->m_Radius))
						{
							if (Math::Equal(x1,pt->m_x) && Math::Equal(y1,pt->m_y))
							{
								m_x=x2;m_y=y2;
							}
							else {m_x=x1;m_y=y1;}
							break;
						}
					}
				}

				if (tmp1!=1 && tmp1!=2)
				{
					if (abs(x1-m_x)>abs(x2-m_x))
					{ m_x=x2; m_y=y2; tmp1=2; }
					else if (abs(x1-m_x)<abs(x2-m_x))
					{ m_x=x1; m_y=y1; tmp1=1; }
					tmp2=b>0;//(b>0 && a>0) || (b<0 && a<0);
				}
				else
				{
					double temp=b>0;//(b>0 && a>0) || (b<0 && a<0);
					if (temp!=tmp2 && !Math::Equal(b,0) /*&& abs(a)>abs(b)*/ /* sửa */)
					{
						tmp1=(tmp1==1) ? 2 : 1;
						tmp2=temp;
						//AfxGetApp()->DoMessageBox(L"...",MB_OK,-1);
					}
					if (tmp1==1) { m_x=x1; m_y=y1; }
					else if (tmp1==2) { m_x=x2; m_y=y2; }
				}
				if (!l->CheckPtIn(m_x,m_y)) bAvail=FALSE;
			}
			break;
		}
	case POINT_MODE_INTERSECTION_CIRCLE_CIRCLE:
		{
			CVGCircle* c1=(CVGCircle*)m_Param[0];
			CVGCircle* c2=(CVGCircle*)m_Param[1];
			double x1,x2,y1,y2;
			int numRoot=Math::GetIntersectionCircleCircle(c1->m_CenterX,c1->m_CenterY,c1->m_Radius,
				c2->m_CenterX,c2->m_CenterY,c2->m_Radius,x1,y1,x2,y2);

			if (numRoot==0)
			{
				bAvail=FALSE;
			}
			else
			{
				if (c2->m_Mode==CIRCLE_MODE_POINT_POINT)
				{
					CVGPoint* pt=(CVGPoint*)c2->m_Param[1];
					if (Math::Equal(Math::GetDistance(pt->m_x,pt->m_y,c1->m_CenterX,c1->m_CenterY),c1->m_Radius))
					{
						if (Math::Equal(x1,pt->m_x) && Math::Equal(y1,pt->m_y))
						{
							m_x=x2;m_y=y2;
						}
						else {m_x=x1;m_y=y1;}
						break;
					}
					else if (c1->m_Mode==CIRCLE_MODE_POINT_POINT)
					{
						CVGPoint* pt=(CVGPoint*)c1->m_Param[1];
						if (Math::Equal(Math::GetDistance(pt->m_x,pt->m_y,c2->m_CenterX,c2->m_CenterY),c2->m_Radius))
						{
							if (Math::Equal(x1,pt->m_x) && Math::Equal(y1,pt->m_y))
							{
								m_x=x2;m_y=y2;
							}
							else {m_x=x1;m_y=y1;}
							break;
						}
					}
				}

				// tmp1 : Thứ tự giao điểm
				// tmp2 : dấu của hệ số góc đường thẳng
				if (!m_bAvailable)
				{
					m_bAvailable=TRUE;
				}

				if (tmp1!=1 && tmp1!=2)
				{
					if (abs(x1-m_x)>abs(x2-m_x))
					{ m_x=x2; m_y=y2; tmp1=2; }
					else if (abs(x1-m_x)<abs(x2-m_x))
					{ m_x=x1; m_y=y1; tmp1=1; }
					tmp2=(c1->m_CenterY>c2->m_CenterY);
				}
				else
				{
					double temp=(c1->m_CenterY>c2->m_CenterY);
					if (temp!=tmp2 && !Math::Equal(c1->m_CenterY,c2->m_CenterY)/*&& !Math::Equal(l->m_b,0) && abs(l->m_a)>abs(l->m_b)*/)
 					{
 						tmp1=(tmp1==1) ? 2 : 1;
 						tmp2=temp;
 					}
					if (tmp1==1) { m_x=x1; m_y=y1; }
					else if (tmp1==2) { m_x=x2; m_y=y2; }
				}
			}

// 			if (*(int*)m_Param[2]==1) { m_x=x1; m_y=y1; }
// 			else { m_x=x2; m_y=y2; }
			break;
		}
	case POINT_MODE_INTERSECTION_LINE_CONIC:
		{
			CVGLineBase* l=(CVGLine*)m_Param[0];
			CVGConic* el=(CVGConic*)m_Param[1];
			double x1,x2,y1,y2;
			double a,b,c;
			l->GetABC(a,b,c);
			int numRoot=Math::GetIntersectionLineConic(el->m_a,el->m_b,el->m_c,el->m_d,el->m_e,el->m_f,a,b,c,x1,y1,x2,y2);

			if (numRoot==0)
			{
				bAvail=FALSE;
			}
			else
			{
				if (tmp1!=1 && tmp1!=2)
				{
					if (abs(x1-m_x)>abs(x2-m_x)) { m_x=x2; m_y=y2; tmp1=2; }
					else if (abs(x1-m_x)<abs(x2-m_x)) { m_x=x1; m_y=y1; tmp1=1; }
				}
				if (tmp1==1) { m_x=x1; m_y=y1; }
				else { m_x=x2; m_y=y2; }
			}
			break;
		}
	case POINT_MODE_MIDPOINT:
		{
			CVGPoint* pt1=(CVGPoint*)m_Param[0];
			CVGPoint* pt2=(CVGPoint*)m_Param[1];
			m_x = ( pt1->m_x + pt2->m_x ) / 2;
			m_y = ( pt1->m_y + pt2->m_y ) / 2;
			break;
		}
	case POINT_MODE_CENTROID:
		{
			m_x=m_y=0;
			CVGPoint* pt;
			for (int i=0;i<m_Param.GetSize();i++)
			{
				pt=(CVGPoint*)m_Param[i];
				m_x += pt->m_x;
				m_y += pt->m_y;
			}
			m_x/=m_Param.GetSize();
			m_y/=m_Param.GetSize();
			break;
		}
	case POINT_MODE_CENTER:
		{
			CVGCircle* c=(CVGCircle*)m_Param[0];
			m_x=c->m_CenterX; m_y=c->m_CenterY;
			break;
		}
	case GEO_MODE_TRANSLATE:
		{
			CVGPoint* pt=(CVGPoint*)m_Param[0];
			CVGLineBase* l=(CVGLineBase*)m_Param[1];
			if (l->m_Type=OBJ_VECTOR)
			{
				CVGPoint* pt1=(CVGPoint*)l->m_Param[0];
				CVGPoint* pt2=(CVGPoint*)l->m_Param[1];
				m_x=pt->m_x+((CVGVector*)l)->x;
				m_y=pt->m_y+((CVGVector*)l)->y;
			}
			break;
		}
	case GEO_MODE_REFLECT_POINT:
		{
			CVGPoint* pt1=(CVGPoint*)m_Param[0];
			CVGPoint* pt2=(CVGPoint*)m_Param[1];
			Math::GetReflectPoint(pt1->m_x,pt1->m_y,pt2->m_x,pt2->m_y,m_x,m_y);
			break;
		}
	case GEO_MODE_REFLECT_LINE:
		{
			CVGPoint* pt=(CVGPoint*)m_Param[0];
			CVGLine* l=(CVGLine*)m_Param[1];
			double A,B,C;
			l->GetABC(A,B,C);
			Math::GetReflectPoint(pt->m_x,pt->m_y,A,B,C,m_x,m_y);
			Math::GetIntersectionLineLine(A,B,C,-B,A,B * pt->m_x - A * pt->m_y,m_x,m_y);
			m_x=2*m_x-pt->m_x;
			m_y=2*m_y-pt->m_y;
			break;
		}
	case GEO_MODE_DILATE:
		{
			CVGPoint* pt=(CVGPoint*)m_Param[0];
			CVGPoint* ptCenter=(CVGPoint*)m_Param[1];
			double k=((CVGExpression*)m_Param[2])->GetDoubleValue();
			Math::GetDilatePoint(pt->m_x,pt->m_y,ptCenter->m_x,ptCenter->m_y,k,m_x,m_y);
			break;
		}
	case GEO_MODE_INVERSE:
		{
			CVGPoint* pt=(CVGPoint*)m_Param[0];
			CVGPoint* ptCenter=(CVGPoint*)m_Param[1];
			double k=((CVGExpression*)m_Param[2])->GetDoubleValue();
			double a,b,c;
			Math::LineThroughTwoPoints(pt->m_x,pt->m_y,ptCenter->m_x,ptCenter->m_y,a,b,c);
			Math::PointOnLineDS(ptCenter->m_x,ptCenter->m_y,a,b,c,k/Math::GetDistanceDS(ptCenter->m_x, ptCenter->m_y, pt->m_x,pt->m_y),m_x,m_y);
			break;
		}
	case GEO_MODE_ROTATO:
		{
			CVGPoint* pt=(CVGPoint*)m_Param[0];
			CVGPoint* ptCenter=(CVGPoint*)m_Param[1];
			double k=((CVGExpression*)m_Param[2])->GetDoubleValue();
			Math::GetRotatePoint(pt->m_x,pt->m_y,ptCenter->m_x,ptCenter->m_y,k,m_x,m_y);
			break;
		}
	}
	
	if (bAvail!=m_bAvailable)
	{
		m_bAvailable=bAvail;
	}
}

void CVGPoint::SetIntersectionLineCircle(CVGLineBase *l, CVGCircle *c, int num)
{
	m_Mode=POINT_MODE_INTERSECTION_LINE_CIRCLE;
	m_Param.RemoveAll();

	m_Param.Add(l);
	m_Param.Add(c);
	tmp1=num;
}

void CVGPoint::SetIntersectionCircleCircle(CVGCircle *c1, CVGCircle *c2, int num)
{
	m_Mode=POINT_MODE_INTERSECTION_CIRCLE_CIRCLE;
	m_Param.RemoveAll();

	m_Param.Add(c1);
	m_Param.Add(c2);
	tmp1=num;
}

void CVGPoint::SetIntersectionLineEllipse(CVGLineBase *l, CVGConic *el, int num)
{
	m_Mode=POINT_MODE_INTERSECTION_LINE_CONIC;
	m_Param.RemoveAll();

	m_Param.Add(l);
	m_Param.Add(el);
	tmp1=num;
}

void CVGPoint::CorrectPointPos(bool bParamChange /* TRUE nếu phải tính lại do di chuyển điểm. FALSE nếu phải tính lại do thay đổi các đt khác */) // Điều chỉnh vị trí khi ở chế độ Child
{
	if (m_Mode==POINT_MODE_CHILD_CIRCLE)
	{
		CVGCircle* c=(CVGCircle*)m_Param[0];

		if (bParamChange)
		{
			m_x -= c->m_CenterX;
			m_y -= c->m_CenterY;
			double k = c->m_Radius / sqrt ( m_x * m_x + m_y * m_y );
			m_x = m_x * k + c->m_CenterX;
			m_y = m_y * k + c->m_CenterY;
			tmp1 = ( m_x - c->m_CenterX ) / c->m_Radius;
			tmp2 = ( m_y - c->m_CenterY ) / c->m_Radius;
		}
		else
		{
			m_x = c->m_Radius * tmp1 + c->m_CenterX;
			m_y = c->m_Radius * tmp2 + c->m_CenterY;
		}
	}

	else if (m_Mode==POINT_MODE_CHILD_LINE)
	{
		if (bParamChange)
		{	// Lưu thay đổi
			CVGLine* l=(CVGLine*)m_Param[0];
			double A,B,C;
			l->GetABC(A,B,C);
			Math::GetIntersectionLineLine(A,B,C,-B,A,B * m_x - A * m_y,m_x,m_y);
			if (l->m_Mode==LINE_MODE_THROUGH_TWO_POINTS && ((CVGPoint*)l->m_Param[1])->m_Type!=OBJ_POINT_HIDDEN_CHILD) 
			{
				CVGPoint* pt1=(CVGPoint*)(l->m_Param[0]);
				CVGPoint* pt2=(CVGPoint*)(l->m_Param[1]);
				tmp1=(m_x-pt1->m_x)/(m_x-pt2->m_x);
			}
			else 
			{
				CVGPoint* pt1=(CVGPoint*)(l->m_Param[0]);
				tmp1=Math::GetDistance(pt1->m_x,pt1->m_y,m_x,m_y);
// 				tmp1=pt1->m_x;
// 				tmp2=pt1->m_y;
			}
		}
		else
		{
			CVGLine* l=(CVGLine*)m_Param[0];
			//double A,B,C;
			//l->GetABC(&A,&B,&C);
			if (l->m_Mode==LINE_MODE_THROUGH_TWO_POINTS && ((CVGPoint*)l->m_Param[1])->m_Type!=OBJ_POINT_HIDDEN_CHILD)
			{
				CVGPoint* pt1=(CVGPoint*)l->m_Param[0];
				CVGPoint* pt2=(CVGPoint*)l->m_Param[1];
				Math::GetPointByRatio(pt1->m_x,pt1->m_y,pt2->m_x,pt2->m_y,tmp1,m_x,m_y);
			}
			else
			{
				CVGPoint* pt=(CVGPoint*)l->m_Param[0];
				double x1,x2,y1,y2;
				double A,B,C;
				l->GetABC(A,B,C);
				int numRoot=Math::GetIntersectionLineCircle(pt->m_x,pt->m_y,tmp1,A,B,C,x1,y1,x2,y2);
				//double y1=(-l->m_c-l->m_a*x1)/l->m_b;
				//double y2=(-l->m_c-l->m_a*x2)/l->m_b;
				if (Math::GetDistance(m_x,m_y,x1,y1)<Math::GetDistance(m_x,m_y,x2,y2)) { m_x=x1; m_y=y1; }
				else { m_x=x2; m_y=y2; }
				//m_y=(-l->m_c-l->m_a*m_x)/l->m_b;
// 				if (pt->m_x!=tmp1) 
// 				{
// 					m_x+=(pt->m_x-tmp1);
// 				}
// 				if (pt->m_y!=tmp2)
// 				{
// 					m_y+=(pt->m_y-tmp2);
// 				}
			}
			//SolveLinearEquationsTwoVariables(A,B,C,-B,A,B * m_x - A * m_y,&m_x,&m_y);
		}
	}
	else if (m_Mode==POINT_MODE_CHILD_FUNCTION)
	{
		CVGFunction* f=(CVGFunction*)m_Param[0];
		f->m_expFunc.Calc(m_x,true);
		m_y=f->m_expFunc.GetDoubleValue();
	}
	else if (m_Mode==POINT_MODE_CHILD_CONIC)
	{
		CVGConic* c=(CVGConic*)m_Param[0];
		double x1,x2,y1,y2;
		int num2=c->GetY(m_x,y1,y2);
		int num1=c->GetX(m_y,x1,x2);
		double m;
		if (num1==2) m=min(abs(m_x-x1),abs(m_x-x2));
		else if (num1==1) m=abs(m_x-x1);
		else m=1000000; // So lon

		if (num2==2) m=min(m,min(abs(m_y-y1),abs(m_y-y2)));
		else if (num2==1) m=min(m,abs(m_y-y1));
		
		if (num2>0 || num1>0)
		{
			if (m==abs(m_x-x1)) m_x=x1; 
			else if (m==abs(m_x-x2)) m_x=x2;
			else if (m==abs(m_y-y1)) m_y=y1;
			else if (m==abs(m_y-y2)) m_y=y2;
		}
	}
}

void CVGPoint::SetChild(CVGObject* obj)
{
	if (IsLineType(obj->m_Type))
	{
		m_Mode=POINT_MODE_CHILD_LINE;
		m_Param.RemoveAll();
		m_Param.Add(obj);

		if (obj->m_Mode==LINE_MODE_THROUGH_TWO_POINTS && ((CVGPoint*)obj->m_Param[1])->m_Type!=OBJ_POINT_HIDDEN_CHILD) // Tính tỉ lệ khoảng cách giữa hai điểm
		{
			CorrectPointPos(TRUE);
		}
		else
		{
			CorrectPointPos(TRUE);
		}
	}
	else if (obj->m_Type==OBJ_CIRCLE)
	{
		m_Mode=POINT_MODE_CHILD_CIRCLE;
		m_Param.RemoveAll();
		m_Param.Add((CVGCircle*)obj);
		CorrectPointPos(TRUE);
	}
	else if (obj->m_Type==OBJ_FUNCTION)
	{
		m_Mode=POINT_MODE_CHILD_FUNCTION;
		m_Param.RemoveAll();
		m_Param.Add(obj);
		CorrectPointPos(TRUE);
	}
	else if (obj->m_Type==OBJ_CONIC)
	{
		m_Mode=POINT_MODE_CHILD_CONIC;
		m_Param.RemoveAll();
		m_Param.Add(obj);
		CorrectPointPos(TRUE);
	}
}

CString CVGPoint::GetValue(bool bName)
{
	if (m_Coordinates==POINT_COORDINATES_CARTESIAN)
	{
		CString str;
		if (bName) str=m_Name;
		CValueFractionEx x,y;
		CaclFr(x,y);
		str=str+L"("+x.ToString()+L","+y.ToString()+L")";
		return str;
	}
	else if (m_Coordinates==POINT_COORDINATES_POLAR)
	{

	}
	else if (m_Coordinates==POINT_COORDINATES_COMPLEX)
	{
		CString str;
		if (bName) str=m_Name+L" = ";
		str=str+Math::DoubleToString(m_x)+L"+"+Math::DoubleToString(m_y)+L"i";
		return str;
	}
	return NULL;
}

bool CVGPoint::CheckObjInRect(Rect rect,CAxisInfo* m_AxisInfo)
{
	Point pt=m_AxisInfo->ToClientPoint(m_x,m_y);
	if (rect.Contains(pt)) return TRUE;
	return FALSE;
}

// Dùng trong chế độ chuyển động
void CVGPoint::OnAnimate(int Speed, CAxisInfo* m_AxisInfo)
{
	Point ptBegin=m_AxisInfo->ToClientPoint(m_x,m_y);
	if (m_Mode==POINT_MODE_CHILD_LINE)
	{
		CVGLineBase* line=(CVGLineBase*)m_Param[0];
		double a,b,c;
		line->GetABC(a,b,c);
		double d=m_AxisInfo->ToAxisLength(Speed);

		double x1,y1,x2,y2;
		Math::GetIntersectionLineCircle(m_x,m_y,d,a,b,c,x1,y1,x2,y2);
		if (m_bDirection) { m_x=x1; m_y=y1; }
		else { m_x=x2; m_y=y2; }

		CorrectPointPos(TRUE);
		
		Point pt1,pt2;
		line->GetScreenStartEndPoint(pt1,pt2,m_AxisInfo);
		Point pt=m_AxisInfo->ToClientPoint(m_x,m_y);
		if (!((pt1.X<pt.X && pt.X<pt2.X) || (pt1.X>pt.X && pt.X>pt2.X))) m_bDirection=!m_bDirection;
	}
	else if (m_Mode==POINT_MODE_CHILD_CIRCLE)
	{
		CVGCircle* c=(CVGCircle*)m_Param[0];
		double d=m_AxisInfo->ToAxisLength(Speed);
		double x1,y1,x2,y2;
		Math::GetIntersectionCircleCircle(c->m_CenterX,c->m_CenterY,c->m_Radius,m_x,m_y,d,x1,y1,x2,y2);
		if (m_bDirection) 
		{
			if (m_y<c->m_CenterY) { m_x=x1; m_y=y1; }
			else { m_x=x2; m_y=y2; }
		}
		else 
		{
			if (m_y<c->m_CenterY) { m_x=x2; m_y=y2; }
			else { m_x=x1; m_y=y1; }
		}
		CorrectPointPos(TRUE);
	}
	else if (m_Mode==POINT_MODE_NUM_NUM)
	{
		double d=m_AxisInfo->ToAxisLength(Speed);
		static int prevMove=0;
		int r=rand()%3;
		if (r>=prevMove) r++;
		if (r==4) r=0;

		if (r==0) { m_x+=d; prevMove=1; }
		else if (r==1) { m_x-=d; prevMove=0; }
		else if (r==2) { m_y+=d; prevMove=3; }
		else if (r==3) { m_y-=d; prevMove=2; }
	}
	Point ptEnd=m_AxisInfo->ToClientPoint(m_x,m_y);
	for (int i=0;i<m_objMove.GetSize();i++)
	{
		m_objMove[i]->m_moved=FALSE;
		m_objMove[i]->Move(ptEnd.X-ptBegin.X,ptEnd.Y-ptBegin.Y,CPoint(ptBegin.X,ptBegin.Y),m_AxisInfo);
	}
}

void CVGPoint::Serialize( CArchive& ar, CArray<CVGObject*>* objArr )
{
	CVGGeoObject::Serialize(ar,objArr);

	if (ar.IsStoring())
	{
		ar << m_bDirection;
		ar << m_bShowPointers;
		ar << m_Coordinates;
		ar << m_Size;
		ar << m_x;
		ar << m_y;
		ar << tmp1;
		ar << tmp2;

		ar << (int)m_objMove.GetSize();

		for (int i=0;i<m_objMove.GetSize();i++)
		{
			for (int j=0;j<objArr->GetSize();j++)
			{
				if (m_objMove[i]==objArr->GetAt(j)) ar << j;
			}
		}
	}
	else
	{
		ar >> m_bDirection;
		ar >> m_bShowPointers;
		ar >> m_Coordinates;
		ar >> m_Size;
		ar >> m_x;
		ar >> m_y;
		ar >> tmp1;
		ar >> tmp2;

		int count;
		ar >> count;
		for (int i=0;i<count;i++)
		{
			int index;
			ar >> index;
			m_objMove.Add((CVGPoint*)index);
		}
	}
}