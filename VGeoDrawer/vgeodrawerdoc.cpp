// VGeoDrawerDoc.cpp : implementation of the CVGeoDrawerDoc class
//

#include "stdafx.h"
#include "VGeoDrawer.h"

#include "VGeoDrawerDoc.h"
#include "InputDlg.h"
#include "ExpressionEditorDlg.h"
#include "appfunc.h"
#include "TextEditorDlg.h"
#include "ScriptDlg.h"
#include "OptionsDlg.h"
#include "EventDlg.h"
#include "PresSetting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern int numRound;
extern bool bRadian;
extern float nZoom;

// CVGeoDrawerDoc

IMPLEMENT_DYNCREATE(CVGeoDrawerDoc, CDocument)

BEGIN_MESSAGE_MAP(CVGeoDrawerDoc, CDocument)
	ON_COMMAND(ID_OPTIONS_COMMANDS_ORGANIZE, &CVGeoDrawerDoc::OnOptionsCommandsOrganize)
	ON_COMMAND(ID_VIEW_AXIS, &CVGeoDrawerDoc::OnViewAxis)
	ON_UPDATE_COMMAND_UI(ID_VIEW_AXIS, &CVGeoDrawerDoc::OnUpdateViewAxis)
	ON_COMMAND(ID_VIEW_GRID, &CVGeoDrawerDoc::OnViewGrid)
	ON_UPDATE_COMMAND_UI(ID_VIEW_GRID, &CVGeoDrawerDoc::OnUpdateViewGrid)
	ON_COMMAND(ID_VIEW_STANDARD_VIEW, &CVGeoDrawerDoc::OnViewStandardView)
	ON_COMMAND(ID_OBJECT_TRACEOBJECT, &CVGeoDrawerDoc::OnObjectTraceobject)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_TRACEOBJECT, &CVGeoDrawerDoc::OnUpdateObjectTraceobject)
	ON_COMMAND(ID_OBJECT_ANIMATEOBJECT, &CVGeoDrawerDoc::OnObjectAnimateobject)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_ANIMATEOBJECT, &CVGeoDrawerDoc::OnUpdateObjectAnimateobject)
	ON_COMMAND(ID_EDIT_COPY, &CVGeoDrawerDoc::OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, &CVGeoDrawerDoc::OnEditCut)
	ON_COMMAND(ID_EDIT_DELETE, &CVGeoDrawerDoc::OnEditDelete)
	ON_COMMAND(ID_EDIT_PASTE, &CVGeoDrawerDoc::OnEditPaste)
//	ON_COMMAND(ID_SETTINGS_DRAWINGPAD, &CVGeoDrawerDoc::OnSettingsDrawingpad)
	ON_COMMAND(ID_FILE_EXPORT_IMAGE, &CVGeoDrawerDoc::OnExportImage)
	ON_COMMAND(ID_EDIT_DELETE_TRACE, &CVGeoDrawerDoc::OnEditDeleteTrace)
	ON_COMMAND(ID_VIEW_SHOW_ALL_OBJECTS, &CVGeoDrawerDoc::OnViewShowAllObjects)
	ON_COMMAND(ID_EDIT_REDO, &CVGeoDrawerDoc::OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, &CVGeoDrawerDoc::OnUpdateEditRedo)
	ON_COMMAND(ID_EDIT_UNDO, &CVGeoDrawerDoc::OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, &CVGeoDrawerDoc::OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_HIDESELECTION, &CVGeoDrawerDoc::OnEditHideselection)
	ON_COMMAND(ID_TOOL_SCRIPT, &CVGeoDrawerDoc::OnToolScript)
	ON_COMMAND(ID_TOOLS_OPTIONS, &CVGeoDrawerDoc::OnToolsOptions)
	
	ON_UPDATE_COMMAND_UI(ID_ZOOM_CUSTOM, &CVGeoDrawerDoc::OnUpdateZoom)
	ON_UPDATE_COMMAND_UI(ID_ZOOM10, &CVGeoDrawerDoc::OnUpdateZoom)
	ON_UPDATE_COMMAND_UI(ID_ZOOM50, &CVGeoDrawerDoc::OnUpdateZoom)
	ON_UPDATE_COMMAND_UI(ID_ZOOM75, &CVGeoDrawerDoc::OnUpdateZoom)
	ON_UPDATE_COMMAND_UI(ID_ZOOM100, &CVGeoDrawerDoc::OnUpdateZoom)
	ON_UPDATE_COMMAND_UI(ID_ZOOM200, &CVGeoDrawerDoc::OnUpdateZoom)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_FIT_SCREEN, &CVGeoDrawerDoc::OnUpdateZoom)
	ON_COMMAND(ID_ZOOM_CUSTOM, &CVGeoDrawerDoc::OnZoom)
	ON_COMMAND(ID_ZOOM100, &CVGeoDrawerDoc::OnZoom100)
	ON_COMMAND(ID_ZOOM200, &CVGeoDrawerDoc::OnZoom200)
	ON_COMMAND(ID_ZOOM50, &CVGeoDrawerDoc::OnZoom50)
	ON_COMMAND(ID_ZOOM75, &CVGeoDrawerDoc::OnZoom75)
	ON_COMMAND(ID_ZOOM_FIT_SCREEN, &CVGeoDrawerDoc::OnZoomFitScreen)
	ON_COMMAND(ID_PAGE_NEXT, &CVGeoDrawerDoc::OnPageNext)
	ON_UPDATE_COMMAND_UI(ID_PAGE_NEXT, &CVGeoDrawerDoc::OnUpdatePageNext)
	ON_COMMAND(ID_PAGE_PREVIOUS, &CVGeoDrawerDoc::OnPagePrevious)
	ON_UPDATE_COMMAND_UI(ID_PAGE_PREVIOUS, &CVGeoDrawerDoc::OnUpdatePagePrevious)
	ON_COMMAND(ID_PAGE_NEW_PAGE, &CVGeoDrawerDoc::OnPageNewPage)
	ON_COMMAND(ID_PAGE_DELETE_PAGE, &CVGeoDrawerDoc::OnPageDeletePage)
	ON_COMMAND(ID_TOOLS_EVENT, &CVGeoDrawerDoc::OnToolsEvent)
	ON_COMMAND(ID_OBJECT_PROPERTIES, &CVGeoDrawerDoc::OnObjectProperties)
	ON_COMMAND(ID_WATCHWND_ADDWATCH, &CVGeoDrawerDoc::OnWatchwndAddwatch)
	ON_COMMAND(ID_TOOL_PRESENT_MODE, &CVGeoDrawerDoc::OnToolPresentMode)
	ON_UPDATE_COMMAND_UI(ID_PAGE_DELETE_PAGE, &CVGeoDrawerDoc::OnUpdatePageDeletePage)
	ON_COMMAND(ID_VIEW_DESIGNMODE, &CVGeoDrawerDoc::OnViewDesignmode)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DESIGNMODE, &CVGeoDrawerDoc::OnUpdateViewDesignmode)
END_MESSAGE_MAP()


// CVGeoDrawerDoc construction/destruction

CVGeoDrawerDoc::CVGeoDrawerDoc()
{
	m_bHasPadSize=FALSE;
	m_Width=800;
	m_Height=600;
	m_Mode=DOCUMENT_MODE_GEO;
	m_bInit=false;
	//nZoom=50;

	m_bFitScreen=true;
	nZoom=100;

	m_defaultAxisInfo.Unit=1;
	m_defaultAxisInfo.Pixel=50;
	m_defaultAxisInfo.m_originPoint=Point(300,300);

	m_bDesignMode=true;

	InitScriptObject();
	//GetObjCont()->m_ScriptObject=&m_ScriptObject;
}

CVGeoDrawerDoc::~CVGeoDrawerDoc()
{
	DeleteAllObjCont();
}

void CVGeoDrawerDoc::DeleteAllObjCont()
{
	for (int i=0;i<m_ObjContainerArr.GetSize();i++) delete m_ObjContainerArr[i];
	m_ObjContainerArr.RemoveAll();
}

void CVGeoDrawerDoc::AddObjContainer(int pos)
{
	CVGObjContainer* objCont=new CVGObjContainer();
	objCont->pView=pView;
	objCont->m_AxisInfo=m_defaultAxisInfo;
	objCont->m_graphAxis=m_defaultGraphAxis;
	objCont->m_pObjTree=&m_wndList->m_tree;
	objCont->m_defaultValue=&m_defaultValue;
	objCont->Init();
	if (pos==-1) m_ObjContainerArr.Add(objCont);
	else m_ObjContainerArr.InsertAt(pos,objCont);
	objCont->m_Name=L"New Page";
	objCont->m_PadHeight=&m_Height;
	objCont->m_PadWidth=&m_Width;
	objCont->m_ScriptObject=&m_ScriptObject;
	objCont->m_cmdArray=&m_cmdArray;
	
	m_cbPage->ResetContent();
	for (int i=0;i<m_ObjContainerArr.GetSize();i++)
		m_cbPage->AddString(Math::DoubleToString(i+1)+L" / "+Math::DoubleToString(m_ObjContainerArr.GetSize()));
	m_cbPage->SetCurSel(m_nCurrentContainer);

	m_wndPageList->FillList();
}

void CVGeoDrawerDoc::SetObjContainer(int nIndex)
{
	m_nCurrentContainer=nIndex;
	m_cbPage->SetCurSel(nIndex);
	CVGObjContainer* objCont=GetObjCont();
	
	m_wndList->m_tree.m_Array=&objCont->m_Array;
	m_wndList->m_tree.pArray=objCont;
	m_wndList->m_tree.Refresh();

	m_wndCommand->m_List.m_objContainer=objCont;
	m_wndCommand->m_List.m_Edit.m_objArr=&objCont->m_Array;
	
	m_wndWatchBar->m_objCont=objCont;
	m_wndWatchBar->pView=pView;
	m_wndWatchBar->Reload();

	m_wndConstructionSteps->m_objArray=objCont;
	m_wndConstructionSteps->Reload();
	
	pView->RefreshSize();

	pView->ResizeDrawingPad();

	pView->DrawBackground();
	pView->Draw();
}

void CVGeoDrawerDoc::InitScriptObject()
{
	m_ScriptObject.Reset();
	CStdioFile file;
	CFileException feError;
	if (file.Open(GetAppDirectory()+L"\\data\\defaultscript.dat",CStdioFile::modeRead,&feError))
	{
		CString strScript;
		CString line;
		while( file.ReadString( line ) )
			strScript += line + L"\r\n";
		file.Close();
		m_ScriptObject.AddScript(strScript);
	}
	else
	{

		TCHAR	errBuff[256];
		feError.GetErrorMessage( errBuff, 256 );
		AfxMessageBox(errBuff,MB_OK | MB_ICONERROR);
	}
}

BOOL CVGeoDrawerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	DeleteAllObjCont();
	
	LoadDefaultSettings();
	m_cmdArray.LoadCommands();

	AddObjContainer();
	SetObjContainer(0);

	pView->m_bTestPointMoving=false;
	pView->DrawBackground();

	m_wndList->m_tree.Reset();
	m_wndCommand->m_List.Reset();
	m_wndProperties->RefreshProperties();

	m_bInit=true;

	m_currentToolName=L"Con trỏ";
	
	m_wndObjToolBar->RemoveAll();
	m_wndObjToolBar->LoadButtons();

	m_presWhenStart=false;
	m_askWhenClose=false;

	return TRUE;
}

// CVGeoDrawerDoc serialization

void CVGeoDrawerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		SerializeSettings(ar);
		ar << m_presWhenStart;
		ar << m_askWhenClose;

		ar << numRound;
		ar << bRadian;
		ar << m_bHasPadSize;
		ar << m_Width;
		ar << m_Height;

		ar << m_ObjContainerArr.GetSize();
		for (int i=0;i<m_ObjContainerArr.GetSize();i++)
			m_ObjContainerArr[i]->Serialize(ar);
		ar << m_nCurrentContainer;
	}
	else
	{
		SerializeSettings(ar);
		ar >> m_presWhenStart;
		ar >> m_askWhenClose;

		ar >> numRound;
		ar >> bRadian;
		ar >> m_bHasPadSize;
		ar >> m_Width;
		ar >> m_Height;

		int count;
		ar >> count;
		for (int i=0;i<count;i++)
		{
			AddObjContainer(-1);
			m_ObjContainerArr[i]->Serialize(ar);
		}
		ar >> m_nCurrentContainer;
		pView->DrawAllBackground();
		SetObjContainer(m_nCurrentContainer);
	}
}


// CVGeoDrawerDoc diagnostics

#ifdef _DEBUG
void CVGeoDrawerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CVGeoDrawerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CVGeoDrawerDoc commands

void CVGeoDrawerDoc::OnOptionsCommandsOrganize()
{
	CCommandsOrganizeDlg dlg;
	if (dlg.DoModal()==IDOK)
	{
		m_cmdArray.LoadCommands();
	}
	m_crCommandName.RemoveAll();
	m_crObj.RemoveAll();
	m_crSubCommand.RemoveAll();
}

void CVGeoDrawerDoc::OnViewGrid() 
{
	GetObjCont()->m_graphAxis.m_bShowGrid=!GetObjCont()->m_graphAxis.m_bShowGrid;
	pView->DrawBackground();
	pView->Draw();
}

void CVGeoDrawerDoc::OnUpdateViewGrid(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(GetObjCont()->m_graphAxis.m_bShowGrid);
}

void CVGeoDrawerDoc::OnUpdateViewAxis(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(GetObjCont()->m_graphAxis.m_bOx && GetObjCont()->m_graphAxis.m_bOy);
}

void CVGeoDrawerDoc::OnViewAxis() 
{
	AfxMessageBox(Math::DoubleToString(GetObjCont()->GetSize()));
 	BOOL b=GetObjCont()->m_graphAxis.m_bOx && GetObjCont()->m_graphAxis.m_bOy;
	GetObjCont()->m_graphAxis.m_bOx=!b;
	GetObjCont()->m_graphAxis.m_bOy=!b;
	GetView()->DrawBackground();
	GetView()->Draw();
}

BOOL CVGeoDrawerDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	DeleteAllObjCont();
	m_cmdArray.LoadCommands();

	pView->m_bTestPointMoving=false;

	m_wndList->m_tree.Reset();
	m_wndCommand->m_List.Reset();
	m_wndProperties->RefreshProperties();

	m_bInit=true;

	m_currentToolName=L"Con trỏ";

	m_wndObjToolBar->RemoveAll();
	m_wndObjToolBar->LoadButtons();

	m_presWhenStart=false;
	m_askWhenClose=false;

	m_wndConstructionSteps->Reload();
	m_wndList->m_tree.DeleteAllItems();

	m_currentToolName=L"Con trỏ";

	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	if (m_presWhenStart) AfxGetMainWnd()->SendMessage(WM_COMMAND,ID_VIEW_SHOWMODE);
	m_wndPageList->FillList();

	return TRUE;
}

CVGeoDrawerView* CVGeoDrawerDoc::GetView()
{
	POSITION pos=this->GetFirstViewPosition();
	return (CVGeoDrawerView*)GetNextView(pos);
}

BOOL CVGeoDrawerDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	GetObjCont()->ResetUndoArray();
	return CDocument::OnSaveDocument(lpszPathName);
}

void CVGeoDrawerDoc::OnCloseDocument()
{
	CDocument::OnCloseDocument();
}

void CVGeoDrawerDoc::ResetSelection()
{
	m_crObj.RemoveAll();
	GetObjCont()->SelectAll(FALSE);
}

void CVGeoDrawerDoc::SetToolMode(CString strToolName)
{
	m_crCommandName.RemoveAll();
	m_crSubCommand.RemoveAll();
	m_crObj.RemoveAll();
	GetObjCont()->DeleteTestObj();
	pView->m_bTestPointMoving=FALSE;
	GetObjCont()->SelectAll(FALSE);

// 	if (m_currentToolName!=strToolName)
// 		GetObjCont()->HighLightObj(NULL);

	if (m_currentToolName==L"Con trỏ" && strToolName!=L"Con trỏ")
	{
		pView->m_SelectionTopLeft=pView->m_SelectionRightBottom;
	}

	for (int i=0;i<m_cmdArray.GetSize();i++)
	{
		for (int j=0;j<m_cmdArray[i]->GetSize();j++)
		{
			if (m_cmdArray[i]->m_SubCmdArray[j].m_ToolName==strToolName)
			{
				m_crCommandName.Add(m_cmdArray[i]->m_CommandName);
				m_crSubCommand.Add(&m_cmdArray[i]->m_SubCmdArray[j]);
			}
		}
	}

	CArray<int> inputParam;
	CCommandArray::GetParamType(m_crSubCommand[0]->m_Input,&inputParam);
	if (inputParam[0]==OBJ_POINTS)
	{
		pView->strFixedTextFollowMouse=L"Chọn dãy điểm. Nhấn Enter để kết thúc.";
	}
	else pView->strFixedTextFollowMouse=L"";
	pView->strTextFollowMouse=L"";

	m_currentToolName=strToolName;
	pView->Draw();
}

//Kiểm tra xem có thể chọn đối tượng không
BOOL CVGeoDrawerDoc::IsObjTypeAvailable(int nType, BOOL* bIsFinal/*=NULL*/)
{
	if (m_currentToolName==L"Con trỏ") { if (bIsFinal!=NULL) *bIsFinal=FALSE; return TRUE; }
	//if (m_currentToolName==L"Đa giác") { if (bIsFinal!=NULL) *bIsFinal=FALSE; return nType==OBJ_POINT; }
	if (m_currentToolName==L"Điểm") { if (bIsFinal!=NULL) *bIsFinal=FALSE; return nType==OBJ_POINT; }
	int type;
	for (int i=0;i<m_crCommandName.GetSize();i++)
	{
		CArray<int> inputParam;
		CCommandArray::GetParamType(m_crSubCommand[i]->m_Input,&inputParam);
		if (inputParam[0]==OBJ_POINTS && inputParam.GetSize()==1) { if (bIsFinal!=NULL) *bIsFinal=(m_crObj.GetSize()>0); return nType==OBJ_POINT; }
		else if (inputParam.GetSize()>m_crObj.GetSize())
		{
			BOOL kt=TRUE; // Các tham số trước phải phù hợp
			for (int j=0;j<m_crObj.GetSize();j++)
			{
				if (/*
					!((IsLineType(m_crObj[j]->m_Type)) && (inputParam[j]==OBJ_LINE)) && 
										(!((m_crObj[j]->m_Type==OBJ_POINT) && inputParam[j]==OBJ_POINT_HIDDEN_CHILD)) && 
										(!((IsNumberType(m_crObj[j]->m_Type)) && (inputParam[j]==OBJ_NUMBER))) &&
										(m_crObj[j]->m_Type!=inputParam[j])*/
					!(CanSelectObject(m_crObj[j]->m_Type,inputParam[j]))) 
				{ 
						kt=FALSE; break; 
				}
			}

			if (kt) // Kiểm tra nType
			{
				type=inputParam[m_crObj.GetSize()];
				if (bIsFinal!=NULL) *bIsFinal = (inputParam.GetSize()==m_crObj.GetSize()+1);
				if ((IsLineType(nType)) && (type==OBJ_LINE)) return TRUE;
				if ((nType==OBJ_POINT) && type==OBJ_POINT_HIDDEN_CHILD) return TRUE;
				if (type==nType) return TRUE;
			}
		}
	}
	return FALSE;
}

void CVGeoDrawerDoc::EndSelectObject()
{
	CArray<int> inputParam;
	CCommandArray::GetParamType(m_crSubCommand[0]->m_Input,&inputParam);
	if (inputParam[0]==OBJ_POINTS)
	{
		GetObjCont()->DeleteTestObj();
		CStringArray arr;
		GetObjCont()->DoCommand(m_crCommandName[0],m_crObj,arr,NULL,NULL);
		ResetSelection();
		pView->Draw();
	}
}

void CVGeoDrawerDoc::AddCrObj(CVGObject* obj, BOOL bTest/*=FALSE*/)
{
	m_crObj.Add(obj);
	if (!bTest) obj->m_Select=true;

	for (int i=0;i<m_crCommandName.GetSize();i++)
	{
		CArray<int> inputParam;
		CCommandArray::GetParamType(m_crSubCommand[i]->m_Input,&inputParam);

		if (inputParam[0]==OBJ_POINTS && inputParam.GetSize()==1) 
		{
			if (bTest)
			{
				CStringArray arr;
				GetObjCont()->DoCommand(m_crCommandName[0],m_crObj,arr,NULL,NULL);
				m_crObj.RemoveAt(m_crObj.GetSize()-1);
			}
			GetObjCont()->Calc();
			GetObjCont()->Draw();
		}
		else if (m_crObj.GetSize()==inputParam.GetSize())
		{
			CArray<int> inputParam;
			CCommandArray::GetParamType(m_crSubCommand[i]->m_Input,&inputParam);
			BOOL kt=TRUE;
			for (int j=0;j<m_crObj.GetSize();j++)
				if (m_crObj[j]->m_Type!=inputParam[j] && 
					!((IsPointType(m_crObj[j]->m_Type)) && (IsPointType(inputParam[j]))) &&
					!((IsLineType(m_crObj[j]->m_Type)) && inputParam[j]==OBJ_LINE))
				{ kt=FALSE; break; }

			if (kt) // Thực hiện câu lệnh
			{
				if (m_crCommandName[i]!=L"Point")
				{
					CStringArray arr;
					CArray<CVGObject*> returnArr;
					GetObjCont()->DoCommand(m_crCommandName[i],m_crObj,arr,&returnArr,NULL);
					if (!bTest) GetObjCont()->SelectAll(FALSE);
					for (int i=0;i<returnArr.GetSize();i++)
						returnArr[i]->m_Select=TRUE;
				}

// 				m_pCommandBar->m_Edit.SetWindowText(L"");
// 				CString str=m_crCommandName[i]+L"(";
// 				for (int j=0;j<m_crObj.GetSize();j++)
// 				{
// 					str+=m_crObj[j]->m_Name + (j==m_crObj.GetSize()-1 ? L"" : L", ");
// 				}
// 				str+=L")";
// 				m_pCommandBar->AddCommand(str);

				GetObjCont()->Calc();
				GetObjCont()->Draw();
				if (!bTest) ResetSelection();
				else m_crObj.RemoveAt(m_crObj.GetSize()-1);
				return;
			}
		}
	}

// 	if (m_crCommandName[0]==L"RegularPolygon" && m_crObj.GetSize()==2)
// 	{
// 		CInputDlg dlg(L"Regular Polygon");
// 		CString strValue;
// 		dlg.AddStatic(L"Nhập số nguyên");
// 		dlg.AddEdit(&strValue,WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | ES_NUMBER);
// 		if (dlg.DoModal()==IDOK)
// 		{
// 			if (_wtoi(strValue)>2)
// 			{
// 				int start=GetObjCont()->GetSize();
// 				GetObjCont()->AddRegularPolygon((CVGPoint*)m_crObj[0],(CVGPoint*)m_crObj[1],_wtoi(strValue));
// 				CArray<int> arr;
// 				for (int i=start;i<GetObjCont()->GetSize();i++)
// 					arr.Add(i);
// 				GetObjCont()->OnAddObjs(&arr);
// 				m_wndConstructionSteps->AddCommand(L"Polygon("+((CVGPoint*)m_crObj[0])->m_Name+L", "+((CVGPoint*)m_crObj[1])->m_Name+L", "+strValue+L")");
// 				m_crObj.RemoveAll();
// 			}
// 		}
// 	}
}

void CVGeoDrawerDoc::AddExpression()
{
	CInputDlg dlg(L"Input");
	CString strValue;
	dlg.AddStatic(L"Nhập biểu thức");
	dlg.AddEdit(&strValue);
NhapBieuThuc:
	if (dlg.DoModal()==IDOK)
	{
		CVGObject* obj=GetObjCont()->GetObjByName(strValue);
		if (obj!=NULL && obj->m_Type==OBJ_EXPRESSION)
			AddCrObj(obj);
		else
		{
			CVGObject* exp=GetObjCont()->AddExpression(strValue);
			if (exp) AddCrObj(exp);
			else 
			{
				(AfxGetApp()->DoMessageBox(L"Biểu thức không hợp lệ",MB_OK | MB_ICONERROR,0));
				dlg.DestroyWindow();
				goto NhapBieuThuc;
			}
		}
	}
	else ResetSelection();
}

void CVGeoDrawerDoc::OnViewStandardView()
{
// 	m_AxisInfo=CAxisInfo();
// 	m_AxisInfo.Unit=1;
// 	m_AxisInfo.Pixel=50;
// 	m_AxisInfo.m_originPoint=Point(300,300);
// 	pView->DrawBackground();
// 	pView->Draw();
}

void CVGeoDrawerDoc::OnObjectTraceobject()
{
	int i=GetObjCont()->GetSelection();
	if (i!=-1)
	{
		CVGGeoObject* obj=(CVGGeoObject*)GetObjCont()->GetAt(i);
		if (obj!=NULL) obj->m_bTrace=!obj->m_bTrace;
	}
}

void CVGeoDrawerDoc::OnUpdateObjectTraceobject(CCmdUI *pCmdUI)
{
	int i=GetObjCont()->GetSelection();
	if (i!=-1)
	{
		CVGGeoObject* obj=(CVGGeoObject*)GetObjCont()->GetAt(i);
		if (obj!=NULL) pCmdUI->SetCheck(obj->m_bTrace);
	}
}

void CVGeoDrawerDoc::OnObjectAnimateobject()
{
	int i=GetObjCont()->GetSelection();
	if (i!=-1)
	{
		CVGGeoObject* obj=(CVGGeoObject*)GetObjCont()->GetAt(i);
		if (obj!=NULL)
		{
			obj->m_bAnimate=!obj->m_bAnimate;
			pView->OnStartAnimate();
			if (obj->m_bAnimate)
				if (!m_wndMotionController->IsWindowVisible()) 
				{
					((CFrameWnd*)AfxGetMainWnd())->SendMessage(WM_COMMAND, ID_VIEW_MOTION_CONTROLLER);
					//m_wndMotionController->OnUpdateCmdUI()
				}
		}
	}
}

void CVGeoDrawerDoc::OnUpdateObjectAnimateobject(CCmdUI *pCmdUI)
{
	int i=GetObjCont()->GetSelection();
	if (i!=-1)
	{
		CVGGeoObject* obj=(CVGGeoObject*)GetObjCont()->GetAt(i);
		if (obj!=NULL) pCmdUI->SetCheck(obj->m_bAnimate);
		//pCmdUI->Enable();
	}
}

void CVGeoDrawerDoc::OnEditCopy()
{
	// TODO: Add your command handler code here
}

void CVGeoDrawerDoc::OnEditCut()
{
	AfxGetApp()->DoMessageBox(Math::IntToString(GetObjCont()->GetSize()),MB_OK,-1);
	for (int i=0;i<GetObjCont()->GetSize();i++)
		AfxGetApp()->DoMessageBox(Math::IntToString(GetObjCont()->GetAt(i)->m_Type),MB_OK,-1);
}

void CVGeoDrawerDoc::OnEditDelete()
{
	GetObjCont()->DeleteSelection();
	pView->Draw();
	GetObjCont()->m_pObjTree->FillList();
	m_wndProperties->RefreshProperties();
	m_wndConstructionSteps->Reload();
	SetModifiedFlag();
}

void CVGeoDrawerDoc::OnEditPaste()
{
	// TODO: Add your command handler code here
}

void CVGeoDrawerDoc::OnExportImage()
{
	CFileDialog dlg(FALSE,L".bmp",NULL,OFN_OVERWRITEPROMPT,
		L"Bitmap (*.bmp)|*.bmp|All Files (*.*)|*.*||", NULL,0);
	if (dlg.DoModal()==IDOK)
	{
		CString path=dlg.GetOFN().lpstrFile;
		CRect r;
		pView->GetClientRect(r);
		Bitmap bmp(m_bHasPadSize ? m_Width : r.Width(),
			m_bHasPadSize ? m_Height : r.Height(),
			PixelFormat32bppARGB);
		pView->DrawToBitmap(&bmp);
		CLSID clsid;
		GetEncoderClsid(L"image/bmp",&clsid);
		bmp.Save(path,&clsid,NULL);
	}
}

BOOL CVGeoDrawerDoc::SaveModified()
{
	if (IsModified())
	{
		int nPrompt=AfxGetApp()->DoMessageBox(L"Bạn có muốn lưu các thay đổi không ?", MB_YESNOCANCEL | MB_ICONASTERISK, -1);
		if (nPrompt==IDYES)
		{
			SetModifiedFlag(FALSE);
			OnSaveDocument(GetPathName());
			return TRUE;
		}
		else if (nPrompt==IDNO) { return TRUE; SetModifiedFlag(FALSE); }
		else return FALSE;
	}
	else return TRUE;
}

void CVGeoDrawerDoc::OnEditDeleteTrace()
{
	pView->DrawBackground();
	pView->Draw();
}

void CVGeoDrawerDoc::SaveDefault()
{
	CFile file;
	file.Open(GetAppDirectory()+L"\\data\\settings.dat",CFile::modeWrite | CFile::modeCreate);
	CArchive ar(&file,CArchive::store);
	SerializeSettings(ar);
	ar.Close();
	file.Close();
}

void CVGeoDrawerDoc::SerializeSettings(CArchive& ar)
{
	if (ar.IsStoring()) // Luu
	{
		ar << numRound;
		ar << bRadian;
		
		ar << GetObjCont()->m_graphAxis.m_bOx;
		ar << GetObjCont()->m_graphAxis.m_bOy;
		ar << GetObjCont()->m_graphAxis.m_bShowGrid;
		ar << GetObjCont()->m_graphAxis.m_bShowNumber;
		ar << GetObjCont()->m_graphAxis.m_GridDashStyle;

		ar << m_bHasPadSize;
		ar << m_Height;
		ar << m_Width;
	}
	else
	{
		ar >> numRound;
		ar >> bRadian;
		
		ar >> m_defaultGraphAxis.m_bOx;
		ar >> m_defaultGraphAxis.m_bOy;
		ar >> m_defaultGraphAxis.m_bShowGrid;
		ar >> m_defaultGraphAxis.m_bShowNumber;
		ar >> m_defaultGraphAxis.m_GridDashStyle;

		ar >> m_bHasPadSize;
		ar >> m_Height;
		ar >> m_Width;
	}

	m_defaultValue.Serialize(ar);
}

void CVGeoDrawerDoc::LoadDefaultSettings()
{
	CFile file;
	file.Open(GetAppDirectory()+L"\\data\\settings.dat",CFile::modeRead);
	CArchive ar(&file,CArchive::load);
	SerializeSettings(ar);
	ar.Close();
	file.Close();
}

void CVGeoDrawerDoc::OnViewShowAllObjects()
{
	for (int i=0;i<GetObjCont()->GetSize();i++)
	{
		if (!GetObjCont()->GetAt(i)->m_bVisible && !GetObjCont()->GetAt(i)->m_bTemp)
		{
			(GetObjCont()->GetAt(i))->m_bVisible=TRUE;
		}
	}
	pView->Draw();
}

void CVGeoDrawerDoc::OnEditRedo()
{
	GetObjCont()->Redo();
	m_wndConstructionSteps->Reload();
	GetObjCont()->m_pObjTree->FillList();
}

void CVGeoDrawerDoc::OnUpdateEditRedo(CCmdUI *pCmdUI)
{
	if (m_ObjContainerArr.GetSize()==0) pCmdUI->Enable(FALSE);
	else pCmdUI->Enable(GetObjCont()->CanRedo());
}

void CVGeoDrawerDoc::OnEditUndo()
{
	GetObjCont()->Undo();
	m_wndConstructionSteps->Reload();
	GetObjCont()->m_pObjTree->FillList();
}

void CVGeoDrawerDoc::OnUpdateEditUndo(CCmdUI *pCmdUI)
{
	if (m_ObjContainerArr.GetSize()==0) pCmdUI->Enable(FALSE);
	else pCmdUI->Enable(GetObjCont()->CanUndo());
}

void CVGeoDrawerDoc::OnEditHideselection()
{
	CArray<CVGObject*> arr;
	for (int i=0;i<GetObjCont()->GetSize();i++)
		if (GetObjCont()->GetAt(i)->m_Select)
		{
			arr.Add(GetObjCont()->GetAt(i));
		}
	
	GetObjCont()->OnChangeProperties(&arr);
	
	for (int i=0;i<arr.GetSize();i++)
		arr[i]->m_bVisible=FALSE;

	pView->Draw();
}

void CVGeoDrawerDoc::OnToolScript()
{
	CScriptDlg dlg;
	dlg.m_DocScriptObj=&m_ScriptObject;
	if (dlg.DoModal()==IDOK)
	{
		m_ScriptObject.Reset();
		m_ScriptObject.AddScript(dlg.m_strCode);
	}

	GetObjCont()->Calc();

	//InitScriptObject();
}

void CVGeoDrawerDoc::OnToolsOptions()
{
	COptionsDlg dlg;
	dlg.m_ObjContainer=GetObjCont();
	dlg.m_pDoc=this;
	dlg.DoModal();
}

void CVGeoDrawerDoc::OnUpdateZoom(CCmdUI *pCmdUI)
{
	if (!m_bHasPadSize) pCmdUI->Enable(FALSE);
	else 
	{
		pCmdUI->Enable(TRUE);
		
		if (pCmdUI->m_nID==ID_ZOOM50) pCmdUI->SetCheck(nZoom==50 && !m_bFitScreen);
		else if (pCmdUI->m_nID==ID_ZOOM75) pCmdUI->SetCheck(nZoom==75 && !m_bFitScreen);
		else if (pCmdUI->m_nID==ID_ZOOM100) pCmdUI->SetCheck(nZoom==100 && !m_bFitScreen);
		else if (pCmdUI->m_nID==ID_ZOOM200) pCmdUI->SetCheck(nZoom==200 && !m_bFitScreen);
		else if (pCmdUI->m_nID==ID_ZOOM_CUSTOM) pCmdUI->SetCheck(nZoom!=50 && nZoom!=75 && nZoom!=100 && nZoom!=200 && !m_bFitScreen);
		else pCmdUI->SetCheck(m_bFitScreen);
	}
}

void CVGeoDrawerDoc::OnZoom()
{
	CInputDlg dlg(L"Zoom",pView);
	dlg.AddStatic(L"Tỉ lệ (%) :");
	CString str=Math::IntToString(nZoom);
	dlg.AddEdit(&str);
	if (dlg.DoModal()==IDOK)
	{
		m_bFitScreen=false;
		nZoom=_wtoi(str);
		pView->DrawBackground();
		pView->RefreshScrollBar();
	}
}

void CVGeoDrawerDoc::OnZoom100()
{
	m_bFitScreen=false;
	nZoom=100;
	pView->DrawBackground();
	pView->RefreshScrollBar();
}

void CVGeoDrawerDoc::OnZoom200()
{
	m_bFitScreen=false;
	nZoom=200;
	pView->DrawBackground();
	pView->RefreshScrollBar();
}

void CVGeoDrawerDoc::OnZoom50()
{
	m_bFitScreen=false;
	nZoom=50;
	pView->DrawBackground();
	pView->RefreshScrollBar();
}

void CVGeoDrawerDoc::OnZoom75()
{
	m_bFitScreen=false;
	nZoom=75;
	pView->DrawBackground();
	pView->RefreshScrollBar();
}

void CVGeoDrawerDoc::OnZoomFitScreen()
{
	m_bFitScreen=true;
	pView->RefreshSize();
}

void CVGeoDrawerDoc::OnPageNext()
{
	SetObjContainer(m_nCurrentContainer+1);
}

void CVGeoDrawerDoc::OnUpdatePageNext(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_nCurrentContainer!=m_ObjContainerArr.GetSize()-1);
}

void CVGeoDrawerDoc::OnPagePrevious()
{
	SetObjContainer(m_nCurrentContainer-1);
}

void CVGeoDrawerDoc::OnUpdatePagePrevious(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_nCurrentContainer!=0);
}

CVGObjContainer* CVGeoDrawerDoc::GetObjCont()
{
	if (m_ObjContainerArr.GetSize()==0) return NULL;
	return m_ObjContainerArr[m_nCurrentContainer];
}

void CVGeoDrawerDoc::OnPageNewPage()
{
	AddObjContainer();
}

void CVGeoDrawerDoc::OnPageDeletePage()
{
	if (AfxGetApp()->DoMessageBox(L"Bạn có chắc chắn muốn xóa trang này không ?",MB_YESNO | MB_ICONQUESTION,-1)==IDYES)
	{
		delete m_ObjContainerArr[m_nCurrentContainer];
		m_ObjContainerArr.RemoveAt(m_nCurrentContainer);
		if (m_nCurrentContainer==m_ObjContainerArr.GetSize())
			SetObjContainer(m_nCurrentContainer-1);
		else
			SetObjContainer(m_nCurrentContainer);
		
		m_cbPage->ResetContent();
		for (int i=0;i<m_ObjContainerArr.GetSize();i++)
			m_cbPage->AddString(Math::DoubleToString(i+1)+L" / "+Math::DoubleToString(m_ObjContainerArr.GetSize()));
		m_cbPage->SetCurSel(m_nCurrentContainer);
	}
}

void CVGeoDrawerDoc::OnToolsEvent()
{
	CEventDlg dlg;
	dlg.m_objCont=GetObjCont();
	dlg.DoModal();
}

void CVGeoDrawerDoc::OnObjectProperties()
{
	if (!m_wndProperties->IsWindowVisible())
	{
		((CFrameWnd*)AfxGetMainWnd())->ShowControlBar(m_wndProperties,TRUE,FALSE);
	}
	m_wndProperties->RefreshProperties();
}

void CVGeoDrawerDoc::OnWatchwndAddwatch()
{
	CInputDlg dlg(L"Add Watch");
	dlg.AddStatic(L"Biểu thức :");
	CString strExp;
	dlg.AddEdit(&strExp);
	if (dlg.DoModal()==IDOK)
	{
		GetObjCont()->AddWatch(strExp);

		m_wndWatchBar->AddWatch();
	}
}

void CVGeoDrawerDoc::OnToolPresentMode()
{
	CPresSetting dlg;
	dlg.m_chkPresWhenStart=(m_presWhenStart);
	dlg.m_chkAskWhenClose=(m_askWhenClose);
	if (dlg.DoModal()==IDOK)
	{
		m_presWhenStart=dlg.m_chkPresWhenStart;
		m_askWhenClose=dlg.m_chkAskWhenClose;
	}
}

void CVGeoDrawerDoc::OnUpdatePageDeletePage(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_ObjContainerArr.GetSize()!=1);
}

void CVGeoDrawerDoc::OnViewDesignmode()
{
	m_bDesignMode=!m_bDesignMode;
	pView->Draw();
}

void CVGeoDrawerDoc::OnUpdateViewDesignmode(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bDesignMode);
}
