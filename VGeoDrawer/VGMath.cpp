#include "stdafx.h"
#include <math.h>
#include "AxisInfo.h"
#include "VGMath.h"

extern int numRound;
extern bool bRadian;

BOOL Math::Equal(double i1, double i2)
{
	return (abs(i1-i2)<0.00000001);
}

int Math::Round(double d)
{
	int i=floor(d);
	if (d-i<0.5) return i;
	else return i+1;
}

int Math::SolveQuadricFunction( double a, double b, double c, double& x1, double& x2 )
{
	if (Math::Equal(a,0)) { if (Math::Equal(b,0)) return 0; else { x1=-c/b; return 1; } }
	int numRoot;
	double delta=b*b-4*a*c;
	if (Equal(delta,0))
	{
		numRoot=1;
		x1=-b/(2*a);
		x2=x1;
	}
	else if (delta<0) 
	{
		numRoot=0;
	}
	else
	{
		numRoot=2;
		x1=(-b+sqrt(delta))/(2*a);
		x2=(-b-sqrt(delta))/(2*a);
	}
	return numRoot;
}

int Math::SolveQuadricFunctionFr(CValueFractionEx& a, CValueFractionEx& b, CValueFractionEx& c, CValueFractionEx& x1, CValueFractionEx& x2)
{
	int numRoot;
	CValueFractionEx delta=b*b-a*c*4;
	if (Math::Equal(delta.ToDouble(),0))
	{
		numRoot=1;
		x1=-b/(a*2);
		x2=x1;
	}
	else if (delta.ToDouble()<0) 
	{
		numRoot=0;
	}
	else
	{
		numRoot=2;
		delta=delta.Sqrt();
		x1=(-b+delta)/(a*2);
		x2=(-b-delta)/(a*2);
	}
	return numRoot;
}

bool Math::IsInt(double d)
{
	return (d-floor(d)<0.00000000000001 || ceil(d)-d<0.00000000000001);
}

void Math::LineThroughTwoPoints(double x1, double y1, double x2, double y2, double& a, double& b, double& c)
{
	a = y2 - y1;
	b = x1 - x2;
	c = x1 * ( y1 - y2 ) + y1 * ( x2 - x1 );
}

void Math::LineThroughTwoPointsFr(CValueFractionEx x1, CValueFractionEx y1, CValueFractionEx x2, CValueFractionEx y2, CValueFractionEx& a, CValueFractionEx& b, CValueFractionEx& c)
{
	a = y2 - y1;
	b = x1 - x2;
	c = (x1 * ( y1 - y2 ) + y1 * ( x2 - x1 ));
}

void Math::AngleBisector(double a1, double b1, double c1, double a2, double b2, double c2, double& a3, double& b3, double& c3, double& a4, double& b4, double& c4)
{
	double x1=sqrt(a1*a1 + b1*b1);
	double x2=sqrt(a2*a2 + b2*b2);
	a3=a1/x1+a2/x2;
	b3=b1/x1+b2/x2;
	c3=c1/x1+c2/x2;

	a4=a1/x1-a2/x2;
	b4=b1/x1-b2/x2;
	c4=c1/x1-c2/x2;
}

void Math::AngleBisectorFr(CValueFractionEx& a1, CValueFractionEx& b1, CValueFractionEx& c1, CValueFractionEx& a2, CValueFractionEx& b2, CValueFractionEx& c2, CValueFractionEx& a3, CValueFractionEx& b3, CValueFractionEx& c3, CValueFractionEx& a4, CValueFractionEx& b4, CValueFractionEx& c4)
{
	CValueFractionEx x1=(a1*a1 + b1*b1); x1=x1.Sqrt();
	CValueFractionEx x2=(a2*a2 + b2*b2); x2=x2.Sqrt();
	a3=a1/x1+a2/x2;
	b3=b1/x1+b2/x2;
	c3=c1/x1+c2/x2;

	a4=a1/x1-a2/x2;
	b4=b1/x1-b2/x2;
	c4=c1/x1-c2/x2;
}

BOOL Math::GetIntersectionLineLine(double a1, double b1, double c1, double a2, double b2, double c2, double& x, double& y)
{
	double d=a1*b2-a2*b1;
	if (Equal(d,0))
	{
		return FALSE;
	}
	else
	{
		x = ( b1 * c2 - c1 * b2 ) / ( d );
		y = - ( a1 * c2 - c1 * a2 ) / ( d );
		return TRUE;
	}
}

BOOL Math::GetIntersectionLineLineFr(CValueFractionEx& a1, CValueFractionEx& b1, CValueFractionEx& c1, CValueFractionEx& a2, CValueFractionEx& b2, CValueFractionEx& c2, CValueFractionEx& x, CValueFractionEx& y)
{
	CValueFractionEx d = a1 * b2 - a2 * b1;
	if (Equal(d.ToDouble(),0))
	{
		return FALSE;
	}
	else
	{
		x = (CValueFractionEx) ( b1 * c2 - c1 * b2 ) / d ;
		y = (CValueFractionEx) - ( a1 * c2 - c1 * a2 ) / d ;
		return TRUE;
	}
}

int Math::GetIntersectionLineCircle(double centerX, double centerY, double r, double A, double B, double C, double& x1, double & y1, double& x2, double& y2)
{
	if (!Equal(B,0))
	{
		int i=SolveQuadricFunction(1+(A*A)/(B*B),
			-2*centerX + 2*A/B * (C/B + centerY),
			centerX * centerX + pow(C/B + centerY,2)-r*r,
			x1,x2);
		y1=( - C - A * x1 ) / ( B );
		y2=( - C - A * x2 ) / ( B );
		return i;
	}
	else
	{
		x1=x2=-C/A;
		double k=r*r-pow((x1-centerX),2);  // (y-y_center)^2
		if (k<0) return 0;
		else if (k==0) 
		{
			y1=y2=centerY;
			return 1;
		}
		else
		{
			y1=sqrt(k)+centerY;
			y2=-sqrt(k)+centerY;
			return 2;
		}
	}
}

int Math::GetIntersectionLineCircleFr(CValueFractionEx& centerX, CValueFractionEx& centerY, CValueFractionEx& r, CValueFractionEx& A, CValueFractionEx& B, CValueFractionEx& C, CValueFractionEx& x1, CValueFractionEx & y1, CValueFractionEx& x2, CValueFractionEx& y2)
{
	if (!Equal(B.ToDouble(),0))
	{
		CValueFractionEx fA=(A*A)/(B*B)+1;
		CValueFractionEx fB=-centerX*2 + A/B*2 * (C/B + centerY);
		CValueFractionEx fC=centerX * centerX + (C/B + centerY)*(C/B + centerY)-r*r;
		int i=SolveQuadricFunctionFr(fA,fB,fC,x1,x2);
		y1=( - C - A * x1 ) / ( B );
		y2=( - C - A * x2 ) / ( B );
		return i;
	}
	else
	{
		x1=-C/A; x2=x1;
		CValueFractionEx k=r*r-(x1-centerX)*(x1-centerX);  // (y-y_center)^2
		if (k.ToDouble()<0) return 0;
		else if (k.ToDouble()==0) 
		{
			y1=centerY; y2=y1;
			return 1;
		}
		else
		{
			k=k.Sqrt();
			y1=k+centerY;
			y2=k+centerY;
			return 2;
		}
	}
}

int Math::GetIntersectionCircleCircle(double centerX1, double centerY1, double r1,double centerX2, double centerY2, double r2, double &x1, double &y1, double &x2, double& y2)
{
	double A = 2 * centerX1 - 2 * centerX2;
	double B = 2 * centerY1 - 2 * centerY2;
	double C = centerX2 * centerX2 - centerX1 * centerX1
		+ centerY2 * centerY2 - centerY1 * centerY1
		+ r1 * r1 - r2 * r2;
	int numRoot=GetIntersectionLineCircle(centerX1,centerY1,r1,A,B,C,x1,y1,x2,y2);
	return numRoot;
}

int Math::GetIntersectionCircleCircleFr(CValueFractionEx& centerX1, CValueFractionEx& centerY1, CValueFractionEx& r1,CValueFractionEx& centerX2, CValueFractionEx& centerY2, CValueFractionEx& r2, CValueFractionEx &x1, CValueFractionEx& y1, CValueFractionEx& x2, CValueFractionEx& y2)
{
	CValueFractionEx A = centerX1*2 - centerX2*2;
	CValueFractionEx B = centerY1*2 - centerY2*2;
	CValueFractionEx C = centerX2 * centerX2 - centerX1 * centerX1
		+ centerY2 * centerY2 - centerY1 * centerY1
		+ r1 * r1 - r2 * r2;
	int numRoot=GetIntersectionLineCircleFr(centerX1,centerY1,r1,A,B,C,x1,y1,x2,y2);
	return numRoot;
}

int Math::GetIntersectionLineConic(double a, double b, double c, double d, double e, double f, double la,double lb, double lc, double& x1, double &y1, double &x2, double &y2)
{
	if (Math::Equal(lb,0))
	{
// 		x1=x2=-lc/la;
// 		double y=(1-x1*x1/ea)*eb; y1=sqrt(y); y2=-y1;
// 		return (y>0 ? 2 : (Math::Equal(y,0) ? 1 : 0));
	}
	else
	{
		int numRoot=SolveQuadricFunction(
			a+b*la*la/(lb*lb)-c*la/lb,
			2*b*la*lc/(lb*lb)-c*lc/lb+d-e*la/lb,
			b*lc*lc/(lb*lb)-e*lc/lb+f,x1,x2);
		y1=( - lc - la * x1 ) / ( lb );
		y2=( - lc - la * x2 ) / ( lb );
		return numRoot;
	}
}

int Math::GetIntersectionLineConicFr(CValueFractionEx& a, CValueFractionEx& b, CValueFractionEx& c, CValueFractionEx& d, CValueFractionEx& e, CValueFractionEx& f, CValueFractionEx& la,CValueFractionEx& lb, CValueFractionEx& lc, CValueFractionEx& x1, CValueFractionEx& y1, CValueFractionEx& x2, CValueFractionEx& y2)
{
	if (Math::Equal(lb.ToDouble(),0))
	{
// 		x1=-lc/la; x2=x1;
// 		CValueFractionEx y=(-x1*x1/ea+1)*eb; y=y.Sqrt(); y1=y; y2=-y1;
// 		return (y.ToDouble()>0 ? 2 : (Math::Equal(y.ToDouble(),0) ? 1 : 0));
	}
	else
	{
		int numRoot=SolveQuadricFunctionFr(
			a+b*la*la/(lb*lb)-c*la/lb,
			b*2*la*lc/(lb*lb)-c*lc/lb+d-e*la/lb,
			b*lc*lc/(lb*lb)-e*lc/lb+f,x1,x2);
		y1=( - lc - la * x1 ) / ( lb );
		y2=( - lc - la * x2 ) / ( lb );
		return numRoot;
	}
	return 0;
}

void Math::GetParallelLine(double x, double y, double a0, double b0, double c0, double& a, double& b, double& c)
{
	a = a0;
	b = b0;
	c = - a0 * x - y * b0;
}

void Math::GetParallelLineFr(CValueFractionEx& x, CValueFractionEx& y, CValueFractionEx& a0, CValueFractionEx& b0, CValueFractionEx& c0, CValueFractionEx& a, CValueFractionEx& b, CValueFractionEx& c)
{
	a = a0;
	b = b0;
	c = - a0 * x - y * b0;
}

void Math::GetPerpendicularLine(double x, double y, double a0, double b0, double c0, double& a, double& b, double& c)
{
	a = -b0;
	b = a0;
	c = b0 * x - a0 * y;
}

void Math::GetPerpendicularLineFr(CValueFractionEx& x, CValueFractionEx& y, CValueFractionEx& a0, CValueFractionEx& b0, CValueFractionEx& c0, CValueFractionEx& a, CValueFractionEx& b, CValueFractionEx& c)
{
	a = -b0;
	b = a0;
	c = b0 * x - a0 * y;
}


// Tìm khoảng cách giữa hai điểm
double Math::GetDistance(double x1, double y1, double x2, double y2)
{
	return sqrt(pow(x1-x2,2)+pow(y1-y2,2));
}

CValueFractionEx Math::GetDistanceFr(CValueFractionEx& x1, CValueFractionEx& y1, CValueFractionEx& x2, CValueFractionEx& y2)
{
	CValueFractionEx f=(x1-x2)*(x1-x2)+(y1-y2)*(y1-y2);
	f=f.Sqrt();
	return f;
}

double Math::GetDistanceDS( double x1, double y1, double x2, double y2 )
{
	double d=GetDistance(x1,y1,x2,y2);
	double x=x2-x1;
	double y=y2-y1;
	if (Equal(y,0))
	{
		if (x>0) return d;
		else if (x<0) return -d;
		else return 0;
	}
	else if (y>0) return d;
	else if (y<0) return -d;
	return 0;
}

CValueFractionEx Math::GetDistanceDSFr( CValueFractionEx& x1, CValueFractionEx& y1, CValueFractionEx& x2, CValueFractionEx& y2 )
{
	CValueFractionEx d=GetDistanceFr(x1,y1,x2,y2);
	double x=x2.ToDouble()-x1.ToDouble();
	double y=y2.ToDouble()-y1.ToDouble();
	if (Equal(y,0))
	{
		if (x>0) return d;
		else if (x<0) return -d;
		else return 0;
	}
	else if (y>0) return d;
	else if (y<0) return -d;
	return 0;
}

double Math::GetDistance(double x, double y, double A, double B, double C)
{
	return abs( A* x + B * y + C ) / sqrt( A*A + B*B );
}

CValueFractionEx Math::GetDistanceFr(CValueFractionEx& x, CValueFractionEx& y, CValueFractionEx& A, CValueFractionEx& B, CValueFractionEx& C)
{
	return ( A*x + B*y + C ).Abs() / (A*A + B*B).Sqrt();
}

bool Math::IsPtInSegment(double x, double y, double x1, double y1, double x2, double y2)
{
	double vx1=x-x1; double vy1=y-y1;
	double vx2=x-x2; double vy2=y-y2;
	return (Math::Equal(vx1/vx2,vy1/vy2) &&
		((vx1>=0 && vx2<=0) || (vx1<=0 && vx2>=0)) && ((vy1>=0 && vy2<=0) || (vy1<=0 && vy2>=0)));
}

// Tìm điểm chia trong (x1,y1) và (x2,y2) theo tỉ số k
void Math::GetPointByRatio(double x1, double y1, double x2, double y2, double k, double& x, double& y)
{
	x = ( x1 - x2 * k ) / ( 1 - k );
	y = ( y1 - y2 * k ) / ( 1 - k );
}

void Math::PointOnLine(double x0, double y0, double a, double b, double c, double length, double& x1, double& y1, double& x2, double& y2)
{
	// Giải hệ (x-x0)^2+(y-y0)^2=l^2; a(x-x0)+b(y-y0)=0
	if (!Equal(b,0))
	{
		double x=length/sqrt(1+(a*a)/(b*b));
		x1=x0+x;
		y1=-a/b*x+y0;
		x2=x0-x;
		y2=a/b*x+y0;
	}
	else
	{
		x1=x0; x2=x0;
		y1=y0-length;
		y2=y0+length;
	}
}

void Math::PointOnLineFr(CValueFractionEx& x0, CValueFractionEx& y0, CValueFractionEx& a, CValueFractionEx& b, CValueFractionEx& c, CValueFractionEx& length, CValueFractionEx& x1, CValueFractionEx& y1, CValueFractionEx& x2, CValueFractionEx& y2)
{
	// Giải hệ (x-x0)^2+(y-y0)^2=l^2; a(x-x0)+b(y-y0)=0
	if (!Equal(b.ToDouble(),0))
	{
		CValueFractionEx x=length/((a*a)/(b*b)+1).Sqrt();
		x1=x0+x;
		y1=-a/b*x+y0;
		x2=x0-x;
		y2=a/b*x+y0;
	}
	else
	{
		x1=x0; x2=x0;
		y1=y0-length;
		y2=y0+length;
	}
}


void Math::PointOnLineDS(double x0, double y0, double a, double b, double c, double length, double& x, double& y)
{
	double x1,y1,x2,y2;
	PointOnLine(x0,y0,a,b,c,length,x1,y1,x2,y2);

	BOOL positive=TRUE;
	double x3=x1-x0;
	double y3=y1-y0;
	
	if ((y3<0) || ((y3==0) && (x3<0))) positive=FALSE;

	if (positive==(length>0)) 
	{
		x=x1;y=y1;
	} 
	else 
	{
		x=x2;y=y2;
	}
}

void Math::PointOnLineDSFr(CValueFractionEx& x0, CValueFractionEx& y0, CValueFractionEx& a, CValueFractionEx& b, CValueFractionEx& c, CValueFractionEx& length, CValueFractionEx& x, CValueFractionEx& y)
{
	CValueFractionEx x1,y1,x2,y2;
	PointOnLineFr(x0,y0,a,b,c,length,x1,y1,x2,y2);

	BOOL positive=TRUE;
	CValueFractionEx x3=x1-x0;
	CValueFractionEx y3=y1-y0;

	if ((y3.ToDouble()<0) || ((y3.ToDouble()==0) && (x3.ToDouble()<0))) positive=FALSE;

	if (positive==(length.ToDouble()>0)) 
	{
		x=x1;y=y1;
	} 
	else 
	{
		x=x2;y=y2;
	}
}

CString Math::DoubleToString(double d)
{
	CString str;
	char * buf = 0;
	int err;	

	int decimal;
	int sign;

	buf = (char*) malloc(_CVTBUFSIZE);
	err = _fcvt_s(buf, _CVTBUFSIZE, d, numRound, &decimal, &sign);

	if (err != 0) return NULL;

	str=buf;
	delete[] buf;
	if (str.GetLength()<=numRound) str.Insert(0,L"0.");
	else str.Insert(str.GetLength()-numRound,'.');
	while (str[str.GetLength()-1]=='0') str.Delete(str.GetLength()-1);
	if (str[str.GetLength()-1]=='.') str.Delete(str.GetLength()-1);
	if (sign!=0) str=L"-"+str;
	return str;
}

CString Math::IntToString( int i )
{
	char buffer[10];
	_itoa_s( i, buffer, 10, 10 );
	return CString(buffer);
}

BOOL Math::IsPerpendicular( double a1, double b1, double c1, double a2, double b2, double c2 )
{
	if (Equal(a1/-b2,b1/a2)) return TRUE;
	else if ((Equal(b1,0) && Equal(a2,0)) || (Equal(a1,0) && Equal(b2,0))) return TRUE;
	else return FALSE;
}

BOOL Math::IsParallel( double a1, double b1, double c1, double a2, double b2, double c2 )
{
	if (Equal(a1/a2,b1/b2)) return TRUE;
	else if ((Equal(a1,0) && Equal(a2,0)) || (Equal(b1,0) && Equal(b2,0))) return TRUE;
	else return FALSE;
}

void Math::GetTwoPointsOnLine(double a, double b, double c, double& x1, double& y1, double& x2, double& y2)
{
	if (Equal(a,0))
	{
		x1=-1; y1=-c/b;
		x2=1; y2=-c/b;
	}
	else if (Equal(b,0))
	{
		x1=-c/a;y1=-1;
		x2=x1,y2=1;
	}
	else
	{
		x1=0; y1=-c/b;
		x2=-c/a; y2=0;
	}
}

void Math::GetTwoPointsOnLineFr(CValueFractionEx& a, CValueFractionEx& b, CValueFractionEx& c, CValueFractionEx& x1, CValueFractionEx& y1, CValueFractionEx& x2, CValueFractionEx& y2)
{
	if (Equal(a.ToDouble(),0))
	{
		x1.FromNumber(-1); y1=-c/b;
		x2.FromNumber(1); y2=-c/b;
	}
	else if (Equal(b.ToDouble(),0))
	{
		x1=-c/a;y1.FromNumber(-1);
		x2=x1,y2.FromNumber(1);
	}
	else
	{
		x1.FromNumber(0); y1=-c/b;
		x2=-c/a; y2.FromNumber(0);
	}
}

void Math::GetReflectLine( double a1, double b1, double c1, double a2, double b2, double c2, double& a, double& b, double& c )
{
	double x1,y1,x2,y2;
	GetTwoPointsOnLine(a1,b1,c1,x1,y1,x2,y2);
	double x3,y3,x4,y4;
	GetReflectPoint(x1,y1,a2,b2,c2,x3,y3);
	GetReflectPoint(x2,y2,a2,b2,c2,x4,y4);
	LineThroughTwoPoints(x3,y3,x4,y4,a,b,c);
}

void Math::GetReflectLineFr( CValueFractionEx& a1, CValueFractionEx& b1, CValueFractionEx& c1, CValueFractionEx& a2, CValueFractionEx& b2, CValueFractionEx& c2, CValueFractionEx& a, CValueFractionEx& b, CValueFractionEx& c )
{
	CValueFractionEx x1,y1,x2,y2;
	GetTwoPointsOnLineFr(a1,b1,c1,x1,y1,x2,y2);
	CValueFractionEx x3,y3,x4,y4;
	GetReflectPointFr(x1,y1,a2,b2,c2,x3,y3);
	GetReflectPointFr(x2,y2,a2,b2,c2,x4,y4);
	LineThroughTwoPointsFr(x3,y3,x4,y4,a,b,c);
}

void Math::GetReflectPoint( double x,double y,double a,double b,double c,double& x1,double& y1 )
{
	GetIntersectionLineLine(a,b,c,-b,a,b * x - a * y,x1,y1);
	x1=2*x1-x;
	y1=2*y1-y;
}

void Math::GetReflectPointFr( CValueFractionEx& x,CValueFractionEx& y,CValueFractionEx& a,CValueFractionEx& b,CValueFractionEx& c,CValueFractionEx& x1,CValueFractionEx& y1 )
{
	GetIntersectionLineLineFr(a,b,c,-b,a,b * x - a * y,x1,y1);
	x1=x1*2-x;
	y1=y1*2-y;
}

void Math::GetReflectPoint(double x1, double y1, double x2, double y2, double& x, double& y)
{
	x=2*x2-x1;
	y=2*y2-y1;
}

void Math::GetReflectPointFr(CValueFractionEx& x1, CValueFractionEx& y1, CValueFractionEx& x2, CValueFractionEx& y2, CValueFractionEx& x, CValueFractionEx& y)
{
	x=x2*2-x1;
	y=y2*2-y1;
}

void Math::GetReflectLine(double a1, double b1, double c1, double x, double y, double& a, double& b, double& c)
{
	double x1,y1,x2,y2;
	GetTwoPointsOnLine(a1,b1,c1,x1,y1,x2,y2);
	double x3,y3,x4,y4;
	GetReflectPoint(x1,y1,x,y,x3,y3);
	GetReflectPoint(x2,y2,x,y,x4,y4);
	LineThroughTwoPoints(x3,y3,x4,y4,a,b,c);
}

void Math::GetReflectLineFr(CValueFractionEx& a1, CValueFractionEx& b1, CValueFractionEx& c1, CValueFractionEx& x, CValueFractionEx& y, CValueFractionEx& a, CValueFractionEx& b, CValueFractionEx& c)
{
	CValueFractionEx x1,y1,x2,y2;
	GetTwoPointsOnLineFr(a1,b1,c1,x1,y1,x2,y2);
	CValueFractionEx x3,y3,x4,y4;
	GetReflectPointFr(x1,y1,x,y,x3,y3);
	GetReflectPointFr(x2,y2,x,y,x4,y4);
	LineThroughTwoPointsFr(x3,y3,x4,y4,a,b,c);
}

void Math::GetDilatePoint(double x1, double y1, double x2, double y2, double k, double& x, double& y)
{
	x=x2+(x1-x2)*k;
	y=y2+(y1-y2)*k;
}

void Math::GetDilatePointFr(CValueFractionEx& x1, CValueFractionEx& y1, CValueFractionEx& x2, CValueFractionEx& y2, CValueFractionEx& k, CValueFractionEx& x, CValueFractionEx& y)
{
	x=x2+(x1-x2)*k;
	y=y2+(y1-y2)*k;
}

void Math::GetDilateLine( double a1, double b1, double c1, double x, double y, double k, double& a, double& b, double& c )
{
	double x1,y1,x2,y2;
	GetTwoPointsOnLine(a1,b1,c1,x1,y1,x2,y2);
	double x3,y3,x4,y4;
	GetDilatePoint(x1,y1,x,y,k,x3,y3);
	GetDilatePoint(x2,y2,x,y,k,x4,y4);
	LineThroughTwoPoints(x3,y3,x4,y4,a,b,c);
}

void Math::GetDilateLineFr(CValueFractionEx& a1, CValueFractionEx& b1, CValueFractionEx& c1, CValueFractionEx& x, CValueFractionEx& y, CValueFractionEx& k, CValueFractionEx& a, CValueFractionEx& b, CValueFractionEx& c )
{
	CValueFractionEx x1,y1,x2,y2;
	GetTwoPointsOnLineFr(a1,b1,c1,x1,y1,x2,y2);
	CValueFractionEx x3,y3,x4,y4;
	GetDilatePointFr(x1,y1,x,y,k,x3,y3);
	GetDilatePointFr(x2,y2,x,y,k,x4,y4);
	LineThroughTwoPointsFr(x3,y3,x4,y4,a,b,c);
}

void Math::GetRotatePoint(double x1, double y1, double x2, double y2, double k, double& x, double& y)
{
	double angle=ConvertToRadian(k);
	x=(x1-x2)*cos(angle)-(y1-y2)*sin(angle)+x2;
	y=(x1-x2)*sin(angle)+(y1-y2)*cos(angle)+y2;
}

void Math::GetRotatePointFr(CValueFractionEx& x1, CValueFractionEx& y1, CValueFractionEx& x2, CValueFractionEx& y2, CValueFractionEx& k, CValueFractionEx& x, CValueFractionEx& y)
{
	double angle=ConvertToRadian(k.ToDouble());
	CValueFractionEx c,s; c.FromNumber(cos(angle)); s.FromNumber(sin(angle));
	x=(x1-x2)*c-(y1-y2)*s+x2;
	y=(x1-x2)*s+(y1-y2)*c+y2;
}

void Math::GetRotateLine(double a1, double b1, double c1, double x, double y, double k, double& a, double& b, double& c)
{
	double x1,y1,x2,y2;
	GetTwoPointsOnLine(a1,b1,c1,x1,y1,x2,y2);
	double x3,y3,x4,y4;
	GetRotatePoint(x1,y1,x,y,k,x3,y3);
	GetRotatePoint(x2,y2,x,y,k,x4,y4);
	LineThroughTwoPoints(x3,y3,x4,y4,a,b,c);
}

void Math::GetRotateLineFr(CValueFractionEx& a1, CValueFractionEx& b1, CValueFractionEx& c1, CValueFractionEx& x, CValueFractionEx& y, CValueFractionEx& k, CValueFractionEx& a, CValueFractionEx& b, CValueFractionEx& c)
{
	CValueFractionEx x1,y1,x2,y2;
	GetTwoPointsOnLineFr(a1,b1,c1,x1,y1,x2,y2);
	CValueFractionEx x3,y3,x4,y4;
	GetRotatePointFr(x1,y1,x,y,k,x3,y3);
	GetRotatePointFr(x2,y2,x,y,k,x4,y4);
	LineThroughTwoPointsFr(x3,y3,x4,y4,a,b,c);
}

double Math::ConvertToRadian(double sd)
{
	if (!bRadian) return sd/180*pi;
	else return sd;
}

double Math::ConvertFromRadianToAngleUnit(double sd)
{
	if (!bRadian) return sd/pi*180;
	else return sd;
}

BOOL Math::IsCollinear(double x1, double y1, double x2, double y2, double x3, double y3)
{
	if (!Math::Equal(y1,y2) && !Math::Equal(y1,y3))
		return TRUE;
	else if (!(Math::Equal(y1,y2) || Math::Equal(y1,y3)))
		return Math::Equal((x1-x2)/(y1-y2),(x1-x3)/(y1-y3));
	else return FALSE;
}

BOOL Math::IsConcurrent(double a1, double b1, double c1, double a2, double b2, double c2, double a3, double b3, double c3)
{
	BOOL bl1=IsParallel(a1,b1,c1,a2,b2,c2);
	BOOL bl2=IsParallel(a1,b1,c1,a3,b3,c3);
	if (bl1 && bl2) return TRUE;
	else if (bl1 || bl2) return FALSE;
	else
	{
		double x,y;
		GetIntersectionLineLine(a1,b1,c1,a2,b2,c2,x,y);
		return (Math::Equal(a3*x+b3*y+c3,0));
	}
}

bool Math::IsInside(const Point              &point,
			   const std::vector<Point> &points_list)
{
	int    wn = 0;    // the winding number counter

	std::vector<Point>::iterator it;
	// loop through all edges of the polygon
	for (int i=0;i<points_list.size()-1;i++)//it=points_list.begin(); it<points_list.end()-1; it++)// edge from V[i] to V[i+1]
	{
		if (points_list[i].Y <= point.Y) {         // start y <= pt->y
			if (points_list[i+1].Y > point.Y)      // an upward crossing
				if (IsLeft( points_list[i], points_list[i+1], point) > 0)  // P left of edge
					++wn;            // have a valid up intersect
		}
		else {                       // start y > P.y (no test needed)
			if (points_list[i+1].Y <= point.Y)     // a downward crossing
				if (IsLeft( points_list[i], points_list[i+1], point) < 0)  // P right of edge
					--wn;            // have a valid down intersect
		}
	}
	if (wn==0)
		return false;
	return true;
}

inline int Math::IsLeft(const Point &p0,
			const Point &p1,
			const Point &point)
{
	return ((p1.X - p0.X) * (point.Y - p0.Y) -
		(point.X - p0.X) * (p1.Y - p0.Y));
}

double Math::Area(PointD* pts, int count)
{
	double S=0;
	for (int i=0;i<count-1;i++)
	{
		S+=pts[i].X*pts[i+1].Y-pts[i+1].X*pts[i].Y;
	}
	S+=pts[count-1].X*pts[0].Y-pts[0].X*pts[count-1].Y;
	return S/2;
}

CValueFractionEx Math::AreaFr(CArray<CValueFractionEx*>& ptsx, CArray<CValueFractionEx*>& ptsy)
{
	CValueFractionEx S;
	S.FromNumber(0);
	for (int i=0;i<ptsx.GetSize()-1;i++)
	{
		S=S+(*ptsx[i])*(*ptsy[i+1])-(*ptsx[i+1])*(*ptsy[i]);
	}
	S=S+(*ptsx[ptsx.GetSize()-1])*(*ptsy[0])-(*ptsx[0])*(*ptsy[ptsx.GetSize()-1]);
	return S/2;
}


// Góc giữa hai vector
double Math::Angle(double x1, double y1, double x2, double y2)
{
	return acos((x1*x2+y1*y2)/sqrt((x1*x1+y1*y1)*(x2*x2+y2*y2)));
}

double Math::Angle(double a1, double b1, double c1, double a2, double b2, double c2)
{
	double ang1=(Equal(a1,0) ? 0 : atan(-b1/a1));
	double ang2=(Equal(a2,0) ? 0 : atan(-b2/a2));
	double angle=ang1-ang2;
	return ConvertFromRadianToAngleUnit(angle);
}

bool Math::IsTangent( double a1, double b1, double c1, double centerX, double centerY, double R )
{
	return Math::Equal(Math::GetDistance(centerX,centerY,a1,b1,c1),R);
}

int Math::gcd( int a, int b )
{
	a=abs(a); 
	b=abs(b);            
	if (a==0 || b==0)
		return a+b;
	while (a!=0 && b!=0)
	{
		if(a>b)
			a%=b;
		else
			b%=a;
	}
	return a+b;
}

int Math::lcm( int a, int b )
{
	return a*b/gcd(a,b);
}

int Math::TangentLine( double x,double y,double a,double b,double c,double d,double e,double f,double& la1,double& lb1,double& lc1,double& la2,double& lb2,double& lc2 )
{
	if (c==0 && d==0 && e==0 && f==-1)
	{
		double t1,t2;
		int num=Math::SolveQuadricFunction(x*x-1/a,2*x*y,y*y-1/b,t1,t2);
		if (x*x-1/a==0)
		{
			la1=t1; lb1=1; lc1=-y-t1*x;
			la2=1; lb2=0; lc2=-x;
			return 2;
		}
		else
		{
			if (num>0) { la1=t1; lb1=1; lc1=-y-t1*x; }
			if (num>1) { la2=t2; lb2=1; lc2=-y-t2*x; }
		}
		return num;
	}
	return 0;
}

int Math::TangentLineFr(CValueFractionEx& x,CValueFractionEx& y,CValueFractionEx& a,CValueFractionEx& b,CValueFractionEx& c,CValueFractionEx& d,CValueFractionEx& e,CValueFractionEx& f,CValueFractionEx& la1,CValueFractionEx& lb1,CValueFractionEx& lc1,CValueFractionEx& la2,CValueFractionEx& lb2,CValueFractionEx& lc2)
{
	if (c.ToDouble()==0 && d.ToDouble()==0 && e.ToDouble()==0 && f.ToDouble()==-1)
	{
		CValueFractionEx t1,t2;
		int num=Math::SolveQuadricFunctionFr(x*x-a.Inverse(),x*y*2,y*y-b.Inverse(),t1,t2);
		if (num>0) { la1=t1; lb1=1; lc1=-y-t1*x; }
		if (num>1) { la2=t2; lb2=1; lc2=-y-t2*x; }
		return num;
	}
	return 0;
}