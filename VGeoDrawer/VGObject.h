#pragma once

#include "gdi+.h"
#include "AxisInfo.h"
#include "VGObjLabel.h"
#include <math.h>
#include "resource.h"
#include "VGMath.h"
#include "Script\ScriptObject.h"
#include "appfunc.h"

class CVGExpression;
class CVGObject;

class CVGObjLabel;
class CGeoExp;

class CVGPoint;
class CVGLineBase;
class CVGLine;
class CVGVector;
class CVGCircle;
class CVGConic;
class CVGPanel;

extern int numRound;
extern float nZoom;
extern Size drPadSize;

#define MAX_OBJECT_SIZE 5

#define IsNumberValueType(type) (type==VALUE_DOUBLE || type==VALUE_FRACTION_EX)

class CGeoExp
{
public:
	CGeoExp(void);
public:
	~CGeoExp(void);
public:
	class CValueExp
	{
	public:
		CValueExp();
		~CValueExp();

		CArray<CVGObject*> m_Param;
		CValue* m_Value;
		int m_Mode;

		void Calc();
		bool CanCalc();
		int GetUnit();
		CValue* CalcFr();
	};

	class CGeoExpStack
	{
	public:
		~CGeoExpStack() { for (int i=0;i<m_vlArr.GetSize();i++) delete m_vlArr[i]; }
		CArray<CValue*> m_vlArr;
		void Push(CValue* vl) { m_vlArr.Add(vl); }
		int GetSize() { return m_vlArr.GetSize(); }
		CValue* Top() { CValue* vl=m_vlArr[m_vlArr.GetSize()-1]; m_vlArr.RemoveAt(m_vlArr.GetSize()-1); return vl; }
		ValueType GetValueType(int nIndex) { return m_vlArr[m_vlArr.GetSize()-1-nIndex]->GetType(); }
	};

	enum ExpValueType
	{
		VARIABLE=0,NUMBER,OPERATOR,OBJVALUE,VARIABLE_X,USER_FUNCTION
	};

	enum ExpOperator
	{
		NONE=0,BRACKET,ADDITION,SUBTRACTION,MULTIPLICATION,DIVISION,POWER,
		EQUAL,GREATER,LESS,GREATER_THAN_OR_EQUAL,LESS_THAN_OR_EQUAL,
		SQRT,SIN,COS,TAN,COT,ARCSIN,ARCCOS,ARCTAN,ARCCOT,SINH,COSH,TANH,
		AND,OR,NOT,LN,LOG10,CBRT,EXP,FLOOR,CEIL,ROUND,MAX,MIN,ABS,IF,OP_VECTOR
	};

	bool IsConstant();

	CScriptObject* m_ScriptObject;

	CArray<ExpValueType> m_TypeArr;

	CArray<CValue*> m_ValueArr; // Số thực, chuỗi, ...

	CArray<CValueExp*> m_ValueExpArr; // VD : Distance(A,B)

	CArray<CVGObject*> m_objArr; // VD A.x
	CArray<int> m_objArrValueType;

	CArray<ExpOperator> m_OperatorArr; // + - * / sin cos ...

	CArray<CString> m_strFuncNameArr; // Tên hàm số trong script người dùng
	CArray<int> m_ParamCountArr; // Số các tham số

	//bool m_bNeedCalc;
	bool m_bIsboolean;

	CValue* m_Value;
	bool m_bErr;

	CString m_strExp;
	CString m_strErr;

	void GetParam(CString strParam, CStringArray* aParam);
	bool GetPointFromSegment(CString segName, CVGPoint*& pt1, CVGPoint*& pt2, CArray<CVGObject*>* objArr, CStringArray* m_strReplacedNameArr);
	bool FillArray(CArray<CVGObject*> *m_Array, CString strExpression, bool bHasX, CStringArray* m_strReplacedNameArr);
	ExpOperator GetFuncCode(CString str);
	int Priority(int op);
	int GetNext(CString str, int startPos);
	int AddValue(CString str, CArray<CVGObject*> *m_Array, CStringArray* m_strReplacedNameArr);
	int AddValue(CString strCmdName, CString strParam, CArray<CVGObject*> *m_Array, CStringArray* m_strReplacedNameArr);
	CVGObject* GetObjByName(CString strName, CArray<CVGObject*> *m_Array, CStringArray* m_strReplacedNameArr);
	int GetNextWithBracket(CString str, int startPos);
	bool SetExpression(CString strExpression, CArray<CVGObject*> *m_Array, bool bHasX=false, CStringArray* m_strReplacedNameArr=NULL);
	bool CanCalc();
	void Calc(double vx=false, bool bRadian=false);
	CString GetStringValue();
	double GetDoubleValue() { return m_Value->ToDouble(); }
	void RemoveUnnecessaryOperate();
	CValue* CalcFr(double vx=false, bool bRadian=false);
	bool IsRelated(CVGObject* obj);
};

class CVGGradientStyle
{
public:
	Color m_Color1;
	Color m_Color2;
	LinearGradientMode m_Mode;
	void Serialize(CArchive& ar)
	{
		SerializeColor(m_Color1,ar);
		SerializeColor(m_Color2,ar);
		if (ar.IsStoring()) ar << (int)m_Mode;
		else { int i; ar >> i; m_Mode=(LinearGradientMode)i; }
	}
};


int GetObjType(CString str);
CString GetObjTypeName(int nID);
CString GetObjCommandTypeName( int nID );

enum ObjType
{
	OBJ_NONE=0,

	OBJ_EXPRESSION,

	OBJ_POINT, OBJ_POINTS,
	OBJ_POINT_HIDDEN_CHILD,
	OBJ_LINE,
	OBJ_RAY,
	OBJ_SEGMENT,
	OBJ_VECTOR,
	OBJ_CIRCLE,
	OBJ_CONIC,
	OBJ_FUNCTION,
	OBJ_CARDINAL_SPLINE,
	OBJ_BEZIER_SPLINE,
	OBJ_ANGLE,
	OBJ_POLYGON,

	OBJ_PANEL,

	OBJ_TEXT,
	OBJ_BUTTON,
	OBJ_SLIDER,
	OBJ_CHECKBOX,
	OBJ_RADIO_BUTTON,
	OBJ_IMAGE,

	OBJ_COUNT
};

#define IsGeoObject(i) (i==OBJ_POINT || i==OBJ_POINT_HIDDEN_CHILD || i==OBJ_LINE || i==OBJ_RAY || i==OBJ_SEGMENT || i==OBJ_VECTOR || i==OBJ_CIRCLE || i==OBJ_FUNCTION || i==OBJ_CONIC || i==OBJ_CARDINAL_SPLINE || i==OBJ_POLYGON) ? TRUE : FALSE
#define IsLineType(i) ((i==OBJ_LINE || i==OBJ_RAY || i==OBJ_SEGMENT || i==OBJ_VECTOR) ? TRUE:FALSE)
#define IsPointType(i) (i==OBJ_POINT || i==OBJ_POINT_HIDDEN_CHILD) ? TRUE:FALSE
#define IsTextType(i) (i==OBJ_TEXT) ? TRUE:FALSE
#define IsBoundType(i) (i==OBJ_IMAGE || i==OBJ_PANEL || i==OBJ_BUTTON || i==OBJ_CHECKBOX || i==OBJ_RADIO_BUTTON || i==OBJ_TEXT || i==OBJ_SLIDER) ? TRUE:FALSE
#define IsButtonType(i) (i==OBJ_BUTTON || i==OBJ_CHECKBOX || i==OBJ_RADIO_BUTTON) ? TRUE:FALSE

#define CanSelectObject(obj,objRight) ( (obj==objRight) || ((IsLineType(obj)) && objRight==OBJ_LINE) || (obj==OBJ_POINT && objRight==OBJ_POINT_HIDDEN_CHILD))

enum VGObjMode
{
	GEO_MODE_TRANSLATE=0,
	GEO_MODE_REFLECT_POINT,
	GEO_MODE_REFLECT_LINE,
	GEO_MODE_ROTATO,
	GEO_MODE_DILATE,
	GEO_MODE_INVERSE,

	POINT_MODE_NUM_NUM,
	POINT_MODE_CHILD,
	POINT_MODE_DEFINED,
	POINT_MODE_CENTROID,
	POINT_MODE_MIDPOINT,
	POINT_MODE_CENTER,
	POINT_MODE_INTERSECTION_LINE_LINE,
	POINT_MODE_INTERSECTION_LINE_CIRCLE,
	POINT_MODE_INTERSECTION_CIRCLE_CIRCLE,
	POINT_MODE_INTERSECTION_LINE_CONIC,
	POINT_MODE_TRANSLATE,
	POINT_MODE_REFLECT_POINT,
	POINT_MODE_REFLECT_LINE,
	POINT_MODE_CHILD_LINE,
	POINT_MODE_CHILD_CIRCLE,
	POINT_MODE_CHILD_FUNCTION,
	POINT_MODE_CHILD_CONIC,
	POINT_MODE_DIVIDED,
	POINT_MODE_ON_LINE_WITH_DISTANCE_TO_OTHER_POINT,
	POINT_MODE_EXP_EXP,
	POINT_MODE_NUM_EXP,
	POINT_MODE_EXP_NUM,

	LINE_MODE_THROUGH_TWO_POINTS,
	LINE_MODE_THROUGH_POINT,
	LINE_MODE_DEFINED,
	LINE_MODE_PERPENDICULAR,
	LINE_MODE_PARALLEL,
	LINE_MODE_ANGLE_BISECTOR,
	LINE_MODE_EXTERIOR_ANGLE_BISECTOR,
	LINE_MODE_ANGLE_BISECTOR_WITH_POINT,
	LINE_MODE_TANGENT_CONIC,

	VECTOR_MODE_POINT_POINT,
	VECTOR_MODE_POINT_EXP,

	CIRCLE_MODE_POINT_POINT, //Xác định tâm và điểm đi qua
	CIRCLE_MODE_POINT_RADIUS,

	ELLIPSE_MODE_A_B,

	ANGLE_MODE_POINT_POINT_POINT,
	ANGLE_MODE_LINE_LINE
};

class CVGObject
{
public:	
	CVGObject(void);
	~CVGObject(void);

	CString m_Name;
	ObjType m_Type;
	bool m_bTemp;
	int m_cmdIndex;  // Thứ tự của câu lệnh định nghĩa

	virtual void Move(int x,int y, CPoint startPt, CAxisInfo* m_AxisInfo);
	virtual bool CheckMouseOver(Point point, CAxisInfo* m_AxisInfo);
	virtual bool CheckObjInRect(Rect rect, CAxisInfo* m_AxisInfo);
	virtual void Draw(Graphics* gr, CAxisInfo* m_AxisInfo, bool bTrace=FALSE);
	virtual bool IsRelated(CVGObject* obj);
	virtual void SetName(CString strText, Font* labelFont, CAxisInfo* m_AxisInfo);
	virtual void Calc(CAxisInfo* m_AxisInfo) { m_bCalc=true; }
	virtual bool CanCalc();
	virtual CString GetValue(bool bName=FALSE) { return L""; }
	virtual void Serialize( CArchive& ar , CArray<CVGObject*>* objArr);
	virtual void Clone(CVGObject* obj);

	virtual int GetValueType(CString strValueType);
	virtual CValue* GetObjValue(int nType);
	virtual bool SetValue(CString strValue, CVGExpression* vl, CArray<CVGObject*>* m_Array);
	VGObjMode m_Mode;
	bool m_Select;
	CString m_Description;
	CString m_Definition;
	bool m_bVisible;
	bool m_HighLight;
	CArray<CVGObject*> m_Param;
	//int* m_ParamIndex;
	bool m_bFixed;
	bool m_bShowName;
	
	CString m_strCondition;
	CGeoExp m_expAvailCond;
	virtual void RefreshCondition(CArray<CVGObject*>* objArr);

	Color m_Color;
	bool m_bAvailable;
	
	// Dùng trong ObjListBox
	bool m_bShowValue;
	bool m_bShowDefinition;
	bool m_bShowDescription;

	bool m_bCalc;

	void CheckAvailable();
	void CheckCondition();
	virtual bool IsVisible();
	bool CheckCanCalc();
};

class CVGExpression : public CVGObject
{
public:
	CVGExpression();

	CGeoExp m_Exp;
	virtual void Calc(CAxisInfo* m_AxisInfo);
	virtual bool CanCalc() { return m_Exp.CanCalc(); }
	virtual double GetDoubleValue() { return m_Exp.m_Value->ToDouble(); }
	CString m_strExp;
	int GetExpValueType() { return m_Exp.m_Value->GetType(); }
	virtual bool SetExp(CString strExp, CArray<CVGObject*> *m_Array, CStringArray* m_strReplacedNameArr);
	virtual void Serialize(CArchive& ar , CArray<CVGObject*>* objArr);
	
	virtual bool IsConstant() { return m_Exp.IsConstant(); }

	virtual int GetValueType(CString strValueType);
	virtual CValue* GetObjValue(int nType);
	virtual bool IsRelated(CVGObject* obj) { return m_Exp.IsRelated(obj); }
	virtual CString GetValue(bool bName=false);
};

class CVGGeoObject : public CVGObject
{
public:
	CVGGeoObject(void);
	~CVGGeoObject(void);

	CVGObjLabel* m_Label;
	bool m_bTrace;
	bool m_bAnimate;
	bool m_bShowLabel;
	bool m_bShowLabelValue;

	virtual void SetName(CString strText, Font* labelFont, CAxisInfo* m_AxisInfo);
	virtual void Draw(Graphics* gr, CAxisInfo* m_AxisInfo, bool bTrace=FALSE);

	virtual void SetTranslate(CVGGeoObject* obj, CVGVector* vec);
	virtual void SetReflectAboutPoint(CVGGeoObject* obj, CVGPoint* pt);
	virtual void SetReflectAboutLine(CVGGeoObject* obj, CVGLineBase* line);
	virtual void SetDilate(CVGGeoObject* obj, CVGPoint* pt, CVGExpression* k);
	virtual void SetRotate( CVGGeoObject* obj, CVGPoint* pt, CVGExpression* k );
	virtual void SetInverse( CVGGeoObject* obj, CVGPoint* pt, CVGExpression* k );
	virtual void OnAnimate(int Speed, CAxisInfo* m_AxisInfo);
	virtual void Serialize( CArchive& ar, CArray<CVGObject*>* objArr );
	virtual void Clone(CVGObject* obj);

	virtual int GetValueType(CString strValueType);
	virtual CValue* GetObjValue(int nType);
	virtual bool SetValue(CString strValue, CVGExpression* vl, CArray<CVGObject*>* m_Array);
};

#define POINT_CHECK_MOUSEOVER_DISTANCE 1 // Khooảng cách khi ktra Mouse Over

#define POINT_COORDINATES_CARTESIAN 0
#define POINT_COORDINATES_POLAR 1
#define POINT_COORDINATES_COMPLEX 2

#define TEXT_EDGE 5

//////////////////////////////////////////////////////////////////////////
//
// Class CVGPoint
//
//////////////////////////////////////////////////////////////////////////

class CVGPoint :
	public CVGGeoObject
{
public:
	bool m_moved;
	int m_Size;
	double m_x,m_y;
	int m_Coordinates;
	CArray<CVGPoint*>m_objMove; //Các điểm chuyển động kéo theo
	double tmp1, tmp2; // Các biến lưu dữ liệu nếu trong chế độ CHILD
	bool m_bShowPointers; // Đường gióng lên đồ thị
	bool m_bDirection; // Dùng trong chế độ Animate

	void SetIntersectionLineCircle(CVGLineBase *l, CVGCircle *c, int num=0);
	void SetIntersectionCircleCircle(CVGCircle *c1, CVGCircle *c2, int num=0);
	void SetIntersectionLineEllipse(CVGLineBase *l, CVGConic *el, int num=0);
	bool CheckObjInRect(Rect rect, CAxisInfo* m_AxisInfo);
	virtual void Draw(Graphics* gr, CAxisInfo* m_AxisInfo, bool bTrace=FALSE);
	virtual CString GetValue(bool bName=FALSE);

	virtual void Calc(CAxisInfo* m_AxisInfo);
	virtual void CaclFr(CValueFractionEx& x, CValueFractionEx& y);

	void Move(int x,int y, CPoint startPt, CAxisInfo* m_AxisInfo);
	void MoveTo(int x, int y, CAxisInfo* m_AxisInfo);
	bool CheckMouseOver(Point point,CAxisInfo* m_AxisInfo);

	CVGPoint(void);
	~CVGPoint(void);
	void SetChild(CVGObject* obj);
	void CorrectPointPos(bool bVectorChange) /* Ði?u ch?nh v? trí khi ? ch? d? Child */;
	void OnAnimate(int Speed, CAxisInfo* m_AxisInfo);
	void Serialize( CArchive& ar, CArray<CVGObject*>* objArr );
	void Clone(CVGObject* obj);

	virtual int GetValueType(CString strValueType);
	virtual CValue* GetObjValue(int nType);
	virtual bool SetValue(CString strValue, CVGExpression* vl, CArray<CVGObject*>* m_Array);
};

//////////////////////////////////////////////////////////////////////////
//
// Class CVGPolygon
//
//////////////////////////////////////////////////////////////////////////

class CVGPolygon :
	public CVGGeoObject
{
public:
	CVGPolygon(void);
	~CVGPolygon(void);
	int m_Opacity;
	bool m_bBorder;

	virtual void Draw(Graphics* gr, CAxisInfo* m_AxisInfo, bool bTrace=FALSE);
	bool CheckMouseOver(Point point, CAxisInfo* m_AxisInfo);
	void Serialize( CArchive& ar, CArray<CVGObject*>* objArr );
	double GetArea();
	bool CheckObjInRect(Rect rect, CAxisInfo* m_AxisInfo);
	virtual void Move(int x,int y, CPoint startPt, CAxisInfo* m_AxisInfo);
	CValueFractionEx GetAreaFr();
};

class CVGAngle :
	public CVGObject
{
public:
	CVGAngle(void);
	~CVGAngle(void);
	int m_Size;
	int m_Style;
	int m_ArcCount;

	virtual void Draw(Graphics* gr, CAxisInfo* m_AxisInfo, bool bTrace=FALSE);
	bool CheckMouseOver(Point point, CAxisInfo* m_AxisInfo);
	void Calc(CAxisInfo* m_AxisInfo);
	void Serialize(CArchive& ar , CArray<CVGObject*>* objArr);
private:
	double start;
	double sweep;
};


//////////////////////////////////////////////////////////////////////////
//
// Class CVGLineBase
//
//////////////////////////////////////////////////////////////////////////

class CVGLineBase :
	public CVGGeoObject
{
public:
	virtual void GetABC(double& A, double& B, double& C) {}
	virtual void GetABCFr(CValueFractionEx& A, CValueFractionEx& B, CValueFractionEx& C) {}

	void SetThroughTwoPoints(CVGPoint* pt1,CVGPoint* pt2);
	int m_PenWidth;
	int m_DashStyle;

	CVGLineBase();
	~CVGLineBase(void);
	void DrawLine(Point pt1, Point pt2, Graphics* gr, bool bTrace);
	virtual void GetScreenStartEndPoint(Point& pt1, Point& pt2, CAxisInfo* m_AxisInfo);
	virtual bool CheckPtIn(double x, double y);
	virtual void Serialize( CArchive& ar, CArray<CVGObject*>* objArr );
	virtual void Clone(CVGObject* obj);
	
	virtual int GetValueType(CString strValueType);
	virtual CValue* GetObjValue(int nType);
	virtual bool SetValue(CString strValue, CVGExpression* vl, CArray<CVGObject*>* m_Array);
};

//////////////////////////////////////////////////////////////////////////
//
// Class CVGLine
//
//////////////////////////////////////////////////////////////////////////

class CVGLine :
	public CVGLineBase
{
public:
	void SetParallel(CVGPoint *pt, CVGLineBase *l);
	
	virtual void Calc(CAxisInfo* m_AxisInfo);

	double m_a,m_b,m_c;
	bool m_bDirection;
	bool m_bCutSuperfluousPart;
	CVGPoint* m_pt1;
	CVGPoint* m_pt2;
	int tmp;
	void SetPerpendicular(CVGPoint *pt, CVGLineBase *l);
	void SetAngleBisector(CVGPoint* pt1, CVGPoint* pt2, CVGPoint* pt3);
	void SetAngleBisector(CVGLineBase* d1, CVGLineBase* d2, CVGPoint* pt);
	void SetExteriorAngleBisector(CVGPoint* pt1, CVGPoint* pt2, CVGPoint* pt3);
	void Move(int x,int y, CPoint startPt, CAxisInfo* m_AxisInfo);
	bool CheckMouseOver(Point point, CAxisInfo* m_AxisInfo);
	virtual void Draw(Graphics* gr, CAxisInfo* m_AxisInfo, bool bTrace=FALSE);

	virtual void GetABC(double& A, double& B, double& C);
	virtual void GetABCFr(CValueFractionEx& A, CValueFractionEx& B, CValueFractionEx& C);

	void GetScreenStartEndPoint(Point& pt1, Point& pt2, CAxisInfo* m_AxisInfo);
	void OnAnimate(int Speed, CAxisInfo* m_AxisInfo);

	virtual CString GetValue(bool bName=FALSE);

	CVGLine();
	~CVGLine(void);
	virtual void Serialize( CArchive& ar, CArray<CVGObject*>* objArr );
	virtual void Clone(CVGObject* obj);

	virtual int GetValueType(CString strValueType);
	virtual CValue* GetObjValue(int nType);
	virtual bool SetValue(CString strValue, CVGExpression* vl, CArray<CVGObject*>* m_Array);
};

//////////////////////////////////////////////////////////////////////////
//
// Class CVGRay
//
//////////////////////////////////////////////////////////////////////////

class CVGRay :
	public CVGLine
{
public:
	void SetAngleBisector(CVGLine* l1, CVGLine* l2);
	bool CheckMouseOver(Point point, CAxisInfo* m_AxisInfo);
	virtual void Draw(Graphics* gr, CAxisInfo* m_AxisInfo, bool bTrace=FALSE);
	bool CheckPtIn(double x, double y);
	void Serialize( CArchive& ar, CArray<CVGObject*>* objArr );

	CVGRay();
	~CVGRay(void);
};

//////////////////////////////////////////////////////////////////////////
//
// Class CVGSegment
//
//////////////////////////////////////////////////////////////////////////

class CVGSegment :
	public CVGLineBase
{
public:
	//void SetThroughTwoPoints(CVGPoint* pt1,CVGPoint* pt2);
	void Move(int x,int y, CPoint startPt, CAxisInfo* m_AxisInfo);
	bool CheckMouseOver(Point point, CAxisInfo* m_AxisInfo);
	virtual void Draw(Graphics *gr, CAxisInfo* m_AxisInfo, bool bTrace=FALSE);
	void GetABC(double& A, double& B, double& C);
	bool CheckPtIn(double x, double y);
	void GetScreenStartEndPoint(Point& pt1, Point& pt2, CAxisInfo* m_AxisInfo);
	virtual void Serialize( CArchive& ar, CArray<CVGObject*>* objArr );
	bool CheckObjInRect(Rect rect, CAxisInfo* m_AxisInfo);

	CVGSegment();
	~CVGSegment(void);
	virtual CString GetValue(bool bName=FALSE);
	//virtual void Calc(CAxisInfo* m_AxisInfo);
};

//////////////////////////////////////////////////////////////////////////
//
// Class CVGVector
//
//////////////////////////////////////////////////////////////////////////

class CVGVector :
	public CVGSegment
{
public:
	virtual void Draw(Graphics* gr, CAxisInfo* m_AxisInfo, bool bTrace=FALSE);
	virtual void Calc(CAxisInfo* m_AxisInfo);

	CVGVector();
	~CVGVector(void);

	double x; double y;

	virtual void Serialize(CArchive& ar, CArray<CVGObject*>* objArr);
	virtual CString GetValue(bool bName=FALSE);

	virtual int GetValueType(CString strValueType);
	virtual CValue* GetObjValue(int nType);
	virtual bool CheckMouseOver(Point point, CAxisInfo* m_AxisInfo);
	virtual void Clone(CVGObject* obj);
};

//////////////////////////////////////////////////////////////////////////
//
// Class CVGCircle
//
//////////////////////////////////////////////////////////////////////////


class CVGCircle :
	public CVGGeoObject
{
public:
	void Calc(CAxisInfo* m_AxisInfo);
	void CalcFr(CValueFractionEx& centerX, CValueFractionEx& centerY, CValueFractionEx& radius);
	void Move(int x,int y, CPoint startPt, CAxisInfo* m_AxisInfo);
	bool CheckMouseOver(Point point, CAxisInfo* m_AxisInfo);
	double m_CenterX,m_CenterY,m_Radius;
	virtual void Draw(Graphics* gr, CAxisInfo* m_AxisInfo, bool bTrace=FALSE);
	bool CheckObjInRect(Rect rect, CAxisInfo* m_AxisInfo);
	void Clone(CVGObject* obj);
	int m_PenWidth;
	int m_DashStyle;
	bool m_bDirection;
	int m_nFuncType;

	CVGCircle(void);
	~CVGCircle(void);

	void Serialize( CArchive& ar, CArray<CVGObject*>* objArr );
	void OnAnimate(int Speed, CAxisInfo* m_AxisInfo);
	virtual CString GetValue(bool bName=FALSE);
	
	virtual int GetValueType(CString strValueType);
	virtual CValue* GetObjValue(int nType);
};

class CVGConic:
	public CVGGeoObject
{
public:
	void Calc(CAxisInfo* m_AxisInfo);
	//void CalcFr(CValueFractionEx& centerX, CValueFractionEx& centerY, CValueFractionEx& radius);
	//void Move(int x,int y, CPoint startPt, CAxisInfo* m_AxisInfo);
	bool CheckMouseOver(Point point, CAxisInfo* m_AxisInfo);
	double m_a,m_b,m_c,m_d,m_e,m_f;
	virtual void Draw(Graphics* gr, CAxisInfo* m_AxisInfo, bool bTrace=FALSE);
	bool CheckObjInRect(Rect rect, CAxisInfo* m_AxisInfo);
	void Clone(CVGObject* obj);
	int m_PenWidth;
	int m_DashStyle;

	CVGConic(void);
	~CVGConic(void);

	void Serialize( CArchive& ar, CArray<CVGObject*>* objArr );
	//void OnAnimate(int Speed, CAxisInfo* m_AxisInfo);
	virtual CString GetValue(bool bName=FALSE);
	virtual void CalcFr(CValueFractionEx& a, CValueFractionEx& b, CValueFractionEx& c, CValueFractionEx& d, CValueFractionEx& e, CValueFractionEx& f);
	
	int GetY(double x, double& y1, double& y2);
	void PtCalc(CAxisInfo* m_AxisInfo);
	int GetX(double y, double& x1, double& x2);
	int* m_PixelVlX1;
	int* m_PixelVlX2;
	
	int* m_PixelVlY1;
	int* m_PixelVlY2;
	//virtual int GetValueType(CString strValueType);
	//virtual CValue* GetObjValue(int nType);
};

//////////////////////////////////////////////////////////////////////////
//
// CVGCardinalSpline
//
//////////////////////////////////////////////////////////////////////////

class CVGCardinalSpline :
	public CVGGeoObject
{
public:
	CVGCardinalSpline(void);
	virtual void Draw(Graphics* gr, CAxisInfo* m_AxisInfo, bool bTrace=FALSE);
	int m_PenWidth;
	int m_DashStyle;
public:
	~CVGCardinalSpline(void);
};

class CVGBezierSpline :
	public CVGGeoObject
{
public:
	CVGBezierSpline(void);
	virtual void Draw(Graphics* gr, CAxisInfo* m_AxisInfo, bool bTrace=FALSE);
	int m_PenWidth;
	int m_DashStyle;
public:
	~CVGBezierSpline(void);
};

//////////////////////////////////////////////////////////////////////////
//
// Class CVGFunction
//
//////////////////////////////////////////////////////////////////////////

class CVGFunction:
	public CVGGeoObject
{
public:
	//bool CheckMouseOver(Point point);
	virtual void Draw(Graphics* gr, CAxisInfo* m_AxisInfo, bool bTrace=FALSE);
	CString m_strFunction;
	CGeoExp m_expFunc;
	int* m_PixelVl;
	bool* m_bContinuous;
	int m_PenWidth;
	void SetFunction(CString strFunc, CArray<CVGObject*>*m_Array,CStringArray* m_strReplacedNameArr);
	//CVGFunction(CString mFunction);

	CVGFunction(void);
public:
	~CVGFunction(void);
	virtual void Calc(CAxisInfo* m_AxisInfo);
	virtual bool CheckMouseOver(Point point, CAxisInfo* m_AxisInfo);
	virtual void Clone(CVGObject* obj);
	void FuncCalc(CAxisInfo* m_AxisInfo);
	virtual bool IsRelated(CVGObject* obj);
};

class CVGBound : public CVGObject
{
	#define BOUND_STATE_TOPLEFT 1
	#define BOUND_STATE_TOP 2
	#define BOUND_STATE_TOPRIGHT 3
	#define BOUND_STATE_LEFT 4
	#define BOUND_STATE_RIGHT 5
	#define BOUND_STATE_BOTTOMLEFT 6
	#define BOUND_STATE_BOTTOM 7
	#define BOUND_STATE_BOTTOMRIGHT 8
	
	#define BOUND_PT_SIZE 3
	#define BOUND_BORDER_SIZE 5
public:
	CVGBound();
	~CVGBound();

	int m_Top, m_Left, m_Height, m_Width;
	int m_minWidth, m_minHeight;
	bool m_bBoundMark[8];
	int CheckMouseOverState(Point point, CAxisInfo* m_AxisInfo, int &x, int &y);
	bool CheckMouseOverPt(Point mouse, Point pt, int& x, int& y);
	virtual void Draw(Graphics* gr, CAxisInfo* m_AxisInfo, bool bTrace=FALSE);
	virtual Rect GetDrawRect();
	void DrawPoint(Graphics* gr, Point pt);
	LPWSTR GetCursor(int state);
	
	int m_ResizeX,m_ResizeY; // Toa do luc chuot bat dau keo thay doi kich thuoc
	virtual void Resize(int state,int X,int Y);
	virtual bool CheckMouseOver(Point point, CAxisInfo* m_AxisInfo);
	virtual void Serialize(CArchive& ar , CArray<CVGObject*>* objArr);
	virtual void Move(int x,int y, CPoint startPt, CAxisInfo* m_AxisInfo);
	virtual bool CheckObjInRect(Rect rect, CAxisInfo* m_AxisInfo);
	virtual void SetRect(Rect rect);
	void ResetBoundMark(bool bValue=true);
	Rect GetRect();
	Rect GetVisibleRect();
	virtual bool IsVisible();
	virtual void Clone(CVGObject* obj);
	CVGPanel* m_ParentPanel;
};

class CVGText : public CVGBound
{
public:
	CVGText();
	~CVGText();
	virtual void SetText(CString strText, CArray<CVGObject*>* m_Array, CScriptObject* scriptObj, CStringArray* m_strReplacedNameArr);
	virtual void Draw(Graphics* gr, CAxisInfo* m_AxisInfo, bool bTrace=FALSE);
	virtual void Serialize(CArchive& ar , CArray<CVGObject*>* objArr);
	virtual CString GetText();
	virtual void DrawText();
	virtual void Clone(CVGObject* obj);
	virtual bool CanCalc();
	virtual void Calc(CAxisInfo* m_AxisInfo);
	virtual bool IsRelated(CVGObject* obj);
	CString m_Text;
	CString m_TextDraw;
	bool m_bModeMath;
	LOGFONT m_Font;
	CArray<CGeoExp*> m_ExpArr;
	CArray<int> m_ExpArrPos;
private:
	Bitmap* m_bmpText;
};

class CVGPanel : public CVGBound
{
	#define PANEL_DOCK_NONE 0
	#define PANEL_DOCK_LEFT 1
	#define PANEL_DOCK_RIGHT 2
	#define PANEL_DOCK_TOP 3	
	#define PANEL_DOCK_BOTTOM 4

public:
	CVGPanel();
	~CVGPanel();
	virtual void Draw(Graphics* gr, CAxisInfo* m_AxisInfo, bool bTrace=FALSE);
	void RefreshDocking(CAxisInfo* m_AxisInfo);
	virtual void Resize(int state,int X,int Y);
	virtual void Move(int x,int y, CPoint startPt, CAxisInfo* m_AxisInfo);
	void SetOpacity();
	virtual void Serialize(CArchive& ar , CArray<CVGObject*>* objArr);
	virtual void Clone(CVGObject* obj);
	Rect m_rcPos;
	int m_dWidth, m_dHeight;
	int m_Dock;
	CArray<CVGBound*> m_objChild;
	
	Color m_BorderColor;
	CVGGradientStyle m_gradientStyle;
	int m_nOpacity;

	bool m_bTitle;	
	CVGGradientStyle m_gradientTitleStyle;
	CString m_Text;
	int m_nTitleHeight;
	LOGFONT m_Font;
};

class CVGImage : public CVGBound
{
public:
	CVGImage();
	~CVGImage();
	
	Bitmap* m_Bitmap;
	void SetBitmap(Bitmap* bmp);
	virtual void Draw(Graphics* gr, CAxisInfo* m_AxisInfo, bool bTrace=FALSE);
	void Serialize(CArchive& ar , CArray<CVGObject*>* objArr);
};

class CVGButtonBase : public CVGBound
{
public:
	CVGButtonBase();
	~CVGButtonBase();
	CVGGradientStyle m_ColorNormal;
	CVGGradientStyle m_ColorMouseDown;
	CVGGradientStyle m_ColorMouseOver;
	Color m_BorderColor;
	CString m_Text;
	CString m_Command;
	
	bool m_bButtonMouseOver;
	bool m_bButtonMouseDown;

	int m_boxSize;

	LOGFONT m_Font;

	virtual bool CheckMouseOverButton(Point pt) {return false;}
	virtual void Draw(Graphics* gr, CAxisInfo* m_AxisInfo, bool bTrace=FALSE) {}
	virtual bool CheckMouseOver(Point point, CAxisInfo* m_AxisInfo);
	virtual void OnClick();
	virtual void Serialize(CArchive& ar , CArray<CVGObject*>* objArr);
	virtual void Clone(CVGObject* obj);
	virtual int GetValueType(CString strValueType);
	virtual CValue* GetObjValue(int nType);
	virtual bool SetValue(CString strValue, CVGExpression* vl, CArray<CVGObject*>* m_Array);
};

class CVGButton : public CVGButtonBase
{
public:
	CVGButton();
	~CVGButton();
	virtual bool CheckMouseOverButton(Point pt);
	virtual void Draw(Graphics* gr, CAxisInfo* m_AxisInfo, bool bTrace=FALSE);
};

class CVGCheckBox : public CVGButtonBase
{
#define CHECK_BOX_DISTANCE 4
public:
	CVGCheckBox();
	~CVGCheckBox();

	virtual void Draw(Graphics* gr, CAxisInfo* m_AxisInfo, bool bTrace=FALSE);
	virtual bool CheckMouseOverButton(Point pt);
	bool m_bState;
};

class CVGRadioButton : public CVGButtonBase
{
#define RADIO_BOX_DISTANCE 4
public:
	CVGRadioButton();
	~CVGRadioButton();

	virtual void Draw(Graphics* gr, CAxisInfo* m_AxisInfo, bool bTrace=FALSE);
	virtual bool CheckMouseOverButton(Point pt);
	bool m_bState;
};

class CVGSlider : public CVGBound
{
public:
	CVGSlider();
	~CVGSlider();

	CVGExpression* m_NumVariable;
	double m_nStart; double m_nEnd;
	CString m_Text;
	bool m_bMouseOver;

	virtual void Draw(Graphics* gr, CAxisInfo* m_AxisInfo, bool bTrace=FALSE);
	virtual bool CheckMouseOver(Point point, CAxisInfo* m_AxisInfo);
	virtual bool CheckMouseOverSlider(Point pt);
	void OnLButton(Point pt);
	virtual void SetRect(Rect rect);
};