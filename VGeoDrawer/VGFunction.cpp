#include "StdAfx.h"
#include "VGObject.h"

CVGFunction::CVGFunction(void)
{
	m_Type=OBJ_FUNCTION;
	m_PixelVl=NULL;
	m_bContinuous=NULL;
	m_PenWidth=1;
}

CVGFunction::~CVGFunction(void)
{

}

bool CVGFunction::IsRelated(CVGObject* obj)
{
	return m_expFunc.IsRelated(obj);
}

void CVGFunction::SetFunction(CString strFunc, CArray<CVGObject*>*m_Array,CStringArray* m_strReplacedNameArr)
{
	m_strFunction=strFunc;
	m_expFunc.SetExpression(strFunc,m_Array,true,m_strReplacedNameArr);
}

void CVGFunction::Calc(CAxisInfo* m_AxisInfo)
{
	if (!CheckCanCalc()) return;
	FuncCalc(m_AxisInfo);
	m_bCalc=TRUE;
}

void CVGFunction::FuncCalc(CAxisInfo* m_AxisInfo)
{
	if (m_PixelVl!=NULL) delete[] m_PixelVl;
	if (m_bContinuous!=NULL) delete[] m_bContinuous;
	m_PixelVl=new int[drPadSize.Width];
	m_bContinuous=new bool[drPadSize.Width];
	for (int i=0;i<drPadSize.Width;i++)
	{
		double x=m_AxisInfo->ToAxisX(i);
		m_expFunc.Calc(x,TRUE);
		double y=m_expFunc.GetDoubleValue();
		m_PixelVl[i]=m_AxisInfo->ToClientY(y);
		if (i>0) 
			if ((m_PixelVl[i]<0 || m_PixelVl[i]>drPadSize.Height)
				&& (m_PixelVl[i-1]<0 || m_PixelVl[i-1]>drPadSize.Height))
				m_bContinuous[i]=FALSE;
			else m_bContinuous[i]=TRUE;
	}
}

void CVGFunction::Draw(Graphics* gr, CAxisInfo* m_AxisInfo, bool bTrace/* =FALSE */)
{
	if (!m_bVisible || m_bTemp || !m_bAvailable) return;
	m_bCalc=FALSE;
	Point pt, prevPt;
	Pen pen(m_Color,m_HighLight || m_Select ? (REAL)m_PenWidth+1 : (REAL)m_PenWidth);
	//pen.SetDashStyle(DashStyleDot);

	for (int i=0;i<drPadSize.Width;i++)
	{
		if (i==0)
		{
			prevPt.X=i*nZoom/100;
			prevPt.Y=m_PixelVl[i]*nZoom/100;
		}
		else
		{
			pt.X=i*nZoom/100;
			pt.Y=m_PixelVl[i]*nZoom/100;
			if (m_bContinuous[i])
				gr->DrawLine(&pen,prevPt,pt);
			prevPt=pt;
		}
	}
}

bool CVGFunction::CheckMouseOver(Point point, CAxisInfo* m_AxisInfo)
{
	if (!IsVisible()) return FALSE;

	return (m_PixelVl[int(point.X*100/nZoom)]*nZoom/100>point.Y-5 && m_PixelVl[int(point.X*100/nZoom)]*nZoom/100<point.Y+5);
}

void CVGFunction::Clone(CVGObject* obj)
{

}