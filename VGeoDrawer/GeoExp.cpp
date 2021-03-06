#include "StdAfx.h"
#include "GeoExp.h"
#include "VGMath.h"
#include "Script\SafeArrayHelper.h"

enum GeoExpValueMode
{
	VALUE_MODE_DISTANCE_POINT_POINT=0,
	VALUE_MODE_DISTANCE_POINT_LINE,
	VALUE_MODE_PERPENDICULAR,
	VALUE_MODE_PARALLEL,
	VALUE_MODE_POINT_IN_LINE,
	VALUE_MODE_POINT_IN_SEGMENT,
	VALUE_MODE_POINT_IN_CIRCLE,
	VALUE_MODE_ANGLE_LINE_LINE,
	VALUE_MODE_ANGLE_POINT_POINT_POINT,
	VALUE_MODE_TANGENT_LINE_CIRCLE,
	VALUE_MODE_TANGENT_CIRCLE_CIRCLE,
	VALUE_MODE_DIRECTED_LENGTH,
	VALUE_MODE_CONCURRENT,
	VALUE_MODE_COLLINEAR,
	VALUE_MODE_AREA,
	VALUE_MODE_ORIENTED_AREA,
	VALUE_MODE_CHECKBOX
};

CValueFractionEx::CValueFractionEx()
{
	bIsDouble=false;
}

CValueFractionEx::CValueFractionEx( double d )
{
	bIsDouble=false;
	FromNumber(d);
}

CValueFractionEx::CValueFractionEx(CValueFractionEx &copy)
{
	*this=copy;
}

CString CValueFractionEx::ToString(bool bBracket)
{
	if (bIsDouble) return Math::DoubleToString(dValue);
	else
	{
		if (a.GetSize()==1 && a[0]<0) return L"-"+Abs().ToString();
		// Sap xep lai
		int tg;
		int m;
		for (int i=0;i<a.GetSize();i++)
		{
			m=i;
			for (int j=i+1;j<a.GetSize();j++)
				if (b[j]<b[i]) m=j;
			tg=a[i]; a[i]=a[m]; a[m]=tg;
			tg=b[i]; b[i]=b[m]; b[m]=tg;
		}

		// Get String
		CString s;
		for (int i=0;i<a.GetSize();i++)
		{
			if (a[i]==1) 
				if (b[i]==1) s+=L"+1"; else s+=L"+sqrt("+Math::DoubleToString(b[i])+L")";
			else if (a[i]==-1) 
				if (b[i]==1) s+=L"-1"; else s+=L"-sqrt("+Math::DoubleToString(b[i])+L")";
			else
			{
				if (b[i]==1) s+=L"+"+Math::DoubleToString(a[i]); 
				else 
				{
					if (a[i]>0) s+=L"+";
					s+=Math::DoubleToString(a[i])+L"sqrt("+Math::DoubleToString(b[i])+L")";
				}
			}
		}
		if (s[0]=='+') s.Delete(0);

		if (ms!=1) s=L"("+s+L")/"+Math::DoubleToString(ms);
		else if (a.GetSize()>1) s=L"("+s+L")";
		if (s==L"") s=L"0";

		return s;
	}
}

void CValueFractionEx::Simplify()
{
	if (bIsDouble=false && ms>1000)
	{
		bIsDouble=true;
		dValue=ToDouble();
		a.RemoveAll(); b.RemoveAll();
		return;
	}

	for (int i=0;i<a.GetSize();i++)
	{
		if (a[i]==0)
		{
			a.RemoveAt(i);
			b.RemoveAt(i);
			i--;
		}
	}
	int d=ms;
	for (int i=0;i<a.GetSize();i++)
	{
		d=Math::gcd(d,a[i]);
		if (d==1) break;
	}
	if (d!=1) 
	{
		ms/=d;
		for (int i=0;i<a.GetSize();i++)
			a[i]/=d;
	}
}

void CValueFractionEx::FromNumber(double d)
{
	a.RemoveAll(); b.RemoveAll();
	if (d==0) { ms=1; return; }
	if (Math::IsInt(d)) { a.Add(d); b.Add(1); ms=1; return; }
	if (Math::IsInt(d*d)) { a.Add(1); b.Add(Math::Round(d*d)); ms=1; return; }
	for (int i=2;i<=20;i++) if (Math::IsInt(d*i)) { a.Add(d*i); b.Add(1); ms=i; return; }
	
	bIsDouble=true; dValue=d;
}

void CValueFractionEx::AddSqrt(int na, int nb)
{
	int i=0;
	for (i;i<a.GetSize();i++) if (b[i]==nb) break;
	if (i==a.GetSize())
	{
		a.Add(na);
		b.Add(nb);
	}
	else
	{
		a[i]+=na;
	}
}

CValueFractionEx CValueFractionEx::Sqrt()
{
	if (!bIsDouble && a.GetSize()==1 && b[0]==1)
	{
		int d=a[0]*ms;
		int na,nb; CValueFractionEx::SqrtTo_a_And_b_(d,na,nb);
		CValueFractionEx r;
		r.a.Add(na);r.b.Add(nb);
		r.ms=ms;
		return r;
	}
	else
	{
		double d=ToDouble();
		CValueFractionEx r;
		r.bIsDouble=true;
		r.dValue=sqrt(d);
		return r;
	}
}

CValueFractionEx CValueFractionEx::Abs()
{
	CValueFractionEx r=*this;
	if (bIsDouble) r.dValue=abs(r.dValue);
	else
	{
		for (int i=0;i<r.a.GetSize();i++) r.a[i]=abs(r.a[i]);
	}
	return r;
}

CValueFractionEx CValueFractionEx::Power(CValueFractionEx& k)
{
	if (Math::IsInt(k.ToDouble()))
	{
		int n=(int)k.ToDouble();
		CValueFractionEx f(*this);
		for (int i=0;i<n-1;i++) f=f*(*this);
		return f;
	}
	else
	{
		CValueFractionEx f;
		f.bIsDouble=true;
		f.dValue=pow(this->ToDouble(),k.ToDouble());
		return f;
	}
}

CValueFractionEx CValueFractionEx::Inverse()
{
	if (bIsDouble)
	{
		CValueFractionEx fr;
		fr.bIsDouble=true;
		fr.dValue=1/ToDouble();
		return fr;
	}
	if (a.GetSize()==1)
	{
		CValueFractionEx fr;
		fr.ms=a[0]*b[0];
		fr.a.Add(ms); fr.b.Add(b[0]);
		if (fr.ms<0) { fr.ms=-fr.ms; fr.a[0]=-fr.a[0]; }
		fr.Simplify();
		return fr;
	}
	else if (a.GetSize()==2)
	{
		CValueFractionEx fr;
		int na1=a[0]; int nb1=b[0];
		int na2=a[1]; int nb2=b[1];
		int s=nb1*na1*na1-nb2*na2*na2;
		if (s>0)
		{
			fr.ms=s;
			fr.a.Add(ms*a[0]); fr.a.Add(-ms*a[1]);
			fr.b.Add(b[0]); fr.b.Add(b[1]);
		}
		else
		{
			fr.ms=-s;
			fr.a.Add(-ms*a[0]); fr.a.Add(ms*a[1]);
			fr.b.Add(b[0]); fr.b.Add(b[1]);
		}
		fr.Simplify();
		return fr;
	}
	else
	{
		CValueFractionEx fr;
		fr.bIsDouble=true;
		fr.dValue=1/ToDouble();
		return fr;
	}
}

CValueFractionEx CValueFractionEx::operator+(CValueFractionEx& f)
{
	if (f.bIsDouble || this->bIsDouble)
	{
		CValueFractionEx fr;
		fr.bIsDouble=true;
		fr.dValue=f.ToDouble()+ToDouble();
		return fr;
	}
	else
	{
		int msc=Math::lcm(f.ms,ms);
		int ms0=msc/ms;
		int msf=msc/f.ms;
		CValueFractionEx fr; fr.ms=msc;
		for (int i=0;i<a.GetSize();i++) fr.AddSqrt(ms0*a[i],b[i]);
		for (int i=0;i<f.a.GetSize();i++) fr.AddSqrt(msf*f.a[i],f.b[i]);
		fr.Simplify();
		return fr;
	}
}

CValueFractionEx CValueFractionEx::operator-(CValueFractionEx& f)
{
	if (f.bIsDouble || this->bIsDouble)
	{
		CValueFractionEx fr;
		fr.bIsDouble=true;
		fr.dValue=ToDouble()-f.ToDouble();
		return fr;
	}
	else
	{
		int msc=Math::lcm(f.ms,ms);
		int ms0=msc/ms;
		int msf=msc/f.ms;
		CValueFractionEx fr; fr.ms=msc;
		for (int i=0;i<a.GetSize();i++) fr.AddSqrt(ms0*a[i],b[i]);
		for (int i=0;i<f.a.GetSize();i++) fr.AddSqrt(-msf*f.a[i],f.b[i]);
		fr.Simplify();
		return fr;
	}
}

CValueFractionEx CValueFractionEx::operator-()
{
	if (this->bIsDouble)
	{
		CValueFractionEx fr;
		fr.bIsDouble=true;
		fr.dValue=-this->dValue;
		return fr;
	}
	else
	{
		CValueFractionEx fr=*this;
		for (int i=0;i<a.GetSize();i++) fr.a[i]=-fr.a[i];
		//Simplify();
		return fr;
	}
}

CValueFractionEx CValueFractionEx::operator*(CValueFractionEx& f)
{
	if (f.bIsDouble || this->bIsDouble)
	{
		CValueFractionEx fr;
		fr.bIsDouble=true;
		fr.dValue=ToDouble()*f.ToDouble();
		return fr;
	}
	else
	{
		CValueFractionEx fr; fr.ms=ms*f.ms;
		for (int i=0;i<a.GetSize();i++) 
		{
			for (int j=0;j<f.a.GetSize();j++)
			{
				int na=a[i]*f.a[j];
				int nb=b[i]*f.b[j];
				int tmpA;
				SqrtTo_a_And_b_(nb,tmpA,nb); na*=tmpA;
				fr.AddSqrt(na,nb);
			}
		}
		fr.Simplify();
		return fr;
	}
}

CValueFractionEx CValueFractionEx::operator/(CValueFractionEx& f)
{
	/*
	if (f.bIsDouble || this->bIsDouble)
		{
			CValueFractionEx fr;
			fr.bIsDouble=true;
			fr.dValue=ToDouble()/f.ToDouble();
			return fr;
		}
		else if (f.a.GetSize()==1)
		{
			CValueFractionEx fr;
			fr.ms=f.a[0]*f.b[0];
			fr.a.Add(f.ms); fr.b.Add(f.b[0]);
			//int na=f.a[0]; int nb=f.b[0];
			//fr.ms=nb*na;
			//CValueFractionEx fMul; fMul.a.Add(fr.ms>0 ? 1 : -1); fMul.b.Add(nb); fMul.ms=1;
			if (fr.ms<0) { fr.ms=-fr.ms; fr.a[0]=-fr.a[0]; }
			fr.Simplify();
			fr=(*this)*fr;
			return fr;
		}
		else if (f.a.GetSize()==2)
		{
			CValueFractionEx fr=*this;
			int na1=f.a[0]; int nb1=f.b[0];
			int na2=f.a[1]; int nb2=f.b[1];
			int s=nb1*na1*na1-nb2*na2*na2;
			if (s>0)
			{
				fr.ms=s;
				CValueFractionEx fMul; 
				fMul.a.Add(na1); fMul.a.Add(-na2);
				fMul.b.Add(nb1); fMul.b.Add(nb2);
				fMul.ms=1;
				fr=fr*fMul;
			}
			else
			{
				fr.ms=-s;
				CValueFractionEx fMul; 
				fMul.a.Add(-na1); fMul.a.Add(na2);
				fMul.b.Add(nb1); fMul.b.Add(nb2);
				fMul.ms=1;
				fr=fr*fMul;
			}
			fr.Simplify();
			return fr;
		}
		else
		{
			CValueFractionEx fr;
			fr.bIsDouble=true;
			fr.dValue=ToDouble()/f.ToDouble();
			return fr;
		}*/
	return (*this) * f.Inverse();
}

double CValueFractionEx::ToDouble()
{
	if (bIsDouble) return dValue;
	else
	{
		double d=0;
		for (int i=0;i<a.GetSize();i++) { d+=a[i]*sqrt((double)b[i]); }
		return d/ms;
	}
}

bool CValueFractionEx::operator=(CValueFractionEx& f)
{
	a.RemoveAll(); b.RemoveAll();
	for (int i=0;i<f.a.GetSize();i++) { a.Add(f.a[i]); b.Add(f.b[i]); }
	ms=f.ms;
	bIsDouble=f.bIsDouble;
	dValue=f.dValue;
	return true;
}

bool CValueFractionEx::operator=(int i)
{
	a.RemoveAll(); b.RemoveAll();
	a.Add(i); b.Add(1);
	ms=1;
	return true;
}

void CValueFractionEx::SqrtTo_a_And_b_( int n, int& y, int& z )
{
	int k=n;
	y=1; z=1;
	int i=2;
	while (k!=1)
	{
		if (k%i==0)
		{
			k/=i;
			if (k%i==0) { y*=i; k/=i; }
			else { z*=i; i++; }
		}
		else i++;
	}
}

CGeoExp::CGeoExp(void)
{
	m_Value=NULL;
	m_bErr=false;
}

CGeoExp::~CGeoExp(void)
{
	if (m_Value!=NULL) delete m_Value;
}

CGeoExp::CValueExp::CValueExp()
{
	m_Value=NULL;
}

CGeoExp::CValueExp::~CValueExp()
{
	if (m_Value!=NULL) delete m_Value;
}

bool CGeoExp::CValueExp::CanCalc()
{
	if (m_Param.GetSize()==0) return true;
	for (int i=0;i<m_Param.GetSize();i++)
	{
		CVGObject* obj=m_Param[i];
		if (obj!=NULL && obj->m_bCalc==FALSE) 
			return false;
	}
	return true;
}

CValue* CGeoExp::CValueExp::CalcFr()
{
	CValueFractionEx* vl;
	vl=new CValueFractionEx();
	switch (m_Mode)
	{
	case VALUE_MODE_DISTANCE_POINT_POINT:
		{
			CVGPoint* pt1=(CVGPoint*)m_Param[0];
			CVGPoint* pt2=(CVGPoint*)m_Param[1];
			CValueFractionEx x1,x2,y1,y2;
			pt1->CaclFr(x1,y1);
			pt2->CaclFr(x2,y2);
			*vl=Math::GetDistanceFr(x1,y1,x2,y2);
			break;
		}
	case VALUE_MODE_DISTANCE_POINT_LINE:
		{
			CVGPoint* pt=(CVGPoint*)m_Param[0];
			CValueFractionEx x,y; pt->CaclFr(x,y);
			CVGLine* line=(CVGLine*)m_Param[1];
			CValueFractionEx A,B,C;
			line->GetABCFr(A,B,C);
			*vl=Math::GetDistanceFr(x,y,A,B,C);
			break;
		}
	case VALUE_MODE_DIRECTED_LENGTH:
		{
			CVGPoint* pt1=(CVGPoint*)m_Param[0];
			CValueFractionEx x1,y1; pt1->CaclFr(x1,y1);
			CVGPoint* pt2=(CVGPoint*)m_Param[1];
			CValueFractionEx x2,y2; pt1->CaclFr(x2,y2);
			*vl=Math::GetDistanceDSFr(x1,y1,x2,y2);
			break;
		}
// 	case VALUE_MODE_ANGLE_LINE_LINE:
// 		{
// 			CVGLineBase* l1=(CVGLineBase*)m_Param[0];
// 			CVGLineBase* l2=(CVGLineBase*)m_Param[1];
// 			double a1,b1,a2,b2,c1,c2;
// 			l1->GetABC(a1,b1,c1);
// 			l2->GetABC(a2,b2,c2);
// 			vl=new CValueDouble(Math::Angle(a1,b1,c1,a2,b2,c2));
// 			break;
// 		}
// 	case VALUE_MODE_ANGLE_POINT_POINT_POINT:
// 		{
// 			CVGPoint* pt1=(CVGPoint*)m_Param[0];
// 			CVGPoint* pt2=(CVGPoint*)m_Param[1];
// 			CVGPoint* pt3=(CVGPoint*)m_Param[2];
// 
// 			double x1=pt1->m_x-pt2->m_x;
// 			double y1=pt1->m_y-pt2->m_y;
// 			double x2=pt3->m_x-pt2->m_x;
// 			double y2=pt3->m_y-pt2->m_y;
// 
// 			double angle=Math::Angle(x1,y1,x2,y2);
// 			vl->FromNumber(angle/pi*180);
// 			break;
// 		}
	case VALUE_MODE_AREA:
		{
			CVGPolygon* p=(CVGPolygon*)m_Param[0];
			*vl=p->GetAreaFr().Abs();
			break;
		}
	case VALUE_MODE_ORIENTED_AREA:
		{
			CVGPolygon* p=(CVGPolygon*)m_Param[0];
			*vl=p->GetAreaFr();
			break;
		}
	default: delete vl; Calc(); return m_Value->Clone();
	}
	return vl;
}

void CGeoExp::CValueExp::Calc()
{
	if (m_Value!=NULL) { delete m_Value; m_Value=NULL; }
	switch (m_Mode)
	{
	case VALUE_MODE_DISTANCE_POINT_POINT:
		{
			CVGPoint* pt1=(CVGPoint*)m_Param[0];
			CVGPoint* pt2=(CVGPoint*)m_Param[1];
			m_Value=new CValueDouble(Math::GetDistance(pt1->m_x,pt1->m_y,pt2->m_x,pt2->m_y));
			break;
		}
	case VALUE_MODE_DISTANCE_POINT_LINE:
		{
			CVGPoint* pt=(CVGPoint*)m_Param[0];
			CVGLine* line=(CVGLine*)m_Param[1];
			double A,B,C;
			line->GetABC(A,B,C);
			m_Value=new CValueDouble(Math::GetDistance(pt->m_x,pt->m_y,A,B,C));
			break;
		}
	case VALUE_MODE_DIRECTED_LENGTH:
		{
			CVGPoint* pt1=(CVGPoint*)m_Param[0];
			CVGPoint* pt2=(CVGPoint*)m_Param[1];
			m_Value=new CValueDouble(Math::GetDistanceDS(pt1->m_x,pt1->m_y,pt2->m_x,pt2->m_y));
			break;
		}
	case VALUE_MODE_PERPENDICULAR:
		{
			CVGLineBase* l1=(CVGLineBase*)m_Param[0];
			CVGLineBase* l2=(CVGLineBase*)m_Param[1];
			double a1,b1,c1,a2,b2,c2;
			l1->GetABC(a1,b1,c1);
			l2->GetABC(a2,b2,c2);
			m_Value=new CValueBool(Math::IsPerpendicular(a1,b1,c1,a2,b2,c2));
			break;
		}
	case VALUE_MODE_PARALLEL:
		{
			CVGLineBase* l1=(CVGLineBase*)m_Param[0];
			CVGLineBase* l2=(CVGLineBase*)m_Param[1];
			double a1,b1,c1,a2,b2,c2;
			l1->GetABC(a1,b1,c1);
			l2->GetABC(a2,b2,c2);
			m_Value=new CValueBool(Math::IsParallel(a1,b1,c1,a2,b2,c2));
			break;
		}
	case VALUE_MODE_POINT_IN_LINE:
		{
			CVGPoint* pt=(CVGPoint*)m_Param[0];
			CVGLineBase* l1=(CVGLineBase*)m_Param[1];
			double a,b,c;
			l1->GetABC(a,b,c);
			m_Value=new CValueBool(Math::Equal(a*pt->m_x+b*pt->m_y+c,0) && l1->CheckPtIn(pt->m_x,pt->m_y));
			break;
		}
	case VALUE_MODE_POINT_IN_SEGMENT:
		{
			CVGPoint* pt=(CVGPoint*)m_Param[0];
			CVGPoint* pt1=(CVGPoint*)m_Param[1];
			CVGPoint* pt2=(CVGPoint*)m_Param[2];
			m_Value=new CValueBool(Math::IsPtInSegment(pt->m_x,pt->m_y,pt1->m_x,pt1->m_y,pt2->m_x,pt2->m_y));
			break;
		}
	case VALUE_MODE_POINT_IN_CIRCLE:
		{
			CVGPoint* pt=(CVGPoint*)m_Param[0];
			CVGCircle* c=(CVGCircle*)m_Param[1];
			m_Value=new CValueBool(Math::Equal(Math::GetDistance(pt->m_x,pt->m_y,c->m_CenterX,c->m_CenterY),c->m_Radius));
			break;
		}
	case VALUE_MODE_ANGLE_LINE_LINE:
		{
			CVGLineBase* l1=(CVGLineBase*)m_Param[0];
			CVGLineBase* l2=(CVGLineBase*)m_Param[1];
			double a1,b1,a2,b2,c1,c2;
			l1->GetABC(a1,b1,c1);
			l2->GetABC(a2,b2,c2);
			m_Value=new CValueDouble(Math::Angle(a1,b1,c1,a2,b2,c2));
			break;
		}
	case VALUE_MODE_ANGLE_POINT_POINT_POINT:
		{
			CVGPoint* pt1=(CVGPoint*)m_Param[0];
			CVGPoint* pt2=(CVGPoint*)m_Param[1];
			CVGPoint* pt3=(CVGPoint*)m_Param[2];

			double x1=pt1->m_x-pt2->m_x;
			double y1=pt1->m_y-pt2->m_y;
			double x2=pt3->m_x-pt2->m_x;
			double y2=pt3->m_y-pt2->m_y;

			double angle=Math::Angle(x1,y1,x2,y2);
			m_Value=new CValueDouble(angle/pi*180);
			break;
		}
	case VALUE_MODE_TANGENT_LINE_CIRCLE:
		{
			CVGLineBase* line=(CVGLineBase*)m_Param[0];
			CVGCircle* cir=(CVGCircle*)m_Param[1];
			double a,b,c;
			line->GetABC(a,b,c);
			m_Value=new CValueBool(Math::IsTangent(a,b,c,cir->m_CenterX,cir->m_CenterY,cir->m_Radius));
			break;
		}
	case VALUE_MODE_TANGENT_CIRCLE_CIRCLE:
		{
			CVGCircle* c1=(CVGCircle*)m_Param[0];
			CVGCircle* c2=(CVGCircle*)m_Param[1];
			m_Value=new CValueBool(Math::Equal(Math::GetDistance(c1->m_CenterX,c1->m_CenterY,c2->m_CenterX,c2->m_CenterY),c1->m_Radius+c2->m_Radius));
			break;
		}
	case VALUE_MODE_COLLINEAR:
		{
			CVGPoint* pt1=(CVGPoint*)m_Param[0];
			CVGPoint* pt2=(CVGPoint*)m_Param[1];
			CVGPoint* pt3=(CVGPoint*)m_Param[2];
			m_Value=new CValueBool(Math::IsCollinear(pt1->m_x,pt1->m_y,pt2->m_x,pt2->m_y,pt3->m_x,pt3->m_y));
			break;
		}
	case VALUE_MODE_AREA:
		{
			CVGPolygon* p=(CVGPolygon*)m_Param[0];
			m_Value=new CValueDouble(abs(p->GetArea()));
			break;
		}
	case VALUE_MODE_ORIENTED_AREA:
		{
			CVGPolygon* p=(CVGPolygon*)m_Param[0];
			m_Value=new CValueDouble(p->GetArea());
			break;
		}
	}
}

bool CGeoExp::IsConstant()
{
	for (int i=0;i<m_TypeArr.GetSize();i++) if (m_TypeArr[i]==VARIABLE_X) return false;
	return m_ValueExpArr.GetSize()==0 && m_objArr.GetSize()==0;
}

bool CGeoExp::IsRelated(CVGObject* obj)
{
	for (int i=0;i<m_ValueExpArr.GetSize();i++)
	{
		for (int j=0;j<m_ValueExpArr[i]->m_Param.GetSize();j++)
			if (m_ValueExpArr[i]->m_Param[j]==obj) return true;
	}
	for (int i=0;i<m_objArr.GetSize();i++)
		if (m_objArr[i]==obj) return true;
	return false;
}

CValue* CGeoExp::CalcFr(double vx, bool bRadian)
{
	m_bErr=FALSE;
	for (int i=0;i<m_ValueExpArr.GetSize();i++)
	{
		m_ValueExpArr[i]->CalcFr();
	}

	//////////////////////////////////////////////////////////////////////////

	CGeoExpStack st;
	CValue* m_Value;

	//CValue* x,y;
	CString str;
	int o=0;
	int n=0;
	int v=0;
	int vn=0;
	int usf=0;
	for (int i=0;i<m_TypeArr.GetSize();i++) 
	{
		if (m_TypeArr[i]==OPERATOR)
		{
			int oper=m_OperatorArr[o];
			int paramCount=m_ParamCountArr[o];
			if (st.GetSize()<paramCount) { m_strErr=L"Syntax Error"; goto error; }
			bool bDone=false;
			if (oper==NOT && st.GetValueType(0)==VALUE_BOOL)
			{
				CValueBool* x=(CValueBool*)st.Top();
				st.Push(new CValueBool(!x->m_Value));
				delete x;
			}
			else if (oper==OP_VECTOR && paramCount==2 && st.GetValueType(0)==VALUE_FRACTION_EX && st.GetValueType(1)==VALUE_FRACTION_EX)
			{
				CValueFractionEx* x=(CValueFractionEx*)st.Top();
				CValueFractionEx* y=(CValueFractionEx*)st.Top();
				st.Push(new CValueVectorEx(*y,*x));
				delete x; delete y;
			}
			else if (oper==ADDITION && st.GetValueType(0)==VALUE_FRACTION_EX && 
				st.GetValueType(1)==VALUE_FRACTION_EX)
			{
				CValueFractionEx* x=(CValueFractionEx*)st.Top(); CValueFractionEx* y=(CValueFractionEx*)st.Top();
				CValueFractionEx* f=new CValueFractionEx(); *f=*x+*y;
				st.Push(f);
				delete x; delete y;
			}
			else if (oper==ADDITION && st.GetValueType(0)==VALUE_VECTOR_EX && 
				st.GetValueType(1)==VALUE_VECTOR_EX)
			{
				CValueVectorEx* x=(CValueVectorEx*)st.Top(); CValueVectorEx* y=(CValueVectorEx*)st.Top();
				st.Push(new CValueVectorEx(x->x+y->x,x->y+y->y));
				delete x; delete y;
			}
			else if (oper==ADDITION)
			{
				CValue* x=(CValue*)st.Top(); CValue* y=(CValue*)st.Top();
				st.Push(new CValueString(y->ToString()+x->ToString()));
				delete x; delete y;
			}
			else if (oper==SUBTRACTION && st.GetValueType(0)==VALUE_FRACTION_EX && 
				st.GetValueType(1)==VALUE_FRACTION_EX)
			{
				CValueFractionEx* x=(CValueFractionEx*)st.Top(); CValueFractionEx* y=(CValueFractionEx*)st.Top();
				CValueFractionEx* f=new CValueFractionEx(); *f=(*y)-(*x);
				st.Push(f);
				delete x; delete y;
			}
			else if (oper==MULTIPLICATION && st.GetValueType(0)==VALUE_FRACTION_EX && 
				st.GetValueType(1)==VALUE_FRACTION_EX)
			{
				CValueFractionEx* x=(CValueFractionEx*)st.Top(); CValueFractionEx* y=(CValueFractionEx*)st.Top();
				CValueFractionEx* f=new CValueFractionEx(); *f=(*y)*(*x);
				st.Push(f);
				delete x; delete y;
			}
			else if (oper==MULTIPLICATION && st.GetValueType(0)==VALUE_FRACTION_EX && 
				st.GetValueType(1)==VALUE_VECTOR_EX)
			{
				CValueFractionEx* x=(CValueFractionEx*)st.Top(); CValueVectorEx* y=(CValueVectorEx*)st.Top();
				st.Push(new CValueVectorEx(y->x*(*x),y->y*(*x)));
				delete x; delete y;
			}
			else if (oper==MULTIPLICATION && st.GetValueType(0)==VALUE_VECTOR_EX && 
				st.GetValueType(1)==VALUE_FRACTION_EX)
			{
				CValueVectorEx* x=(CValueVectorEx*)st.Top(); CValueFractionEx* y=(CValueFractionEx*)st.Top();
				st.Push(new CValueVectorEx(x->x*(*y),x->y*(*y)));
				delete x; delete y;
			}
			else if (oper==DIVISION && st.GetValueType(0)==VALUE_FRACTION_EX && 
				st.GetValueType(1)==VALUE_FRACTION_EX)
			{
				CValueFractionEx* x=(CValueFractionEx*)st.Top(); CValueFractionEx* y=(CValueFractionEx*)st.Top();
				CValueFractionEx* f=new CValueFractionEx(); *f=(*y)/(*x);
				st.Push(f);
				delete x; delete y;
			}
			else if (oper==DIVISION && st.GetValueType(0)==VALUE_FRACTION_EX && 
				st.GetValueType(1)==VALUE_VECTOR_EX)
			{
				CValueFractionEx* x=(CValueFractionEx*)st.Top(); CValueVectorEx* y=(CValueVectorEx*)st.Top();
				st.Push(new CValueVectorEx(y->x/(*x),y->y/(*x)));
				delete x; delete y;
			}
			else if (oper==POWER && st.GetValueType(0)==VALUE_FRACTION_EX && 
				st.GetValueType(1)==VALUE_FRACTION_EX)
			{
				CValueFractionEx* x=(CValueFractionEx*)st.Top(); CValueFractionEx* y=(CValueFractionEx*)st.Top();
				CValueFractionEx* f=new CValueFractionEx(); *f=y->Power(*x);
				st.Push(f);
				delete x; delete y;
			}
			else if (oper==SQRT && paramCount==1 && st.GetValueType(0)==VALUE_FRACTION_EX)
			{
				CValueFractionEx* x=(CValueFractionEx*)st.Top();
				if (x->ToDouble()<0)
				{ m_strErr=L"Math Error"; goto error; }
					
				*x=x->Sqrt();
				st.Push(x);
			}
			else if (oper==ABS && paramCount==1 && st.GetValueType(0)==VALUE_FRACTION_EX)
			{
				CValueFractionEx* x=(CValueFractionEx*)st.Top(); *x=x->Abs();
				st.Push(x);
			}
			else if (oper==EQUAL && st.GetValueType(0)==VALUE_FRACTION_EX 
				&& st.GetValueType(1)==VALUE_FRACTION_EX)
			{
				CValueFractionEx* x=(CValueFractionEx*)st.Top(); CValueFractionEx* y=(CValueFractionEx*)st.Top();
				st.Push(new CValueBool(x->ToDouble()==y->ToDouble()));
				delete x; delete y;
			}
			else if (oper==EQUAL && st.GetValueType(0)==VALUE_STRING && st.GetValueType(1)==VALUE_STRING)
			{
				CValueString* x=(CValueString*)st.Top(); CValueString* y=(CValueString*)st.Top();
				st.Push(new CValueBool(x->m_Value==y->m_Value));
				delete x; delete y;
			}
			else if (oper==GREATER && st.GetValueType(0)==VALUE_FRACTION_EX 
				&& st.GetValueType(1)==VALUE_FRACTION_EX)
			{
				CValueFractionEx* x=(CValueFractionEx*)st.Top(); CValueFractionEx* y=(CValueFractionEx*)st.Top();
				st.Push(new CValueBool(x->ToDouble()>y->ToDouble()));
				delete x; delete y;
			}
			else if (oper==LESS && st.GetValueType(0)==VALUE_FRACTION_EX 
				&& st.GetValueType(1)==VALUE_FRACTION_EX)
			{
				CValueFractionEx* x=(CValueFractionEx*)st.Top(); CValueFractionEx* y=(CValueFractionEx*)st.Top();
				st.Push(new CValueBool(x->ToDouble()<y->ToDouble()));
				delete x; delete y;
			}
			else if (oper==GREATER_THAN_OR_EQUAL && st.GetValueType(0)==VALUE_FRACTION_EX 
				&& st.GetValueType(1)==VALUE_FRACTION_EX)
			{
				CValueFractionEx* x=(CValueFractionEx*)st.Top(); CValueFractionEx* y=(CValueFractionEx*)st.Top();
				st.Push(new CValueBool(x->ToDouble()>=y->ToDouble()));
				delete x; delete y;
			}
			else if (oper==LESS_THAN_OR_EQUAL && st.GetValueType(0)==VALUE_FRACTION_EX 
				&& st.GetValueType(1)==VALUE_FRACTION_EX)
			{
				CValueFractionEx* x=(CValueFractionEx*)st.Top(); CValueFractionEx* y=(CValueFractionEx*)st.Top();
				st.Push(new CValueBool(x->ToDouble()<=y->ToDouble()));
				delete x; delete y;
			}
			else if (oper==IF && paramCount==3 && st.GetValueType(2)==VALUE_BOOL)
			{
				CValue* x=(CValue*)st.Top();
				CValue* y=(CValue*)st.Top();
				CValueBool* b=(CValueBool*)st.Top();
				if (b->m_Value) { st.Push(y); delete x; }
				else { st.Push(x); delete y; }
				delete b;
			}
			else if (oper==LOG10 && paramCount==1 && st.GetValueType(0)==VALUE_FRACTION_EX)
			{
				CValue* x=(CValue*)st.Top();
				st.Push(new CValueFractionEx(log10(x->ToDouble())));
				delete x;
			}
			else if (oper==CBRT && paramCount==1 && st.GetValueType(0)==VALUE_FRACTION_EX)
			{
				CValue* x=(CValue*)st.Top();
				st.Push(new CValueFractionEx(pow(x->ToDouble(),1/3)));
				delete x;
			}
			else if (oper==EXP && paramCount==1 && st.GetValueType(0)==VALUE_FRACTION_EX)
			{
				CValue* x=(CValue*)st.Top();
				st.Push(new CValueFractionEx(exp(x->ToDouble())));
				delete x;
			}
			else if (oper==FLOOR && paramCount==1 && st.GetValueType(0)==VALUE_FRACTION_EX)
			{
				CValue* x=(CValue*)st.Top();
				st.Push(new CValueFractionEx(floor(x->ToDouble())));
				delete x;
			}
			else if (oper==CEIL && paramCount==1 && st.GetValueType(0)==VALUE_FRACTION_EX)
			{
				CValue* x=(CValue*)st.Top();
				st.Push(new CValueFractionEx(ceil(x->ToDouble())));
				delete x;
			}
			else if (oper==ROUND && paramCount==1 && st.GetValueType(0)==VALUE_FRACTION_EX)
			{
				CValue* x=(CValue*)st.Top();
				st.Push(new CValueFractionEx(Math::Round(x->ToDouble())));
				delete x;
			}
			else if (oper==SIN && paramCount==1 && st.GetValueType(0)==VALUE_FRACTION_EX)
			{
				CValue* x=(CValue*)st.Top();
				st.Push(new CValueFractionEx(sin(bRadian ? x->ToDouble() : Math::ConvertToRadian(x->ToDouble()))));
				delete x;
			}
			else if (oper==COS && paramCount==1 && st.GetValueType(0)==VALUE_FRACTION_EX)
			{
				CValue* x=(CValue*)st.Top();
				st.Push(new CValueFractionEx(cos(bRadian ? x->ToDouble() : Math::ConvertToRadian(x->ToDouble()))));
				delete x;
			}
			else if (oper==TAN && paramCount==1 && st.GetValueType(0)==VALUE_FRACTION_EX)
			{
				CValue* x=(CValue*)st.Top();
				st.Push(new CValueFractionEx(tan(bRadian ? x->ToDouble() : Math::ConvertToRadian(x->ToDouble()))));
				delete x;
			}
			else if (oper==COT && paramCount==1 && st.GetValueType(0)==VALUE_FRACTION_EX)
			{
				CValue* x=(CValue*)st.Top();
				st.Push(new CValueFractionEx(1/tan(bRadian ? x->ToDouble() : Math::ConvertToRadian(x->ToDouble()))));
				delete x;
			}
			else if (oper==SINH && paramCount==1 && st.GetValueType(0)==VALUE_FRACTION_EX)
			{
				CValue* x=(CValue*)st.Top();
				st.Push(new CValueFractionEx(sinh(bRadian ? x->ToDouble() : Math::ConvertToRadian(x->ToDouble()))));
				delete x;
			}
			else if (oper==COSH && paramCount==1 && st.GetValueType(0)==VALUE_FRACTION_EX)
			{
				CValue* x=(CValue*)st.Top();
				st.Push(new CValueFractionEx(cosh(bRadian ? x->ToDouble() : Math::ConvertToRadian(x->ToDouble()))));
				delete x;
			}
			else if (oper==TANH && paramCount==1 && st.GetValueType(0)==VALUE_FRACTION_EX)
			{
				CValue* x=(CValue*)st.Top();
				st.Push(new CValueFractionEx(tanh(bRadian ? x->ToDouble() : Math::ConvertToRadian(x->ToDouble()))));
				delete x;
			}
			else if (oper==ARCSIN && paramCount==1 && st.GetValueType(0)==VALUE_FRACTION_EX)
			{
				CValue* x=(CValue*)st.Top();
				if (abs(x->ToDouble())>1)
				{ m_strErr=L"Math Error"; goto error; }
				st.Push(new CValueFractionEx(bRadian ? asin(x->ToDouble()) : Math::ConvertFromRadianToAngleUnit(asin(x->ToDouble()))));
				delete x;
			}
			else if (oper==ARCCOS && paramCount==1 && st.GetValueType(0)==VALUE_FRACTION_EX)
			{
				CValue* x=(CValue*)st.Top();
				if (abs(x->ToDouble())>1)
				{ m_strErr=L"Math Error"; goto error; }
				st.Push(new CValueFractionEx(bRadian ? acos(x->ToDouble()) : Math::ConvertFromRadianToAngleUnit(acos(x->ToDouble()))));
				delete x;
			}
			else if (oper==ARCTAN && paramCount==1 && st.GetValueType(0)==VALUE_FRACTION_EX)
			{
				CValue* x=(CValue*)st.Top();
				st.Push(new CValueFractionEx(bRadian ? atan(x->ToDouble()) : Math::ConvertFromRadianToAngleUnit(atan(x->ToDouble()))));
				delete x;
			}
			else if (oper==MAX || oper==MIN && paramCount>0)
			{
				CValue* vl=st.Top();
				double m;
				if (vl->GetType()==VALUE_FRACTION_EX) { m=vl->ToDouble(); delete vl; }
				else { delete vl; goto error; }
				for (int i=1;i<paramCount;i++)
				{
					vl=st.Top();
					if (vl->GetType()==VALUE_FRACTION_EX) { m=oper==MAX ? max(vl->ToDouble(),m) : min(vl->ToDouble(),m); delete vl; }
					else { delete vl; goto error; }
				}
				st.Push(new CValueFractionEx(m));
			}
			else { m_strErr=L"Syntax Error"; goto error; }
			o++;
		}
		else if (m_TypeArr[i]==NUMBER)
		{
			CValueFractionEx* vl=new CValueFractionEx(); vl->FromNumber(m_ValueArr[n]->ToDouble());
			st.Push(vl);

			n++;
		}
		else if (m_TypeArr[i]==VARIABLE)
		{
			CValue* vl=m_ValueExpArr[v]->CalcFr();
			//f->FromNumber(vl->ToDouble());
			st.Push(vl);

			v++;
		}
		else if (m_TypeArr[i]==VARIABLE_X)
		{
			CValueFractionEx* f=new CValueFractionEx();
			f->FromNumber(vx);
			st.Push(f);
		}
		else if (m_TypeArr[i]==USER_FUNCTION)
		{
			CSafeArrayHelper sfHelper;
			sfHelper.Create(VT_VARIANT, 1, 0, m_ParamCountArr[usf]);

			_variant_t var;
			for (int i=0;i<m_ParamCountArr[usf];i++)
			{
				CValue* vl=st.Top();
				var = _variant_t(vl->GetType()==VALUE_DOUBLE ? ((CValueDouble*)vl)->m_Value : 0);
				sfHelper.PutElement(i, (void*)&var);
				delete vl;
			}

			_variant_t varRet;
			LPSAFEARRAY sa =  sfHelper.GetArray();
			m_ScriptObject->RunProcedure(m_strFuncNameArr[usf],&sa,&varRet);

			CValueFractionEx* f=new CValueFractionEx();
			f->FromNumber((double)varRet);
			st.Push(f);

			usf++;
		}
		else //OBJVALUE
		{
			CValue* d=(m_objArr[vn])->GetObjValue(m_objArrValueType[vn]);
			CValueFractionEx* f=new CValueFractionEx();
			f->FromNumber(d->ToDouble());
			st.Push(f);
			delete d;

			vn++;
		}

	}

	if (st.GetSize()!=1)
	{
		for (int i=0;i<st.GetSize();i++) 
		{
			CValue* x=st.Top();
			delete x;
		}
		m_strErr=L"Syntax Error"; goto error;
	}
endLb:
	if (st.GetSize()==1) 
	{
		m_Value=(CValue*)st.Top();
		return m_Value;
	}
	else return m_Value;
error:
	m_strErr=L"#"+m_strErr;
	m_bErr=true;
	goto endLb;
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

void CGeoExp::Calc(double vx, bool bRadian)
{
	if (IsConstant() && m_Value!=NULL) return;
	if (m_Value!=NULL) delete m_Value;
	m_bErr=FALSE;
 	for (int i=0;i<m_ValueExpArr.GetSize();i++)
 	{
 		m_ValueExpArr[i]->Calc();
 	}
//  	bool kt=FALSE;
//  	while (kt==FALSE)
//  	{
//  		for(int i=0;i<m_ValueArr.GetSize();i++) 
//  			m_ValueArr[i]->Calc();
//  		kt=TRUE;
//  		for(int i=0;i<m_ValueArr.GetSize();i++) 
//  			if (m_ValueArr[i]->m_bCalc==FALSE)
//  			{ 
//  				kt=FALSE; break; 
//  			}
//  	}
//  	m_bCalc=TRUE;


	//////////////////////////////////////////////////////////////////////////

	CGeoExpStack st;

	//CValue* x,y;
	CString str;
	int o=0;
	int n=0;
	int v=0;
	int vn=0;
	int usf=0;
	for (int i=0;i<m_TypeArr.GetSize();i++) 
	{
		if (m_TypeArr[i]==OPERATOR)
		{
			int oper=m_OperatorArr[o];
			int paramCount=m_ParamCountArr[o];
			if (st.GetSize()<paramCount) { m_strErr=L"Syntax Error"; goto error; }
			bool bDone=false;
			if (oper==NOT && st.GetValueType(0)==VALUE_BOOL)
			{
				CValueBool* x=(CValueBool*)st.Top();
				st.Push(new CValueBool(!x->m_Value));
				delete x;
			}
			else if (oper==OP_VECTOR && paramCount==2 && st.GetValueType(0)==VALUE_DOUBLE && st.GetValueType(1)==VALUE_DOUBLE)
			{
				CValueDouble* x=(CValueDouble*)st.Top();
				CValueDouble* y=(CValueDouble*)st.Top();
				st.Push(new CValueVector(y->m_Value,x->m_Value));
				delete x; delete y;
			}
			else if (oper==ADDITION && st.GetValueType(0)==VALUE_DOUBLE && 
				st.GetValueType(1)==VALUE_DOUBLE)
			{
				CValueDouble* x=(CValueDouble*)st.Top(); CValueDouble* y=(CValueDouble*)st.Top();
				st.Push(new CValueDouble(x->m_Value+y->m_Value));
				delete x; delete y;
			}
			else if (oper==ADDITION && st.GetValueType(0)==VALUE_VECTOR && 
				st.GetValueType(1)==VALUE_VECTOR)
			{
				CValueVector* x=(CValueVector*)st.Top(); CValueVector* y=(CValueVector*)st.Top();
				st.Push(new CValueVector(x->x+y->x,x->y+y->y));
				delete x; delete y;
			}
			else if (oper==ADDITION)
			{
				CValue* x=(CValue*)st.Top(); CValue* y=(CValue*)st.Top();
				st.Push(new CValueString(y->ToString()+x->ToString()));
				delete x; delete y;
			}
			else if (oper==SUBTRACTION && st.GetValueType(0)==VALUE_DOUBLE && 
				st.GetValueType(1)==VALUE_DOUBLE)
			{
				CValueDouble* x=(CValueDouble*)st.Top(); CValueDouble* y=(CValueDouble*)st.Top();
				st.Push(new CValueDouble(y->m_Value-x->m_Value));
				delete x; delete y;
			}
			else if (oper==MULTIPLICATION && st.GetValueType(0)==VALUE_DOUBLE && 
				st.GetValueType(1)==VALUE_DOUBLE)
			{
				CValueDouble* x=(CValueDouble*)st.Top(); CValueDouble* y=(CValueDouble*)st.Top();
				st.Push(new CValueDouble(x->m_Value*y->m_Value));
				delete x; delete y;
			}
			else if (oper==MULTIPLICATION && st.GetValueType(0)==VALUE_DOUBLE && 
				st.GetValueType(1)==VALUE_VECTOR)
			{
				CValueDouble* x=(CValueDouble*)st.Top(); CValueVector* y=(CValueVector*)st.Top();
				st.Push(new CValueVector(y->x*x->m_Value,y->y*x->m_Value));
				delete x; delete y;
			}
			else if (oper==MULTIPLICATION && st.GetValueType(0)==VALUE_VECTOR && 
				st.GetValueType(1)==VALUE_DOUBLE)
			{
				CValueVector* x=(CValueVector*)st.Top(); CValueDouble* y=(CValueDouble*)st.Top();
				st.Push(new CValueVector(x->x*y->m_Value,x->y*y->m_Value));
				delete x; delete y;
			}
			else if (oper==DIVISION && st.GetValueType(0)==VALUE_DOUBLE && 
				st.GetValueType(1)==VALUE_DOUBLE)
			{
				CValueDouble* x=(CValueDouble*)st.Top(); CValueDouble* y=(CValueDouble*)st.Top();
				st.Push(new CValueDouble(y->m_Value/x->m_Value));

				delete x; delete y;
			}
			else if (oper==DIVISION && st.GetValueType(0)==VALUE_DOUBLE && 
				st.GetValueType(1)==VALUE_VECTOR)
			{
				CValueDouble* x=(CValueDouble*)st.Top(); CValueVector* y=(CValueVector*)st.Top();
				st.Push(new CValueVector(y->x/x->m_Value,y->y/x->m_Value));
				delete x; delete y;
			}
			else if (oper==POWER && st.GetValueType(0)==VALUE_DOUBLE && 
				st.GetValueType(1)==VALUE_DOUBLE)
			{
				CValueDouble* x=(CValueDouble*)st.Top(); CValueDouble* y=(CValueDouble*)st.Top();
				st.Push(new CValueDouble(pow(y->m_Value,x->m_Value)));
				delete x; delete y;
			}
			else if (oper==SQRT && paramCount==1 && st.GetValueType(0)==VALUE_DOUBLE)
			{
				CValueDouble* x=(CValueDouble*)st.Top();
				st.Push(new CValueDouble(sqrt(x->m_Value)));
				delete x;
			}
			else if (oper==ABS && paramCount==1 && st.GetValueType(0)==VALUE_DOUBLE)
			{
				CValueDouble* x=(CValueDouble*)st.Top();
				st.Push(new CValueDouble(abs(x->m_Value)));
				delete x;
			}
			else if (oper==EQUAL && st.GetValueType(0)==VALUE_DOUBLE 
				&& st.GetValueType(1)==VALUE_DOUBLE)
			{
				CValueDouble* x=(CValueDouble*)st.Top(); CValueDouble* y=(CValueDouble*)st.Top();
				st.Push(new CValueBool(x->m_Value==y->m_Value));
				delete x; delete y;
			}
			else if (oper==EQUAL && st.GetValueType(0)==VALUE_STRING && st.GetValueType(1)==VALUE_STRING)
			{
				CValueString* x=(CValueString*)st.Top(); CValueString* y=(CValueString*)st.Top();
				st.Push(new CValueBool(x->m_Value==y->m_Value));
				delete x; delete y;
			}
			else if (oper==GREATER && st.GetValueType(0)==VALUE_DOUBLE 
				&& st.GetValueType(1)==VALUE_DOUBLE)
			{
				CValue* x=(CValue*)st.Top(); CValue* y=(CValue*)st.Top();
				st.Push(new CValueBool(x->ToDouble()>y->ToDouble()));
				delete x; delete y;
			}
			else if (oper==LESS && st.GetValueType(0)==VALUE_DOUBLE 
				&& st.GetValueType(1)==VALUE_DOUBLE)
			{
				CValue* x=(CValue*)st.Top(); CValue* y=(CValue*)st.Top();
				st.Push(new CValueBool(x->ToDouble()<y->ToDouble()));
				delete x; delete y;
			}
			else if (oper==GREATER_THAN_OR_EQUAL && st.GetValueType(0)==VALUE_DOUBLE 
				&& st.GetValueType(1)==VALUE_DOUBLE)
			{
				CValue* x=(CValue*)st.Top(); CValue* y=(CValue*)st.Top();
				st.Push(new CValueBool(x->ToDouble()>=y->ToDouble()));
				delete x; delete y;
			}
			else if (oper==LESS_THAN_OR_EQUAL && st.GetValueType(0)==VALUE_DOUBLE 
				&& st.GetValueType(1)==VALUE_DOUBLE)
			{
				CValue* x=(CValue*)st.Top(); CValue* y=(CValue*)st.Top();
				st.Push(new CValueBool(x->ToDouble()<=y->ToDouble()));
				delete x; delete y;
			}
			else if (oper==IF && paramCount==3 && st.GetValueType(2)==VALUE_BOOL)
			{
				CValue* x=(CValue*)st.Top();
				CValue* y=(CValue*)st.Top();
				CValueBool* b=(CValueBool*)st.Top();
				if (b->m_Value) st.Push(y->Clone()); else st.Push(x->Clone());
				delete b;
				delete x; delete y;
			}
			else if (oper==LOG10 && paramCount==1 && st.GetValueType(0)==VALUE_DOUBLE)
			{
				CValue* x=(CValue*)st.Top();
				st.Push(new CValueDouble(log10(x->ToDouble())));
				delete x;
			}
			else if (oper==CBRT && paramCount==1 && st.GetValueType(0)==VALUE_DOUBLE)
			{
				CValue* x=(CValue*)st.Top();
				st.Push(new CValueDouble(pow(x->ToDouble(),1/3)));
				delete x;
			}
			else if (oper==EXP && paramCount==1 && st.GetValueType(0)==VALUE_DOUBLE)
			{
				CValue* x=(CValue*)st.Top();
				st.Push(new CValueDouble(exp(x->ToDouble())));
				delete x;
			}
			else if (oper==FLOOR && paramCount==1 && st.GetValueType(0)==VALUE_DOUBLE)
			{
				CValue* x=(CValue*)st.Top();
				st.Push(new CValueDouble(floor(x->ToDouble())));
				delete x;
			}
			else if (oper==CEIL && paramCount==1 && st.GetValueType(0)==VALUE_DOUBLE)
			{
				CValue* x=(CValue*)st.Top();
				st.Push(new CValueDouble(ceil(x->ToDouble())));
				delete x;
			}
			else if (oper==ROUND && paramCount==1 && st.GetValueType(0)==VALUE_DOUBLE)
			{
				CValue* x=(CValue*)st.Top();
				st.Push(new CValueDouble(Math::Round(x->ToDouble())));
				delete x;
			}
			else if (oper==SIN && paramCount==1 && st.GetValueType(0)==VALUE_DOUBLE)
			{
				CValue* x=(CValue*)st.Top();
				st.Push(new CValueDouble(sin(bRadian ? x->ToDouble() : Math::ConvertToRadian(x->ToDouble()))));
				delete x;
			}
			else if (oper==COS && paramCount==1 && st.GetValueType(0)==VALUE_DOUBLE)
			{
				CValue* x=(CValue*)st.Top();
				st.Push(new CValueDouble(cos(bRadian ? x->ToDouble() : Math::ConvertToRadian(x->ToDouble()))));
				delete x;
			}
			else if (oper==TAN && paramCount==1 && st.GetValueType(0)==VALUE_DOUBLE)
			{
				CValue* x=(CValue*)st.Top();
				st.Push(new CValueDouble(tan(bRadian ? x->ToDouble() : Math::ConvertToRadian(x->ToDouble()))));
				delete x;
			}
			else if (oper==COT && paramCount==1 && st.GetValueType(0)==VALUE_DOUBLE)
			{
				CValue* x=(CValue*)st.Top();
				st.Push(new CValueDouble(1/tan(bRadian ? x->ToDouble() : Math::ConvertToRadian(x->ToDouble()))));
				delete x;
			}
			else if (oper==SINH && paramCount==1 && st.GetValueType(0)==VALUE_DOUBLE)
			{
				CValue* x=(CValue*)st.Top();
				st.Push(new CValueDouble(sinh(bRadian ? x->ToDouble() : Math::ConvertToRadian(x->ToDouble()))));
				delete x;
			}
			else if (oper==COSH && paramCount==1 && st.GetValueType(0)==VALUE_DOUBLE)
			{
				CValue* x=(CValue*)st.Top();
				st.Push(new CValueDouble(cosh(bRadian ? x->ToDouble() : Math::ConvertToRadian(x->ToDouble()))));
				delete x;
			}
			else if (oper==TANH && paramCount==1 && st.GetValueType(0)==VALUE_DOUBLE)
			{
				CValue* x=(CValue*)st.Top();
				st.Push(new CValueDouble(tanh(bRadian ? x->ToDouble() : Math::ConvertToRadian(x->ToDouble()))));
				delete x;
			}
			else if (oper==ARCSIN && paramCount==1 && st.GetValueType(0)==VALUE_DOUBLE)
			{
				CValue* x=(CValue*)st.Top();
				st.Push(new CValueBool(bRadian ? asin(x->ToDouble()) : Math::ConvertFromRadianToAngleUnit(asin(x->ToDouble()))));
				delete x;
			}
			else if (oper==ARCCOS && paramCount==1 && st.GetValueType(0)==VALUE_DOUBLE)
			{
				CValue* x=(CValue*)st.Top();
				st.Push(new CValueBool(bRadian ? acos(x->ToDouble()) : Math::ConvertFromRadianToAngleUnit(acos(x->ToDouble()))));
				delete x;
			}
			else if (oper==ARCTAN && paramCount==1 && st.GetValueType(0)==VALUE_DOUBLE)
			{
				CValue* x=(CValue*)st.Top();
				st.Push(new CValueBool(bRadian ? atan(x->ToDouble()) : Math::ConvertFromRadianToAngleUnit(atan(x->ToDouble()))));
				delete x;
			}
			else if (oper==MAX || oper==MIN && paramCount>0)
			{
				CValue* vl=st.Top();
				double m;
				if (vl->GetType()==VALUE_DOUBLE) { m=vl->ToDouble(); delete vl; }
				else { delete vl; goto error; }
				for (int i=1;i<paramCount;i++)
				{
					vl=st.Top();
					if (vl->GetType()==VALUE_DOUBLE) { m=oper==MAX ? max(vl->ToDouble(),m) : min(vl->ToDouble(),m); delete vl; }
					else { delete vl; goto error; }
				}
				st.Push(new CValueDouble(m));
			}
			else { m_strErr=L"Syntax Error"; goto error; }
			o++;
		}
		else if (m_TypeArr[i]==NUMBER)
		{
			st.Push(m_ValueArr[n]->Clone());

			n++;
		}
		else if (m_TypeArr[i]==VARIABLE)
		{
			CValue* vl=m_ValueExpArr[v]->m_Value;
			st.Push(vl->Clone());

			v++;
		}
		else if (m_TypeArr[i]==VARIABLE_X)
		{
			st.Push(new CValueDouble(vx));
		}
		else if (m_TypeArr[i]==USER_FUNCTION)
		{
			CSafeArrayHelper sfHelper;
			sfHelper.Create(VT_VARIANT, 1, 0, m_ParamCountArr[usf]);

			_variant_t var;
			for (int i=0;i<m_ParamCountArr[usf];i++)
			{
				CValue* vl=st.Top();
				var = _variant_t(vl->GetType()==VALUE_DOUBLE ? ((CValueDouble*)vl)->m_Value : 0);
				sfHelper.PutElement(i, (void*)&var);
				delete vl;
			}

			_variant_t varRet;
			LPSAFEARRAY sa =  sfHelper.GetArray();
			m_ScriptObject->RunProcedure(m_strFuncNameArr[usf],&sa,&varRet);
			
			st.Push(new CValueDouble((double)varRet));

			usf++;
		}
		else //OBJVALUE
		{
			CValue* d=(m_objArr[vn])->GetObjValue(m_objArrValueType[vn]);
			
			st.Push(d);

			vn++;
		}

	}

	if (st.GetSize()==1) m_Value=st.Top();
	else { m_strErr=L"Syntax Error"; goto error; }
endLb:
	return;
error:
	m_bErr=true;
	goto endLb;
}

int CGeoExp::Priority(int op)
{
	if (op==BRACKET) return 0;
	if (op==ADDITION || op==SUBTRACTION || 
		op==EQUAL || op==GREATER || op==LESS ||
		op==GREATER_THAN_OR_EQUAL || op==LESS_THAN_OR_EQUAL) return 1;
	if (op==MULTIPLICATION || op==DIVISION) return 2;
	if (op==POWER) return 3;
	else return -1;
}

CGeoExp::ExpOperator CGeoExp::GetFuncCode(CString str)
{
	if (str==_T("(")) return BRACKET;
	else if (str==L"+") return ADDITION;
	else if (str==L"-") return SUBTRACTION;
	else if (str==L"*") return MULTIPLICATION;
	else if (str==L"/") return DIVISION;
	else if (str==L"^") return POWER;
	else if (str==L"abs") return ABS;
	else if (str==L"=") return EQUAL;
	else if (str==L">") return GREATER;
	else if (str==L"<") return LESS;
	else if (str==L"≥") return GREATER_THAN_OR_EQUAL;
	else if (str==L"≤") return LESS_THAN_OR_EQUAL;
	else if (str==L"sqrt") return SQRT;
	else if (str==L"sin") return SIN;
	else if (str==L"cos") return COS;
	else if (str==L"tan") return TAN;
	else if (str==L"cot") return COT;
	else if (str==L"sinh") return SINH;
	else if (str==L"cosh") return COSH;
	else if (str==L"tanh") return TANH;
	else if (str==L"arcsin") return ARCSIN;
	else if (str==L"arccos") return ARCCOS;
	else if (str==L"arctan") return ARCTAN;
	else if (str==L"arccot") return ARCCOT;
	else if (str==L"and") return AND;
	else if (str==L"or") return OR;
	else if (str==L"not") return NOT;
	else if (str==L"ln") return LN;
	else if (str==L"log") return LOG10;
	else if (str==L"cbrt") return CBRT;
	else if (str==L"exp") return EXP;
	else if (str==L"floor") return FLOOR;
	else if (str==L"ceil") return CEIL;
	else if (str==L"round") return ROUND;
	else if (str==L"max") return MAX;
	else if (str==L"min") return MIN;
	else if (str==L"if") return IF;
	else return NONE;
}

bool CGeoExp::SetExpression(CString strExpression, CArray<CVGObject*> *m_Array, bool bHasX, CStringArray* m_strReplacedNameArr)
{
	m_strExp=strExpression;
	//m_strExp.Replace(L" ",L"");
	CString str=m_strExp;	
	bool b=true;
	while (b)
	{
		b=false;
		b=str.Replace(L"++",L"+") || str.Replace(L"+-",L"-") || 
			str.Replace(L"-+",L"-") || str.Replace(L"--",L"+") || 
			str.Replace(L"(-",L"(0-") || str.Replace(L"(+",L"(0+");
	}
	if (str[0]=='+') str.Delete(0);
	if (str[0]=='-') str=L"0"+str;

	m_TypeArr.RemoveAll();
	for (int i=0;i<m_ValueArr.GetSize();i++) delete m_ValueArr[i];
	m_ValueArr.RemoveAll();
	for (int i=0;i<m_ValueArr.GetSize();i++) delete m_ValueExpArr[i];
	m_ValueExpArr.RemoveAll();
	m_objArr.RemoveAll();
	m_objArrValueType.RemoveAll();
	m_OperatorArr.RemoveAll();
	m_strFuncNameArr.RemoveAll();
	m_ParamCountArr.RemoveAll();
	delete m_Value; m_Value=NULL;

	bool bReturn=FillArray(m_Array, str, bHasX, m_strReplacedNameArr);
	if (bReturn) Calc();
	return bReturn;
}

//VD : 2 dấu cộng ở cạnh nhau thì bỏ một dấu
//Sửa lỗi VD : 5++--3 phải ra 8
void CGeoExp::RemoveUnnecessaryOperate()
{
	bool b=true;
	int nIndex=-1;
	while (b)
	{
		b=false;
		for (int i=0;i<m_TypeArr.GetSize()-1;i++)
		{
			if (m_TypeArr[i]==OPERATOR) 
			{
				nIndex++;
				if (nIndex==m_TypeArr.GetSize()-1) break;
				if (m_TypeArr[i+1]!=OPERATOR) continue;
				if (m_OperatorArr[nIndex]==ADDITION && m_OperatorArr[nIndex+1]==ADDITION) { 
					b=true; m_TypeArr.RemoveAt(i); m_OperatorArr.RemoveAt(nIndex); }
				else if (m_OperatorArr[nIndex]==ADDITION && m_OperatorArr[nIndex+1]==SUBTRACTION) {
					b=true; m_TypeArr.RemoveAt(i); m_OperatorArr.RemoveAt(nIndex); }
				else if (m_OperatorArr[nIndex]==SUBTRACTION && m_OperatorArr[nIndex+1]==ADDITION) {
				}
				else if (m_OperatorArr[nIndex]==SUBTRACTION && m_OperatorArr[nIndex+1]==SUBTRACTION) {
					/*
					b=true; m_TypeArr.RemoveAt(i); m_OperatorArr.RemoveAt(nIndex);
										m_TypeArr.RemoveAt(i); m_OperatorArr.RemoveAt(nIndex);
										m_TypeArr.InsertAt(i,OPERATOR); m_OperatorArr.InsertAt(i,ADDITION);*/
					 }
			}
		}
	}
}

bool CGeoExp::FillArray(CArray<CVGObject*> *m_Array, CString strExpression, bool bHasX, CStringArray* m_strReplacedNameArr)
{
	if (strExpression[0]=='+') strExpression.Delete(0);
	if (strExpression[0]=='-') strExpression=L"0"+strExpression;
	stack <ExpOperator> sOperator;
	double d;
	int i=0;
	int j;
	ExpOperator tmp;
	int tmp2;
	CString str,tmpStr;
	bool tmpFlag;
	bool t=FALSE;
	while (i<strExpression.GetLength())
	{
		t=FALSE;
		j=GetNext(strExpression,i);

		str=strExpression.Mid(i,j); 
		d=wcstod(str,NULL);
		if (str[0]=='"')
		{
			m_TypeArr.Add(NUMBER);
			m_ValueArr.Add(new CValueString(str.Mid(1,str.GetLength()-2)));
			i+=j;
			t=true;
		}
		if (d!=0 || str==_T("0")) //Xu ly cac so
		{
			m_TypeArr.Add(NUMBER);
			m_ValueArr.Add(new CValueDouble(d));
			i+=j;
			t=TRUE;
		}
		if (bHasX && str==L"x")
		{
			m_TypeArr.Add(VARIABLE_X);
			i+=j;
			t=TRUE;
		}
		if (!t && ((str[0]>='a' && str[0]<='z') || 
			(str[0]>='A' && str[0]<='Z'))) // Xu ly cac bien
		{
			if (AddValue(str,m_Array,m_strReplacedNameArr)!=-1)
			{ 
				i+=j;
				t=TRUE;
			}
			else
			{
				CString s=str;
				if (s.MakeLower()==L"pi")
				{
					m_ValueArr.Add(new CValueDouble(pi));
					m_TypeArr.Add(NUMBER);
					i+=j;
					t=TRUE;
				}
			}
		}
		
		if (!t && (str==L"true" || str==L"false"))
		{
			m_ValueArr.Add(str==L"true" ? new CValueBool(true) : new CValueBool(false));
			m_TypeArr.Add(NUMBER);
			i+=j;
			t=TRUE;
		}

		if (!t) // Xu ly phep toan & ham so
		{
			if (str==_T("(")) // Dấu ngoặc
			{
				int pos=GetNextWithBracket(strExpression,i);
				str=strExpression.Mid(i,pos); 
				if (str.Find(',')!=-1) 
				{
					CStringArray a;
					GetParam(str.Mid(1,str.GetLength()-2),&a);
					for (int i=0;i<a.GetSize();i++)
					{
						FillArray(m_Array,a[i],bHasX,m_strReplacedNameArr);
					}
					m_TypeArr.Add(OPERATOR);
					m_OperatorArr.Add(OP_VECTOR); i+=pos;
					m_ParamCountArr.Add(a.GetSize());
				}
				else 
				{ sOperator.push(BRACKET); i++; }
			}
			else if (str==_T(")"))
			{ 
				while (sOperator.top()!=BRACKET)
				{
					m_TypeArr.Add(OPERATOR);
					tmp=sOperator.top();
					m_OperatorArr.Add(tmp);
					m_ParamCountArr.Add(2);
					sOperator.pop();
				}
				sOperator.pop();
				i++;
			}
			else // Phép toán, hàm số
			{
				tmp=GetFuncCode(str);

				if (tmp>10 || m_ScriptObject->Exist(str)) // Ham so ( sqrt , sin , cos , ... )
				{
					i+=j;
 					tmp2=0;tmpFlag=FALSE;j=0;
 					tmpStr=strExpression.Right(strExpression.GetLength()-i);
					int paramCount=0;

 					while (j<tmpStr.GetLength() && (!(tmpFlag==TRUE && tmp2==0)))
 					{
 						if (tmpStr.GetAt(j)=='(')
 						{
 							if (!tmpFlag)
							{
								tmpFlag=TRUE;
								tmpStr=tmpStr.Right(tmpStr.GetLength()-1);
								j--; i--;
							}
 							tmp2++;
 						}
 						else if (tmpStr.GetAt(j)==')') tmp2--;
						else if (tmpStr[j]==',' && tmp2==1)
						{
							CString strParam=tmpStr.Mid(0,j);
							tmpStr=tmpStr.Right(tmpStr.GetLength()-j-1);
							j=-1;
							FillArray(m_Array,strParam,bHasX,m_strReplacedNameArr);
							paramCount++;
						}
						j++; i++;
 					}

 					tmpStr=tmpStr.Mid(0,j-1);
 					FillArray(m_Array,tmpStr,bHasX,m_strReplacedNameArr);
					//i+=j-1;
					i++;

					if (m_ScriptObject->Exist(str)) // User's script
					{
						m_TypeArr.Add(USER_FUNCTION);
						m_strFuncNameArr.Add(str);
					}
					else
					{
						m_OperatorArr.Add(tmp);
						m_TypeArr.Add(OPERATOR);
					}
					m_ParamCountArr.Add(paramCount+1);
				}
				else if (tmp!=NONE) //Phep toan + - * / ^
				{
					if (sOperator.size()!=0)
					{
						tmp=sOperator.top();
						while (Priority(GetFuncCode(str))<=Priority(tmp) && sOperator.size()!=0)
						{
							m_OperatorArr.Add(tmp);
							m_ParamCountArr.Add(2);
							m_TypeArr.Add(OPERATOR);
							sOperator.pop();
							if (sOperator.size()!=0) tmp=sOperator.top();
						}
					}
					sOperator.push(GetFuncCode(str));
					i++;
				}
				else // Các phép toán đặc biệt với đt hình học
				{
					tmp2=1; // Dấu ngoặc
					i+=j;
					j=1;
					tmpStr=strExpression.Right(strExpression.GetLength()-i);
					if (tmpStr[0]=='(')
					{
						while (j<tmpStr.GetLength() && (!(tmp2==0)))
						{
							if (tmpStr.GetAt(j)=='(')
							{
								tmp2++;
							}
							else if (tmpStr.GetAt(j)==')') tmp2--;
							j++;
						}
						tmpStr=tmpStr.Left(j);
						int index=AddValue(str,tmpStr.Mid(1,tmpStr.GetLength()-2),m_Array,m_strReplacedNameArr);
						if (index!=-1) m_TypeArr.Add(VARIABLE);
						else return FALSE;
						i+=j;
						t=TRUE;
					}
					else
					{
						return FALSE;
					}
				}
			}
		}
	}
	while (sOperator.size()!=0)
	{
		m_OperatorArr.Add(sOperator.top());
		m_ParamCountArr.Add(2);
		m_TypeArr.Add(OPERATOR);
		sOperator.pop();
	}
	RemoveUnnecessaryOperate();
	//m_bNeedCalc=m_ValueArr.GetSize()!=0 && m_vgNumberArr.GetSize()!=0;
	return TRUE;
}

//Tìm tên biến, toán tử, ... tiếp theo trong chuỗi
int CGeoExp::GetNext(CString str, int startPos)
{
	int i=startPos;
	if (str[startPos]=='"')
		return str.Find('"',startPos+1)-startPos+1;
	if (startPos>0)
	{
		if ((str[startPos-1]=='*' || str[startPos-1]=='/') && 
			(str[startPos]=='+' || str[startPos]=='-'))
			return GetNext(str,startPos+1)+1;
	}
	bool startByLetter=(str[startPos]>='a' && str[startPos]<='z') || (str[startPos]>='A' && str[startPos]<='Z');
	bool startByNumber=(str[startPos]>='0' && str[startPos]<='9');
	if ((!startByLetter) && (!startByNumber)) return 1;
	do 
	{
		i++;
		if (startByLetter && !(((str[i]>='a' && str[i]<='z') || str[i]=='.' || (str[i]>='0' && str[i]<='9')) || 
			((str[i]>='A' && str[i]<='Z') || str[i]=='.'))) return i-startPos;
		if (startByNumber && !((str[i]>='0' && str[i]<='9') || 
			(str[i]=='.'))) return i-startPos;
	} while (i<str.GetLength());
	return str.GetLength()-1-startPos;
}

int CGeoExp::GetNextWithBracket(CString str, int startPos)
{
	int i=startPos;
	int d=1;
	bool b=FALSE;
	bool startByLetter=(str[startPos]>='a' && str[startPos]<='z') || (str[startPos]>='A' && str[startPos]<='Z');
	bool startByNumber=(str[startPos]>='0' && str[startPos]<='9');
	if ((!startByLetter) && (!startByNumber) && !str[0]=='(') return 1;
	do 
	{
		i++;
		if (str[i]=='(') 
		{ d++; b=TRUE; }
		else if (str[i]==')') { d--; if (d==0) return i-startPos+1; }
		if (!b)
		{
			char c=str[i];
			if (startByLetter && !(((str[i]>='a' && str[i]<='z') || str[i]=='.' || (str[i]>='0' && str[i]<='9')) || 
				((str[i]>='A' && str[i]<='Z') || str[i]=='.'))) return i-startPos;
			if (startByNumber && !((str[i]>='0' && str[i]<='9') || 
				(str[i]=='.'))) return i-startPos;
		}
	} while (i<str.GetLength());
	return str.GetLength()-1-startPos;
}

// Lấy giá trị
// VD : AB trả về độ dài AB
// A.x trả về tọa độ x của A
int CGeoExp::AddValue(CString str, CArray<CVGObject*> *m_Array, CStringArray* m_strReplacedNameArr)
{
	int pos=str.Find('.'); if (pos==-1) pos=str.GetLength();
	CString strObjName=str.Left(pos);
	CString strVari=(pos==str.GetLength()) ? L"" :str.Right(str.GetLength()-pos-1);
	CVGObject* obj=GetObjByName(strObjName,m_Array,m_strReplacedNameArr);
	if (obj!=NULL)
	{
		int i=(obj->GetValueType(strVari));
		if (i==-1) return -1;
		else
		{
			m_objArr.Add(obj);
			m_objArrValueType.Add(i);
			m_TypeArr.Add(OBJVALUE);
			return m_objArr.GetSize()-1;
		}
	}
	else
	{
		CVGPoint* pt1=NULL;
		CVGPoint* pt2=NULL;
		if ((bool)GetPointFromSegment(str,pt1,pt2,m_Array,m_strReplacedNameArr) && pt1!=NULL && pt2!=NULL)
		{
			CValueExp* vl=new CValueExp();

			vl->m_Mode=VALUE_MODE_DISTANCE_POINT_POINT;
			vl->m_Param.Add(pt1);
			vl->m_Param.Add(pt2);
			m_ValueExpArr.Add(vl);
			m_TypeArr.Add(VARIABLE);
			return m_ValueExpArr.GetSize()-1;
		}
		else return -1;
	}

// 	if (pos==-1)
// 	{
// 		CVGObject* obj=GetObjByName(str,m_Array,m_strReplacedNameArr);
// 		if (obj)
// 		{
// 			CValueExp* vl=new CValueExp();
// 			switch (obj->m_Type)
// 			{
// 			case OBJ_CHECKBOX:
// 				{
// 					vl->m_Mode=VALUE_MODE_CHECKBOX;
// 					vl->m_Param.Add(obj);
// 				}
// 			}
// 			m_ValueArr.Add(vl);
// 		}
// 		else
// 		{
// 			
// 		}
// 	}
// 	else
// 	{

// 		CValueExp* vl=new CValueExp();
// 		switch (obj->m_Type)
// 		{
// 		case OBJ_POINT:
// 			{
// 				if (strVari==L"x")
// 				{
// 					vl->m_Mode=VALUE_MODE_POINT_X;
// 					vl->m_Param.Add(obj);
// 				}
// 				else if (strVari==L"y")
// 				{
// 					vl->m_Mode=VALUE_MODE_POINT_Y;
// 					vl->m_Param.Add(obj);
// 				}
// 
// 				break;
// 			}
// 		case OBJ_CIRCLE:
// 			{
// 				if (strVari==L"Center.x")
// 				{
// 					vl->m_Mode=VALUE_MODE_CIRCLE_CENTER_X;
// 					vl->m_Param.Add(obj);
// 				}
// 				else if (strVari==L"Center.y")
// 				{
// 					vl->m_Mode=VALUE_MODE_CIRCLE_CENTER_Y;
// 					vl->m_Param.Add(obj);
// 				}
// 				else if (strVari==L"Radius" || strVari==L"R")
// 				{
// 					vl->m_Mode=VALUE_MODE_CIRCLE_RADIUS;
// 					vl->m_Param.Add(obj);
// 				}
// 
// 				break;
// 			}
// 		}
// 
// 		m_ValueArr.Add(vl);
// 	}
}

CVGObject* CGeoExp::GetObjByName(CString strName, CArray<CVGObject*> *m_Array, CStringArray* m_strReplacedNameArr)
{
	if (m_strReplacedNameArr==NULL)
	{
		for (int i=0;i<m_Array->GetSize();i++)
			if (strName==(*m_Array)[i]->m_Name && !(m_Array->GetAt(i)->m_Type==OBJ_POINT_HIDDEN_CHILD)) return (*m_Array)[i];
	}
	else
	{
		for (int i=0;i<m_strReplacedNameArr->GetSize();i++)
			if (strName==(*m_strReplacedNameArr)[i] && !(m_Array->GetAt(i)->m_Type==OBJ_POINT_HIDDEN_CHILD)) return (*m_Array)[i];
		for (int i=m_strReplacedNameArr->GetSize();i<m_Array->GetSize();i++)
			if (strName==(*m_Array)[i]->m_Name && !(m_Array->GetAt(i)->m_Type==OBJ_POINT_HIDDEN_CHILD)) return (*m_Array)[i];
	}
	return NULL;
}

//Phép toán trên đối tượng hình học
int CGeoExp::AddValue(CString strCmdName, CString strParam, CArray<CVGObject*> *m_Array, CStringArray* m_strReplacedNameArr)
{
	CStringArray aParam;
	GetParam(strParam,&aParam);
	CArray<CVGObject*> m_objArr;
	for (int i=0;i<aParam.GetSize();i++)
	{
		CVGObject* obj=GetObjByName(aParam[i],m_Array,m_strReplacedNameArr);
		if (obj==NULL)
		{
			CVGPoint* pt1=NULL; CVGPoint* pt2=NULL;
			if (GetPointFromSegment(aParam[i],pt1,pt2,m_Array,m_strReplacedNameArr))
			{
				m_objArr.Add(pt1);
				m_objArr.Add(pt2);
			}
			else return -1;
		}
		else
			m_objArr.Add(obj);
	}

	CValueExp* vl=new CValueExp();
	if (strCmdName==L"Distance")
	{
		if (m_objArr.GetSize()==2 && m_objArr[0]->m_Type==OBJ_POINT && m_objArr[1]->m_Type==OBJ_POINT)
		{
			vl->m_Mode=VALUE_MODE_DISTANCE_POINT_POINT;
			vl->m_Param.Add(m_objArr[0]); vl->m_Param.Add(m_objArr[1]);
		}
		else if (m_objArr.GetSize()==2 && m_objArr[0]->m_Type==OBJ_POINT && (IsLineType(m_objArr[1]->m_Type)))
		{
			vl->m_Mode=VALUE_MODE_DISTANCE_POINT_LINE;
			vl->m_Param.Add(m_objArr[0]); vl->m_Param.Add(m_objArr[1]);
		}
	}
	else if (strCmdName==L"IsPerpendicular")
	{
		if (m_objArr.GetSize()==2 && (IsLineType(m_objArr[0]->m_Type)) && (IsLineType(m_objArr[1]->m_Type)))
		{
			vl->m_Mode=VALUE_MODE_PERPENDICULAR;
			vl->m_Param.Add(m_objArr[0]); vl->m_Param.Add(m_objArr[1]);
		}
	}
	else if (strCmdName==L"IsParallel")
	{
		if (m_objArr.GetSize()==2 && (IsLineType(m_objArr[0]->m_Type)) && (IsLineType(m_objArr[1]->m_Type)))
		{
			vl->m_Mode=VALUE_MODE_PARALLEL;
			vl->m_Param.Add(m_objArr[0]); vl->m_Param.Add(m_objArr[1]);
		}
	}
	else if (strCmdName==L"IsPointOnLine")
	{
		if (m_objArr.GetSize()==2 && m_objArr[0]->m_Type==OBJ_POINT && (IsLineType(m_objArr[1]->m_Type)))
		{
			vl->m_Mode=VALUE_MODE_POINT_IN_LINE;
			vl->m_Param.Add(m_objArr[0]); vl->m_Param.Add(m_objArr[1]);
		}
	}
	else if (strCmdName==L"IsPointOnSegment")
	{
		if (m_objArr.GetSize()==3 && m_objArr[0]->m_Type==OBJ_POINT && m_objArr[1]->m_Type==OBJ_POINT && m_objArr[2]->m_Type==OBJ_POINT)
		{
			vl->m_Mode=VALUE_MODE_POINT_IN_SEGMENT;
			vl->m_Param.Add(m_objArr[0]); vl->m_Param.Add(m_objArr[1]); vl->m_Param.Add(m_objArr[2]);
		}
	}
	else if (strCmdName==L"IsPointOnCircle")
	{
		if (m_objArr.GetSize()==2 && m_objArr[0]->m_Type==OBJ_POINT && m_objArr[1]->m_Type==OBJ_CIRCLE)
		{
			vl->m_Mode=VALUE_MODE_POINT_IN_CIRCLE;
			vl->m_Param.Add(m_objArr[0]); vl->m_Param.Add(m_objArr[1]);
		}
	}
	else if (strCmdName==L"IsTangent")
	{
		if (m_objArr.GetSize()==2 && (IsLineType(m_objArr[0]->m_Type)) && m_objArr[1]->m_Type==OBJ_CIRCLE)
		{
			vl->m_Mode=VALUE_MODE_TANGENT_LINE_CIRCLE;
			vl->m_Param.Add(m_objArr[0]); vl->m_Param.Add(m_objArr[1]);
		}
		if (m_objArr.GetSize()==2 && m_objArr[0]->m_Type==OBJ_CIRCLE && (IsLineType(m_objArr[1]->m_Type)))
		{
			vl->m_Mode=VALUE_MODE_TANGENT_LINE_CIRCLE;
			vl->m_Param.Add(m_objArr[1]); vl->m_Param.Add(m_objArr[0]);
		}
		else if (m_objArr.GetSize()==2 && m_objArr[0]->m_Type==OBJ_CIRCLE && m_objArr[1]->m_Type==OBJ_CIRCLE)
		{
			vl->m_Mode=VALUE_MODE_TANGENT_CIRCLE_CIRCLE;
			vl->m_Param.Add(m_objArr[0]); vl->m_Param.Add(m_objArr[1]);
		}
	}
	else if (strCmdName==L"IsCollinear")
	{
		if (m_objArr.GetSize()==3 && m_objArr[0]->m_Type==OBJ_POINT && m_objArr[1]->m_Type==OBJ_POINT && m_objArr[2]->m_Type==OBJ_POINT)
		{
			vl->m_Mode=VALUE_MODE_COLLINEAR;
			vl->m_Param.Add(m_objArr[0]); vl->m_Param.Add(m_objArr[1]); vl->m_Param.Add(m_objArr[2]);
		}
	}
	else if (strCmdName==L"IsConcurrent")
	{
		if (m_objArr.GetSize()==3 && (IsLineType(m_objArr[0]->m_Type)) && (IsLineType(m_objArr[1]->m_Type)) && (IsLineType(m_objArr[2]->m_Type)))
		{
			vl->m_Mode=VALUE_MODE_CONCURRENT;
			vl->m_Param.Add(m_objArr[0]); vl->m_Param.Add(m_objArr[1]); vl->m_Param.Add(m_objArr[2]);
		}
	}
	else if (strCmdName==L"Angle")
	{
		if (m_objArr.GetSize()==2 && (IsLineType(m_objArr[0]->m_Type)) && (IsLineType(m_objArr[1]->m_Type)))
		{
			vl->m_Mode=VALUE_MODE_ANGLE_LINE_LINE;
			vl->m_Param.Add(m_objArr[0]); vl->m_Param.Add(m_objArr[1]);
		}
		if (m_objArr.GetSize()==3 && m_objArr[0]->m_Type==OBJ_POINT && m_objArr[1]->m_Type==OBJ_POINT && m_objArr[2]->m_Type==OBJ_POINT)
		{
			vl->m_Mode=VALUE_MODE_ANGLE_POINT_POINT_POINT;
			vl->m_Param.Add(m_objArr[0]); vl->m_Param.Add(m_objArr[1]); vl->m_Param.Add(m_objArr[2]);
		}
	}
	else if (strCmdName==L"DirectedLength")
	{
		if (m_objArr.GetSize()==2 && m_objArr[0]->m_Type==OBJ_POINT && m_objArr[1]->m_Type==OBJ_POINT)
		{
			vl->m_Mode=VALUE_MODE_DIRECTED_LENGTH;
			vl->m_Param.Add(m_objArr[0]); vl->m_Param.Add(m_objArr[1]);
		}
	}
	else if (strCmdName==L"S" || strCmdName==L"Area")
	{
		if (m_objArr.GetSize()==1 && m_objArr[0]->m_Type==OBJ_POLYGON)
		{
			vl->m_Mode=VALUE_MODE_AREA;
			vl->m_Param.Add(m_objArr[0]);
		}
	}
	else if (strCmdName==L"OrientedArea")
	{
		if (m_objArr.GetSize()==1 && m_objArr[0]->m_Type==OBJ_POLYGON)
		{
			vl->m_Mode=VALUE_MODE_ORIENTED_AREA;
			vl->m_Param.Add(m_objArr[0]);
		}
	}
	else return -1;

	m_ValueExpArr.Add(vl);
	return m_ValueExpArr.GetSize()-1;
	return -1;
}

void CGeoExp::GetParam(CString strParam, CStringArray* aParam)
{
	int i=0;
	int j=i;
	int br=0;
	for (i;i<strParam.GetLength()-1;i++)
	{
		if ((strParam[i]==',') && (br==0)){aParam->Add(strParam.Mid(j,i-j)); j=i+1;}
		else if (strParam[i]=='(') br++;
		else if (strParam[i]==')') br--;
	}
	aParam->Add(strParam.Mid(j,i-j+1));
}

bool CGeoExp::GetPointFromSegment(CString segName, CVGPoint*& pt1, CVGPoint*& pt2, CArray<CVGObject*>* objArr, CStringArray* m_strReplacedNameArr)
{
	if ((segName[0]>='a' && segName[0]<='z') || (segName[0]>='A' && segName[0]<='Z'))
	{
		for (int i=1;i<segName.GetLength();i++)
			if ((segName[i]>='a' && segName[i]<='z') || (segName[i]>='A' && segName[i]<='Z'))
			{
				pt1=(CVGPoint*)GetObjByName(segName.Left(i),objArr,m_strReplacedNameArr);
				pt2=(CVGPoint*)GetObjByName(segName.Right(segName.GetLength()-i),objArr,m_strReplacedNameArr);
				return (pt1!=NULL && pt2!=NULL);
			}
	}
	return 0;
}

bool CGeoExp::CanCalc()
{
	for (int i=0;i<m_ValueExpArr.GetSize();i++)
		if (!m_ValueExpArr[i]->CanCalc()) return FALSE;
	for (int i=0;i<m_objArr.GetSize();i++)
		if (!(m_objArr[i])->CanCalc()) return FALSE;
	return TRUE;
}

CString CGeoExp::GetStringValue()
{
	return m_bErr ? m_strErr : m_Value!=NULL ? m_Value->ToString() : L"Error";
}