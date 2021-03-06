#pragma once
#include <vector>

using namespace std;

#define WM_COLORCHANGED WM_USER+1

#define COLOR_SIZE 14
#define COLOR_DISTANCE 2


// CColorPalette

class CColorPicker : public CWnd
{
	DECLARE_DYNAMIC(CColorPicker)

public:
	CColorPicker();
	virtual ~CColorPicker();
	BOOL RegisterWndClass();
	vector<Color> m_Colors;
	int m_Column;
	void DrawColor();
	Color m_leftClr;
	Color m_rightClr;
	BOOL showChosenColor;
	BOOL m_bOneColor; // Chỉ chọn 1 màu
private:
	void DrawColorItem(int x, int y, Color clr, Graphics* gr);
	int GetColorIndex(CPoint pt);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};

