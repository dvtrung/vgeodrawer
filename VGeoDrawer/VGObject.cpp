#include "StdAfx.h"
#include "VGObject.h"
#include "appfunc.h"

CVGObject::CVGObject(void)
{
	m_bTemp=FALSE;
	m_cmdIndex=0;

	m_Select=false;
	m_HighLight=false;
	m_bVisible=true;
	m_bShowDefinition=false;
	m_bShowDescription=false;
	m_bShowValue=false;
	m_bAvailable=true;
	m_bFixed=false;
}

CVGObject::~CVGObject(void)
{

}

bool CVGObject::IsVisible()
{
	return (m_bVisible && !m_bTemp && m_bAvailable);
}

void CVGObject::Draw(Graphics* gr, CAxisInfo* m_AxisInfo, bool bTrace)
{
	
}

void CVGObject::CheckAvailable()
{
	for (int i=0;i<m_Param.GetSize();i++)
	{
		CVGObject* obj= m_Param[i];
		if (obj!=NULL && obj->m_bAvailable==FALSE) { m_bAvailable=FALSE; return; }
	}
	m_bAvailable=TRUE;
}

bool CVGObject::CheckMouseOver(Point point, CAxisInfo* m_AxisInfo)
{
	return false;
}

void CVGObject::Move(int x,int y, CPoint startPt, CAxisInfo* m_AxisInfo)
{

}

bool CVGObject::CheckObjInRect(Rect rect, CAxisInfo* m_AxisInfo)
{
	return false;
}

bool CVGObject::CanCalc()
{
//  	if (m_Param.GetSize()==0) return TRUE;
//  	for (int i=0;i<m_Param.GetSize();i++)
// 	{
//  		CVGObject* obj=m_Param[i];
//  		if (obj->m_bCalc==FALSE) 
//  			return FALSE;
//  	}
 	return TRUE;
}

bool CVGObject::CheckCanCalc()
{
	if (m_bCalc || !CanCalc()) return false;
	CheckAvailable();
	m_bCalc=TRUE;
	if (!m_bAvailable) return false;
	return true;
}

void CVGObject::Serialize( CArchive& ar , CArray<CVGObject*>* objArr)
{
	if (ar.IsStoring())
	{
		ar << m_bTemp;
		ar << m_Name;
		ar << m_cmdIndex;

		ar << m_bFixed;
		ar << m_bAvailable;
		ar << m_bShowDefinition;
		ar << m_bShowDescription;
		ar << m_bShowValue;
		ar << m_Color.GetValue();
		ar << m_Definition;
		ar << m_Description;
		ar << m_HighLight;
		ar << m_Mode;
		ar << m_Select;
		ar << m_bVisible;
		ar << m_Param.GetSize();

		for (int i=0;i<m_Param.GetSize();i++)
		{
			for (int j=0;j<objArr->GetSize();j++)
			{
				if (m_Param[i]==objArr->GetAt(j)) ar << j;
			}
		}
	}
	else
	{
		ar >> m_bTemp;
		ar >> m_Name;
		ar >> m_cmdIndex;

		ar >> m_bFixed;
		ar >> m_bAvailable;
		ar >> m_bShowDefinition;
		ar >> m_bShowDescription;
		ar >> m_bShowValue;
		ARGB argb;
		ar >> argb;
		m_Color.SetValue(argb);
		//m_Color.SetFromCOLORREF(RGB(r,g,b));
		ar >> m_Definition;
		ar >> m_Description;
		ar >> m_HighLight;
		int i;
		ar >> i; m_Mode=(VGObjMode)i;
		ar >> m_Select;
		ar >> m_bVisible;

		int count;
		ar >> count;
		// m_Param lưu vị trí objParam trong mảng
		for (int i=0;i<count;i++)
		{
			int index;
			ar >> index;
			m_Param.Add((CVGObject*)index);
		}
	}
}

bool CVGObject::IsRelated(CVGObject* obj)
{
	for (int i=0;i<m_Param.GetSize();i++)
	{
		if (obj==m_Param[i]) return true;
		if (m_Param[i]->m_Type==OBJ_EXPRESSION && ((CVGExpression*)m_Param[i])->m_Exp.IsRelated(obj)) return true;
	}
	return FALSE;
}

void CVGObject::SetName( CString strText, Font* labelFont, CAxisInfo* m_AxisInfo )
{
	m_Name=strText;
}

void CVGObject::Clone(CVGObject* obj)
{
	m_Type=obj->m_Type;
	m_Name=obj->m_Name;
	m_bTemp=obj->m_bTemp;
	m_cmdIndex=obj->m_cmdIndex;
}

void CVGObject::RefreshCondition(CArray<CVGObject*>* objArr)
{
	m_expAvailCond.SetExpression(m_strCondition,objArr,false);
}

void CVGObject::CheckCondition()
{
	//if (m_bTemp || !m_bAvailable) return;
	if (m_strCondition!=L"")
	{
		m_expAvailCond.Calc();
		m_bAvailable=((CValueBool*)m_expAvailCond.m_Value)->m_Value;
	}
	//if (m_exp.m_TypeArr.GetSize()!=0) m_bVisible=!(m_exp.GetDoubleValue()==0);
}

void CVGGeoObject::SetName(CString strText, Font* labelFont, CAxisInfo* m_AxisInfo)
{
	if (m_Label==NULL) 
	{
		m_Label=new CVGObjLabel();
		m_Label->m_Text=&m_Name;
		m_Label->m_Font=labelFont;
		m_Label->SetLabel(strText);
		m_Label->SetObject(this);
	}

	m_Name=strText;
}

void CVGGeoObject::Draw(Graphics* gr, CAxisInfo* m_AxisInfo, bool bTrace)
{
	if (m_Label!=NULL && !bTrace) m_Label->Draw(gr, m_AxisInfo);
}

void CVGGeoObject::SetTranslate(CVGGeoObject* obj, CVGVector* vec)
{
	m_Mode=GEO_MODE_TRANSLATE;
	m_Param.RemoveAll();
	m_Param.Add(obj);
	m_Param.Add(vec);
}
void CVGGeoObject::SetReflectAboutPoint(CVGGeoObject* obj, CVGPoint* pt)
{
	m_Mode=GEO_MODE_REFLECT_POINT;
	m_Param.RemoveAll();
	m_Param.Add(obj);
	m_Param.Add(pt);
}

void CVGGeoObject::SetReflectAboutLine(CVGGeoObject* obj, CVGLineBase* line)
{
	m_Mode=GEO_MODE_REFLECT_LINE;
	m_Param.RemoveAll();
	m_Param.Add(obj);
	m_Param.Add(line);
}

void CVGGeoObject::SetDilate( CVGGeoObject* obj, CVGPoint* pt, CVGExpression* k )
{
	m_Mode=GEO_MODE_DILATE;
	m_Param.RemoveAll();
	m_Param.Add(obj);
	m_Param.Add(pt);
	m_Param.Add(k);
}

void CVGGeoObject::SetInverse( CVGGeoObject* obj, CVGPoint* pt, CVGExpression* k )
{
	m_Mode=GEO_MODE_INVERSE;
	m_Param.RemoveAll();
	m_Param.Add(obj);
	m_Param.Add(pt);
	m_Param.Add(k);
}

void CVGGeoObject::SetRotate( CVGGeoObject* obj, CVGPoint* pt, CVGExpression* k )
{
	m_Mode=GEO_MODE_ROTATO;
	m_Param.RemoveAll();
	m_Param.Add(obj);
	m_Param.Add(pt);
	m_Param.Add(k);
}

CVGGeoObject::CVGGeoObject(void)
{
	m_Label=NULL;
	m_bTrace=FALSE;
	m_bAnimate=FALSE;
	m_bShowLabel=FALSE;
	m_bShowLabelValue=FALSE;
}

CVGGeoObject::~CVGGeoObject(void)
{
	if (m_Label!=NULL) delete m_Label;
}

void CVGGeoObject::Serialize( CArchive& ar, CArray<CVGObject*>* objArr )
{
	CVGObject::Serialize(ar,objArr);
	
	if (ar.IsStoring())
	{
		ar << m_bAnimate;
		ar << m_bTrace;
		ar << (bool)(m_Label==NULL);
		if (m_Label!=NULL) m_Label->Serialize(ar);
	}
	else
	{
		ar >> m_bAnimate;
		ar >> m_bTrace;
		bool b;
		ar >> b;
		if (!b) 
		{
			m_Label=new CVGObjLabel();
			m_Label->m_Object=this;
			m_Label->m_Text=&m_Name;
			m_Label->Serialize(ar);
		}
	}
}

void CVGGeoObject::OnAnimate( int Speed, CAxisInfo* m_AxisInfo )
{

}

void CVGGeoObject::Clone(CVGObject* obj)
{
	CVGObject::Clone(obj);
	//m_Label->Clone(;
	CVGGeoObject* Obj=(CVGGeoObject*)obj;
	if (Obj->m_Label!=NULL)
	{
		CVGObjLabel* lb=new CVGObjLabel();
		lb->Clone(Obj->m_Label);
		lb->m_Text=&m_Name;
		lb->m_Object=this;
	}

	m_bTrace=Obj->m_bTrace;
	m_bAnimate=Obj->m_bAnimate;
	m_bShowLabel=Obj->m_bShowLabel;
	m_bShowLabelValue=Obj->m_bShowLabelValue;
}

CString GetObjTypeName( int nID )
{
	switch (nID)
	{
	case OBJ_POINT: return L"Điểm";
	case OBJ_LINE: return L"Đường thẳng";
	case OBJ_RAY: return L"Tia";
	case OBJ_SEGMENT: return L"Đoạn thẳng";
	case OBJ_VECTOR: return L"Véc-tơ";
	case OBJ_CIRCLE: return L"Đường tròn";
	case OBJ_TEXT: return L"Văn bản";
	case OBJ_POLYGON: return L"Đa giác";
	case OBJ_ANGLE: return L"Góc";
	case OBJ_IMAGE: return L"Ảnh";
	case OBJ_BUTTON: return L"Nút bấm";
	case OBJ_CHECKBOX: return L"CheckBox";
	case OBJ_PANEL: return L"Vùng làm việc";
	case OBJ_CONIC: return L"Conic";
	}
	return NULL;
}

CString GetObjCommandTypeName( int nID )
{
	switch (nID)
	{
	case OBJ_POINT: return LoadAppString(IDS_POINT);
	case OBJ_LINE: return LoadAppString(IDS_LINE);
	case OBJ_RAY: return LoadAppString(IDS_RAY);
	case OBJ_SEGMENT: return LoadAppString(IDS_SEGMENT);
	case OBJ_VECTOR: return LoadAppString(IDS_VECTOR);
	case OBJ_CIRCLE: return LoadAppString(IDS_CIRCLE);
	case OBJ_TEXT: return L"Text";
	case OBJ_POLYGON: return L"Polygon";
	case OBJ_ANGLE: return L"Angle";
	case OBJ_IMAGE: return L"Image";
	case OBJ_BUTTON:return L"Button";
	case OBJ_CHECKBOX: return L"CheckBox";
	case OBJ_PANEL: return L"Panel";
	case OBJ_CONIC: return L"Conic";
	}
	return NULL;
}

int GetObjType( CString str )
{
	if (str==LoadAppString(IDS_POINT)) return OBJ_POINT;
	else if (str==L"Points") return OBJ_POINTS;
	else if (str==LoadAppString(IDS_HIDDEN_POINT)) return OBJ_POINT_HIDDEN_CHILD;
	else if (str==LoadAppString(IDS_LINE)) return OBJ_LINE;
	else if (str==LoadAppString(IDS_RAY)) return OBJ_RAY;
	else if (str==LoadAppString(IDS_SEGMENT)) return OBJ_SEGMENT;
	else if (str==LoadAppString(IDS_VECTOR)) return OBJ_VECTOR;
	else if (str==LoadAppString(IDS_CIRCLE)) return OBJ_CIRCLE;
	else if (str==L"CardinalSpline") return OBJ_CARDINAL_SPLINE;
	else if (str==L"BezierSpline") return OBJ_BEZIER_SPLINE;
	else if (str==L"Text") return OBJ_TEXT;
	else if (str==L"Angle") return OBJ_ANGLE;
	else if (str==L"Polygon") return OBJ_POLYGON;
	else if (str==L"Image") return OBJ_IMAGE;
	else if (str==L"Button") return OBJ_BUTTON;
	else if (str==L"CheckBox") return OBJ_CHECKBOX;
	else if (str==L"Panel") return OBJ_PANEL;
	else if (str==L"Number") return OBJ_EXPRESSION;
	else if (str==L"VectorExp") return OBJ_EXPRESSION;
	else if (str==L"Conic") return OBJ_CONIC;
	return -1;
}
