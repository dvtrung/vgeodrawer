#pragma once
#include "gdi+.h"
#include <vector>

using namespace std;

#define SubFontSize(fs) (fs>=8 ? fs-2 : fs)

class CMathDraw
{
private:
	class MDrawingSurface
	{
	public:
		MDrawingSurface(Font* font);
		void AddLine(Point pt1, Point pt2);
		void AddDr(MDrawingSurface* gr);
		void Translate(int x, int y);
		void Draw(Graphics* gr, Color clr);
		Rect GetBounds(Graphics* gr);
		void AddString(CString str, PointF pt, int fontSize, bool bItalic=FALSE);
		void AddSymbol(CString str, PointF pt, int fontSize);
		void DrawBracket(Point pt,int width,int height,Graphics* gr,Color clr) /* pt2 co toa do Y nam giua pt1 va pt3 */;
		void AddBracket(Point pt,int brWidth, int brHeight);
		//DrawBracket(Point pt1,Point pt2,Point pt3,Graphics* gr) /* pt2 co toa do Y nam giua pt1 va pt3 */;
		//void AddChar(TCHAR str, PointF pt, int fontSize);
		Font* m_Font;
	private:	
		vector<Point> m_Pt1;
		vector<Point> m_Pt2;

		vector<Point> m_BrPt;
		vector<int> m_BrHeight;
		vector<int> m_BrWidth;

		//vector<CString> m_String;
		vector<CString> m_String;
		vector<PointF> m_StringPos; // Vi chi cua ki tu khi in ra man hinh
		vector<bool> m_StringItalic;
		vector<int> m_FontSize;

		vector<CString> m_Symbol;
		vector<PointF> m_SymbolPos;
		vector<int>m_SymbolFontSize;
		vector<MDrawingSurface> m_Dr;
	};
public:
	CMathDraw();
	~CMathDraw();
	static Bitmap* DrawExpression(CString strExp, Color clr, Font* font);
	static int DrawExp(CString strExp, Font* font, int fontSize, MDrawingSurface* gr, Graphics* graph, BOOL bFormula=TRUE);
	static int GetNextExp(CString strExp);
	static int DrawExpLatex(CString strExp, CString* fontName, int fontStyle, int fontSize, MDrawingSurface* gr, Graphics* graph, BOOL latex=TRUE);
	static Bitmap* DrawText(CString text, Color clr, Font* font);
	static Bitmap* DrawLine(CString strText, Color clr, Graphics* gr, Font* font);
	static void AddString(MDrawingSurface* gr, Font* font, int fontSize, CString& strText, int& x, Graphics* graph, bool bModeMath=false);
	static void AddSymbolString(MDrawingSurface* gr, Font* font, int fontSize, CString strText, int& x, Graphics* graph);
	static Bitmap* DrawObjectName(CString strName, Font* font);
	static const CString strFuncList;
};