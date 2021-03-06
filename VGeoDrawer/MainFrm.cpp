// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "VGeoDrawer.h"
#include "appfunc.h"

#include "MainFrm.h"
#include "MathDrawDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
//	ON_COMMAND(ID_APP_ABOUT, &CMainFrame::OnAppAbout)

ON_COMMAND(ID_VIEW_INPUT_WINDOW, &CMainFrame::OnViewInputWindow)
ON_UPDATE_COMMAND_UI(ID_VIEW_INPUT_WINDOW, &CMainFrame::OnUpdateViewInputWindow)
ON_UPDATE_COMMAND_UI(ID_VIEW_FULL_SCREEN, &CMainFrame::OnUpdateViewFullscreen)
ON_COMMAND(ID_VIEW_FULLSCREEN, &CMainFrame::OnViewFullscreen)
ON_WM_GETMINMAXINFO()
ON_UPDATE_COMMAND_UI(ID_VIEW_PROPERTIESWINDOW, &CMainFrame::OnUpdateViewPropertieswindow)
ON_WM_MEASUREITEM()
ON_WM_MENUCHAR()
ON_WM_INITMENUPOPUP()
//ON_WM_DESTROY()
ON_WM_CLOSE()
ON_WM_SHOWWINDOW()
ON_COMMAND(ID_VIEW_PROPERTIESWINDOW, &CMainFrame::OnViewPropertieswindow)
ON_COMMAND(ID_VIEW_PROPERTIES_WINDOW, &CMainFrame::OnViewPropertiesWindow)
ON_UPDATE_COMMAND_UI(ID_VIEW_PROPERTIES_WINDOW, &CMainFrame::OnUpdateViewPropertiesWindow)
ON_COMMAND(ID_VIEW_COMMAND_INPUT, &CMainFrame::OnViewCommandInput)
ON_UPDATE_COMMAND_UI(ID_VIEW_COMMAND_INPUT, &CMainFrame::OnUpdateViewCommandInput)
ON_COMMAND(ID_VIEW_GEOOBJ_WINDOW, &CMainFrame::OnViewGeoobjWindow)
ON_UPDATE_COMMAND_UI(ID_VIEW_GEOOBJ_WINDOW, &CMainFrame::OnUpdateViewGeoobjWindow)
ON_COMMAND(ID_VIEW_TOOLBOX, &CMainFrame::OnViewToolbox)
ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBOX, &CMainFrame::OnUpdateViewToolbox)
ON_COMMAND(ID_OPTIONS_CUSTOMIZE_TOOLBOX, &CMainFrame::OnOptionsCustomizeToolbox)
ON_COMMAND(ID_OPTIONS_NEW_TOOL, &CMainFrame::OnOptionsNewTool)
ON_COMMAND(ID_OPTIONS_EQUATIONEDITOR, &CMainFrame::OnOptionsEquationeditor)
ON_COMMAND(ID_OBJECT_PROPERTIES, &CMainFrame::OnObjectsProperties)
ON_COMMAND(ID_VIEW_MOTION_CONTROLLER, &CMainFrame::OnViewMotionController)
ON_UPDATE_COMMAND_UI(ID_VIEW_MOTION_CONTROLLER, &CMainFrame::OnUpdateViewMotionController)
ON_COMMAND(ID_VIEW_SHOWMODE, &CMainFrame::OnViewShowmode)
ON_WM_KEYDOWN()
ON_UPDATE_COMMAND_UI(ID_VIEW_SHOWMODE, &CMainFrame::OnUpdateViewShowmode)
ON_COMMAND(ID_EXIT_FULL_MODE, &CMainFrame::OnExitFullMode)
ON_COMMAND(ID_VIEW_PAGE_VIEW_BAR, &CMainFrame::OnViewPageViewBar)
ON_UPDATE_COMMAND_UI(ID_VIEW_PAGE_VIEW_BAR, &CMainFrame::OnUpdateViewPageViewBar)
ON_COMMAND(ID_VIEW_WATCH, &CMainFrame::OnViewWatch)
ON_UPDATE_COMMAND_UI(ID_VIEW_WATCH, &CMainFrame::OnUpdateViewWatch)

ON_COMMAND_RANGE(TOOL_MENU_START,TOOL_MENU_START+100,&CMainFrame::OnToolCommand)
ON_UPDATE_COMMAND_UI_RANGE(TOOL_MENU_START,TOOL_MENU_START+100,&CMainFrame::OnUpdateToolCommand)

ON_CBN_SELCHANGE(IDC_CBPAGE, &CMainFrame::OnSelChangePage)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_MOUSE_POS,
};

extern int numRound;
extern bool bRadian;

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_bFullScreen=FALSE;
	m_bFullScreenShowMode=FALSE;
}

CMainFrame::~CMainFrame()
{
	for (int i=0;i<m_toolBarArray.GetSize();i++)
	{
		delete m_toolBarArray[i];
	}
}

CVGeoDrawerDoc* CMainFrame::GetDocument()
{
	POSITION pos=AfxGetApp()->GetFirstDocTemplatePosition();
	CDocTemplate* pDT=theApp.GetNextDocTemplate(pos);
	pos=pDT->GetFirstDocPosition();
	return (CVGeoDrawerDoc*)pDT->GetNextDoc(pos);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	pDoc=GetDocument();
	pDoc->pView->m_ToolMenu.CreateMenu();
	//pDoc->pView->m_ToolMenu.LoadMenu(IDR_MAINFRAME);
	pDoc->m_StatusBar=&m_wndStatusBar;
	pDoc->m_cbPage=&m_cbPage;

	m_Font.CreatePointFont(80,L"Tahoma",GetDC());
	
	m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.LoadToolBar(IDR_MAINFRAME);

	m_wndPageTBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY,CRect(0,0,0,0),IDR_TBAR_PAGE);
	m_wndPageTBar.LoadToolBar(IDR_TOOLBAR_PAGE);
	m_wndPageTBar.SetButtonInfo(4, IDC_CBPAGE, TBBS_SEPARATOR, 50);
	CRect rect;
	m_wndPageTBar.GetItemRect(4,&rect);
	rect.bottom = rect.top+125;
	m_cbPage.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST, rect, &m_wndPageTBar, IDC_CBPAGE);
	m_cbPage.SetFont(&m_Font);

	CRect temp;
	m_wndToolBar.GetItemRect(0,&temp);
	m_wndToolBar.SetSizes(CSize(temp.Width(),temp.Height()),CSize(16,16));

	m_wndMotionController.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY,CRect(0,0,0,0),IDR_MOTION);
	m_wndMotionController.LoadToolBar(IDR_TOOLBAR_MOTION_CONTROLLER);
	pDoc->m_wndMotionController=&m_wndMotionController;

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)));
	m_wndStatusBar.GetStatusBarCtrl().SetMinHeight(22);
	pDoc->m_StatusBar=&m_wndStatusBar;

	m_wndList.Create(_T("Geometry Objects"), this, CSize(500,30),FALSE, IDR_OBJLIST);
	m_wndList.SetBarStyle(m_wndList.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	pDoc->m_wndList=&m_wndList;

	m_wndPageViewBar.m_PageListBox.m_PadHeight=&pDoc->m_Height;
	m_wndPageViewBar.m_PageListBox.m_PadWidth=&pDoc->m_Width;
	m_wndPageViewBar.Create(_T("Pages View"), this, CSize(500,30),FALSE, IDR_PAGE_VIEW);
	m_wndPageViewBar.SetBarStyle(m_wndPageViewBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	pDoc->m_wndPageList=&m_wndPageViewBar.m_PageListBox;
	m_wndPageViewBar.m_PageListBox.m_contArr=&pDoc->m_ObjContainerArr;

	m_wndCommand.Create(_T("Command"), this, CSize(500,30),FALSE, IDR_COMMAND);
	m_wndCommand.SetBarStyle(m_wndCommand.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	pDoc->m_wndCommand=&m_wndCommand;
	m_wndCommand.m_List.m_Edit.m_cmdArray=&pDoc->m_cmdArray;

	m_wndWatchBar.Create(_T("Watch"), this, CSize(500,30),FALSE, IDR_WATCH_BAR);
	m_wndWatchBar.SetBarStyle(m_wndWatchBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	pDoc->m_wndWatchBar=&m_wndWatchBar;

	m_wndConstructionSteps.Create(_T("Construction Steps"), this, CSize(500,30),FALSE, IDR_CONSTRUCTIONSTEPS);
	m_wndConstructionSteps.SetBarStyle(m_wndCommand.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	pDoc->m_wndConstructionSteps=&m_wndConstructionSteps;

	m_wndObjToolBar.pDoc=pDoc;
	m_wndObjToolBar.Create(_T("Toolbox"), this, CSize(500,30),FALSE, IDR_OBJTOOLBAR, WS_CHILD | WS_VISIBLE | CBRS_TOP | WS_CLIPCHILDREN);
	m_wndObjToolBar.SetBarStyle(m_wndCommand.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | /*CBRS_SIZE_DYNAMIC*/ CBRS_SIZE_FIXED);
	pDoc->m_wndObjToolBar=&m_wndObjToolBar;
	m_wndObjToolBar.m_Menu=&pDoc->pView->m_ToolMenu;

	m_wndProperties.pDoc=pDoc;
	m_wndProperties.Create(_T("Properties"), this, CSize(220,150), FALSE, IDR_PROPERTIES);
	m_wndProperties.SetBarStyle(m_wndCommand.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	pDoc->m_wndProperties=&m_wndProperties;

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndMotionController.EnableDocking(CBRS_ALIGN_ANY);
	m_wndPageTBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndObjToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndCommand.EnableDocking(CBRS_ALIGN_BOTTOM | CBRS_ALIGN_TOP);
	m_wndConstructionSteps.EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
	m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
	m_wndList.EnableDocking(CBRS_ALIGN_ANY);
	m_wndPageViewBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndWatchBar.EnableDocking(CBRS_ALIGN_ANY);
	
	EnableDocking(CBRS_ALIGN_ANY);

	DockControlBar(&m_wndToolBar);
	DockControlBar(&m_wndMotionController);
	DockControlBar(&m_wndPageTBar);
	DockControlBar(&m_wndObjToolBar,AFX_IDW_DOCKBAR_LEFT);
	DockControlBar(&m_wndCommand,AFX_IDW_DOCKBAR_BOTTOM);
	DockControlBar(&m_wndConstructionSteps,AFX_IDW_DOCKBAR_BOTTOM);
	DockControlBar(&m_wndProperties,AFX_IDW_DOCKBAR_LEFT);
	DockControlBar(&m_wndList,AFX_IDW_DOCKBAR_LEFT);
	DockControlBar(&m_wndPageViewBar,AFX_IDW_DOCKBAR_LEFT);
	DockControlBar(&m_wndWatchBar,AFX_IDW_DOCKBAR_BOTTOM);

	LoadState();

	//////////////////////////////////////////////////////////////////////////
	// Full Screen

	HDC hDC = ::GetDC(NULL);
	int XSpan = GetDeviceCaps(hDC, HORZRES);
	int YSpan = GetDeviceCaps(hDC, VERTRES);
	::ReleaseDC(NULL, hDC);

	// Calculates size of window elements
	int XBorder = GetSystemMetrics(SM_CXFRAME);
	int YCaption = GetSystemMetrics(SM_CYCAPTION);
	int YMenu = GetSystemMetrics(SM_CYMENU);
	int YBorder = GetSystemMetrics(SM_CYFRAME);

	// Calculates window origin and span for full screen mode
	CPoint Origin = CPoint(-XBorder, -YBorder - YCaption - YMenu);
	XSpan += 2 * XBorder, YSpan += 2 * YBorder + YCaption + YMenu;


	// Calculates full screen window rectangle
	rectFull = CRect(Origin, CSize(XSpan+1, YSpan+1));

	//////////////////////////////////////////////////////////////////////////

	

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	//Comment out if do not want fullscreen

	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers
void CMainFrame::OnViewInputWindow()
{
	ShowControlBar(&m_wndCommand,!m_wndCommand.IsVisible(),FALSE);
}

void CMainFrame::OnUpdateViewInputWindow(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndCommand.IsVisible());
}

void CMainFrame::OnUpdateViewFullscreen(CCmdUI *pCmdUI)
{
	if (m_bFullScreenShowMode) pCmdUI->Enable(FALSE);
	else
	{
		pCmdUI->Enable(TRUE);
		WINDOWPLACEMENT wp;
		GetWindowPlacement(&wp);
		if (m_bFullScreen && wp.showCmd==SW_SHOWNORMAL)	m_bFullScreen=FALSE;
		pCmdUI->SetCheck(m_bFullScreen);
	}
}

void CMainFrame::OnUpdateViewShowmode(CCmdUI *pCmdUI)
{
	if (m_bFullScreen) pCmdUI->Enable(FALSE);
	else { pCmdUI->Enable(TRUE); pCmdUI->SetCheck(m_bFullScreenShowMode); }
}

void CMainFrame::OnViewShowmode()
{
	m_bFullScreenShowMode=!m_bFullScreenShowMode;
	static int bHasPadSize=FALSE;
	static int nZoom;
	static int bFit;
	if (m_bFullScreenShowMode)
	{
		SetRedraw(FALSE);
		bHasPadSize=pDoc->m_bHasPadSize;
		bFit=pDoc->m_bFitScreen;

		bFit=true;
		//pDoc->m_bHasPadSize=FALSE;
		//Save State
		m_bFullScreenShowMode=false;
		SaveState();
		m_bFullScreenShowMode=true;
		//Full screen
		//this->ShowWindow(SW_MINIMIZE);
		this->ShowWindow(SW_SHOWMAXIMIZED);

		ShowControlBar(&m_wndCommand,FALSE,FALSE);
		ShowControlBar(&m_wndList,FALSE,FALSE);
		ShowControlBar(&m_wndProperties,FALSE,FALSE);
		ShowControlBar(&m_wndToolBar,FALSE,FALSE);
		ShowControlBar(&m_wndMotionController,FALSE,FALSE);
		ShowControlBar(&m_wndStatusBar,FALSE,FALSE);
		ShowControlBar(&m_wndConstructionSteps,FALSE,FALSE);
		ShowControlBar(&m_wndObjToolBar,FALSE,FALSE);
		ShowControlBar(&m_wndPageTBar,FALSE,FALSE);
		ShowControlBar(&m_wndWatchBar,FALSE,FALSE);
		ShowControlBar(&m_wndPageViewBar,FALSE,FALSE);
		
		SetRedraw(TRUE);
		RedrawWindow();
	}
	else
	{
		//pDoc->m_bHasPadSize=bHasPadSize;
		pDoc->m_bFitScreen=bFit;
		pDoc->m_bHasPadSize=bHasPadSize;
		this->ShowWindow(SW_MINIMIZE);
		//Load State
		LoadState();
	}
}

void CMainFrame::OnViewFullscreen()
{
	m_bFullScreen=!m_bFullScreen;
	if (m_bFullScreen)
	{
		WINDOWPLACEMENT wp;
		GetWindowPlacement(&wp);
		AfxGetApp()->WriteProfileBinary(L"WP",L"WP",(LPBYTE)&wp,sizeof(wp));
		//Full screen
		//this->ShowWindow(SW_MINIMIZE);
		this->ShowWindow(SW_SHOWMAXIMIZED);

		RedrawWindow();
		//GetDocument()->pView->Draw();
	}
	else
	{
		this->ShowWindow(SW_MINIMIZE);

		WINDOWPLACEMENT *lwp;
		UINT nl;

		if(AfxGetApp()->GetProfileBinary(_T("WP"), _T("WP"), (LPBYTE*)&lwp, &nl))
		{
			SetWindowPlacement(lwp);
			delete [] lwp;
		}
	}
}

void CMainFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	if (m_bFullScreen || m_bFullScreenShowMode)
	{
		lpMMI->ptMaxPosition = rectFull.TopLeft();
		lpMMI->ptMaxTrackSize = lpMMI->ptMaxSize = CPoint(rectFull.Size());
	}

	CFrameWnd::OnGetMinMaxInfo(lpMMI);
}

void CMainFrame::OnUpdateViewPropertieswindow(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndProperties.IsWindowVisible());
}

HMENU CMainFrame::NewMenu()
{
	m_menu.LoadMenu(IDR_MAINFRAME);  
	m_menu.LoadToolbar(IDR_MAINFRAME);
	m_menu.LoadToolbar(IDR_TOOLBAR_MOTION_CONTROLLER);
	m_menu.LoadToolbar(IDR_MENU_ICON);

	return(m_menu.Detach());
}

void CMainFrame::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	BOOL setflag=FALSE;
	if(lpMeasureItemStruct->CtlType==ODT_MENU)
	{
		if(IsMenu((HMENU)lpMeasureItemStruct->itemID))
		{
			CMenu* cmenu = CMenu::FromHandle((HMENU)lpMeasureItemStruct->itemID);

			if (m_menu.IsMenu(cmenu))
			{
				m_menu.MeasureItem(lpMeasureItemStruct);
				setflag=TRUE;
			}
		}
	}

	if(!setflag)CFrameWnd::OnMeasureItem(nIDCtl, 
		lpMeasureItemStruct);
}

LRESULT CMainFrame::OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu)
{
	LRESULT lresult;
	if(BCMenu::IsMenu(pMenu))
		lresult=BCMenu::FindKeyboardShortcut(nChar, nFlags, pMenu);
	else
		lresult=CMainFrame::OnMenuChar(nChar, nFlags, pMenu);
	return(lresult);
}

void CMainFrame::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CFrameWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	if(!bSysMenu)
	{
		if(BCMenu::IsMenu(pPopupMenu))
			BCMenu::UpdateMenu(pPopupMenu);
	}
}

void CMainFrame::OnClose()
{
	SaveState();

	CFrameWnd::OnClose();
}

void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CFrameWnd::OnShowWindow(bShow, nStatus);
}

void CMainFrame::OnViewPropertieswindow()
{
	ShowControlBar(&m_wndProperties,!m_wndProperties.IsVisible(),FALSE);
}

void CMainFrame::SaveState(void)
{
	if (m_bFullScreenShowMode) return;
	CString mainfrm, barstate;

	mainfrm=L"MainFrmGeo";
	barstate=L"DockBarGeo";

	WINDOWPLACEMENT wp;
	GetWindowPlacement(&wp);
	AfxGetApp()->WriteProfileBinary(L"WP",L"WP",(LPBYTE)&wp,sizeof(wp));

	SaveBarState(mainfrm);
	CSizingControlBar::GlobalSaveState(this,barstate);

	BOOL b=m_wndStatusBar.IsWindowVisible();
	AfxGetApp()->WriteProfileInt(barstate,L"StatusBar",m_wndStatusBar.IsWindowVisible());
}

void CMainFrame::LoadState(void)
{
	CString mainfrm, barstate;

	mainfrm=L"MainFrmGeo";
	barstate=L"DockBarGeo";

	WINDOWPLACEMENT *lwp;
	UINT nl;

	if(AfxGetApp()->GetProfileBinary(_T("WP"), _T("WP"), (LPBYTE*)&lwp, &nl))
	{
		SetWindowPlacement(lwp);
		delete [] lwp;
	}

	LoadBarState(mainfrm);
	CSizingControlBar::GlobalLoadState(this,barstate);
	
	m_wndStatusBar.ShowWindow(AfxGetApp()->GetProfileInt(barstate,L"StatusBar",1) ? SW_SHOW : SW_HIDE);
}

void CMainFrame::OnViewPropertiesWindow()
{
	ShowControlBar(&m_wndProperties,!m_wndProperties.IsWindowVisible(),FALSE);
}

void CMainFrame::OnUpdateViewPropertiesWindow(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndProperties.IsWindowVisible());
}

void CMainFrame::OnViewCommandInput()
{
	ShowControlBar(&m_wndCommand,!m_wndCommand.IsWindowVisible(),FALSE);
}

void CMainFrame::OnUpdateViewCommandInput(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndCommand.IsWindowVisible());
}

void CMainFrame::OnViewGeoobjWindow()
{
	ShowControlBar(&m_wndList,!m_wndList.IsWindowVisible(),FALSE);
}

void CMainFrame::OnUpdateViewGeoobjWindow(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndList.IsWindowVisible());
}
BOOL CMainFrame::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
#define lpnm   ((LPNMHDR)lParam)
#define lpnmTB ((LPNMTOOLBAR)lParam)

	switch(lpnm->code)
	{
	case TBN_DROPDOWN:
		{
			//this->CloseWindow();
 			//Get the coordinates of the button.
			
		}
	}

	return CFrameWnd::OnNotify(wParam, lParam, pResult);
}

void CMainFrame::OnViewToolbox()
{
	ShowControlBar(&m_wndObjToolBar,!m_wndObjToolBar.IsVisible(),FALSE);
}

void CMainFrame::OnUpdateViewToolbox(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndObjToolBar.IsVisible());
}

void CMainFrame::OnViewWatch()
{
	ShowControlBar(&m_wndWatchBar,!m_wndWatchBar.IsVisible(),FALSE);
}

void CMainFrame::OnUpdateViewWatch(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndWatchBar.IsVisible());
}


void CMainFrame::OnViewPageViewBar()
{
	ShowControlBar(&m_wndPageViewBar,!m_wndPageViewBar.IsVisible(),FALSE);
}

void CMainFrame::OnUpdateViewPageViewBar(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndPageViewBar.IsVisible());
}


void CMainFrame::OnOptionsCustomizeToolbox()
{
	CCustomizeToolboxDlg dlg;
	if (dlg.DoModal()==IDOK)
	{
		m_wndObjToolBar.LoadButtons();
		GetDocument()->m_cmdArray.LoadCommands();
		GetDocument()->SetToolMode(L"Con trỏ");
	}
}

void CMainFrame::OnOptionsNewTool()
{
	CAddToolDlg dlg;
	if (dlg.DoModal()==IDOK);
}

void CMainFrame::OnOptionsEquationeditor()
{
	CMathDrawDlg dlg;
	dlg.DoModal();
}

void CMainFrame::OnObjectsProperties()
{
	if (!m_wndProperties.IsWindowVisible())
	{
		ShowControlBar(&m_wndProperties,TRUE,FALSE);
		m_wndProperties.RefreshProperties();
	}
}

void CMainFrame::OnViewMotionController()
{
	ShowControlBar(&m_wndMotionController,!m_wndMotionController.IsWindowVisible(),FALSE);
}

void CMainFrame::OnUpdateViewMotionController(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndMotionController.IsVisible());
}

void CMainFrame::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar==VK_ESCAPE)
	{
		if (m_bFullScreenShowMode) OnViewShowmode();
		if (m_bFullScreen) OnViewFullscreen();
	}

	CFrameWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}
void CMainFrame::OnExitFullMode()
{
	if (m_bFullScreen) OnViewFullscreen();
	if (m_bFullScreenShowMode) OnViewShowmode();
}

void CMainFrame::OnToolCommand(UINT nIndex)
{
	m_wndObjToolBar.OnCommand(nIndex);
}

void CMainFrame::OnUpdateToolCommand(CCmdUI* pCmdUI)
{
	m_wndObjToolBar.OnUpdateCommand(pCmdUI->m_nIndex,pCmdUI);
}

void CMainFrame::OnSelChangePage()
{
	pDoc->SetObjContainer(m_cbPage.GetCurSel());
}