// VGeoDrawerView.h : interface of the CVGeoDrawerView class
//

#pragma once
#include "resource.h"
#include "gdi+.h"
#include "AxisInfo.h"
#include "Script\ScriptObject.h"
#include "BCMenu.h"

#define MODE_MOVE 0
#define MODE_SELECT_OBJECT 1

#define VIEW_DISTANCE 0

class CVGeoDrawerDoc;
extern Size drPadSize;

class CVGeoDrawerView : public CScrollView
{
protected: // create from serialization only
	CVGeoDrawerView();
	DECLARE_DYNCREATE(CVGeoDrawerView)

// Attributes

private:
	enum BtdMode{ 
		BTD_MODE_NONE=0,
		BTD_MODE_SELECT, 
		BTD_MODE_DRAG_SELECTION, 
		BTD_MODE_DRAG_DRAWING_PAD, 
		BTD_MODE_DRAG_SELECTION_CTRL, 
		BTD_MODE_DRAG_LABEL, 
		BTD_MODE_RESIZE_OBJ,
		BTD_MODE_BUTTON_MOUSE_DOWN,
		BTD_MODE_NEW_BOUND_ITEM,
		BTD_MODE_LBUTTON_SLIDER
	};

	Point m_lPoint;
	Point m_PrevPt;
	bool m_bCursor;
	Point m_beginPoint;
	CVGObject* m_objLabelIndex; // Thứ tự của object đc drag
	CVGObject* objMouseOver1; CVGObject* objMouseOver2;
	CVGBound* m_objBoundResizing;
	int m_boundState;
	HCURSOR m_currentCursor;
	ObjType m_newBoundObjType;
public:
	bool m_bTestPointMoving;
	int m_Mode;
	BtdMode m_btdMode;
	Point m_SelectionRightBottom;
	Point m_SelectionTopLeft;
	void Draw();
	CAxisInfo* GetAxisInfo();
	CString strTextFollowMouse;
	CString strFixedTextFollowMouse;
	int m_Top, m_Left;

	BCMenu m_menu;
	BCMenu m_ToolMenu;
private:
	BOOL m_bMovingObj;
public:
	CVGeoDrawerDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
//	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
//	virtual void OnDraw(CDC* pDC);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CVGeoDrawerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnPaint();
public:
// 	CBitmap* bmpView;
// 	HBITMAP* hbmpView;
// 	Bitmap* BmpView;
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
public:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
public:
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnEditSelectAll();
	afx_msg void OnStartAnimate();
	afx_msg void OnUpdateStartAnimate(CCmdUI *pCmdUI);
	afx_msg void OnPauseAnimate();
	afx_msg void OnUpdatePauseAnimate(CCmdUI *pCmdUI);
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void OnTimer(UINT_PTR nIDEvent);
public:
//	afx_msg void OnPaint();
protected:
	virtual void OnDraw(CDC* pDC);
public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
public:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
protected:
//	virtual void OnActivateView(BOOL bActivate, CScrollView* pActivateView, CScrollView* pDeactiveView);
public:
	virtual void OnInitialUpdate();
	void RefreshScrollBar();
	void DrawToBitmap(Bitmap* memBitmap);
	void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	void AddToCommandList(CString str);
	void DrawBackground();
	void RefreshProperties();
	Point GetPlotPoint(CPoint point, CVGObject* except);
	void RefreshConstructionSteps();
	BOOL m_bAnimating;
public:
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
public:
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
public:
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	void SetViewCursor(LPCTSTR lpszCursorName);
	CScriptObject* GetScriptObject();
	void ResizeDrawingPad();
	Point GetPadPoint(int x, int y);
	void RefreshSize();
public:
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
public:
	afx_msg LRESULT OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu);
public:
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	void PopupMenu(int nIndex, CPoint point);
	void DrawAllBackground();
	CVGObject* GetObject(int nIndex);
	void RefreshWatch();
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // debug version in VGeoDrawerView.cpp
inline CVGeoDrawerDoc* CVGeoDrawerView::GetDocument() const
   { return reinterpret_cast<CVGeoDrawerDoc*>(m_pDocument); }
#endif

