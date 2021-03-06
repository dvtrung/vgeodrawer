#include "StdAfx.h"
#include "VGObjContainer.h"
#include "resource.h"
#include "appfunc.h"
#include "GeoObjTree.h"
#include "InputDlg.h"
#include "TextEditorDlg.h"
#include "ExpressionEditorDlg.h"


CVGObjContainer::CVGObjContainer(bool bTemp)
{	
	m_pObjTree=NULL;
	cmdCount=0;
	m_nUndoPos=-1;

	m_bIsTesting=FALSE;
	m_nTestStartPos=0;

	m_strReplacedNameArr=NULL;

	m_LabelFont=new Font(L"Tahoma",10);

	m_bgBitmap=NULL;

	m_bTemp=bTemp;
}

void CVGObjContainer::Init()
{
	if (!m_bTemp)
	{
		CVGLine* l1=(CVGLine*)AddObject(OBJ_LINE);
		l1->m_a=1;
		l1->m_b=0;
		l1->m_c=0;
		l1->m_Color=Color::Black;
		l1->m_Name=L"y-Axis";

		CVGLine* l2=(CVGLine*)AddObject(OBJ_LINE);
		l2->m_a=0;
		l2->m_b=1;
		l2->m_c=0;
		l2->m_Color=Color::Black;
		l2->m_Name=L"x-Axis";
	}
}

CVGObjContainer::~CVGObjContainer(void)
{
	if (m_strReplacedNameArr!=NULL) delete m_strReplacedNameArr;
	delete m_LabelFont;
}

void CVGObjContainer::ResetContent()
{
	for (int i=0;i<m_Array.GetSize();i++)
		delete m_Array[i];
	m_Array.RemoveAll();
	m_pObjTree->DeleteAllItems();
	ResetUndoArray();
}

void CVGObjContainer::ResetUndoArray()
{
	int count=m_UndoType.GetSize();
	for (int i=0;i<count;i++)
		RemoveUndoArrayElement(0);
}


void CVGObjContainer::RefreshDocking()
{
	for (int i=0;i<m_Array.GetSize();i++)
		if (m_Array[i]->m_Type==OBJ_PANEL)
		{
			((CVGPanel*)m_Array[i])->RefreshDocking(&m_AxisInfo);
		}
}

void CVGObjContainer::RefreshAxisLine()
{
	m_Array[1]->m_bVisible=m_graphAxis.m_bOx;
	m_Array[0]->m_bVisible=m_graphAxis.m_bOy;
}

CVGObject* CVGObjContainer::operator [](int nIndex)
{
	return m_Array[nIndex];
}

CVGObject* CVGObjContainer::operator [](CString strName)
{
	return GetObjByName(strName);
}

CVGObject* CVGObjContainer::GetObjByName(CString strName)
{
	for (int i=0;i<m_Array.GetSize();i++)
		if (strName==m_Array[i]->m_Name) return m_Array[i];
	return NULL;
}

int CVGObjContainer::GetObjIndex(CVGObject* obj)
{
	for (int i=0;i<m_Array.GetSize();i++) if (obj==m_Array[i]) return i;
	return -1;
}

CVGObject* CVGObjContainer::GetAt(int nIndex)
{
	return m_Array[nIndex];
}

int CVGObjContainer::GetSize()
{
	return (int)m_Array.GetSize();
}

void CVGObjContainer::StartTestObj()
{
	m_nTestStartPos=GetSize();
	m_bIsTesting=TRUE;
}

void CVGObjContainer::DeleteTestObj()
{
	if (m_bIsTesting)
	{
		int count=GetSize();
		for (int i=m_nTestStartPos;i<count;i++)
		{
			CVGObject* obj=m_Array[i];
			delete obj;
		}
		m_Array.RemoveAt(m_nTestStartPos,count-m_nTestStartPos);
		if (GetCapture()==pView->m_hWnd) ReleaseCapture();
	}
	m_bIsTesting=FALSE;
	m_bIsTestingPoint=FALSE;
	pView->m_bTestPointMoving=FALSE;
}

void CVGObjContainer::SetObjParent(CVGBound* obj, CVGPanel* panel)
{
	if (obj->m_ParentPanel!=NULL)
	{
		for (int i=0;i<obj->m_ParentPanel->m_objChild.GetSize();i++)
			if (obj->m_ParentPanel->m_objChild[i]==obj) obj->m_ParentPanel->m_objChild.RemoveAt(i);
		obj->m_ParentPanel=NULL;
	}

	if (panel!=NULL)
	{
		obj->m_ParentPanel=panel;
		panel->m_objChild.Add(obj);

		int i=GetObjIndex(obj);
		int j=GetObjIndex(panel);
		if (i<j)
		{
			m_Array.RemoveAt(i);
			m_Array.InsertAt(j,obj);
		}
	}
}

void CVGObjContainer::DrawBackground(int width, int height)
{
	if (m_bgBitmap!=NULL) delete m_bgBitmap;
	m_bgBitmap=new Bitmap(width, height, PixelFormat32bppARGB);

	Graphics gr(m_bgBitmap);
	gr.SetTextRenderingHint(TextRenderingHintSingleBitPerPixelGridFit);

	m_graphAxis.DrawGrid(&gr,width,height,&m_AxisInfo);
	m_graphAxis.DrawAxis(&gr,width,height,&m_AxisInfo);
}

void CVGObjContainer::DrawBackground(Graphics* gr, int width, int height)
{
	m_graphAxis.DrawGrid(gr,width,height,&m_AxisInfo);
	m_graphAxis.DrawAxis(gr,width,height,&m_AxisInfo);
}

void CVGObjContainer::Draw(Graphics* gr)
{
	RefreshAxisLine();
	if (m_bgBitmap==NULL) return;

	gr->Clear(Color(255,255,255));
	gr->SetSmoothingMode(SmoothingModeAntiAlias);
	
	Graphics traceGr(m_bgBitmap);
	Draw(&traceGr,TRUE);

	gr->DrawImage(m_bgBitmap,0,0,m_bgBitmap->GetWidth(),m_bgBitmap->GetHeight());
	
	//Ve doi tuong hinh hoc

	Draw(gr,FALSE);
}

// Vẽ toàn bộ các đối tượng
void CVGObjContainer::Draw(Graphics* gr, BOOL bTrace)
{
	for (int type=0;type<OBJ_PANEL;type++)
	{
		for (int i=0;i<m_Array.GetSize();i++)
		{
			if (m_Array[i]->m_Type==type)
			{
				if (!(m_Array[i])->IsVisible()) continue;
				if (!(bTrace && !(IsGeoObject(m_Array[i]->m_Type)))) // Nếu bTrace và không phải là đt hình học thì ko vẽ
				{
					if (!(bTrace && ((CVGGeoObject*)m_Array[i])->m_bTrace==FALSE)) ((CVGGeoObject*)m_Array[i])->Draw(gr, &m_AxisInfo, bTrace);
				}
			}
		}
	}

	if (bTrace) return;
	for (int i=0;i<m_Array.GetSize();i++)
	{
		if (m_Array[i]->m_Type==OBJ_PANEL)
		{
			if (!(m_Array[i])->IsVisible()) continue;

			CVGBound* obj=(CVGBound*)m_Array[i];
			if (obj->m_ParentPanel!=NULL)
			{
				gr->SetClip(obj->m_ParentPanel->GetVisibleRect());
				obj->Draw(gr, &m_AxisInfo, false);
				gr->ResetClip();
			}
			else
				obj->Draw(gr, &m_AxisInfo, false);
		}
	}

	for (int i=0;i<m_Array.GetSize();i++)
	{
		if (m_Array[i]->m_Type!=OBJ_PANEL && IsBoundType(m_Array[i]->m_Type))
		{
			if (!(m_Array[i])->IsVisible()) continue;

			CVGBound* obj=(CVGBound*)m_Array[i];
			if (obj->m_ParentPanel!=NULL)
			{
				gr->SetClip(obj->m_ParentPanel->GetVisibleRect());
				obj->Draw(gr, &m_AxisInfo, false);
				gr->ResetClip();
			}
			else
				obj->Draw(gr, &m_AxisInfo, false);
		}
	}
}

void CVGObjContainer::Draw()
{
	pView->Draw();
}

CString CVGObjContainer::GetName(int nIndex)
{
	if (m_strReplacedNameArr!=NULL && nIndex<m_strReplacedNameArr->GetSize())
		return m_strReplacedNameArr->GetAt(nIndex);
	else
		return m_Array[nIndex]->m_Name;
}

CVGObject* CVGObjContainer::AddObject(ObjType objType)
{
	CVGObject* obj;
	switch (objType)
	{
	case OBJ_POINT:
		obj=new CVGPoint();
		obj->m_Type=OBJ_POINT; 
		(obj)->m_Color=m_defaultValue->m_clrDefaultPoint;
		((CVGPoint*)obj)->m_bShowPointers=m_defaultValue->m_bPointer;
		((CVGPoint*)obj)->m_Size=m_defaultValue->m_nPointSize;
		break;
	case OBJ_POINT_HIDDEN_CHILD:
		obj=new CVGPoint();
		obj->m_Type=OBJ_POINT_HIDDEN_CHILD;
		obj->m_bTemp=TRUE; (obj)->m_bVisible=false; break;
	case OBJ_LINE:
		obj=new CVGLine();
		obj->m_Type=OBJ_LINE; 
		(obj)->m_Color=m_defaultValue->m_clrDefaultLine;
		((CVGLine*)obj)->m_bCutSuperfluousPart=m_defaultValue->m_bLineShowSeg;
		((CVGLine*)obj)->m_DashStyle=m_defaultValue->m_nLineStyle;
		((CVGLine*)obj)->m_PenWidth=m_defaultValue->m_nLineWidth;
		break;
	case OBJ_SEGMENT:
		obj=new CVGSegment();
		obj->m_Type=OBJ_SEGMENT; 
		(obj)->m_Color=m_defaultValue->m_clrDefaultSeg;
		((CVGLine*)obj)->m_DashStyle=m_defaultValue->m_nSegStyle;
		((CVGLine*)obj)->m_PenWidth=m_defaultValue->m_nSegWidth;
		break;
	case OBJ_RAY:
		obj=new CVGRay();
		obj->m_Type=OBJ_RAY; 
		obj->m_Color=m_defaultValue->m_clrDefaultLine;
		break;
	case OBJ_VECTOR:
		obj=new CVGVector();
		obj->m_Type=OBJ_VECTOR; 
		obj->m_Color=m_defaultValue->m_clrDefaultVector;
		break;
	case OBJ_CIRCLE:
		obj=new CVGCircle();
		(obj)->m_Color=m_defaultValue->m_clrDefaultCircle;
		((CVGCircle*)obj)->m_PenWidth=m_defaultValue->m_nCirWidth;
		((CVGCircle*)obj)->m_DashStyle=m_defaultValue->m_nCirStyle;
		break;
	case OBJ_CONIC:
		obj=new CVGConic();
		break;
	case OBJ_TEXT:
		obj=new CVGText();
		obj->m_Type=OBJ_TEXT; break;
	case OBJ_FUNCTION:
		obj=new CVGFunction();
		obj->m_Type=OBJ_FUNCTION;
		((CVGFunction*)obj)->m_expFunc.m_ScriptObject=m_ScriptObject; break;
		break;
	case OBJ_CARDINAL_SPLINE:
		obj=new CVGCardinalSpline();
		break;
	case OBJ_BEZIER_SPLINE:
		obj=new CVGBezierSpline();
		break;
	case OBJ_POLYGON:
		obj=new CVGPolygon();
		obj->m_Type=OBJ_POLYGON; break;
	case OBJ_ANGLE:
		obj=new CVGAngle();
		obj->m_Type=OBJ_ANGLE; break;
	case OBJ_IMAGE:
		obj=new CVGImage();
		obj->m_Type=OBJ_IMAGE; break;
	case OBJ_BUTTON:
		obj=new CVGButton();
		break;
	case OBJ_CHECKBOX:
		obj=new CVGCheckBox();
		break;
	case OBJ_PANEL:
		obj=new CVGPanel();
		((CVGPanel*)obj)->RefreshDocking(&m_AxisInfo);
		break;
	case OBJ_SLIDER:
		obj=new CVGSlider();
		break;
	case OBJ_RADIO_BUTTON:
		obj=new CVGRadioButton();
		break;
	case OBJ_EXPRESSION:
		obj=new CVGExpression();
		((CVGExpression*)obj)->m_Exp.m_ScriptObject=m_ScriptObject;
		break;
	}
	if (IsBoundType(obj->m_Type)) SetName(obj,GetNextAvailableName(obj->m_Type));
	obj->m_expAvailCond.m_ScriptObject=m_ScriptObject;
	if (obj!=NULL) { m_Array.Add(obj); return obj; }
	else return NULL;
}

void CVGObjContainer::AddFunctionD()
{
	CInputDlg dlg(L"Function",pView);
	dlg.AddStatic(L"Phương trình y=f(x) :");
	CString str;
	dlg.AddEdit(&str);
	if (dlg.DoModal()==IDOK)
	{
		CVGFunction* f=(CVGFunction*)AddObject(OBJ_FUNCTION);
		f->SetFunction(str,&m_Array,m_strReplacedNameArr);
		f->m_bCalc=FALSE;
		f->FuncCalc(&m_AxisInfo);
		pView->Draw();
	}
}

// Hiện hộp thoại nhập đối tượng là biểu thức
// void CVGObjContainer::AddExpressionD(CPoint pt)
// {
// 	CExpressionEditorDlg dlgEditor;
// 	if (dlgEditor.DoModal()==IDOK)
// 	{
// 		CVGExpression* obj=(CVGExpression*)AddObject(OBJ_EXPRESSION);
// 		CArray<int> arr; arr.Add(m_Array.GetSize()-1);
// 		OnAddObjs(&arr);
// 		obj->SetExpression(dlgEditor.strExp,&m_Array,m_strReplacedNameArr);
// 		obj->SetName(GetNextAvailableName(OBJ_NUMBER),NULL,&m_AxisInfo);
// 		obj->m_Definition=dlgEditor.strExp;
// 		//obj->m_Pos=PointD(pt.x,pt.y);
// 		Calc();
// 		pView->Draw();
// 	}
// }

// void CVGObjContainer::AddChechBoxD(CPoint pt)
// {
// 	CInputDlg dlg(L"CheckBox");
// 	dlg.AddStatic(L"Tiêu đề ChechBox");
// 	CString strText;
// 	dlg.AddEdit(&strText);
// 	if (dlg.DoModal()==IDOK)
// 	{
// 		CVGButton* obj=(CVGButton*)AddObject(OBJ_BUTTON);
// 		obj->m_Text=strText;
// 		obj->m_Left=pt.x;
// 		obj->m_Top=pt.y;
// 	}
// }

void CVGObjContainer::AddRegularPolygon(CVGPoint* pt1, CVGPoint* pt2, int num)
{
// 	if (!m_bTemp) cmdCount++;
// 	CVGExpression* angle=(CVGExpression*)AddObject(OBJ_NUMBER);
// 	angle->m_Param.Add(pt1);
// 	angle->m_Param.Add(pt2);
// 	angle->m_Value=Math::ConvertFromRadianToAngleUnit((num-2)*pi/num);
// 	angle->m_bTemp=true;
// 	angle->m_cmdIndex=cmdCount;
// 	CString sDefinition=L"Polygon("+pt1->m_Name+L", "+pt2->m_Name+L", "+Math::IntToString(num)+L")";
// 
// 	int start=m_Array.GetSize();
// 	CVGSegment* seg=(CVGSegment*)AddObject(OBJ_SEGMENT);
// 	seg->SetThroughTwoPoints(pt1,pt2);
// 	AddObjTreeItem(seg);
// 	SetName(seg,GetNextAvailableName(OBJ_SEGMENT));
// 	seg->m_Definition=sDefinition;
// 	for (int i=2;i<num;i++)
// 	{
// 		CVGPoint* pt=(CVGPoint*)AddObject(OBJ_POINT);
// 		CVGSegment* s=(CVGSegment*)AddObject(OBJ_SEGMENT);
// 		
// 		if (i==2)
// 		{
// 			pt->SetRotate(pt1,pt2,angle);
// 			s->SetThroughTwoPoints(pt2,pt);
// 		}
// 		else if (i==3)
// 		{
// 			pt->SetRotate(pt2,(CVGPoint*)m_Array.GetAt(m_Array.GetSize()-4),angle);
// 			s->SetThroughTwoPoints(pt,(CVGPoint*)m_Array.GetAt(m_Array.GetSize()-4));
// 		}
// 		else
// 		{
// 			pt->SetRotate((CVGPoint*)m_Array.GetAt(m_Array.GetSize()-6),(CVGPoint*)m_Array.GetAt(m_Array.GetSize()-4),angle);
// 			s->SetThroughTwoPoints(pt,(CVGPoint*)m_Array.GetAt(m_Array.GetSize()-4));
// 		}
// 		pt->m_Definition=sDefinition;
// 		pt->m_cmdIndex=cmdCount;
// 		s->m_cmdIndex=cmdCount;
// 		s->m_Definition=sDefinition;
// 		
// 		AddObjTreeItem(pt);
// 		AddObjTreeItem(s);
// 		SetName(pt,GetNextAvailableName(OBJ_POINT));
// 		SetName(s,GetNextAvailableName(OBJ_SEGMENT));
// 	}
// 	seg=(CVGSegment*)AddObject(OBJ_SEGMENT);
// 	seg->SetThroughTwoPoints(pt1,(CVGPoint*)m_Array.GetAt(m_Array.GetSize()-3));
// 	AddObjTreeItem(seg);
// 	SetName(seg,GetNextAvailableName(OBJ_SEGMENT));
// 	seg->m_Definition=sDefinition;
// 	
// 	// Các điểm đc tạo mới có liên kết
// 	// Khi xóa sẽ xóa tất cả
// 	for (int i=start;i<m_Array.GetSize();i++)
// 	{
// 		for (int j=start;j<m_Array.GetSize();j++)
// 			(m_Array[i])->m_Param.Add(m_Array[j]);
// 		angle->m_Param.Add(m_Array[i]);
// 	}
// 	Calc();
}

int CVGObjContainer::ClickButton()
{
	for (int i=0;i<m_Array.GetSize();i++)
	{
		if (m_Array[i]->m_Type==OBJ_BUTTON)
		{
			if (((CVGButtonBase*)m_Array[i])->m_bButtonMouseDown)
			{
				AddCommands(((CVGButtonBase*)m_Array[i])->m_Command);
				((CVGButtonBase*)m_Array[i])->OnClick();
				Calc();
				return i;
			}
		}
		if (m_Array[i]->m_Type==OBJ_CHECKBOX)
		{
			if (((CVGButtonBase*)m_Array[i])->m_bButtonMouseDown)
			{
				((CVGCheckBox*)m_Array[i])->m_bState=!((CVGCheckBox*)m_Array[i])->m_bState;
				AddCommands(((CVGButtonBase*)m_Array[i])->m_Command);
				((CVGButtonBase*)m_Array[i])->OnClick();
				Calc();
				return i;
			}
		}
		if (m_Array[i]->m_Type==OBJ_RADIO_BUTTON)
		{
			if (((CVGButtonBase*)m_Array[i])->m_bButtonMouseDown)
			{
				for (int j=0;j<m_Array.GetSize();j++)
					if (m_Array[j]->m_Type==OBJ_RADIO_BUTTON && ((CVGRadioButton*)m_Array[i])->m_ParentPanel==((CVGRadioButton*)m_Array[j])->m_ParentPanel)
						((CVGRadioButton*)m_Array[j])->m_bState=false;
				((CVGRadioButton*)m_Array[i])->m_bState=TRUE;
				AddCommands(((CVGButtonBase*)m_Array[i])->m_Command);
				((CVGButtonBase*)m_Array[i])->OnClick();
				Calc();
				return i;
			}
		}
	}
	return -1;
}

bool CVGObjContainer::ResetButtonState()
{
	bool b=false;
	for (int i=0;i<m_Array.GetSize();i++)
	{
		if (IsButtonType(m_Array[i]->m_Type))
		{
			if (((CVGButtonBase*)m_Array[i])->m_bButtonMouseOver==true)
			{
				((CVGButtonBase*)m_Array[i])->m_bButtonMouseOver=false;
				b=true;
			}
			if (((CVGButtonBase*)m_Array[i])->m_bButtonMouseDown==true)
			{
				((CVGButtonBase*)m_Array[i])->m_bButtonMouseDown=false;
				b=true;
			}
		}
		else if (m_Array[i]->m_Type==OBJ_SLIDER)
			if (((CVGSlider*)m_Array[i])->m_bMouseOver)
			{
				((CVGSlider*)m_Array[i])->m_bMouseOver=false;
				b=true;
			}
	}
	return b;
}

void CVGObjContainer::CheckMouseOverGeoObj(Point clientPoint, bool& bIsLabel, CVGObject*& i1, CVGObject*& i2)
{
	i1=NULL;i2=NULL;
	BOOL bTwo=FALSE;
	bIsLabel=false;

	int count=m_bIsTesting ? m_nTestStartPos : m_Array.GetSize();

	for (int i=0;i<count;i++)
	{
		if (m_Array[i]->m_Type==OBJ_POINT || m_Array[i]->m_Type==OBJ_ANGLE)
		{
			if (!m_Array[i]->IsVisible()) continue;
			if ((m_Array[i])->CheckMouseOver(clientPoint, &m_AxisInfo))
			{
				i1=m_Array[i];
				return;
			}
		}
	}

	for (int i=0;i<count;i++)
	{
		if (m_Array[i]->m_Type!=OBJ_POINT && m_Array[i]->m_Type!=OBJ_ANGLE 
			&& m_Array[i]->m_Type!=OBJ_POLYGON)
		{
			if (!m_Array[i]->IsVisible()) continue;
			if (m_Array[i]->CheckMouseOver(clientPoint, &m_AxisInfo))
			{
				if (i1==NULL)
					i1=m_Array[i];
				else
				{
					i2=m_Array[i];
					return;
				}
			}
		}
	}

	for (int i=0;i<count;i++)
	{
		if (m_Array[i]->m_Type==OBJ_POINT)
		{
			if (!m_Array[i]->IsVisible()) continue;
			//if (((CVGGeoObject*)m_Array[i])->m_Label->CheckMouseOver(clientPoint,&m_AxisInfo))
			if (IsGeoObject(m_Array[i]->m_Type))
				if (((CVGGeoObject*)m_Array[i])->m_Label!=NULL && 
					((CVGGeoObject*)m_Array[i])->m_Label->CheckMouseOver(clientPoint,&m_AxisInfo))
				{
					bIsLabel=true;
					i1=m_Array[i];
					return;
				}
		}
	}

	if (i1==NULL)
	{
		for (int i=0;i<count;i++)
			if (m_Array[i]->m_Type==OBJ_POLYGON)
			{
				if (!m_Array[i]->IsVisible()) continue;
				if ((m_Array[i])->CheckMouseOver(clientPoint, &m_AxisInfo))
				{
					i1=m_Array[i];
					return;
				}
			}
	}
}

void CVGObjContainer::CheckMouseOverObj(Point clientPoint, bool& bIsLabel, CVGObject*& i1, CVGObject*& i2)
{
	i1=NULL;i2=NULL;
	BOOL bTwo=FALSE;
	bIsLabel=false;

	int count=m_bIsTesting ? m_nTestStartPos : m_Array.GetSize();

	for (int i=count-1;i>=0;i--)
	{
		if (m_Array[i]->m_Type!=OBJ_PANEL && IsBoundType(m_Array[i]->m_Type))
		{
			if (!m_Array[i]->IsVisible()) continue;
			if ((m_Array[i])->CheckMouseOver(clientPoint, &m_AxisInfo))
			{
				i1=m_Array[i];
				return;
			}
			else if ((IsButtonType(m_Array[i]->m_Type)) && ((CVGButtonBase*)m_Array[i])->CheckMouseOverButton(clientPoint))
			{
				i1=m_Array[i];
				return;
			}
		}
	}
	for (int i=count-1;i>=0;i--)
	{
		if (m_Array[i]->m_Type==OBJ_PANEL)
		{
			if (!m_Array[i]->IsVisible()) continue;
			if ((m_Array[i])->CheckMouseOver(clientPoint, &m_AxisInfo))
			{
				i1=m_Array[i];
				return;
			}
		}
	}

	CheckMouseOverGeoObj(clientPoint, bIsLabel, i1, i2);
}

void CVGObjContainer::CheckMouseOver(Point clientPoint, bool& bIsLabel, bool& bIsBoundMark, bool& bIsMouseOverButton, CVGObject*& i1, CVGObject*& i2 ,int& nState, int& x, int& y)
{
	i1=NULL;i2=NULL;
	BOOL bTwo=FALSE;
	bIsLabel=false;
	bIsBoundMark=false;
	bIsMouseOverButton=false;

	int count=m_bIsTesting ? m_nTestStartPos : m_Array.GetSize();

	for (int i=count-1;i>=0;i--)
	{
		if (m_Array[i]->m_Type!=OBJ_PANEL && (IsBoundType(m_Array[i]->m_Type)))
		{
			if (!m_Array[i]->IsVisible()) continue;
			if ((m_Array[i])->CheckMouseOver(clientPoint, &m_AxisInfo))
			{
				i1=m_Array[i];
				return;
			}
			else 
			{
				int k=((CVGBound*)m_Array[i])->CheckMouseOverState(clientPoint, &m_AxisInfo, x, y);
				if (k!=0)
				{
					bIsBoundMark=true;
					nState=k;
					i1=m_Array[i];
					return;
				}
				else if ((IsButtonType(m_Array[i]->m_Type)) && ((CVGButtonBase*)m_Array[i])->CheckMouseOverButton(clientPoint))
				{
					i1=m_Array[i];
					bIsMouseOverButton=true;
					return;
				}
				else if (m_Array[i]->m_Type==OBJ_SLIDER && ((CVGSlider*)m_Array[i])->CheckMouseOverSlider(clientPoint))
				{
					i1=m_Array[i];
					bIsMouseOverButton=true;
					return;
				}
			}
		}
	}

	for (int i=count-1;i>=0;i--)
	{
		if (m_Array[i]->m_Type==OBJ_PANEL)
		{
			if (!m_Array[i]->IsVisible()) continue;
			if ((m_Array[i])->CheckMouseOver(clientPoint, &m_AxisInfo))
			{
				i1=m_Array[i];
				return;
			}
			else
			{
				int k=((CVGBound*)m_Array[i])->CheckMouseOverState(clientPoint, &m_AxisInfo, x, y);
				if (k!=0)
				{
					bIsBoundMark=true;
					nState=k;
					i1=m_Array[i];
					return;
				}
			}
		}
	}

	CheckMouseOverGeoObj(clientPoint, bIsLabel, i1, i2);
	
}

int CVGObjContainer::CheckPtInPanel(Point pt, CVGBound* except)
{
	int count=m_bIsTesting ? m_nTestStartPos : m_Array.GetSize();

	for (int i=count-1;i>=0;i--)
	{
		if (m_Array[i]->m_Type==OBJ_PANEL && m_Array[i]!=except)
		{
			CVGPanel* panel=(CVGPanel*)m_Array[i];
			if (Rect(panel->m_Left,panel->m_Top,panel->m_Width,panel->m_Height).Contains(pt))
				return i;
		}
	}

	return -1;
}

int CVGObjContainer::CheckRectInPanel(Rect rect, CVGBound* except)
{
	int count=m_bIsTesting ? m_nTestStartPos : m_Array.GetSize();

	for (int i=count-1;i>=0;i--)
	{
		if (m_Array[i]->m_Type==OBJ_PANEL && m_Array[i]!=except)
		{
			CVGPanel* panel=(CVGPanel*)m_Array[i];
			if (Rect(panel->m_Left,panel->m_Top,panel->m_Width,panel->m_Height).Contains(rect))
				return i;
		}
	}

	return -1;
}


void CVGObjContainer::Select(CVGObject* obj, BOOL bInverse, BOOL bSelect)
{
	if (bInverse) obj->m_Select=!obj->m_Select;
	else obj->m_Select=bSelect;
	m_pObjTree->Invalidate();
}

void CVGObjContainer::SelectAll(BOOL bSelect)
{
	for (int i=0;i<m_Array.GetSize();i++) Select(m_Array[i],FALSE,bSelect);
	m_pObjTree->Invalidate();
}

void CVGObjContainer::MoveSelection(int x, int y, CPoint startPt)
{
	for(int i=0;i<m_Array.GetSize();i++)
	{
		CVGObject* obj=m_Array[i];
		if (IsPointType(m_Array[i]->m_Type))
		{
			((CVGPoint*)obj)->m_moved=FALSE;
		}
		if (obj->m_Select || obj->m_HighLight) obj->Move(x,y,startPt,&m_AxisInfo);
	}
	Calc();
}

void CVGObjContainer::SetIntersection(CVGGeoObject* obj1, CVGGeoObject* obj2, CVGPoint* pt)
{
	if ((IsLineType(obj1->m_Type)) && (IsLineType(obj2->m_Type)))
	{
		pt->m_Mode=POINT_MODE_INTERSECTION_LINE_LINE;
		pt->m_Param.RemoveAll();
		pt->m_Param.Add(obj1);
		pt->m_Param.Add(obj2);
	}
	else if ((IsLineType(obj1->m_Type)) && obj2->m_Type==OBJ_CIRCLE)
	{
		pt->SetIntersectionLineCircle((CVGLineBase*)obj1,(CVGCircle*)obj2);
	}
	else if ((IsLineType(obj2->m_Type)) && obj1->m_Type==OBJ_CIRCLE)
	{
		pt->SetIntersectionLineCircle((CVGLineBase*)obj2,(CVGCircle*)obj1);
	}
	else if (obj1->m_Type==OBJ_CIRCLE && obj2->m_Type==OBJ_CIRCLE)
	{
		pt->SetIntersectionCircleCircle((CVGCircle*)obj1,(CVGCircle*)obj2);
	}
	else if (obj1->m_Type==OBJ_LINE && obj2->m_Type==OBJ_CONIC)
	{
		pt->SetIntersectionLineEllipse((CVGLineBase*)obj1,(CVGConic*)obj2);
	}
	else if (obj1->m_Type==OBJ_CONIC && obj2->m_Type==OBJ_LINE)
	{
		pt->SetIntersectionLineEllipse((CVGLineBase*)obj2,(CVGConic*)obj1);
	}
}

void CVGObjContainer::SetName(CVGObject* obj,CString name)
{
	if (IsGeoObject(obj->m_Type))
	{
		if (!m_bTemp && !m_bIsTesting) ((CVGGeoObject*)obj)->SetName(name,m_LabelFont,&m_AxisInfo);
		else obj->m_Name=name;
	}
	else obj->m_Name=name;
}

CString CVGObjContainer::GetName(CStringArray &objOutputName,int index, int type)
{
	if (index<objOutputName.GetSize()) return objOutputName[index];
	else return GetNextAvailableName(type);
}

void CVGObjContainer::AddCommands(CString strCommandLines)
{
	int p_i=0;
	int i=0;
	CString str;
	while (i<strCommandLines.GetLength())
	{
		i=strCommandLines.Find(L"\r\n",p_i);
		if (i==-1) i=strCommandLines.GetLength();
		str=strCommandLines.Mid(p_i,i-p_i);
		AddCommand(str);
		p_i=i+2;
	}
}

void CVGObjContainer::DeleteObj(CVGObject* obj)
{
	for (int i=0;i<m_Array.GetSize();i++)
		if (m_Array[i]==obj)
		{
			delete obj;
			m_Array.RemoveAt(i);
			return;
		}
}

CVGObject* CVGObjContainer::AddExpression(CString strExp, bool bTemp)
{
	CVGExpression* exp=(CVGExpression*)AddObject(OBJ_EXPRESSION);
	exp->m_Exp.m_ScriptObject=m_ScriptObject;
	if (!exp->SetExp(strExp,&m_Array,m_strReplacedNameArr)) { DeleteObj(exp); return NULL; }
	else
	{
		exp->m_bTemp=bTemp;
		return exp;
	}
}

BOOL CVGObjContainer::AddCommand(CString strCmd, CArray<CVGObject*>* objOutputArray, CString* strOutputString)
{
	CArray<CVGObject*> returnObj;
	//strCmd.Remove(' ');

	// Tên một đối tượng ?
	
	if (!strOutputString)
	{
		for (int i=0;i<GetSize();i++)
		{
			if (strCmd==GetName(i)) { returnObj.Add(m_Array[i]); break; }
		}
	}
	else 
	{
		for (int i=0;i<GetSize();i++)
		{
			if (strCmd==GetName(i)) { *strOutputString=L"$"+m_Array[i]->GetValue(true)+L"$"; return TRUE; }
		}

		if (GetExpStringValue(strCmd, *strOutputString)) return TRUE;
	}
	
	if (returnObj.GetSize()==1)
	{
		if (objOutputArray!=NULL)
		{
			objOutputArray->RemoveAll();
			objOutputArray->Add(returnObj[0]);
			if (strOutputString!=NULL)
				*strOutputString=L"$"+returnObj[0]->GetValue(TRUE)+L"$";
		}
		return TRUE;
	}
	
	// Biểu thức ?
	CVGObject* exp=AddExpression(strCmd);
	if (exp)
	{
		if (objOutputArray!=NULL)
		{
			objOutputArray->RemoveAll();
			objOutputArray->Add(exp);
		}
		return TRUE;
	}

	int pos;
	CString strErr;
	pos=strCmd.Find(L":=");
	if (pos!=-1) // :=
	{
		CString strLHS=strCmd.Left(pos);
		CString strRHS=strCmd.Right(strCmd.GetLength()-2-pos);
		int pos2=strLHS.Find(L"."); if (pos2==-1) pos2=strLHS.GetLength();
		CString strObj=strLHS.Left(pos2); CVGObject* obj=GetObjByName(strObj);
		if (obj==NULL && pos2==strLHS.GetLength()) // Khai báo biến mới
		{
			CVGExpression* exp=(CVGExpression*)AddObject(OBJ_EXPRESSION);
			exp->m_Name=strObj;
			exp->m_Exp.m_ScriptObject=m_ScriptObject;
			exp->SetExp(strRHS,&m_Array,m_strReplacedNameArr);
		}
		else // Đổi thuộc tính
		{
			CString strValue=(pos2==strLHS.GetLength()) ? L"" : strLHS.Right(strLHS.GetLength()-1-pos2);
			if (obj->m_Type==OBJ_EXPRESSION && strValue=="")
			{
				((CVGExpression*)obj)->SetExp(strRHS,&m_Array,m_strReplacedNameArr);
				Calc();
			}
			else
			{
				CVGExpression* exp=(CVGExpression*)AddObject(OBJ_EXPRESSION);
				exp->SetExp(strRHS,&m_Array,m_strReplacedNameArr);
				exp->Calc(&m_AxisInfo);
				//CValue* vl=GetExpValue(strRHS);
				if (!obj->SetValue(strValue,exp,&m_Array)) DeleteObj(exp);
				Calc();
			}
		}
		
		return TRUE;
	}
	else
		pos=strCmd.Find(':');

	/************************************************************************/
	// Câu lệnh có dấu ':'

	// Câu lệnh chuẩn VD : M:MidPoint(A,B)
	// Câu lệnh trong thư viện lệnh VD : d:PerpendicularBisector(A,B)
	/************************************************************************/
	if (pos!=-1) // Khai báo đối tượng là giá trị trả về một câu lệnh
	{
		CString strOut=strCmd.Left(pos); //Bên trái dầu :
		CStringArray objOutputName;
		GetParam(strOut,&objOutputName);

		for (int i=0;i<objOutputName.GetSize();i++) if (!CheckRegularName(objOutputName[i])) { strErr=L"Tên đối tượng không hợp lệ"; goto error; }
		for (int i=0;i<m_Array.GetSize();i++)
			for (int j=0;j<objOutputName.GetSize();j++) if (m_Array[i]->m_Name==objOutputName[j]) { strErr=L"Tên trùng với 1 đối tượng có sẵn"; goto error; }

		CString strF=strCmd.Right(strCmd.GetLength()-pos-1); //Bên phải dấu :

		CString strCmd; //Tên câu lệnh
		CStringArray aParam; //Danh sách tham số
		GetFuncAndParam(strF,strCmd,&aParam);

		if (strCmd==L"Exp" || strCmd==L"Expression")
		{
			if (aParam.GetSize()==1)
			{
				CVGExpression* expr=(CVGExpression*)AddObject(OBJ_EXPRESSION);
				expr->m_Exp.SetExpression(aParam[0],&m_Array,m_strReplacedNameArr);
				expr->m_Name=GetName(objOutputName,0,OBJ_EXPRESSION);
				returnObj.Add(expr);
			}
		}
		else if (strCmd==L"Function")
		{
			if (aParam.GetSize()==1)
			{
				CVGFunction* func=(CVGFunction*)AddObject(OBJ_FUNCTION);
				func->SetFunction(aParam[0],&m_Array,m_strReplacedNameArr);
				func->m_bCalc=FALSE;
				func->Calc(&m_AxisInfo);
				func->m_Name=GetName(objOutputName,0,OBJ_FUNCTION);
				returnObj.Add(func);
			}
		}
		else 
		{
			CArray<CVGObject*> objInput; //Đối tượng hình học tham số
			CArray<CVGObject*>objReturn;
			for (int i=0;i<aParam.GetSize();i++) 
			{
				if (!AddCommand(aParam[i], &objReturn)) { strErr=L"Câu lệnh không hợp lệ ("+aParam[i]+L")"; goto error; }
				if (objReturn.GetSize()!=0) objInput.Add(objReturn[0]);
			}

			DoCommand(strCmd,objInput,objOutputName,&returnObj,strOutputString);
		}
	}

	/************************************************************************/
	// Câu lệnh không có dấu ':'

	// Câu lệnh trả về một đối tượng VD : A
	// Câu lệnh khai báo một đối tượng VD : Point A(0,0)
	/************************************************************************/

	else // Không có dấu :
	{
		if (strCmd.Find('(')==-1)
		{				
			CArray<CVGObject*> objInput;
			CStringArray objOutputName;
			DoCommand(strCmd,objInput,objOutputName,&returnObj,strOutputString);
		}
		else AddCommand(L":"+strCmd,&returnObj,strOutputString);
	}

	if (objOutputArray!=NULL)
	{
		objOutputArray->RemoveAll();
		for (int i=0;i<returnObj.GetSize();i++)
			objOutputArray->Add(returnObj[i]);
	}
	return TRUE;
error:
	AfxGetApp()->DoMessageBox(strErr,MB_OK | MB_ICONERROR,-1);
	return FALSE;
}

//Kiểm tra xem mảng có phải toàn là điểm ko
BOOL CVGObjContainer::IsPointsArray(CArray<CVGObject*> &objArr)
{
	for (int i=0;i<objArr.GetSize();i++)
		if (objArr[i]->m_Type!=OBJ_POINT) return FALSE;
	return TRUE;
}

void CVGObjContainer::DoCommand(CString strCmd, CArray<CVGObject*> &objInput, CStringArray &objOutputName, CArray<CVGObject*>* objOutput, CString* strOutputString)
{
	int start=m_Array.GetSize();
	CArray<CVGObject*> returnObj;
	CString strErr;
	BOOL bDone=FALSE;

	int startIndex=(int)m_Array.GetSize();

	if (strCmd==L"Point")
	{
		if (objInput.GetSize()==2 && objInput[0]->m_Type==OBJ_EXPRESSION && objInput[1]->m_Type==OBJ_EXPRESSION)
		{
			CVGExpression* exp1=(CVGExpression*)objInput[0];
			CVGExpression* exp2=(CVGExpression*)objInput[1];
			CVGPoint* pt=(CVGPoint*)AddObject(OBJ_POINT);
				
			pt->m_Mode=POINT_MODE_EXP_EXP;
			pt->m_Param.Add(exp1); pt->m_Param.Add(exp2);

// 			if ((!exp1->m_bTemp && !exp2->m_bTemp) ||
// 				(!exp1->m_bTemp && !exp2->IsConstant()) ||
// 				(!exp2->m_bTemp && !exp1->IsConstant()))
// 			{
// 				pt->m_Mode=POINT_MODE_EXP_EXP;
// 				pt->m_Param.Add(exp1); pt->m_Param.Add(exp2);
// 			}
// 			else if (!exp1->m_bTemp || !exp1->IsConstant())
// 			{
// 				pt->m_Mode=POINT_MODE_EXP_NUM;
// 				pt->m_Param.Add(exp1); pt->m_y=exp2->GetDoubleValue();
// 				DeleteObj(exp2);
// 			}
// 			else if (!exp2->m_bTemp || !exp2->IsConstant())
// 			{
// 				pt->m_Mode=POINT_MODE_NUM_EXP;
// 				pt->m_Param.Add(exp2); pt->m_x=exp1->GetDoubleValue();
// 				DeleteObj(exp1);
// 			}
// 			else
// 			{
// 				pt->m_Mode=POINT_MODE_NUM_NUM;
// 				pt->m_x=exp1->GetDoubleValue(); pt->m_y=exp2->GetDoubleValue();
// 				DeleteObj(exp1); DeleteObj(exp2);
// 			}
// 			pt->m_Mode=POINT_MODE_REAL;
// 			// Khi xóa điểm thì xóa cả biểu thức
// 			if (objInput[0]->m_bTemp) 
// 			{	m_Array.Add(objInput[0]);(objInput[0])->m_Param.Add(pt);	}
// 			if (objInput[1]->m_bTemp) 
// 			{	m_Array.Add(objInput[1]); (objInput[1])->m_Param.Add(pt);	}
// 			pt->m_Param.Add(objInput[0]);
// 			pt->m_Param.Add(objInput[1]);
			SetName(pt,GetName(objOutputName,0,OBJ_POINT));
			returnObj.Add(pt);
		}
		if (objInput.GetSize()==0)
		{
			CVGPoint* pt=(CVGPoint*)AddObject(OBJ_POINT);
			int r1=rand();
			r1%=(drPadSize.Width/2);
			int r2=rand();
			r2%=(drPadSize.Height/2);
			pt->m_Mode=POINT_MODE_NUM_NUM;
			PointD ptD=m_AxisInfo.ToAxisPoint(Point(r1+drPadSize.Width/4,r2+drPadSize.Height/4));
			pt->m_x=ptD.X;
			pt->m_y=ptD.Y;
			SetName(pt,GetName(objOutputName,0,OBJ_POINT));
			returnObj.Add(pt);
		}
		if (objInput.GetSize()==1 && (IsLineType(objInput[0]->m_Type)))
		{
			CVGPoint* pt=(CVGPoint*)AddObject(OBJ_POINT);
			//((CVGLine*)objInput[0])->Set
			pt->SetChild((CVGLine*)objInput[0]);
			SetName(pt,GetName(objOutputName,0,OBJ_POINT));
			returnObj.Add(pt);
		}
		if (objInput.GetSize()==3 && (IsLineType(objInput[0]->m_Type)) && objInput[1]->m_Type==OBJ_POINT && objInput[2]->m_Type==OBJ_EXPRESSION)
		{
			CVGPoint* pt=(CVGPoint*)AddObject(OBJ_POINT);
			pt->m_Mode=POINT_MODE_ON_LINE_WITH_DISTANCE_TO_OTHER_POINT;
			pt->m_Param.Add(objInput[0]);
			pt->m_Param.Add(objInput[1]);
			pt->m_Param.Add(objInput[2]);
			SetName(pt,GetName(objOutputName,0,OBJ_POINT));
			returnObj.Add(pt);
		}
	}
	

	if (strCmd==L"Line" || strCmd==L"Segment" || strCmd==L"Ray")
	{
		CVGObject* obj=objInput[1];
		if (strCmd==L"Line" && objInput.GetSize()==3 && objInput[0]->m_Type==OBJ_EXPRESSION && objInput[1]->m_Type==OBJ_EXPRESSION && objInput[2]->m_Type==OBJ_EXPRESSION)
		{
			CVGLine* l=(CVGLine*)AddObject(OBJ_LINE);
			l->m_Mode=LINE_MODE_DEFINED;
			l->m_Param.Add(objInput[0]);
			l->m_Param.Add(objInput[1]);
			l->m_Param.Add(objInput[2]);
			SetName(l,GetName(objOutputName,0,OBJ_LINE));
			returnObj.Add(l);
		}
		else if (objInput.GetSize()==2 && objInput[0]->m_Type==OBJ_POINT && 
			(IsPointType(objInput[1]->m_Type)))
		{
			CVGLineBase* line;
			if (strCmd==L"Line") line=(CVGLine*)AddObject(OBJ_LINE);
			else if (strCmd=="Ray") line=(CVGRay*)AddObject(OBJ_RAY);
			else if (strCmd=="Segment") line=(CVGSegment*)AddObject(OBJ_SEGMENT);

			line->SetThroughTwoPoints((CVGPoint*)objInput[0],
				(CVGPoint*)objInput[1]);

			if (objInput[1]->m_Type==OBJ_POINT_HIDDEN_CHILD) 
			{
				((CVGPoint*)objInput[0])->m_objMove.Add((CVGPoint*)objInput[1]);
				//line->m_Param.Add(objInput[1]);
				(objInput[1])->m_Param.Add(line);
			}

			SetName(line,GetName(objOutputName,0,OBJ_LINE));
			returnObj.Add(line);
		}
	}

	if (strCmd==L"Vector")
	{
		if (objInput.GetSize()==2 && objInput[0]->m_Type==OBJ_POINT && objInput[1]->m_Type==OBJ_POINT)
		{
			CVGVector* obj=(CVGVector*)AddObject(OBJ_VECTOR);
			obj->m_Mode=VECTOR_MODE_POINT_POINT;
			obj->m_Param.Add(objInput[0]); obj->m_Param.Add(objInput[1]);
			SetName(obj,GetName(objOutputName,0,OBJ_LINE));
			returnObj.Add(obj);
		}
		else if (objInput.GetSize()==2 && objInput[0]->m_Type==OBJ_POINT && objInput[1]->m_Type==OBJ_EXPRESSION)
		{
			CVGVector* obj=(CVGVector*)AddObject(OBJ_VECTOR);
			obj->m_Mode=VECTOR_MODE_POINT_EXP;
			obj->m_Param.Add(objInput[0]); obj->m_Param.Add(objInput[1]);
			SetName(obj,GetName(objOutputName,0,OBJ_LINE));

			CVGPoint* pt=(CVGPoint*)AddObject(OBJ_POINT);
			pt->m_Mode=GEO_MODE_TRANSLATE;
			pt->m_Param.Add(objInput[0]);
			pt->m_Param.Add(obj);
			SetName(pt,GetName(objOutputName,0,OBJ_POINT));

			returnObj.Add(obj);
		}
	}

	if (strCmd==L"Circle")
	{
		if (objInput.GetSize()==2 && objInput[0]->m_Type==OBJ_POINT && (IsPointType(objInput[1]->m_Type)))
		{
			CVGCircle* circle=(CVGCircle*)AddObject(OBJ_CIRCLE);
			circle->m_Mode=CIRCLE_MODE_POINT_POINT;
			circle->m_Param.Add(objInput[0]);
			circle->m_Param.Add(objInput[1]);

			SetName(circle,GetName(objOutputName,0,OBJ_CIRCLE));
			if (((CVGPoint*)objInput[1])->m_Type==OBJ_POINT_HIDDEN_CHILD)
			{
				((CVGPoint*)objInput[0])->m_objMove.Add((CVGPoint*)objInput[1]);
				((CVGPoint*)objInput[1])->m_Param.Add(circle);
			}
			returnObj.Add(circle);
		}
		if (objInput.GetSize()==2 && objInput[0]->m_Type==OBJ_POINT && objInput[1]->m_Type==OBJ_EXPRESSION)
		{
			CVGCircle* circle=(CVGCircle*)AddObject(OBJ_CIRCLE);
			circle->m_Type=OBJ_CIRCLE;
			circle->m_Mode=CIRCLE_MODE_POINT_RADIUS;
			circle->m_Param.Add(objInput[0]);
			circle->m_Param.Add(objInput[1]);
			SetName(circle,GetName(objOutputName,0,OBJ_CIRCLE));
			returnObj.Add(circle);
		}
	}

	if (strCmd==L"Conic")
	{
		if (objInput.GetSize()==6 && objInput[0]->m_Type==OBJ_EXPRESSION && objInput[1]->m_Type==OBJ_EXPRESSION && objInput[2]->m_Type==OBJ_EXPRESSION && objInput[3]->m_Type==OBJ_EXPRESSION && objInput[4]->m_Type==OBJ_EXPRESSION && objInput[5]->m_Type==OBJ_EXPRESSION)
		{
			CVGConic* el=(CVGConic*)AddObject(OBJ_CONIC);
			el->m_Mode=ELLIPSE_MODE_A_B;
			el->m_Param.Add(objInput[0]);
			el->m_Param.Add(objInput[1]);
			el->m_Param.Add(objInput[2]);
			el->m_Param.Add(objInput[3]);
			el->m_Param.Add(objInput[4]);
			el->m_Param.Add(objInput[5]);

			SetName(el,GetName(objOutputName,0,OBJ_CONIC));
			returnObj.Add(el);
		}
	}

	if (strCmd==L"CardinalSpline")
	{
		if (IsPointsArray(objInput))
		{
			CVGCardinalSpline* spline=(CVGCardinalSpline*)AddObject(OBJ_CARDINAL_SPLINE);
			for (int i=0;i<objInput.GetSize();i++)
			{
				spline->m_Param.Add(objInput[i]);
			}
			returnObj.Add(spline);
		}
	}

	if (strCmd==L"BezierSpline")
	{
		if (objInput.GetSize()==4 && objInput[0]->m_Type==OBJ_POINT
			 && objInput[1]->m_Type==OBJ_POINT && objInput[2]->m_Type==OBJ_POINT && objInput[3]->m_Type==OBJ_POINT)
		{
			CVGBezierSpline* spline=(CVGBezierSpline*)AddObject(OBJ_BEZIER_SPLINE);
			for (int i=0;i<objInput.GetSize();i++)
			{
				spline->m_Param.Add(objInput[i]);
			}
			returnObj.Add(spline);
		}
	}

	if (strCmd==L"Polygon")
	{
		if (IsPointsArray(objInput))
		{
			CVGPolygon* plg=(CVGPolygon*)AddObject(OBJ_POLYGON);
			for (int i=0;i<objInput.GetSize();i++)
				plg->m_Param.Add(objInput[i]);
			SetName(plg,GetName(objOutputName,0,OBJ_POLYGON));
			returnObj.Add(plg);
		}
	}

	if (strCmd=="RegularPolygon")
	{
		if (objInput.GetSize()==3 && objInput[0]->m_Type==OBJ_POINT && objInput[1]->m_Type==OBJ_POINT && objInput[2]->m_Type==OBJ_EXPRESSION)
		{
			if (((CVGExpression*)objInput[2])->GetExpValueType()==VALUE_DOUBLE)
			{
				CVGPoint* pt1=(CVGPoint*)objInput[0];
				CVGPoint* pt2=(CVGPoint*)objInput[1];

				int num=((CVGExpression*)objInput[2])->GetDoubleValue();
				if (!m_bTemp) cmdCount++;
				CVGExpression* angle=(CVGExpression*)AddObject(OBJ_EXPRESSION);
				//angle->m_Param.Add(pt1);
				//angle->m_Param.Add(pt2);
				angle->m_Exp.m_Value=new CValueDouble(Math::ConvertFromRadianToAngleUnit((num-2)*pi/num));
				angle->m_bTemp=true;
				angle->m_cmdIndex=cmdCount;
				CString sDefinition=L"Polygon("+pt1->m_Name+L", "+pt2->m_Name+L", "+Math::IntToString(num)+L")";

				int start=m_Array.GetSize();
				CVGSegment* seg=(CVGSegment*)AddObject(OBJ_SEGMENT);
				seg->SetThroughTwoPoints(pt1,pt2);
				//AddObjTreeItem(seg);
				SetName(seg,GetNextAvailableName(OBJ_SEGMENT));
				//seg->m_Definition=sDefinition;
				for (int i=2;i<num;i++)
				{
					CVGPoint* pt=(CVGPoint*)AddObject(OBJ_POINT);
					CVGSegment* s=(CVGSegment*)AddObject(OBJ_SEGMENT);

					if (i==2)
					{
						pt->SetRotate(pt1,pt2,angle);
						s->SetThroughTwoPoints(pt2,pt);
					}
					else if (i==3)
					{
						pt->SetRotate(pt2,(CVGPoint*)m_Array.GetAt(m_Array.GetSize()-4),angle);
						s->SetThroughTwoPoints(pt,(CVGPoint*)m_Array.GetAt(m_Array.GetSize()-4));
					}
					else
					{
						pt->SetRotate((CVGPoint*)m_Array.GetAt(m_Array.GetSize()-6),(CVGPoint*)m_Array.GetAt(m_Array.GetSize()-4),angle);
						s->SetThroughTwoPoints(pt,(CVGPoint*)m_Array.GetAt(m_Array.GetSize()-4));
					}
					
					//pt->m_Definition=sDefinition;
					//pt->m_cmdIndex=cmdCount;
					//s->m_cmdIndex=cmdCount;
					//s->m_Definition=sDefinition;

					//AddObjTreeItem(pt);
					//AddObjTreeItem(s);
					SetName(pt,GetNextAvailableName(OBJ_POINT));
					SetName(s,GetNextAvailableName(OBJ_SEGMENT));
				}
				seg=(CVGSegment*)AddObject(OBJ_SEGMENT);
				seg->SetThroughTwoPoints(pt1,(CVGPoint*)m_Array.GetAt(m_Array.GetSize()-3));
				//AddObjTreeItem(seg);
				SetName(seg,GetNextAvailableName(OBJ_SEGMENT));
				//seg->m_Definition=sDefinition;

				// Các điểm đc tạo mới có liên kết
				// Khi xóa sẽ xóa tất cả
// 				for (int i=start;i<m_Array.GetSize();i++)
// 				{
// 					for (int j=start;j<m_Array.GetSize();j++)
// 						(m_Array[i])->m_Param.Add(m_Array[j]);
// 					angle->m_Param.Add(m_Array[i]);
// 				}
// 				Calc();
// 				for (int i=0;i<num;i++)
			}
		}
	}

// 	if (strCmd==L"Number")
// 	{
// 		if (objInput.GetSize()==1 && objInput[0]->m_Type==OBJ_NUMBER && objInput[0]->m_bTemp)
// 		{
// 			CVGExpression* n=(CVGExpression*)AddObject(OBJ_NUMBER);
// 
// 			SetName(n,GetName(objOutputName,0,OBJ_POLYGON));
// 			n->SetValue(((CVGExpression*)objInput[0])->m_Value);
// 
// 			n->m_Top=0;
// 			n->m_Left=0;
// 
// 			returnObj.Add(n);
// 		}
// 	}

	if (strCmd==L"Angle")
	{
		if (objInput.GetSize()==3 && objInput[0]->m_Type==OBJ_POINT && objInput[1]->m_Type==OBJ_POINT && objInput[2]->m_Type==OBJ_POINT)
		{
			CVGAngle* angle=(CVGAngle*)AddObject(OBJ_ANGLE);
			angle->m_Mode=ANGLE_MODE_POINT_POINT_POINT;
			angle->m_Param.Add(objInput[0]);
			angle->m_Param.Add(objInput[1]);
			angle->m_Param.Add(objInput[2]);
			SetName(angle,GetNextAvailableName(OBJ_ANGLE));
			returnObj.Add(angle);
		}
		else if (objInput.GetSize()==2 && objInput[0]->m_Type==OBJ_LINE && objInput[1]->m_Type==OBJ_LINE)
		{
			CVGAngle* angle=(CVGAngle*)AddObject(OBJ_ANGLE);
			angle->m_Mode=ANGLE_MODE_LINE_LINE;
			angle->m_Param.Add(objInput[0]);
			angle->m_Param.Add(objInput[1]);
			SetName(angle,GetNextAvailableName(OBJ_ANGLE));
			returnObj.Add(angle);
		}
	}

	if (strCmd==L"MidPoint")
	{
		if (objInput.GetSize()==2 && objInput[0]->m_Type==OBJ_POINT && objInput[1]->m_Type==OBJ_POINT)
		{
			CVGPoint* pt=(CVGPoint*)AddObject(OBJ_POINT);
			pt->m_Mode=POINT_MODE_MIDPOINT;
			pt->m_Param.Add(objInput[0]);
			pt->m_Param.Add(objInput[1]);
			SetName(pt,GetName(objOutputName,0,OBJ_POINT));
			returnObj.Add(pt);
		}
	}

	if (strCmd==L"Centroid")
	{
		int i=0;
		for (i=0;i<objInput.GetSize();i++)
			if (objInput[i]->m_Type!=OBJ_POINT) break;
		if (i==objInput.GetSize())
		{
			CVGPoint* pt=(CVGPoint*)AddObject(OBJ_POINT);
			for (i=0;i<objInput.GetSize();i++)
			{
				pt->m_Param.Add((CVGPoint*)objInput[i]);
			}
			pt->m_Mode=POINT_MODE_CENTROID;
			SetName(pt,GetName(objOutputName,0,OBJ_POINT));
			returnObj.Add(pt);
		}
	}

	if (strCmd==L"Center" && objInput[0]->m_Type==OBJ_CIRCLE)
	{
		CVGPoint* pt=(CVGPoint*)AddObject(OBJ_POINT);
		pt->m_Mode=POINT_MODE_CENTER;
		pt->m_Param.Add(objInput[0]);
		SetName(pt,GetName(objOutputName,0,OBJ_POINT));
		returnObj.Add(pt);
	}

	if (strCmd==L"PerpendicularLine")
	{
		if (objInput.GetSize()==2 && (IsPointType(objInput[0]->m_Type)) && (IsLineType(objInput[1]->m_Type)))
		{
			CVGLine* line=(CVGLine*)AddObject(OBJ_LINE);
			line->SetPerpendicular((CVGPoint*)objInput[0],(CVGLine*)objInput[1]);
			SetName(line,GetName(objOutputName,0,OBJ_LINE));
			returnObj.Add(line);
		}
		if (objInput.GetSize()==2 && (IsPointType(objInput[1]->m_Type)) && (IsLineType(objInput[0]->m_Type)))
		{
			CVGLine* line=(CVGLine*)AddObject(OBJ_LINE);
			line->SetPerpendicular((CVGPoint*)objInput[1],(CVGLine*)objInput[0]);
			SetName(line,GetName(objOutputName,0,OBJ_LINE));
			returnObj.Add(line);
		}
	}

	if (strCmd==L"ParallelLine") 
	{
		if (objInput.GetSize()==2 && (IsPointType(objInput[1]->m_Type)) && (IsLineType(objInput[0]->m_Type)))
		{
			CVGLine* line=(CVGLine*)AddObject(OBJ_LINE);
			line->SetParallel((CVGPoint*)objInput[1], (CVGLine*)objInput[0]);
			SetName(line,GetName(objOutputName,0,OBJ_LINE));
			returnObj.Add(line);
		}
		if (objInput.GetSize()==2 && (IsPointType(objInput[0]->m_Type)) && (IsLineType(objInput[1]->m_Type)))
		{
			CVGLine* line=(CVGLine*)AddObject(OBJ_LINE);
			line->SetParallel((CVGPoint*)objInput[0], (CVGLine*)objInput[1]);
			SetName(line,GetName(objOutputName,0,OBJ_LINE));
			returnObj.Add(line);
		}
	}

	if (strCmd==L"DividedPoint") 
	{
		if (objInput.GetSize()==3 && objInput[0]->m_Type==OBJ_POINT && objInput[1]->m_Type==OBJ_POINT && (objInput[2]->m_Type==OBJ_EXPRESSION))
		{
			CVGPoint* pt=(CVGPoint*)AddObject(OBJ_POINT);
			pt->m_Param.Add(objInput[0]);
			pt->m_Param.Add(objInput[1]);
			pt->m_Param.Add(objInput[2]);
			pt->m_Mode=POINT_MODE_DIVIDED;
			SetName(pt,GetName(objOutputName,0,OBJ_POINT));
			returnObj.Add(pt);
		}
	}

	if (strCmd==L"Intersection")
	{
		if (objInput.GetSize()==2 && (IsLineType(objInput[0]->m_Type)) && 
			(IsLineType(objInput[1]->m_Type)))
		{
			CVGPoint* pt=(CVGPoint*)AddObject(OBJ_POINT);
			pt->m_Mode=POINT_MODE_INTERSECTION_LINE_LINE;
			pt->m_Param.Add(objInput[0]);
			pt->m_Param.Add(objInput[1]);
			SetName(pt,GetName(objOutputName,0,OBJ_POINT));
			returnObj.Add(pt);
		}
		else if (objInput.GetSize()==2 && (IsLineType(objInput[0]->m_Type))
			&& objInput[1]->m_Type==OBJ_CIRCLE)
		{
			CVGLineBase* l=(CVGLineBase*)objInput[0];
			CVGCircle* cir=(CVGCircle*)objInput[1];

			CVGPoint* pt1=(CVGPoint*)AddObject(OBJ_POINT);
			CVGPoint* pt2=(CVGPoint*)AddObject(OBJ_POINT);
			pt1->SetIntersectionLineCircle((CVGLine*)objInput[0],
				(CVGCircle*)objInput[1],1);
			pt2->SetIntersectionLineCircle((CVGLine*)objInput[0],
				(CVGCircle*)objInput[1],2);
			SetName(pt1,GetName(objOutputName,0,OBJ_POINT));
			SetName(pt2,GetName(objOutputName,1,OBJ_POINT));
			returnObj.Add(pt1);
			returnObj.Add(pt2);
		}
		else if (objInput.GetSize()==2 && (IsLineType(objInput[0]->m_Type))
			&& objInput[1]->m_Type==OBJ_CONIC)
		{
			CVGLineBase* l=(CVGLineBase*)objInput[0];
			CVGConic* el=(CVGConic*)objInput[1];

			CVGPoint* pt1=(CVGPoint*)AddObject(OBJ_POINT);
			CVGPoint* pt2=(CVGPoint*)AddObject(OBJ_POINT);
			pt1->SetIntersectionLineEllipse((CVGLine*)objInput[0],
				(CVGConic*)objInput[1],1);
			pt2->SetIntersectionLineEllipse((CVGLine*)objInput[0],
				(CVGConic*)objInput[1],2);
			SetName(pt1,GetName(objOutputName,0,OBJ_POINT));
			SetName(pt2,GetName(objOutputName,1,OBJ_POINT));
			returnObj.Add(pt1);
			returnObj.Add(pt2);
		}
		else if (objInput.GetSize()==2 && objInput[0]->m_Type==OBJ_CIRCLE && 
			objInput[1]->m_Type==OBJ_CIRCLE)
		{
			CVGPoint* point1=(CVGPoint*)AddObject(OBJ_POINT);
			CVGPoint* point2=(CVGPoint*)AddObject(OBJ_POINT);
			point1->SetIntersectionCircleCircle((CVGCircle*)objInput[0],
				(CVGCircle*)objInput[1],1);
			point2->SetIntersectionCircleCircle((CVGCircle*)objInput[0],
				(CVGCircle*)objInput[1],2);
			SetName(point1,GetName(objOutputName,0,OBJ_POINT));
			SetName(point2,GetName(objOutputName,1,OBJ_POINT));
			returnObj.Add(point1);
			returnObj.Add(point2);
		}
	}

	if (strCmd==L"AngleBisector")
	{
		if (objInput.GetSize()==3 && objInput[0]->m_Type==OBJ_POINT && objInput[1]->m_Type==OBJ_POINT && objInput[2]->m_Type==OBJ_POINT)
		{
			CVGLine* line=(CVGLine*)AddObject(OBJ_LINE);
			line->SetAngleBisector((CVGPoint*)objInput[0],(CVGPoint*)objInput[1],(CVGPoint*)objInput[2]);
			SetName(line,GetName(objOutputName,0,OBJ_LINE));
			returnObj.Add(line);
		}
		else if (objInput.GetSize()==3 && (IsLineType(objInput[0]->m_Type)) && (IsLineType(objInput[1]->m_Type)) && objInput[2]->m_Type==OBJ_POINT)
		{
			CVGLine* line=(CVGLine*)AddObject(OBJ_LINE);
			line->SetAngleBisector((CVGLineBase*)objInput[0],(CVGLineBase*)objInput[1],(CVGPoint*)objInput[2]);
			SetName(line,GetName(objOutputName,0,OBJ_LINE));
			returnObj.Add(line);
		}
	}

	if (strCmd==L"ExteriorAngleBisector")
	{
		if (objInput.GetSize()==3 && objInput[0]->m_Type==OBJ_POINT && objInput[1]->m_Type==OBJ_POINT && objInput[2]->m_Type==OBJ_POINT)
		{
			CVGLine* line=(CVGLine*)AddObject(OBJ_LINE);
			line->SetExteriorAngleBisector((CVGPoint*)objInput[0],(CVGPoint*)objInput[1],(CVGPoint*)objInput[2]);
			SetName(line,GetName(objOutputName,0,OBJ_LINE));
			returnObj.Add(line);
		}
	}

	if (strCmd==L"Tangent")
	{
		if (objInput.GetSize()==2 && objInput[0]->m_Type==OBJ_POINT && objInput[1]->m_Type==OBJ_CONIC)
		{
			CVGLine* l1=(CVGLine*)AddObject(OBJ_LINE);
			l1->m_Mode=LINE_MODE_TANGENT_CONIC;
			l1->m_Param.Add(objInput[0]); l1->m_Param.Add(objInput[1]); l1->tmp=1;
			SetName(l1,GetName(objOutputName,0,OBJ_LINE));
			returnObj.Add(l1);

			CVGLine* l2=(CVGLine*)AddObject(OBJ_LINE);
			l2->m_Mode=LINE_MODE_TANGENT_CONIC;
			l2->m_Param.Add(objInput[0]); l2->m_Param.Add(objInput[1]); l2->tmp=2;
			SetName(l2,GetName(objOutputName,0,OBJ_LINE));
			returnObj.Add(l2);
		}
	}

	if (strCmd==L"Translate") 
	{
		if (objInput.GetSize()==2 && (objInput[0]->m_Type==OBJ_POINT || objInput[0]->m_Type==OBJ_CIRCLE) && objInput[1]->m_Type==OBJ_VECTOR)
		{
			CVGVector* v=(CVGVector*)objInput[1];
			CVGGeoObject* obj=(objInput[0]->m_Type==OBJ_POINT) ? (CVGPoint*)AddObject(OBJ_POINT) : (CVGGeoObject*)(CVGCircle*)AddObject(OBJ_CIRCLE);
			obj->SetTranslate((CVGGeoObject*)objInput[0],v);
			SetName(obj,GetName(objOutputName,0,obj->m_Type));
			returnObj.Add(obj);
		}
		if (objInput.GetSize()==2 && objInput[0]->m_Type==OBJ_POLYGON)
		{
			CVGVector* v=(CVGVector*)objInput[1];
			CVGPolygon* p=(CVGPolygon*)objInput[0];
			CVGPolygon* plg=(CVGPolygon*)AddObject(OBJ_POLYGON);
			returnObj.Add(plg);
			for (int i=0;i<p->m_Param.GetSize();i++)
			{
				CVGPoint* pt=(CVGPoint*)AddObject(OBJ_POINT);
				SetName(pt,GetNextAvailableName(OBJ_POINT));
				returnObj.Add(pt);
				pt->SetTranslate((CVGGeoObject*)p->m_Param[i],v);
				plg->m_Param.Add(pt);
			}
			plg->m_Color=p->m_Color;
			SetName(plg,GetName(objOutputName,0,OBJ_POLYGON));
		}
		else if (objInput.GetSize()==2 && (IsLineType(objInput[0]->m_Type)) && objInput[1]->m_Type==OBJ_VECTOR)
		{
			CVGLineBase* l;
			switch (objInput[0]->m_Type)
			{
			case OBJ_LINE:l=(CVGLine*)AddObject(OBJ_LINE);break;
			case OBJ_RAY:l=(CVGRay*)AddObject(OBJ_RAY);break;
			case OBJ_SEGMENT:l=(CVGSegment*)AddObject(OBJ_SEGMENT);break;
			case OBJ_VECTOR:l=(CVGVector*)AddObject(OBJ_VECTOR);break;
			}
			if (objInput[0]->m_Type==OBJ_SEGMENT || objInput[0]->m_Type==OBJ_VECTOR || objInput[0]->m_Type==OBJ_RAY)
			{
				CVGPoint* pt1=(CVGPoint*)AddObject(OBJ_POINT); SetName(pt1,GetNextAvailableName(OBJ_POINT));
				CVGPoint* pt2=(CVGPoint*)AddObject(OBJ_POINT); SetName(pt2,GetNextAvailableName(OBJ_POINT));
				pt1->SetTranslate((CVGGeoObject*)(objInput[0])->m_Param[0],(CVGVector*)objInput[1]);
				pt2->SetTranslate((CVGGeoObject*)(objInput[0])->m_Param[1],(CVGVector*)objInput[1]);
				if (((CVGGeoObject*)(objInput[0])->m_Param[1])->m_Type==OBJ_POINT_HIDDEN_CHILD) pt2->m_bTemp=true;
				l->SetThroughTwoPoints(pt1,pt2);
				SetName(l,GetName(objOutputName,0,l->m_Type));
				returnObj.Add(l);
				returnObj.Add(pt1);
				returnObj.Add(pt2);
			}
			else
			{
				l->SetTranslate((CVGGeoObject*)objInput[0],(CVGVector*)objInput[1]);
				SetName(l,GetName(objOutputName,0,l->m_Type));
				returnObj.Add(l);
			}
		}
	}

	if (strCmd==L"Reflect") 
	{
		//Đối xứng qua điểm
		if (objInput.GetSize()==2 && (objInput[0]->m_Type==OBJ_POINT || objInput[0]->m_Type==OBJ_CIRCLE) && objInput[1]->m_Type==OBJ_POINT)
		{
			CVGGeoObject* obj=(objInput[0]->m_Type==OBJ_POINT) ? (CVGPoint*)AddObject(OBJ_POINT) : (CVGGeoObject*)(CVGCircle*)AddObject(OBJ_CIRCLE);
			obj->SetReflectAboutPoint((CVGGeoObject*)objInput[0],(CVGPoint*)objInput[1]);
			SetName(obj,GetName(objOutputName,0,obj->m_Type));
			returnObj.Add(obj);
		}

		else if (objInput.GetSize()==2 && (IsLineType(objInput[0]->m_Type)) && objInput[1]->m_Type==OBJ_POINT)
		{
			CVGLineBase* l;
			switch (objInput[0]->m_Type)
			{
			case OBJ_LINE:l=(CVGLine*)AddObject(OBJ_LINE);break;
			case OBJ_RAY:l=(CVGRay*)AddObject(OBJ_RAY);break;
			case OBJ_SEGMENT:l=(CVGSegment*)AddObject(OBJ_SEGMENT);break;
			case OBJ_VECTOR:l=(CVGVector*)AddObject(OBJ_VECTOR);break;
			}
			if (objInput[0]->m_Type==OBJ_SEGMENT || objInput[0]->m_Type==OBJ_VECTOR || objInput[0]->m_Type==OBJ_RAY)
			{
				CVGPoint* pt1=(CVGPoint*)AddObject(OBJ_POINT);
				CVGPoint* pt2=(CVGPoint*)AddObject(OBJ_POINT);
				pt1->SetReflectAboutPoint((CVGGeoObject*)(objInput[0])->m_Param[0],(CVGPoint*)objInput[1]);
				pt2->SetReflectAboutPoint((CVGGeoObject*)(objInput[0])->m_Param[1],(CVGPoint*)objInput[1]);
				if (((CVGGeoObject*)(objInput[0])->m_Param[1])->m_Type==OBJ_POINT_HIDDEN_CHILD) pt2->m_bTemp=TRUE;
				l->SetThroughTwoPoints(pt1,pt2);
				SetName(l,GetName(objOutputName,0,l->m_Type));
			}
			else
			{
				l->SetReflectAboutPoint((CVGGeoObject*)objInput[0],(CVGPoint*)objInput[1]);
				SetName(l,GetName(objOutputName,0,l->m_Type));
			}
			returnObj.Add(l);
		}
		
		//////////////////////////////////////////////////////////////////////////
		
		if (objInput.GetSize()==2 && (objInput[0]->m_Type==OBJ_POINT || objInput[0]->m_Type==OBJ_CIRCLE) && (IsLineType(objInput[1]->m_Type)))
		{
			CVGGeoObject* obj=(objInput[0]->m_Type==OBJ_POINT) ? (CVGPoint*)AddObject(OBJ_POINT) : (CVGGeoObject*)(CVGCircle*)AddObject(OBJ_CIRCLE);
			obj->SetReflectAboutLine((CVGGeoObject*)objInput[0],(CVGLineBase*)objInput[1]);
			SetName(obj,GetName(objOutputName,0,obj->m_Type));
			returnObj.Add(obj);
		}

		else if (objInput.GetSize()==2 && (IsLineType(objInput[0]->m_Type)) && (IsLineType(objInput[1]->m_Type)))
		{
			CVGLineBase* l;
			switch (objInput[0]->m_Type)
			{
			case OBJ_LINE:l=(CVGLine*)AddObject(OBJ_LINE);break;
			case OBJ_RAY:l=(CVGRay*)AddObject(OBJ_RAY);break;
			case OBJ_SEGMENT:l=(CVGSegment*)AddObject(OBJ_SEGMENT);break;
			case OBJ_VECTOR:l=(CVGVector*)AddObject(OBJ_VECTOR);break;
			}
			if (objInput[0]->m_Type==OBJ_SEGMENT || objInput[0]->m_Type==OBJ_VECTOR || objInput[0]->m_Type==OBJ_RAY)
			{
				CVGPoint* pt1=(CVGPoint*)AddObject(OBJ_POINT);
				CVGPoint* pt2=(CVGPoint*)AddObject(OBJ_POINT);
				pt1->SetReflectAboutLine((CVGGeoObject*)(objInput[0])->m_Param[0],(CVGLine*)objInput[1]);
				pt2->SetReflectAboutLine((CVGGeoObject*)(objInput[0])->m_Param[1],(CVGLine*)objInput[1]);
				if (((CVGGeoObject*)(objInput[0])->m_Param[1])->m_Type==OBJ_POINT_HIDDEN_CHILD) pt2->m_bTemp=TRUE;
				l->SetThroughTwoPoints(pt1,pt2);
				SetName(l,GetName(objOutputName,0,l->m_Type));
			}
			else
			{
				l->m_Mode=GEO_MODE_REFLECT_LINE;
				l->m_Param.Add(objInput[0]);
				l->m_Param.Add(objInput[1]);
				SetName(l,GetName(objOutputName,0,l->m_Type));
			}
			returnObj.Add(l);
		}
	}

	if (strCmd==L"Dilate") 
	{
		if (objInput.GetSize()==3 && (objInput[0]->m_Type==OBJ_POINT || objInput[0]->m_Type==OBJ_CIRCLE) && objInput[1]->m_Type==OBJ_POINT && (objInput[2]->m_Type==OBJ_EXPRESSION))
		{
			CVGGeoObject* obj=(objInput[0]->m_Type==OBJ_POINT) ? (CVGPoint*)AddObject(OBJ_POINT) : (CVGGeoObject*)(CVGCircle*)AddObject(OBJ_CIRCLE);
			obj->SetDilate((CVGGeoObject*)objInput[0],(CVGPoint*)objInput[1],(CVGExpression*)objInput[2]);
			SetName(obj,GetName(objOutputName,0,obj->m_Type));
			returnObj.Add(obj);
		}
		else if (objInput.GetSize()==3 && (IsLineType(objInput[0]->m_Type)) && (objInput[1]->m_Type==OBJ_POINT) && (objInput[2]->m_Type==OBJ_EXPRESSION))
		{
			CVGLineBase* l;
			switch (objInput[0]->m_Type)
			{
			case OBJ_LINE:l=(CVGLine*)AddObject(OBJ_LINE);break;
			case OBJ_RAY:l=(CVGRay*)AddObject(OBJ_RAY);break;
			case OBJ_SEGMENT:l=(CVGSegment*)AddObject(OBJ_SEGMENT);break;
			case OBJ_VECTOR:l=(CVGVector*)AddObject(OBJ_VECTOR);break;
			}
			if (objInput[0]->m_Type==OBJ_SEGMENT || objInput[0]->m_Type==OBJ_VECTOR || objInput[0]->m_Type==OBJ_RAY)
			{
				CVGPoint* pt1=(CVGPoint*)AddObject(OBJ_POINT);
				CVGPoint* pt2=(CVGPoint*)AddObject(OBJ_POINT);
				pt1->SetDilate((CVGGeoObject*)(objInput[0])->m_Param[0],(CVGPoint*)objInput[1],(CVGExpression*)objInput[2]);
				pt2->SetDilate((CVGGeoObject*)(objInput[0])->m_Param[1],(CVGPoint*)objInput[1],(CVGExpression*)objInput[2]);
				if (((CVGGeoObject*)(objInput[0])->m_Param[1])->m_Type==OBJ_POINT_HIDDEN_CHILD) pt2->m_bTemp=TRUE;
				l->SetThroughTwoPoints(pt1,pt2);
				SetName(l,GetName(objOutputName,0,l->m_Type));
			}
			else
			{
				l->SetDilate((CVGGeoObject*)objInput[0],(CVGPoint*)objInput[1],(CVGExpression*)objInput[2]);
				SetName(l,GetName(objOutputName,0,l->m_Type));
			}
			returnObj.Add(l);
		}
	}

	if (strCmd==L"Dilate") 
	{
		if (objInput.GetSize()==3 && (objInput[0]->m_Type==OBJ_POINT || objInput[0]->m_Type==OBJ_CIRCLE) && objInput[1]->m_Type==OBJ_POINT && (objInput[2]->m_Type==OBJ_EXPRESSION))
		{
			CVGGeoObject* obj=(objInput[0]->m_Type==OBJ_POINT) ? (CVGPoint*)AddObject(OBJ_POINT) : (CVGGeoObject*)(CVGCircle*)AddObject(OBJ_CIRCLE);
			obj->SetDilate((CVGGeoObject*)objInput[0],(CVGPoint*)objInput[1],(CVGExpression*)objInput[2]);
			SetName(obj,GetName(objOutputName,0,obj->m_Type));
			returnObj.Add(obj);
		}
		else if (objInput.GetSize()==3 && (IsLineType(objInput[0]->m_Type)) && (objInput[1]->m_Type==OBJ_POINT) && (objInput[2]->m_Type==OBJ_EXPRESSION))
		{
			CVGLineBase* l;
			switch (objInput[0]->m_Type)
			{
			case OBJ_LINE:l=(CVGLine*)AddObject(OBJ_LINE);break;
			case OBJ_RAY:l=(CVGRay*)AddObject(OBJ_RAY);break;
			case OBJ_SEGMENT:l=(CVGSegment*)AddObject(OBJ_SEGMENT);break;
			case OBJ_VECTOR:l=(CVGVector*)AddObject(OBJ_VECTOR);break;
			}
			if (objInput[0]->m_Type==OBJ_SEGMENT || objInput[0]->m_Type==OBJ_VECTOR || objInput[0]->m_Type==OBJ_RAY)
			{
				CVGPoint* pt1=(CVGPoint*)AddObject(OBJ_POINT);
				CVGPoint* pt2=(CVGPoint*)AddObject(OBJ_POINT);
				pt1->SetDilate((CVGGeoObject*)(objInput[0])->m_Param[0],(CVGPoint*)objInput[1],(CVGExpression*)objInput[2]);
				pt2->SetDilate((CVGGeoObject*)(objInput[0])->m_Param[1],(CVGPoint*)objInput[1],(CVGExpression*)objInput[2]);
				if (((CVGGeoObject*)(objInput[0])->m_Param[1])->m_Type==OBJ_POINT_HIDDEN_CHILD) pt2->m_bTemp=TRUE;
				l->SetThroughTwoPoints(pt1,pt2);
				SetName(l,GetName(objOutputName,0,l->m_Type));
			}
			else
			{
				l->SetDilate((CVGGeoObject*)objInput[0],(CVGPoint*)objInput[1],(CVGExpression*)objInput[2]);
				SetName(l,GetName(objOutputName,0,l->m_Type));
			}
			returnObj.Add(l);
		}
	}

	if (strCmd==L"Rotate") 
	{
		if (objInput.GetSize()==3 && (objInput[0]->m_Type==OBJ_POINT || objInput[0]->m_Type==OBJ_CIRCLE) && objInput[1]->m_Type==OBJ_POINT && (objInput[2]->m_Type==OBJ_EXPRESSION))
		{
			CVGGeoObject* obj=(objInput[0]->m_Type==OBJ_POINT) ? (CVGPoint*)AddObject(OBJ_POINT) : (CVGGeoObject*)(CVGCircle*)AddObject(OBJ_CIRCLE);
			obj->SetRotate((CVGGeoObject*)objInput[0],(CVGPoint*)objInput[1],(CVGExpression*)objInput[2]);
			SetName(obj,GetName(objOutputName,0,obj->m_Type));
			returnObj.Add(obj);
		}
		else if (objInput.GetSize()==3 && (IsLineType(objInput[0]->m_Type)) && (objInput[1]->m_Type==OBJ_POINT) && (objInput[2]->m_Type==OBJ_EXPRESSION))
		{
			CVGLineBase* l;
			switch (objInput[0]->m_Type)
			{
			case OBJ_LINE:l=(CVGLine*)AddObject(OBJ_LINE);break;
			case OBJ_RAY:l=(CVGRay*)AddObject(OBJ_RAY);break;
			case OBJ_SEGMENT:l=(CVGSegment*)AddObject(OBJ_SEGMENT);break;
			case OBJ_VECTOR:l=(CVGVector*)AddObject(OBJ_VECTOR);break;
			}
			if (objInput[0]->m_Type==OBJ_SEGMENT || objInput[0]->m_Type==OBJ_VECTOR || objInput[0]->m_Type==OBJ_RAY)
			{
				CVGPoint* pt1=(CVGPoint*)AddObject(OBJ_POINT);
				CVGPoint* pt2=(CVGPoint*)AddObject(OBJ_POINT);
				pt1->SetRotate((CVGGeoObject*)(objInput[0])->m_Param[0],(CVGPoint*)objInput[1],(CVGExpression*)objInput[2]);
				pt2->SetRotate((CVGGeoObject*)(objInput[0])->m_Param[1],(CVGPoint*)objInput[1],(CVGExpression*)objInput[2]);
				if (((CVGGeoObject*)(objInput[0])->m_Param[1])->m_Type==OBJ_POINT_HIDDEN_CHILD) pt2->m_bTemp=TRUE;
				l->SetThroughTwoPoints(pt1,pt2);
				SetName(l,GetName(objOutputName,0,l->m_Type));
			}
			else
			{
				l->SetRotate((CVGGeoObject*)objInput[0],(CVGPoint*)objInput[1],(CVGExpression*)objInput[2]);
				SetName(l,GetName(objOutputName,0,l->m_Type));
			}
			returnObj.Add(l);
		}
	}

	if (strCmd==L"Inverse") 
	{
		if (objInput.GetSize()==3 && (objInput[0]->m_Type==OBJ_POINT) && (objInput[1]->m_Type==OBJ_POINT) && (objInput[2]->m_Type==OBJ_EXPRESSION))
		{
			CVGGeoObject* obj=(objInput[0]->m_Type==OBJ_POINT) ? (CVGPoint*)AddObject(OBJ_POINT) : (CVGGeoObject*)(CVGCircle*)AddObject(OBJ_CIRCLE);
			obj->SetInverse((CVGGeoObject*)objInput[0],(CVGPoint*)objInput[1],(CVGExpression*)objInput[2]);
			SetName(obj,GetName(objOutputName,0,obj->m_Type));
			returnObj.Add(obj);
		}
	}

	//////////////////////////////////////////////////////////////////////////

	if (strCmd==L"MessageBox_Warning" && objInput.GetSize()==1 && objInput[0]->m_Type==OBJ_EXPRESSION)
	{
		AfxMessageBox(((CVGExpression*)objInput[0])->m_Exp.GetStringValue(),MB_OK | MB_ICONWARNING);
		bDone=TRUE;
	}
	if (strCmd==L"MessageBox_Info" && objInput.GetSize()==1 && objInput[0]->m_Type==OBJ_EXPRESSION)
	{
		AfxMessageBox(((CVGExpression*)objInput[0])->m_Exp.GetStringValue(),MB_OK | MB_ICONINFORMATION);
		bDone=TRUE;
	}
	if (strCmd==L"MessageBox_Error" && objInput.GetSize()==1 && objInput[0]->m_Type==OBJ_EXPRESSION)
	{
		AfxMessageBox(((CVGExpression*)objInput[0])->m_Exp.GetStringValue(),MB_OK | MB_ICONERROR);
		bDone=TRUE;
	}

	if (strCmd==L"Delete")
	{
		SelectAll(FALSE);
		for (int i=0;i<objInput.GetSize();i++) 
			objInput[i]->m_Select=TRUE;
		DeleteSelection();
		m_pObjTree->FillList();
		pView->RefreshProperties();
		pView->RefreshConstructionSteps();
		//*strOutputString="deleted";
		bDone=TRUE;
	}

	if (strCmd==L"DeleteAll" && objInput.GetSize()==0)
	{
		SelectAll(TRUE);
		DeleteSelection();
		m_pObjTree->FillList();
		pView->RefreshProperties();
		pView->RefreshConstructionSteps();
		//*strOutputString="deleted";
		bDone=TRUE;
	}

	if (strCmd==L"DeleteSelection" && objInput.GetSize()==0)
	{
		DeleteSelection();
		m_pObjTree->FillList();
		pView->RefreshProperties();
		pView->RefreshConstructionSteps();
		//*strOutputString="deleted";
		bDone=TRUE;
	}

	if (strCmd==L"Animate" && objInput.GetSize()==0)
	{
		pView->OnStartAnimate();
		bDone=TRUE;
	}
	
	if (strCmd==L"StopAnimate" && objInput.GetSize()==0)
	{
		pView->OnPauseAnimate();
		bDone=TRUE;
	}

	if (strCmd==L"EraseTrace" && objInput.GetSize()==0)
	{
		pView->DrawBackground();
		pView->Draw();
		bDone=TRUE;
	}

	if (strCmd==L"ShowAxis" && objInput.GetSize()==0)
	{
		m_graphAxis.m_bOx=TRUE;
		m_graphAxis.m_bOy=TRUE;
		pView->DrawBackground();
		pView->Draw();
		bDone=TRUE;
	}

	if (strCmd==L"ShowGrid" && objInput.GetSize()==0)
	{
		m_graphAxis.m_bShowGrid=TRUE;
		pView->DrawBackground();
		pView->Draw();
		bDone=TRUE;
	}

	if (strCmd==L"HideAxis" && objInput.GetSize()==0)
	{
		m_graphAxis.m_bOx=FALSE;
		m_graphAxis.m_bOy=FALSE;
		pView->DrawBackground();
		pView->Draw();
		bDone=TRUE;
	}

	if (strCmd==L"HideGrid" && objInput.GetSize()==0)
	{
		m_graphAxis.m_bShowGrid=FALSE;
		pView->DrawBackground();
		pView->Draw();
		bDone=TRUE;
	}

	if (strCmd==L"ShowObject")
	{
		for (int i=0;i<objInput.GetSize();i++)
		{
			(objInput[i])->m_bVisible=true;
		}
		bDone=TRUE;
	}

	if (strCmd==L"HideObject")
	{
		for (int i=0;i<objInput.GetSize();i++)
		{
			(objInput[i])->m_bVisible=false;
		}
		bDone=TRUE;
	}

	if (strCmd==L"ShowHideObject")
	{
		for (int i=0;i<objInput.GetSize();i++)
		{
			(objInput[i])->m_bVisible=!(objInput[i])->m_bVisible;
		}
		bDone=TRUE;
	}

	if (strCmd==L"FullScreen" && objInput.GetSize()==0)
	{
		
	}

	if (returnObj.GetSize()==0 && !bDone) //Không phải lệnh chuẩn, tìm lệnh trong thư viện công cụ
	{
		int iCmd=-1; // Vị trí nhóm lệnh trong câu lệnh cùng tên
		CCommandArray::CCmd* cmd=m_cmdArray->GetCommandByString(strCmd);
		if (cmd==NULL)
		{
			strErr=L"Không tìm thấy câu lệnh ("+strCmd+L")";
			goto error;
		}
		for (int j=0;j<cmd->GetSize();j++)
		{
			//CString str=m_cmdArray.m_aCommandName[i];
			CCommandArray::SubCommand* subCmd=(*cmd)[j];
			CArray<int> inputParam;
			CCommandArray::GetParamType(subCmd->m_Input,&inputParam);
			// Kiểm tra xem input của lệnh có trùng với yêu cầu không
			if (inputParam.GetSize()==objInput.GetSize())
			{
				int k=0;
				for (k;k<inputParam.GetSize();k++)
				{
					if (!(CanSelectObject(objInput[k]->m_Type,inputParam[k]))/*inputParam[k]!=objInput[k]->m_Type && !(inputParam[k]==OBJ_POINT && objInput[k]->m_Type==OBJ_POINT_HIDDEN_CHILD)*/)
						break;
				}
				if (k==inputParam.GetSize())
				{
					iCmd=j;
					break;
				}
			}
		}

		if (iCmd==-1) 
		{
			strErr=L"Tham số câu lệnh không đúng ("+strCmd+L")";
			goto error;
		}
		CVGObjContainer objArr(true);
		objArr.Copy(this);
		objArr.m_ScriptObject=m_ScriptObject;
		objArr.m_cmdArray=m_cmdArray;
		objArr.m_AxisInfo.Clone(&m_AxisInfo);
		objArr.cmdCount=m_bTemp ? cmdCount : cmdCount+1;
		int i=0, j=0;

		// Nạp Input
		{
			CString strIn=(*cmd)[iCmd]->m_Input;
			CStringArray aObjInput;
			GetParam(strIn,&aObjInput);
			objArr.m_strReplacedNameArr=new CStringArray();

			for (int i=0;i<objInput.GetSize();i++) 
			{
				CVGObject* obj=objInput[i];
				objArr.m_strReplacedNameArr->Add(
					aObjInput[i].Right(aObjInput[i].GetLength()-aObjInput[i].Find(' ')-1));
				objArr.m_Array.Add(obj);
			}
		}

		// Duyệt qua từng câu lệnh
		while (j!=(*cmd)[iCmd]->m_Command.GetLength()+1) 
		{
			j=(*cmd)[iCmd]->m_Command.Find('\n',j+1);
			if (j==-1) j=(*cmd)[iCmd]->m_Command.GetLength()+1;
			CString strCmd=(*cmd)[iCmd]->m_Command.Mid(i,j-i-1);
			objArr.AddCommand(strCmd,NULL);
			i=j+1;
		}

		// Xử lý Output
		{
			CString strOut=(*cmd)[iCmd]->m_Output;
			CStringArray aObjOutput;
			GetParam(strOut,&aObjOutput);

			for (int i=(int)objInput.GetSize();i<objArr.GetSize();i++)
			{
				CVGObject* obj=objArr[i];
				m_Array.Add(obj);
				int j=0;
				for (j;j<aObjOutput.GetSize();j++)
					if (obj->m_Name==aObjOutput[j]) break;
				if (j==aObjOutput.GetSize())
				{
					obj->m_Name=L"";
					obj->m_bVisible=FALSE;
					obj->m_bTemp=TRUE;
				}
				else
				{
					SetName(obj,GetName(objOutputName,j,obj->m_Type));
					//if (j<objOutputName.GetSize()) SetName(obj,objOutputName[j]);
					//else SetName(obj,GetNextAvailableName(obj->m_Type));
					//for (int k=0;k<objInput.GetSize();k++)
					//	if (objInput[k]->m_Type==OBJ_POINT_HIDDEN_CHILD) obj->m_Param.Add(objInput[k]);
					returnObj.Add(obj);
				}
			}
		}
	}

	if (objOutput!=NULL)
	{
		objOutput->RemoveAll();
		for (int i=0;i<returnObj.GetSize();i++)
			objOutput->Add(returnObj[i]);
	}
	
	if (!m_bTemp && !m_bIsTesting)
	{
		Calc();
		if (strOutputString!=NULL)
		{
			for (int i=0;i<returnObj.GetSize();i++)
			{
				*strOutputString+=L"$"+returnObj[i]->GetValue(TRUE)+L"$\r\n";
			}
		}

// 		CString str;
// 		for (int i=0;i<returnObj.GetSize();i++)
// 		{
// 			str+=GetObjCommandTypeName(returnObj[i]->m_Type)+L" "+returnObj[i]->m_Name+L", ";
// 		}
// 		str.Delete(str.GetLength()-2,2);
// 		str+=L" : "+strCmd+L"(";
// 
// 		for (int i=0;i<objInput.GetSize();i++)
// 		{
// 			if (objInput[i]->m_Type!=OBJ_POINT_HIDDEN_CHILD)
// 			{
// 				str+=L"~"+objInput[i]->m_Name+L"~";
// 				if (i!=objInput.GetSize()-1) str+=L", ";
// 			}
// 			else str.Delete(str.GetLength()-2,2);
// 		}
// 		pView->AddToCommandList(str+L")");
		
		for (int i=0;i<returnObj.GetSize();i++)
		{
			//(returnObj[i])->m_Definition=str+L")";
			m_pObjTree->AddObj(returnObj[i]);
		}

		//////////////////////////////////////////////////////////////////////////

		CArray<int> arr;
		for (int i=start;i<m_Array.GetSize();i++)
			arr.Add(i);
		OnAddObjs(&arr);
	}
	
	{
		if (!m_bTemp) cmdCount++;
		for (int i=start;i<m_Array.GetSize();i++)
			m_Array[i]->m_cmdIndex=cmdCount;
	}

	return;

error:
	if (strOutputString==NULL)
		AfxGetApp()->DoMessageBox(strErr,MB_OK | MB_ICONERROR, -1);
	else
		*strOutputString=L"#"+strErr;
}

void CVGObjContainer::Copy(CVGObjContainer* c)
{
	m_defaultValue=c->m_defaultValue;
}

CValue* CVGObjContainer::GetExpValue(CString strExp)
{
	CGeoExp* exp=new CGeoExp();
	exp->m_ScriptObject=m_ScriptObject;
	if (!exp->SetExpression(strExp,&m_Array,false,m_strReplacedNameArr)) { delete exp; return NULL; }
	exp->Calc();
	CValue* vl=exp->m_Value->Clone();
	delete exp;
	return vl;
}

bool CVGObjContainer::GetExpStringValue(CString strExp, CString& str)
{
	CGeoExp* exp=new CGeoExp();
	exp->m_ScriptObject=m_ScriptObject;
	if (!exp->SetExpression(strExp,&m_Array,false,m_strReplacedNameArr)) { delete exp; return false; }
	CValue* vl=exp->CalcFr();
	if (exp->m_bErr) str=exp->m_strErr;
	else str=L"$"+(vl->GetType()==VALUE_FRACTION_EX ? ((CValueFractionEx*)vl)->ToString(false) : vl->ToString())+L"$";
	delete exp;
	return true;
}

int CVGObjContainer::GetReturnValueType(CString cmd)
{
	CString str=cmd.Left(cmd.Find(L" "));
	if (str=="Point") return OBJ_POINT;
	else if (str=="Line") return OBJ_LINE;
	else if (str=="Circle") return OBJ_CIRCLE;
	else return OBJ_NONE;
}

// double* CVGObjContainer::AddVariable(CString name, double dValue)
// {
// 	//if (m_Variable.GetSize()<m_Variable.GetSize()+1) m_Variable.SetSize(m_Variable.GetSize()+1);
// 	m_Variable.push_back(dValue);
// 	m_VariableName.Add(name);
// 	return &m_Variable[m_Variable.size()-1];
// }



void CVGObjContainer::CutSuperfluousLinePart()
{
	for (int i=0;i<m_Array.GetSize();i++)
	{
		if (m_Array[i]->m_Type==OBJ_LINE && ((CVGLine*)m_Array[i])->m_bCutSuperfluousPart)
		{
			CArray<CVGPoint*> ptArr;
			if ((m_Array[i])->m_Mode==LINE_MODE_THROUGH_TWO_POINTS) 
			{ 
				CVGPoint* pt=(CVGPoint*)(m_Array[i])->m_Param[0];
				ptArr.Add(pt);
				pt=(CVGPoint*)(m_Array[i])->m_Param[1];
				if (((m_Array[i])->m_Param[1])->m_Type!=OBJ_POINT_HIDDEN_CHILD) 
					ptArr.Add(pt); 
			}
			else if ((m_Array[i])->m_Mode==LINE_MODE_PARALLEL ||
				(m_Array[i])->m_Mode==LINE_MODE_PERPENDICULAR) 
			{
				CVGPoint* pt=(CVGPoint*)(m_Array[i])->m_Param[0];
				ptArr.Add(pt);
			}
			else if ((m_Array[i])->m_Mode==LINE_MODE_ANGLE_BISECTOR ||
				(m_Array[i])->m_Mode==LINE_MODE_EXTERIOR_ANGLE_BISECTOR) 
			{
				CVGPoint* pt=(CVGPoint*)(m_Array[i])->m_Param[1];
				ptArr.Add(pt);
			}

			for (int j=0;j<m_Array.GetSize();j++)
			{
				if (m_Array[j]->m_Type==OBJ_POINT &&
					(m_Array[j])->m_Mode==POINT_MODE_CHILD_LINE &&
					(m_Array[j])->m_Param[0]==m_Array[i])
				{
					ptArr.Add((CVGPoint*)m_Array[j]);
				}

				if (m_Array[j]->m_Type==OBJ_POINT &&
					(m_Array[j])->m_Mode==POINT_MODE_INTERSECTION_LINE_LINE &&
					((m_Array[j])->m_Param[0]==m_Array[i] || (m_Array[j])->m_Param[1]==m_Array[i]))
				{
					ptArr.Add((CVGPoint*)m_Array[j]);
				}

				if (m_Array[j]->m_Type==OBJ_POINT &&
					(m_Array[j])->m_Mode==POINT_MODE_INTERSECTION_LINE_CIRCLE &&
					(m_Array[j])->m_Param[0]==m_Array[i])
				{
					ptArr.Add((CVGPoint*)m_Array[j]);
				}
			}

			for (int i=0;i<ptArr.GetSize();i++)
			{
				if (!ptArr[i]->m_bVisible || !ptArr[i]->m_bAvailable || ptArr[i]->m_bTemp)
				{
					ptArr.RemoveAt(i);
					i--;
				}
			}

			if (ptArr.GetSize()<2)
			{
				((CVGLine*)m_Array[i])->m_pt1=NULL;
				((CVGLine*)m_Array[i])->m_pt2=NULL;
			}
			else
			{
				CVGPoint* pt1=ptArr[0];
				CVGPoint* pt2=ptArr[0];
				for (int k=1;k<ptArr.GetSize();k++)
				{
					if (Math::Equal(ptArr[k]->m_x,pt1->m_x))
					{
						if (ptArr[k]->m_y<pt1->m_y) pt1=ptArr[k];
					}
					else if (ptArr[k]->m_x<pt1->m_x)
						pt1=ptArr[k];

					if (Math::Equal(ptArr[k]->m_x,pt2->m_x))
					{
						if (ptArr[k]->m_y>pt2->m_y) pt2=ptArr[k];
					}
					else if (ptArr[k]->m_x>pt2->m_x)
						pt2=ptArr[k];
				}
				((CVGLine*)m_Array[i])->m_pt1=pt1;
				((CVGLine*)m_Array[i])->m_pt2=pt2;
			}
		}
	}
}

void CVGObjContainer::Calc()
{
	for (int i=0;i<m_Array.GetSize();i++)
	{
		m_Array[i]->m_bCalc=false;
	}
	BOOL kt=FALSE;
	while (kt==FALSE)
	{
		for(int i=0;i<m_Array.GetSize();i++) 
			if (m_Array[i])(m_Array[i])->Calc(&m_AxisInfo);
		kt=TRUE;
		for(int i=0;i<m_Array.GetSize();i++) 
			if ((m_Array[i])->m_bCalc==FALSE)
			{ 
				kt=FALSE; break; 
			}
	}

	// Xử lý thuộc tính cắt bỏ phần thừa

	CutSuperfluousLinePart();

	for (int i=0;i<m_Array.GetSize();i++)
		m_Array[i]->CheckCondition();

	for (int i=0;i<m_expWatchArray.GetSize();i++)
		m_expWatchArray[i]->Calc();

	pView->RefreshWatch();

	m_pObjTree->Invalidate(FALSE);
}


void CVGObjContainer::GetRelatedObjArray(CVGObject* obj, CArray<CVGObject*>& arr)
{
	CArray<bool> b;
	for (int i=0;i<m_Array.GetSize();i++) b.Add(false);
	int index=GetObjIndex(obj);
	b[index]=true;
	bool kt=false;
	while (!kt)
	{
		kt=true;
		for (int i=0;i<m_Array.GetSize();i++)
		{
			if (b[i]) continue;
			for (int j=0;j<m_Array.GetSize();j++)
			{
				if (b[j])
				{
					if (m_Array[i]->IsRelated(m_Array[j]))
					{
						b[i]=true;
						kt=false;
					}
				}
			}
		}
	}
	for (int i=0;i<m_Array.GetSize();i++) if (i!=index && b[i]) arr.Add(m_Array[i]);
}

void CVGObjContainer::Calc(CVGObject* obj)
{
	if (obj==NULL) return;
	CArray<CVGObject*> arr;
	GetRelatedObjArray(obj,arr);
	obj->m_bCalc=false;
	obj->Calc(&m_AxisInfo);
	for (int i=0;i<arr.GetSize();i++)
	{
		arr[i]->m_bCalc=false;
	}
	
	BOOL kt=FALSE;
	while (!kt)
	{
		for(int i=0;i<arr.GetSize();i++) 
			(arr[i])->Calc(&m_AxisInfo);
		kt=TRUE;
		
		for(int i=0;i<arr.GetSize();i++) 
			if ((arr[i])->m_bCalc==FALSE)
			{ 
				kt=FALSE; break; 
			}
	}

	// Xử lý thuộc tính cắt bỏ phần thừa

	CutSuperfluousLinePart();

	for (int i=0;i<arr.GetSize();i++)
		m_Array[i]->CheckCondition();

	for (int i=0;i<m_expWatchArray.GetSize();i++)
		m_expWatchArray[i]->Calc();

	pView->RefreshWatch();

	m_pObjTree->Invalidate(FALSE);
}

void CVGObjContainer::CalcAnimate()
{
	for (int i=0;i<m_Array.GetSize();i++) 
		if (IsGeoObject(m_Array[i]->m_Type) && ((CVGGeoObject*)m_Array[i])->m_bAnimate) Calc(m_Array[i]);
}

void CVGObjContainer::Calc_Resize()
{
	for (int i=0;i<m_Array.GetSize();i++)
	{
		m_Array[i]->m_bCalc=FALSE;
	}
	for (int i=0;i<m_Array.GetSize();i++)
		if (m_Array[i]->m_Type==OBJ_FUNCTION ||
			m_Array[i]->m_Type==OBJ_CONIC)
			m_Array[i]->Calc(&m_AxisInfo);
		else m_Array[i]->m_bCalc=true;
}

void CVGObjContainer::GetParam(CString strParam, CStringArray* aParam)
{
	if (strParam==L"") return;
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

void CVGObjContainer::GetFuncAndParam(CString strFunc, CString& sFunc, CStringArray *aParam)
{
	int i=strFunc.Find(L"(");
	if (i==-1)
	{
		sFunc=strFunc;
	}
	else
	{
		sFunc=strFunc.Left(i);
		i++;
		GetParam(strFunc.Mid(i,strFunc.GetLength()-i-1),aParam);
	}
}

//Trả về TRUE nếu thay đổi 1 đt, (obj=NULL)
BOOL CVGObjContainer::HighLightObj(CVGObject* obj,BOOL bHighLight,BOOL bAdd)
{
	if (bAdd)
	{
		obj->m_HighLight=bHighLight;
		return TRUE;
	}

	if (obj==NULL)
	{
		BOOL kt=FALSE;
		for (int i=0;i<m_Array.GetSize();i++)
			if (m_Array[i]->m_HighLight)
			{
				(m_Array[i])->m_HighLight=FALSE;
				kt=TRUE;
			}
		return kt;
	}
	else 
	{
		for (int i=0;i<m_Array.GetSize();i++)
			m_Array[i]->m_HighLight=FALSE;
		obj->m_HighLight=bHighLight;
	}
	return FALSE;
}

void CVGObjContainer::DeleteObjFrom(int nIndex)
{
	int s=(int)m_Array.GetSize();
	for (int i=nIndex;i<s;i++)
	{
		//delete m_Array[nIndex];
		//m_Array.erase(nIndex);
	}
}

void CVGObjContainer::SetSelection(int nIndex)
{
	SelectAll(FALSE);
	Select(m_Array[nIndex],FALSE);
}

void CVGObjContainer::DragLabel(CVGObject* obj, int x, int y)
{
	((CVGGeoObject*)obj)->m_Label->MoveTo(x,y,&m_AxisInfo);
}

void CVGObjContainer::ResetObjMoved()
{
	for (int i=0;i<m_Array.GetSize();i++)
		if (m_Array[i]->m_Type==OBJ_POINT || m_Array[i]->m_Type==OBJ_POINT_HIDDEN_CHILD)
		{
			CVGPoint* pt=(CVGPoint*)m_Array[i];
			pt->m_moved=FALSE;
		}
}

void CVGObjContainer::AddObjTreeItem(CVGObject* obj)
{
	m_pObjTree->AddObj(obj);
// 	if (m_pObjTree!=NULL) 
// 	{
// 		m_pObjTree->FillList();
// 	}
}

void CVGObjContainer::SelectInRect(Rect rect)
{
	for (int i=0;i<m_Array.GetSize();i++)
	{
		if (m_Array[i]->m_bVisible==TRUE)
		{
			if ((m_Array[i])->CheckObjInRect(rect,&m_AxisInfo)) Select(m_Array[i],FALSE);
		}
	}
}

int CVGObjContainer::GetSelection(int iStart)
{
	for (int i=iStart;i<GetSize();i++)
	{
		if ((m_Array[i])->m_Select) return i;
	}
	return -1;
}

void CVGObjContainer::DeleteSelection()
{
	CArray<BOOL> deletedObj;
	for (int i=0;i<m_Array.GetSize();i++) deletedObj.Add(FALSE);

	// Đánh dấu xóa
	for (int i=0;i<m_Array.GetSize();i++)
	{
		if ((m_Array[i])->m_Select) 
		{
			deletedObj[i]=TRUE;
			BOOL b=FALSE; // Kiểm tra xem có đt nào được dựng cùng lượt với obj ko
			for (int k=0;k<m_Array.GetSize();k++)
				if (!deletedObj[k] && m_Array[k]->m_cmdIndex==m_Array[i]->m_cmdIndex && !m_Array[k]->m_bTemp) 
					b=TRUE;
			if (!b)
			{
				for (int k=0;k<m_Array.GetSize();k++)
					if (m_Array[k]->m_cmdIndex==m_Array[i]->m_cmdIndex) 
					{
						deletedObj[k]=TRUE;
					}
			}
		}	
	}

	BOOL d=TRUE;
	while (d) {
	d=FALSE;
	for (int i=0;i<m_Array.GetSize();i++)
	{
		if (!deletedObj[i])
		{
			for (int j=0;j<m_Array.GetSize();j++)
			{
				if (deletedObj[j]) 
				{
					if ((m_Array[i])->IsRelated((m_Array[j]))) 
					{
						deletedObj[i]=TRUE;
						d=TRUE;

						BOOL b=FALSE; // Kiểm tra xem có đt nào được dựng cùng lượt với obj ko
						for (int k=0;k<m_Array.GetSize();k++)
						{
							if (m_Array[k]->m_cmdIndex==m_Array[i]->m_cmdIndex && !m_Array[k]->m_bTemp) b=TRUE;
						}
						if (!b)
						{
							for (int k=0;k<m_Array.GetSize();k++)
								if (m_Array[k]->m_cmdIndex==m_Array[i]->m_cmdIndex) deletedObj[k]=TRUE;
						}
					}
				}
			}
		}
	}
	}

	// Xoá các điểm con
// 	for (int i=0;i<m_Array.GetSize();i++)
// 	{
// 		if (m_Array[i]->m_Type==OBJ_POINT_HIDDEN_CHILD)
// 		{
// 			for (int j=0;j<m_Array.GetSize();j++)
// 				if (m_Array[j]==m_Array[i]->m_Param[0] && deletedObj[j]) deletedObj[i]=TRUE;
// 		}
// 	}

	CArray<CVGObject*> arr;
	for (int i=0;i<deletedObj.GetSize();i++)
		if (deletedObj[i]) arr.Add(m_Array[i]);
	OnRemoveObjs(&arr);

	for (int i=deletedObj.GetSize()-1;i>=0;i--)
		if (deletedObj[i])
		{
			m_Array.RemoveAt(i);
		}
}

// void CVGObjContainer::Delete(CVGObject* obj, CArray<BOOL>* deletedObj)
// {
// 	for (int i=0;i<m_Array.GetSize();i++) if (obj==m_Array[i]) { deletedObj[i]=TRUE; break; }
// 
// 	BOOL b=FALSE; // Kiểm tra xem có đt nào được dựng cùng lượt với obj ko
// 	for (int i=0;i<m_Array.GetSize();i++)
// 		if (m_Array[i]->m_cmdIndex==obj->m_cmdIndex && !m_Array[i]->m_bTemp) b=TRUE;
// 	if (!b)
// 	{
// 		for (int i=0;i<m_Array.GetSize();i++)
// 			if (m_Array[i]->m_cmdIndex==obj->m_cmdIndex) m_objDelete.Add(i);
// 	}
// 
// }

void CVGObjContainer::NextAnimatePos()
{
	for (int i=0;i<m_Array.GetSize();i++)
	{
		if (IsGeoObject(m_Array[i]->m_Type))
		{
			CVGGeoObject* obj=(CVGGeoObject*) m_Array[i];
			if (obj->m_bAnimate) obj->OnAnimate(3,&m_AxisInfo);
		}
	}
}

CString CVGObjContainer::GetNextAvailableName(int nType, CStringArray* stringArr)
{
	if (IsBoundType(nType))
	{
		CString strPre;
		switch (nType)
		{
		case OBJ_PANEL: strPre=L"Panel"; break;
		case OBJ_BUTTON: strPre=L"Button"; break;
		case OBJ_CHECKBOX: strPre=L"Check"; break;
		case OBJ_RADIO_BUTTON: strPre=L"Option"; break;
		case OBJ_TEXT: strPre=L"Text"; break;
		case OBJ_SLIDER: strPre=L"Slider"; break;
		}

		char c[10];
		for (int i=1;TRUE;i++)
		{
			_itoa_s(i,c,10);
			if (!GetObjByName(strPre+(CString)c)) return strPre+(CString)c;
		}
	}
	else
	{
		char chr;
		char c[10];
		CString strName;
		int i=0;
		while (TRUE)
		{
			for (chr='a';chr<='z';chr++)
			{
				strName=chr;
				if (nType==OBJ_POINT) strName.MakeUpper();
				if (i!=0)
				{
					_itoa_s(i,c,10);
					strName+=L"_";
					strName+=c;
				}
				int j=0;
				for (j;j<m_Array.GetSize();j++)
					if (strName==m_Array[j]->m_Name) break;

				int k=0;
				if (stringArr!=NULL)
				{
					for (k;k<stringArr->GetSize();k++)
						if (stringArr->GetAt(k)==strName) break;
				}

				if (j==m_Array.GetSize() && (stringArr==NULL || k==stringArr->GetSize())) return strName;
			}
			i++;
		}
	}
}

BOOL CVGObjContainer::CheckRegularName(CString strName)
{
	if (!((strName[0]>='a' && strName[0]<='z') || (strName[0]>='A' && strName[0]<='Z'))) return FALSE;
	for (int i=0;i<strName.GetLength();i++)
		if (!((strName[i]>='a' && strName[i]<='z') || (strName[i]>='A' && strName[i]<='Z') || (strName[i]>='0' && strName[i]<='9') || strName[i]=='_' || strName[i]=='\'' || strName=='*')) return FALSE;
	return TRUE;
}

// BOOL CVGObjContainer::CheckRegularName(CString strName)
// {
// 	if (!((strName[0]>='a' && strName[0]<='z') || (strName[0]>='A' && strName[0]<='Z'))) return FALSE;
// 	else
// 	{
// 		BOOL b1, b2, b3;
// 		b1=TRUE; b2=TRUE; b3=TRUE;
// 		for (int i=1;i<strName.GetLength();i++)
// 		{
// 			if (!(strName[i]>='0' && strName[i]<='9')) b1=FALSE;
// 			if (!strName[i]=='\'') b2=FALSE;
// 			if (!strName[i]=='*') b3=FALSE;
// 			if (!(b1 || b2 || b3)) return FALSE;
// 		}
// 	}
// 	return TRUE;
// }

CString CVGObjContainer::GetSimilarName(CString strName)
{
	CString str=strName;
	while (str[str.GetLength()-1]=='\'') str.Delete(str.GetLength()-1);
	while (TRUE)
	{
		str+='\'';
		if (!GetObjByName(str)) return str;
	}
}

BOOL CVGObjContainer::RenameObject(CVGObject* obj, CString newName)
{
	if (CheckRegularName(newName))
	{
		if (IsBoundType(obj->m_Type))
		{
			CVGObject* o=GetObjByName(newName);
			if (o!=NULL && o!=obj)
			{
				AfxMessageBox(L"Đã có đối tượng khác cùng tên",MB_OK | MB_ICONERROR,-1);
				return FALSE;
			}
			else obj->m_Name=newName;
		}
		else
		{
			for (int i=0;i<m_Array.GetSize();i++)
			{
				if (obj!=m_Array[i] && m_Array[i]->m_Name==newName)
				{
					if (AfxMessageBox(L"Đã có đối tượng hình học cùng tên. Bạn có muốn tiếp tục đổi tên không ?\n(Đối tượng cùng tên sẽ được đặt lại tên khác)",MB_YESNO | MB_ICONQUESTION,-1)==IDYES)
						RenameObject(m_Array[i],GetSimilarName(newName));
					else return FALSE;
					break;
				}
			}

			CString strOldName=L"~"+obj->m_Name+L"~";
			CString strNewName=L"~"+newName+L"~";
			obj->SetName(newName,m_LabelFont,&m_AxisInfo);

			for (int i=0;i<m_Array.GetSize();i++)
			{
				(m_Array[i])->m_Definition.Replace(strOldName, strNewName);
				if (m_Array[i]->m_Type==OBJ_EXPRESSION)
				{
					((CVGExpression*)m_Array[i])->m_strExp.Replace(strOldName,strNewName);
				}
			}
			m_pObjTree->Invalidate(FALSE);
			pView->RefreshConstructionSteps();
			return TRUE;
		}
	}
	else
	{
		AfxGetApp()->DoMessageBox(L"Tên không hợp lệ", MB_OK | MB_ICONERROR, -1);
		return FALSE;
	}
	pView->RefreshProperties();
}

void CVGObjContainer::Serialize(CArchive& ar)
{
	m_AxisInfo.Serialize(ar);
	m_graphAxis.Serialize(ar);
 	if (ar.IsStoring()) //Lưu
 	{
 		ar << m_Array.GetSize()-2;
 		for (int i=2;i<m_Array.GetSize();i++)
		{
			ar << m_Array[i]->m_Type;
 			m_Array[i]->Serialize(ar,&m_Array);
		}
		
		ar << m_expWatchArray.GetSize();
		for (int i=0;i<m_expWatchArray.GetSize();i++)
			ar << m_expWatchArray[i]->m_strExp;
 	}
	else
	{
		int count;
		ar >> count;
		int type;
		for (int i=0;i<count;i++)
		{
			ar >> type;
			CVGObject* obj=AddObject((ObjType)type);
			obj->Serialize(ar,&m_Array);
			if (obj->m_Type==OBJ_EXPRESSION)
				((CVGExpression*)obj)->SetExp(((CVGExpression*)obj)->m_strExp,&m_Array,m_strReplacedNameArr);
		}

		for (int i=0;i<m_Array.GetSize();i++)
		{
			if (m_Array[i]->m_Type==OBJ_POINT)
			{
				CVGPoint* pt=(CVGPoint*)m_Array[i];
				for (int j=0;j<pt->m_objMove.GetSize();j++)
					pt->m_objMove[j]=(CVGPoint*)m_Array[(int)pt->m_objMove[j]];
			}
		}

		for (int i=0;i<m_Array.GetSize();i++)
		{
			CVGObject* obj=m_Array[i];
			for (int j=0;j<obj->m_Param.GetSize();j++)
				obj->m_Param[j]=m_Array[(int)obj->m_Param[j]];
		}

		for (int i=0;i<m_Array.GetSize();i++)
		{
			if (IsBoundType(m_Array[i]->m_Type))
			{
				CVGBound* obj=(CVGBound*)m_Array[i];
				obj->m_ParentPanel=((int)obj->m_ParentPanel==-1 ? NULL : (CVGPanel*)m_Array[(int)obj->m_ParentPanel]);
			}
		}

		for (int i=0;i<m_Array.GetSize();i++)
		{
			if (m_Array[i]->m_Type==OBJ_PANEL)
			{
				CVGPanel* obj=(CVGPanel*)m_Array[i];
				for (int j=0;j<obj->m_objChild.GetSize();j++)
					obj->m_objChild[j]=(CVGBound*)m_Array[(int)obj->m_objChild[j]];
			}
		}

		for (int i=0;i<m_Array.GetSize();i++)
		{
			if (IsGeoObject(m_Array[i]->m_Type))
			{
				CVGGeoObject* obj=(CVGGeoObject*)m_Array[i];
				if (obj->m_Label!=NULL)
					obj->m_Label->m_Font=m_LabelFont;
			}
		}

		ar >> count;
		CString s;
		for (int i=0;i<count;i++)
		{
			ar >> s;
			AddWatch(s);
		}
	}
}

void CVGObjContainer::Redo()
{
	m_nUndoPos++;
	Undo();
	m_nUndoPos++;
}

void CVGObjContainer::Undo()
{
	if (m_UndoType[m_nUndoPos]==UNREDO_TYPE_CHANGE_PROPERTIES)
	{
		CArray<CVGObject*>* arr1=(CArray<CVGObject*>*)m_UndoArray1[m_nUndoPos]; // new properties
		CArray<CVGObject*>* arr2=(CArray<CVGObject*>*)m_UndoArray2[m_nUndoPos]; // old properties

		for (int i=0;i<arr1->GetSize();i++)
			for (int j=0;j<m_Array.GetSize();j++)
				if (m_Array[j]==arr2->GetAt(i))
				{
					CVGObject* obj=AddObject(m_Array[j]->m_Type);
					m_Array.RemoveAt(m_Array.GetSize()-1);
					obj->Clone(m_Array[j]);
					m_Array[j]->Clone(arr1->GetAt(i));
					arr1->GetAt(i)->Clone(obj);
					delete obj;					
				}
	}
	else if (m_UndoType[m_nUndoPos]==UNREDO_TYPE_ADD)
	{
		CArray<CVGObject*>* arr1=(CArray<CVGObject*>*)m_UndoArray1[m_nUndoPos];
		CArray<int>* arr2=(CArray<int>*)m_UndoArray2[m_nUndoPos];
		for (int j=0;j<arr2->GetSize();j++)
			m_Array.InsertAt(arr2->GetAt(j),arr1->GetAt(j));
		delete arr1;
		m_UndoArray2[m_nUndoPos]=NULL;
		m_UndoArray1[m_nUndoPos]=arr2;
		m_UndoType[m_nUndoPos]=UNREDO_TYPE_REMOVE;
	}
	else if (m_UndoType[m_nUndoPos]==UNREDO_TYPE_REMOVE)
	{
		CArray<int>* arr1=(CArray<int>*)m_UndoArray1[m_nUndoPos];
		CArray<CVGObject*>* arr2=new CArray<CVGObject*>;
		for (int i=arr1->GetSize()-1;i>=0;i--)
		{
			arr2->Add(m_Array[arr1->GetAt(i)]);
			m_Array.RemoveAt(arr1->GetAt(i));
		}
		m_UndoArray1[m_nUndoPos]=arr2;
		m_UndoArray2[m_nUndoPos]=arr1;
		m_UndoType[m_nUndoPos]=UNREDO_TYPE_ADD;
	}
	
	m_nUndoPos--;

	Calc();
	Draw();
}

void CVGObjContainer::FreeUndo()
{
	int count=m_UndoType.GetSize()-UNDO_MAX;
	for (int i=0;i<count;i++)
		RemoveUndoArrayElement(0);
}

void CVGObjContainer::OnChangeProperties(CArray<CVGObject*>* objArr)
{
	ResetRedo();
	FreeUndo();
	CArray<CVGObject*>* arr1=new CArray<CVGObject*>;
	// Lưu lại thuộc tính cũ
	for (int i=0;i<objArr->GetSize();i++)
	{
		CVGObject* obj=AddObject(objArr->GetAt(i)->m_Type);
		m_Array.RemoveAt(m_Array.GetSize()-1);
		obj->Clone(objArr->GetAt(i));
		arr1->Add(obj);
	}

	m_UndoArray1.Add(arr1);

	//Các đối tượng có thuộc tính được lưu lại
	CArray<CVGObject*>* arr2=new CArray<CVGObject*>;
	for (int i=0;i<objArr->GetSize();i++)
		arr2->Add(objArr->GetAt(i));

	m_UndoArray2.Add(arr2);

	m_UndoType.Add(UNREDO_TYPE_CHANGE_PROPERTIES);
	m_nUndoPos=m_UndoType.GetSize()-1;
}

void CVGObjContainer::OnRemoveObjs(CArray<CVGObject*>* objArr)
{
	ResetRedo();
	FreeUndo();
	// Đối tượng bị loại bỏ
	CArray<CVGObject*>* arr1=new CArray<CVGObject*>;
	for (int i=0;i<objArr->GetSize();i++)
		arr1->Add(objArr->GetAt(i));

	m_UndoArray1.Add(arr1);

	// Số thứ tự bị loại bỏ
	CArray<int>* arr2=new CArray<int>;
	for (int i=0;i<arr1->GetSize();i++)
		for (int j=0;j<m_Array.GetSize();j++)
			if (arr1->GetAt(i)==m_Array.GetAt(j))
				arr2->Add(i);

	m_UndoArray2.Add(arr2);

	m_UndoType.Add(UNREDO_TYPE_ADD);
	m_nUndoPos=m_UndoType.GetSize()-1;
}

void CVGObjContainer::OnAddObjs(CArray<int>* objArr)
{
	ResetRedo();
	FreeUndo();
	CArray<int>* arr1=new CArray<int>;
	for (int i=0;i<objArr->GetSize();i++)
		arr1->Add(objArr->GetAt(i));

	m_UndoArray1.Add(arr1);
	m_UndoArray2.Add(NULL);
	m_UndoType.Add(UNREDO_TYPE_REMOVE);
	m_nUndoPos=m_UndoType.GetSize()-1;
}

void CVGObjContainer::ResetRedo()
{
	int count=m_UndoType.GetSize();
	for (int i=m_nUndoPos+1;i<count;i++)
	{
		RemoveUndoArrayElement(m_nUndoPos+1);
	}
}

void CVGObjContainer::RemoveUndoArrayElement(int nIndex)
{
	if (m_UndoType[nIndex]==UNREDO_TYPE_CHANGE_PROPERTIES)
	{
		CArray<CVGObject*>* arr1=(CArray<CVGObject*>*)m_UndoArray1[nIndex]; // new properties
		CArray<CVGObject*>* arr2=(CArray<CVGObject*>*)m_UndoArray2[nIndex]; // old properties

		for (int i=0;i<arr1->GetSize();i++)
			delete arr1->GetAt(i);
		delete arr1;
		delete arr2;
		m_UndoArray1.RemoveAt(nIndex);
		m_UndoArray2.RemoveAt(nIndex);
		m_UndoType.RemoveAt(nIndex);
	}
	else if (m_UndoType[nIndex]==UNREDO_TYPE_ADD)
	{
		CArray<CVGObject*>* arr1=(CArray<CVGObject*>*)m_UndoArray1[nIndex];
		for (int i=0;i<arr1->GetSize();i++)
			delete arr1->GetAt(i);
		CArray<int>* arr2=(CArray<int>*)m_UndoArray2[nIndex];
		delete arr1;
		delete arr2;
		m_UndoArray1.RemoveAt(nIndex);
		m_UndoArray2.RemoveAt(nIndex);
		m_UndoType.RemoveAt(nIndex);
	}
	else if (m_UndoType[nIndex]==UNREDO_TYPE_REMOVE)
	{
		CArray<int>* arr1=(CArray<int>*)m_UndoArray1[nIndex];
		delete arr1;
		m_UndoArray1.RemoveAt(nIndex);
		m_UndoArray2.RemoveAt(nIndex);
		m_UndoType.RemoveAt(nIndex);
	}
}

BOOL CVGObjContainer::CanUndo()
{
	return (m_nUndoPos!=-1 && m_UndoType.GetSize()!=0);
}

BOOL CVGObjContainer::CanRedo()
{
	return (m_UndoType.GetSize()!=m_nUndoPos+1 && m_UndoType.GetSize()!=0);
}

bool CVGObjContainer::AddWatch(CString strExp)
{
	CGeoExp* exp=new CGeoExp();
	exp->m_ScriptObject=m_ScriptObject;
	if (!exp->SetExpression(strExp,&m_Array)) { delete exp; return false; }
	else
	{
		m_expWatchArray.Add(exp);
		return true;
	}
}