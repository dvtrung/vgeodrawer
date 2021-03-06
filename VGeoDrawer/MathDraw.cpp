#include "stdafx.h"
#include "MathDraw.h"
#include "VGMath.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////////
//
// MDrawingSurface class
//
//////////////////////////////////////////////////////////////////////////

CMathDraw::MDrawingSurface::MDrawingSurface(Font* font)
{
	m_Font=font;
}

//Cần cải thiện để tăng tốc độ
void CMathDraw::MDrawingSurface::DrawBracket(Point pt,int width,int height,Graphics* gr,Color clr) // pt2 co toa do Y nam giua pt1 va pt3
{
	Pen pen(clr);
	if (width>0) gr->DrawArc(&pen,pt.X,pt.Y,2*width,height,90,180);
	else gr->DrawArc(&pen,pt.X+width,pt.Y,-2*width,height,90,-180);
// 	if (width>0)
// 	{
// 		gr->DrawArc(&pen,pt.X,pt.Y,2*width,2*width,180,90);
// 		gr->DrawArc(&pen,pt.X,pt.Y+height-2*width,2*width,2*width,90,90);
// 		gr->DrawLine(&pen,pt.X,pt.Y+width,pt.X,pt.Y+height-width);
// 	}
//  	int y=pt2.Y;
//  	int x=(pow(float(y-pt1.Y),2)+pt1.X*pt1.X-pt2.X*pt2.X)/(2*(pt1.X-pt2.X));
//  	int r=x-pt2.X;
// 	Rect rect;
//  	if (r>=0) rect=Rect(pt2.X,y-r,2*r,2*r);	
// 	else rect=Rect(pt2.X+2*r,y+r,-2*r,-2*r);
// 	REAL ang=180*acos(1-(pow(float(pt1.X-pt2.X),2)+pow(float(pt1.Y-pt2.Y),2))/(2*r*r))/pi;
// 	Pen pen(Color::Black);
// 	//gr->DrawEllipse(&pen,rect);
// 	if (pt2.X<pt1.X) gr->DrawArc(&pen,rect,(180-ang),2*ang);
// 	else gr->DrawArc(&pen,rect,ang,-2*ang);
}

void CMathDraw::MDrawingSurface::AddLine(Point pt1,Point pt2)
{
	m_Pt1.push_back(pt1);
	m_Pt2.push_back(pt2);
}

void CMathDraw::MDrawingSurface::AddBracket(Point pt,int brWidth, int brHeight)
{
	m_BrPt.push_back(pt);
	m_BrWidth.push_back(brWidth);
	m_BrHeight.push_back(brHeight);
}

// void CMathDraw::MDrawingSurface::AddString(CString str, PointF pt, int fontSize)
// {
// 	m_String.push_back(str);
// 	m_StringPos.push_back(pt);
// 	m_FontSize.push_back(fontSize);
// }

void CMathDraw::MDrawingSurface::AddString(CString str, PointF pt, int fontSize, bool bItalic)
{
	m_String.push_back(str);
	m_StringPos.push_back(pt);
	m_FontSize.push_back(fontSize);
	m_StringItalic.push_back(bItalic);
}

void CMathDraw::MDrawingSurface::AddSymbol(CString str, PointF pt, int fontSize)
{
	m_Symbol.push_back(str);
	m_SymbolPos.push_back(pt);
	m_SymbolFontSize.push_back(fontSize);
}


void CMathDraw::MDrawingSurface::AddDr(MDrawingSurface* gr)
{
	//for (int i=0;i<gr->m_Pt1.size();i++)
	//	this->AddLine(gr->m_Pt1[i],gr->m_Pt2[i]);
	//for (i=0;i<gr->m_StringPos.size();i++)
	//	this->AddChar(gr->m_String[i],gr->m_StringPos[i],gr->m_FontSize[i]);
	m_Dr.push_back(*gr);
}

void CMathDraw::MDrawingSurface::Translate(int x, int y)
{
	Point pt(x,y);
	for (int i=0;i<m_Pt1.size();i++)
	{
		m_Pt1[i]=m_Pt1[i]+pt;
		m_Pt2[i]=m_Pt2[i]+pt;
	}
	for (int i=0;i<m_BrPt.size();i++)
	{
		m_BrPt[i]=m_BrPt[i]+pt;
	}
	for (int i=0;i<m_StringPos.size();i++) m_StringPos[i]=m_StringPos[i]+PointF(pt.X,pt.Y);
	for (int i=0;i<m_SymbolPos.size();i++) m_SymbolPos[i]=m_SymbolPos[i]+PointF(pt.X,pt.Y);
	for (int i=0;i<m_Dr.size();i++) m_Dr[i].Translate(pt.X,pt.Y);
}

void CMathDraw::MDrawingSurface::Draw(Graphics* gr, Color clr)
{
	SolidBrush solidBrush(clr);
	Pen pen(clr);
	for (int i=0;i<m_Pt1.size();i++)
		gr->DrawLine(&pen,m_Pt1[i],m_Pt2[i]);
	for (int i=0;i<m_BrPt.size();i++)
	{
		DrawBracket(m_BrPt[i],m_BrWidth[i],m_BrHeight[i],gr,clr);
// 		Point ptArr[3]; ptArr[0]=m_ArcPt1[i]; ptArr[1]=m_ArcPt2[i]; ptArr[2]=m_ArcPt3[i];
// 		gr->DrawCurve(&pen,ptArr,3,1.5);
	}
	for (int i=0;i<m_StringPos.size();i++)
	{
		FontFamily family; m_Font->GetFamily(&family);
		Font font(&family,m_FontSize[i],m_StringItalic[i] ? m_Font->GetStyle() | FontStyleItalic : m_Font->GetStyle(),m_Font->GetUnit());
		gr->DrawString(m_String[i],-1,&font,m_StringPos[i],NULL,&solidBrush);
	}
	for (int i=0;i<m_SymbolPos.size();i++)
	{
		Font font(L"Symbol",m_SymbolFontSize[i],m_Font->GetStyle(),m_Font->GetUnit());
		gr->DrawString(m_Symbol[i],-1,&font,m_SymbolPos[i],NULL,&solidBrush);
	}
	for (int i=0;i<m_Dr.size();i++)
	{
		m_Dr[i].Draw(gr,clr);
	}
}

Rect CMathDraw::MDrawingSurface::GetBounds(Graphics* gr)
{
	int t/*,l*/,b,r,i;
	t=b=r=0;
	for (i=0;i<m_Pt1.size();i++)
	{
		if (t>m_Pt1[i].Y) t=m_Pt1[i].Y; //if (l>m_Pt1[i].Y) l=m_Pt1[i].Y;
		if (t>m_Pt2[i].Y) t=m_Pt2[i].Y; //if (l>m_Pt2[i].Y) l=m_Pt2[i].Y;
		if (b<m_Pt1[i].Y) b=m_Pt1[i].Y; if (r<m_Pt1[i].X) r=m_Pt1[i].X;
		if (b<m_Pt2[i].Y) b=m_Pt2[i].Y; if (r<m_Pt2[i].X) r=m_Pt2[i].X;
	}

	RectF stringRect;
	for (i=0;i<m_StringPos.size();i++)
	{
		FontFamily family; m_Font->GetFamily(&family);
		Font font(&family,m_FontSize[i],m_Font->GetStyle(),m_Font->GetUnit());
		gr->MeasureString(m_String[i],-1,&font,m_StringPos[i],&stringRect);
		if (stringRect.Y<t) t=stringRect.Y;
		if (stringRect.GetBottom()>b) b=stringRect.GetBottom();
		if (stringRect.GetRight()>r) r=stringRect.GetRight();
	}

	for (i=0;i<m_SymbolPos.size();i++)
	{
		Font font(L"Symbol",m_SymbolFontSize[i],m_Font->GetStyle(),m_Font->GetUnit());
		gr->MeasureString(m_Symbol[i],-1,&font,m_SymbolPos[i],&stringRect);
		if (stringRect.Y<t) t=stringRect.Y;
		if (stringRect.GetBottom()>b) b=stringRect.GetBottom();
		if (stringRect.GetRight()>r) r=stringRect.GetRight();
	}


	Rect rect(0,t,r,b-t);

	for (i=0;i<m_BrPt.size();i++)
	{
		Rect::Union(rect,rect,Rect(m_BrPt[i].X,m_BrPt[i].Y,abs(m_BrWidth[i]),m_BrHeight[i]));
	}

	for (i=0;i<m_Dr.size();i++)
	{
		Rect::Union(rect,rect,m_Dr[i].GetBounds(gr));
	}
	return Rect(rect.X-1,rect.Y,rect.Width,rect.Height+1);
}

//////////////////////////////////////////////////////////////////////////
//
// CMathDraw
//
//////////////////////////////////////////////////////////////////////////

CMathDraw::CMathDraw()
{

}

CMathDraw::~CMathDraw()
{

}

Bitmap* CMathDraw::DrawLine(CString strText, Color clr, Graphics* gr, Font* font)
{
	MDrawingSurface Mgr(font);
	//DrawExpLatex(strText,&fontName,fontStyle,fontSize,&Mgr,gr,FALSE);
	DrawExp(strText,font,font->GetSize(),&Mgr,gr,FALSE);
	Rect rect=Mgr.GetBounds(gr);

	Bitmap* outBitmap=new Bitmap(rect.Width,rect.Height,PixelFormat32bppARGB);
	Graphics grbmp(outBitmap);
	grbmp.SetTextRenderingHint(TextRenderingHintSingleBitPerPixelGridFit);
	//grbmp.SetSmoothingMode(SmoothingModeAntiAlias);
	Mgr.Draw(&grbmp, clr);
	return outBitmap;
}

Bitmap* CMathDraw::DrawText(CString text, Color clr, Font* font)
{
	int i=0;
	CArray<Bitmap*> m_bmpArr;
	CArray<BOOL> m_bCenter;
	Graphics gr(AfxGetMainWnd()->GetDC()->m_hDC);

	//Unit u=font->GetUnit();
	//gr.SetPageUnit(UnitPoint);

	while (i<text.GetLength())
	{
		int pos=text.Find(L"\r\n",i);
		if (pos==-1) pos=text.GetLength();
		CString str=text.Mid(i,pos-i);
		int j=0;
		BOOL modeLatexNewLine=FALSE;
		while (j<str.GetLength())
		{
			if (!modeLatexNewLine)
			{
				int pos=str.Find(L"\\[",j);
				if (pos==-1) pos=str.GetLength();
				CString strLine=str.Mid(j,pos-j);
				m_bmpArr.Add(DrawLine(strLine,clr,&gr,font));
				m_bCenter.Add(FALSE);
				modeLatexNewLine=TRUE;
				j=pos+2;
			}
			else
			{
				int pos=str.Find(L"\\]",j);
				if (pos==-1) pos=str.GetLength();
				CString strLine=str.Mid(j,pos-j);
				m_bmpArr.Add(DrawLine(L"$"+strLine+L"$",clr,&gr,font));
				m_bCenter.Add(TRUE);
				modeLatexNewLine=FALSE;
				j=pos+2;
			}
		}
		//m_bmpArr.Add(DrawLine(str,&gr));
		i=pos+2;
	}
	// DrawExpression các dòng lên Bitmap
	int h=0;
	int w=0;
	for (int i=0;i<m_bmpArr.GetSize();i++) { h+=m_bmpArr[i]->GetHeight(); w=max(w,m_bmpArr[i]->GetWidth()); }
	Bitmap* outBitmap=new Bitmap(w,h,PixelFormat32bppARGB);
	Graphics graph(outBitmap);
	graph.SetTextRenderingHint(TextRenderingHintSingleBitPerPixelGridFit);
	h=0;
	for (int i=0;i<m_bmpArr.GetSize();i++)
	{
		if (!m_bCenter[i])
			graph.DrawImage(m_bmpArr[i],0,h,m_bmpArr[i]->GetWidth(),m_bmpArr[i]->GetHeight());
		else
			graph.DrawImage(m_bmpArr[i],(w-m_bmpArr[i]->GetWidth())/2,h,m_bmpArr[i]->GetWidth(),m_bmpArr[i]->GetHeight());
		h+=m_bmpArr[i]->GetHeight();
		delete m_bmpArr[i];
	}

	return outBitmap;
}

// Bitmap* CMathDraw::DrawExpression(CString strExp, Color clr, Font* font)
// {
// 	Graphics gr(AfxGetMainWnd()->m_hWnd);
// 
// 	MDrawingSurface Mgr(font);
// 	DrawExp(strExp,font,&Mgr,&gr);
// 	Rect rect=Mgr.GetBounds(&gr);
// 
// 	Bitmap* outBitmap=new Bitmap(rect.Width,rect.Height,PixelFormat32bppARGB);
// 	Graphics grbmp(outBitmap);
// 	Mgr.Draw(&grbmp,clr);
// 	return outBitmap;
// }

Bitmap* CMathDraw::DrawObjectName(CString strName, Font* font)
{
	int pos=strName.Find('_');
	CString strChrName;
	CString strNumber;
	CString strNameTag;
	if (pos!=-1)
	{
		strChrName=strName.Left(pos);
		int i=pos+1;
		for (i;i<strName.GetLength();i++)
		{
			if (!(strName[i]>='0' && strName[i]<='9')) break;
		}
		strNumber=strName.Mid(pos+1,i-pos-1);
		strNameTag=strName.Right(strName.GetLength()-i);
	}
	else strChrName=strName;

	Graphics gr(AfxGetMainWnd()->m_hWnd);
	FontFamily family; font->GetFamily(&family);
	WCHAR fontName[LF_FACESIZE];
	family.GetFamilyName(fontName);
	Font font2(fontName,SubFontSize(font->GetSize()),font->GetStyle(),font->GetUnit());
	RectF rect1,rect2,rect3;
	gr.MeasureString(strChrName,-1,font,PointF(0,0),&rect1);
	gr.MeasureString(strNameTag,-1,font,PointF(0,0),&rect2);
	gr.MeasureString(strNumber,-1,&font2,PointF(0,0),&rect3);
	
	Bitmap* bmp=new Bitmap(rect1.Width+max(rect2.Width,rect3.Width),max(rect1.Height,rect1.Height*1/2+rect3.Height),PixelFormat32bppARGB);
	Graphics graph(bmp);
	graph.SetTextRenderingHint(TextRenderingHintSingleBitPerPixelGridFit);
	SolidBrush br(Color::Black);
	graph.DrawString(strChrName+strNameTag,-1,font,PointF(0,0),NULL,&br);
	graph.DrawString(strNumber,-1,&font2,PointF(rect1.Width-font->GetSize()/2+2,rect1.Height*1/2),NULL,&br);

	return bmp;
}

Bitmap* CMathDraw::DrawExpression(CString strExp, Color clr, Font* font)
{
	Graphics gr(AfxGetMainWnd()->m_hWnd);

	MDrawingSurface Mgr(font);
	DrawExp(strExp,font,font->GetSize(),&Mgr,&gr);
	Rect rect=Mgr.GetBounds(&gr);

	Bitmap* outBitmap=new Bitmap(rect.Width,rect.Height,PixelFormat32bppARGB);
	Graphics grbmp(outBitmap);
	grbmp.SetTextRenderingHint(TextRenderingHintSingleBitPerPixelGridFit);

	//grbmp.SetSmoothingMode(SmoothingModeAntiAlias);
	Mgr.Draw(&grbmp,clr);
	return outBitmap;
}

void CMathDraw::AddString(MDrawingSurface* gr, Font* font, int fontSize, CString& strText, int& x, Graphics* graph, bool bModeMath)
{
	if (!bModeMath)
	{
		gr->AddString(strText,PointF(x,0),fontSize,false);

		RectF outRect;
		FontFamily family; font->GetFamily(&family);
		Font f(&family,(REAL)fontSize,font->GetStyle());
		graph->MeasureString(strText,-1,&f,PointF(0,0),&outRect);
		x+=outRect.Width;
	}
	else
	{
		int i=0;
		int prev=0;
		bool bNum='0'<=strText[0] && strText[0]<='9';
		while (i<=strText.GetLength())
		{
			if ((bNum && (i==strText.GetLength() || !('0'<=strText[i] && strText[i]<='9'))) || 
				(!bNum && (i==strText.GetLength() || ('0'<=strText[i] && strText[i]<='9'))))
			{
				CString str=strText.Mid(prev,i-prev);
				bool bItalic;
				if (bNum) 
					bItalic=false;
				else
				{
					CString strFuncList=L"|sin|cos|tan|cot|arcsin|arccos|arctan|arccot|log|ln|";
					bItalic=(strFuncList.Find(L"|"+str+L"|")==-1);
				}

				gr->AddString(str,PointF(x,0),fontSize,bItalic);

				RectF outRect;
				FontFamily family; font->GetFamily(&family);
				Font f(&family,(REAL)fontSize,font->GetStyle());
				graph->MeasureString(str,-1,&f,PointF(0,0),&outRect);
				x+=outRect.Width;

				prev=i;
				bNum=!bNum;
			}
			i++;
		}
	}
	strText="";
}

void CMathDraw::AddSymbolString(MDrawingSurface* gr, Font* font, int fontSize, CString strText, int& x, Graphics* graph)
{
	gr->AddSymbol(strText,PointF(x,0),fontSize);

	RectF outRect;
	Font f(L"Symbol",(REAL)fontSize,(INT)font->GetStyle());
	graph->MeasureString(strText,-1,&f,PointF(0,0),&outRect);
	x+=outRect.Width;
}

int CMathDraw::DrawExpLatex(CString strExp, CString* fontName, int fontStyle, int fontSize, MDrawingSurface* gr, Graphics* graph, BOOL latex)
{
	/*
	SolidBrush solidBrush(Color(255, 0, 0, 0));
	int i=0;
	int x=0;
	_TCHAR chr;
 	CString strTmpText;
	BOOL modeLatex=latex;

	while (i<strExp.GetLength())
	{
		chr=strExp.GetAt(i);
		if (modeLatex && chr=='^')
		{
			if (strTmpText!=L"") { AddString(gr,fontName,fontStyle,fontSize,strTmpText,x,graph); }
			int pos=GetNextExp(strExp.Right(strExp.GetLength()-i-1));
			CString str=strExp.Mid(i+1,pos);
			if (str.GetAt(0)=='{') str=str.Mid(1,str.GetLength()-2);
			int subFontSize=SubFontSize(fontSize); //Font nho
			MDrawingSurface tmpGr(fontName,fontStyle);
			DrawExpLatex(str,fontName,fontStyle,subFontSize,&tmpGr,graph); //Ve cong thuc len tmpGr
			i+=pos+1;
			//Dat cong thuc vao dung vi tri
			Rect rect=tmpGr.GetBounds(graph);
			tmpGr.Translate(x-fontSize/4,-rect.Height+fontSize);
			//Ve cong thuc
			gr->AddDr(&tmpGr);
			//Cach mot doan de them tiep cong thuc tiep
			x+=rect.Width;
		}
		else if (modeLatex && chr=='_')
		{
 			if (strTmpText!=L"") { AddString(gr,fontName,fontStyle,fontSize,strTmpText,x,graph); }
			int pos=GetNextExp(strExp.Right(strExp.GetLength()-i-1));
			CString str=strExp.Mid(i+1,pos);
			if (str.GetAt(0)=='{') str=str.Mid(1,str.GetLength()-2);
			int subFontSize=SubFontSize(fontSize); //Font nho
			MDrawingSurface tmpGr(fontName,fontStyle);
			DrawExpLatex(str,fontName,fontStyle,subFontSize,&tmpGr,graph); //Ve cong thuc len tmpGr
			i+=pos+1;
			//Dat cong thuc vao dung vi tri
			Rect rect=tmpGr.GetBounds(graph);
			Matrix matrix;
			tmpGr.Translate(x-fontSize/4,fontSize);
			//Ve cong thuc
			gr->AddDr(&tmpGr);
			//Cach mot doan de them tiep cong thuc tiep
			x+=rect.Width;
		}
		else if (modeLatex && chr=='\\')
		{
			if (strExp.Mid(i+1,4)==L"sqrt")
			{
 				if (strTmpText!=L"") { AddString(gr,fontName,fontStyle,fontSize,strTmpText,x,graph); }
				//Chi so cua can
				i+=5;
				if (strExp.GetAt(i)=='[')
				{
					int pos=GetNextExp(strExp.Right(strExp.GetLength()-i));
					CString subStr=strExp.Mid(i+1,pos-2);
					MDrawingSurface tmpGr(fontName,fontStyle);
					i+=pos+2;
				}
				//Bieu thuc trong can
				int pos=GetNextExp(strExp.Right(strExp.GetLength()-i));
				CString str; //bieu thuc trong can
				str=strExp.Mid(i+1,pos-2);
				if (str==L"") str=L" ";
				MDrawingSurface tmpGr(fontName,fontStyle);
				int top=DrawExpLatex(str,fontName,fontStyle,fontSize,&tmpGr,graph);
				i+=pos;
				Rect rect=tmpGr.GetBounds(graph);
				gr->AddLine(Point(x+6,top-3),Point(x+rect.Width+9,top-3));
				gr->AddLine(Point(x+6,top-3),Point(x+4,top+rect.Height));
				gr->AddLine(Point(x+4,top+rect.Height),Point(x+2,top+rect.Height*2/3));
				tmpGr.Translate(x+7,top);
				x+=rect.Width+7;
				gr->AddDr(&tmpGr);
			}
			else if (modeLatex && strExp.Mid(i+1,4)==L"frac")
			{
 				if (strTmpText!=L"") { AddString(gr,fontName,fontStyle,fontSize,strTmpText,x,graph); }
				i+=5;
				int pos1=GetNextExp(strExp.Right(strExp.GetLength()-i));
				CString str1=strExp.Mid(i+1,pos1-2);
				int pos2=GetNextExp(strExp.Right(strExp.GetLength()-i-pos1));
				CString str2=strExp.Mid(i+pos1+1,pos2-2);

				if (str1==L"") str1=L" ";
				if (str2==L"") str2=L" ";
				MDrawingSurface tmpGr1(fontName,fontStyle);
				MDrawingSurface tmpGr2(fontName,fontStyle);
				DrawExpLatex(str1,fontName,fontStyle,fontSize,&tmpGr1,graph);
				DrawExpLatex(str2,fontName,fontStyle,fontSize,&tmpGr2,graph);

				i+=pos1+pos2;

				Rect rect1=tmpGr1.GetBounds(graph);
				Rect rect2=tmpGr2.GetBounds(graph);

				int w=max(rect1.Width,rect2.Width)+4;
				gr->AddLine(Point(x+2,fontSize/2+2),Point(x+w,fontSize/2+2));

				tmpGr1.Translate(x+2+(w-rect1.Width)/2,-rect1.Height+fontSize/2-2);
				tmpGr2.Translate(x+2+(w-rect2.Width)/2,fontSize/2+5);

				x+=w+4;
				gr->AddDr(&tmpGr1);
				gr->AddDr(&tmpGr2);
			}
			else if (modeLatex && strExp.Mid(i+1,4)==L"over")
			{

			}
			else
			{
				if (chr=='$' && !latex) modeLatex=!modeLatex;
				else if (!(modeLatex && (chr==' ' || chr=='{' || chr=='}')))
				{
					if (chr=='*') strTmpText+=L"·";
					else strTmpText+=(CString)chr;
				}
				i++;
			}
		}
		else
		{
			if (chr=='$') modeLatex=!modeLatex;
			else if (!(modeLatex && (chr==' ' || chr=='{' || chr=='}')))
			{
				if (chr=='*') strTmpText+=L"·";
				else strTmpText+=(CString)chr;
			}
			i++;
		}
	}
	if (strTmpText!=L"") { AddString(gr,fontName,fontStyle,fontSize,strTmpText,x,graph); }

	Rect rect=gr->GetBounds(graph);
	gr->Translate(-rect.GetLeft(),-rect.GetTop());
	return rect.GetTop();
	*/
	return 0;
}


int CMathDraw::DrawExp(CString strExp, Font* font, int fontSize, MDrawingSurface* gr, Graphics* graph, BOOL bFormula)
{
	SolidBrush solidBrush(Color(255, 0, 0, 0));
	int i=0;
	int x=0;
	_TCHAR chr;
	CString strTmpText;
	BOOL modeFormula=bFormula;
	int spaceWidth=fontSize/3;

	while (i<strExp.GetLength())
	{
		chr=strExp.GetAt(i);

		int pos1=GetNextExp(strExp.Right(strExp.GetLength()-i));
		if (modeFormula)
		{
			if (strExp[pos1+i]=='/')
			{
				if (strTmpText!=L"") AddString(gr,font,fontSize,strTmpText,x,graph,true);
				//else AddString(gr,font,fontSize,strTmpText=L" ",x,graph);
				x+=spaceWidth;
				int pos1=GetNextExp(strExp.Right(strExp.GetLength()-i));
				CString str1;
				if (strExp[i]=='(') str1=strExp.Mid(i+1,pos1-2);
				else str1=strExp.Mid(i,pos1);
				int pos2=GetNextExp(strExp.Right(strExp.GetLength()-i-pos1-1));
				CString str2;
				if (strExp[i+pos1+1]=='(') str2=strExp.Mid(i+pos1+1+1,pos2-2);
				else str2=strExp.Mid(i+pos1+1,pos2);

				MDrawingSurface tmpGr1(font);
				MDrawingSurface tmpGr2(font);
				DrawExp(str1,font,fontSize,&tmpGr1,graph);
				DrawExp(str2,font,fontSize,&tmpGr2,graph);

				i+=pos1+pos2+1;

				Rect rect1=tmpGr1.GetBounds(graph);
				Rect rect2=tmpGr2.GetBounds(graph);

				int w=max(rect1.Width,rect2.Width);
				gr->AddLine(Point(x,fontSize-1),Point(x+w,fontSize-1));

				tmpGr1.Translate(x+(w-rect1.Width)/2,-rect1.Height+fontSize-3);
				tmpGr2.Translate(x+(w-rect2.Width)/2,fontSize+3);

				x+=w;
				x+=spaceWidth;
				//AddString(gr,font,fontSize,strTmpText=L" ",x,graph);
				gr->AddDr(&tmpGr1);
				gr->AddDr(&tmpGr2);
			}
			else if (chr=='^')
			{
				if (strTmpText!=L"") { AddString(gr,font,fontSize,strTmpText,x,graph,true); }
				int pos=GetNextExp(strExp.Right(strExp.GetLength()-i-1));
				CString str=strExp.Mid(i+1,pos);
				if (str.GetAt(0)=='(') str=str.Mid(1,str.GetLength()-2);
				int subFontSize=SubFontSize(fontSize); //Font nho
				MDrawingSurface tmpGr(font);
				DrawExp(str,font,subFontSize,&tmpGr,graph); //Ve cong thuc len tmpGr
				i+=pos+1;
				//Dat cong thuc vao dung vi tri
				Rect rect=tmpGr.GetBounds(graph);
				tmpGr.Translate(x-fontSize/4,-rect.Height+fontSize);
				//Ve cong thuc
				gr->AddDr(&tmpGr);
				//Cach mot doan de them tiep cong thuc tiep
				x+=rect.Width;
			}
			else if (chr=='_')
			{
				if (strTmpText!=L"") { AddString(gr,font,fontSize,strTmpText,x,graph,true); }
				int pos=GetNextExp(strExp.Right(strExp.GetLength()-i-1));
				CString str=strExp.Mid(i+1,pos);
				if (str.GetAt(0)=='(') str=str.Mid(1,str.GetLength()-2);
				int subFontSize=SubFontSize(fontSize); //Font nho
				MDrawingSurface tmpGr(font);
				DrawExp(str,font,subFontSize,&tmpGr,graph); //Ve cong thuc len tmpGr
				i+=pos+1;
				//Dat cong thuc vao dung vi tri
				Rect rect=tmpGr.GetBounds(graph);
				Matrix matrix;
				tmpGr.Translate(x-fontSize/4,fontSize);
				//Ve cong thuc
				gr->AddDr(&tmpGr);
				//Cach mot doan de them tiep cong thuc tiep
				x+=rect.Width;
			}
			else if (chr=='[' || chr=='(')
			{
				if (strTmpText!=L"") { AddString(gr,font,fontSize,strTmpText,x,graph,true); }
				//x+=spaceWidth;
				int pos=GetNextExp(strExp.Right(strExp.GetLength()-i));
				CString str=strExp.Mid(i+1,pos);
				if (strExp[i+pos-1]==(chr=='[' ? ']' : ')')) str=strExp.Mid(i+1,pos-2);
				else str=strExp.Mid(i+1,pos-1);
				if (str=="") str=" ";
				MDrawingSurface tmpGr(font);
				int top=DrawExp(str,font,fontSize,&tmpGr,graph);
				i+=pos;
				Rect rect=tmpGr.GetBounds(graph);
				int brLength;
				if (chr=='[')
				{
					brLength=fontSize/3;
					gr->AddLine(Point(x,top-2),Point(x+brLength,top-2));
					gr->AddLine(Point(x,top-2),Point(x,top+rect.Height));
					gr->AddLine(Point(x,top+rect.Height),Point(x+brLength,top+rect.Height));

					if (strExp[i-1]==']')
					{
						gr->AddLine(Point(x+rect.Width+brLength,top-2),Point(x+rect.Width+2*brLength,top-2));
						gr->AddLine(Point(x+rect.Width+2*brLength,top-2),Point(x+rect.Width+2*brLength,top+rect.Height));
						gr->AddLine(Point(x+rect.Width+brLength,top+rect.Height),Point(x+rect.Width+2*brLength,top+rect.Height));
					}
				}
				else // if (chr==')')
				{
					brLength=fontSize/2;
					gr->AddBracket(Point(x,top-2),brLength,rect.Height);

					if (strExp[i-1]==')')
					{
						gr->AddBracket(Point(x+brLength+rect.Width,top-2),-brLength,rect.Height);
						//gr->AddLine(Point(x+rect.Width+brLength,top-2),Point(x+rect.Width+2*brLength,top-2));
						//gr->AddLine(Point(x+rect.Width+2*brLength,top-2),Point(x+rect.Width+2*brLength,top+rect.Height));
						//gr->AddLine(Point(x+rect.Width+brLength,top+rect.Height),Point(x+rect.Width+2*brLength,top+rect.Height));
					}
				}

				tmpGr.Translate(x+brLength,top);
				x+=rect.Width+3*brLength;
				gr->AddDr(&tmpGr);
			}
			else if (strExp.Mid(i,4)==L"sqrt")
			{
				if (strTmpText!=L"") { AddString(gr,font,fontSize,strTmpText,x,graph,true); }
				//Chi so cua can
				i+=4;
				/*
				if (strExp.GetAt(i)=='[')
				{
					int pos=GetNextExp(strExp.Right(strExp.GetLength()-i));
					CString subStr=strExp.Mid(i+1,pos-2);
					MDrawingSurface tmpGr(font);
					i+=pos+2;
				}
				*/
				//Bieu thuc trong can
				x+=spaceWidth;
				int pos=GetNextExp(strExp.Right(strExp.GetLength()-i));
				CString str; //bieu thuc trong can
				if (strExp[i+pos-1]==')') str=strExp.Mid(i+1,pos-2);
				else str=strExp.Mid(i+1,pos-1);
				if (str=="") str=" ";
				MDrawingSurface tmpGr(font);
				int top=DrawExp(str,font,fontSize,&tmpGr,graph);
				i+=pos;
				Rect rect=tmpGr.GetBounds(graph);
				int sqrtWidth=fontSize/3;
				gr->AddLine(Point(x+2*sqrtWidth,top-4),Point(x+2*sqrtWidth+rect.Width+spaceWidth/2,top-4));
				gr->AddLine(Point(x+2*sqrtWidth,top-4),Point(x+sqrtWidth,top+rect.Height));
				gr->AddLine(Point(x+sqrtWidth,top+rect.Height),Point(x,top+rect.Height*7/12));
				tmpGr.Translate(x+2*spaceWidth,top);
				x+=rect.Width+2*sqrtWidth;
				x+=spaceWidth;
				gr->AddDr(&tmpGr);
			}
			else if (strExp.Mid(i,14)==L"DirectedLength" || strExp.Mid(i,2)==L"DL")
			{
				if (strTmpText!=L"") { AddString(gr,font,fontSize,strTmpText,x,graph,true); }
				i+=strExp.Mid(i,14)==L"DirectedLength" ? 14 : 2;
				x+=spaceWidth;
				int pos=GetNextExp(strExp.Right(strExp.GetLength()-i));
				CString str;
				if (strExp[i+pos-1]==')') str=strExp.Mid(i+1,pos-2);
				else str=strExp.Mid(i+1,pos-1);
				if (str=="") str=" ";
				//str.Remove(',');
				MDrawingSurface tmpGr(font);
				int top=DrawExp(str,font,fontSize,&tmpGr,graph);
				i+=pos;
				Rect rect=tmpGr.GetBounds(graph);
				gr->AddLine(Point(x,top-2),Point(x+rect.Width+1,top-2));

				tmpGr.Translate(x+1,top);
				x+=rect.Width+1;
				gr->AddDr(&tmpGr);
			}
			else if (strExp.Mid(i,5)==L"Angle")
			{
				if (strTmpText!=L"") { AddString(gr,font,fontSize,strTmpText,x,graph,true); }
				i+=5;
				x+=spaceWidth;
				int pos=GetNextExp(strExp.Right(strExp.GetLength()-i));
				CString str;
				if (strExp[i+pos-1]==')') str=strExp.Mid(i+1,pos-2);
				else str=strExp.Mid(i+1,pos-1);
				if (str=="") str=" ";

				MDrawingSurface tmpGr(font);
				int top=DrawExp(str,font,fontSize,&tmpGr,graph);
				i+=pos;
				Rect rect=tmpGr.GetBounds(graph);
				gr->AddLine(Point(x,top-2),Point(x+rect.Width/2,top-2-fontSize/4));
				gr->AddLine(Point(x+rect.Width/2,top-2-fontSize/4),Point(x+rect.Width,top-2));

				tmpGr.Translate(x+1,top);
				x+=rect.Width+1;
				gr->AddDr(&tmpGr);
			}
			else
			{
				if (chr=='$') 
				{
					if (strTmpText!=L"") { AddString(gr,font,fontSize,strTmpText,x,graph,true); }
					modeFormula=!modeFormula;
					i++;
				}
				else //if (!(chr==' ' || chr=='{' || chr=='}'))
				{
					if (chr=='\\' && strExp.GetLength()>i+1)
					{
						if (strTmpText!=L"") { AddString(gr,font,fontSize,strTmpText,x,graph,true); }
						AddSymbolString(gr,font,fontSize,(CString)strExp[i+1],x,graph);
						i+=2;
						x+=spaceWidth;
					}
					else if (strExp.Mid(i,2)==L">=")
					{
						if (strTmpText!=L"") { AddString(gr,font,fontSize,strTmpText,x,graph,true); }
						AddSymbolString(gr,font,fontSize,L"",x,graph);
						i+=2;
						x+=spaceWidth;
					}
					else if (strExp.Mid(i,2)==L"<=")
					{
						if (strTmpText!=L"") { AddString(gr,font,fontSize,strTmpText,x,graph,true); }
						AddSymbolString(gr,font,fontSize,L"",x,graph);
						i+=2;
						x+=spaceWidth;
					}
					else if (chr=='+' || chr=='-' || chr=='/' || chr=='>' || chr=='=' || chr=='<')
					{
						if (strTmpText!=L"") { AddString(gr,font,fontSize,strTmpText,x,graph,true); }
						AddSymbolString(gr,font,fontSize,CString(chr),x,graph);
						i++;
						x+=spaceWidth*2/3;
					}
					else if (chr=='*')
					{
						if (strTmpText!=L"") { AddString(gr,font,fontSize,strTmpText,x,graph,true); }
						AddSymbolString(gr,font,fontSize,L"",x,graph);
						i++;
						x+=spaceWidth;
					}
					else
					{
						strTmpText+=(CString)chr;
						i++;
					}
				}
			}
		}
		
		else
		{
			if (chr=='$') 
			{
				if (strTmpText!=L"") { AddString(gr,font,fontSize,strTmpText,x,graph); }
				modeFormula=!modeFormula;
			}
			else if (!(modeFormula && (chr==' ' || chr=='{' || chr=='}')))
			{
				strTmpText+=(CString)chr;
			}
			i++;
// 				FontFamily fontFamily(L"Tahoma");
// 				Font font(&fontFamily,10);
// 				gr->AddString((CString)chr,PointF(x,0),fontSize);
// 				RectF outRect;
// 				graph->MeasureString((CString)chr,1,&font,PointF(0,0),&outRect);
// 				x+=outRect.Width-2;
// 				i++;
		}
	}
	if (strTmpText!=L"") { AddString(gr,font,fontSize,strTmpText,x,graph,modeFormula); }
	
	Rect rect=gr->GetBounds(graph);
	gr->Translate(-rect.GetLeft(),-rect.GetTop());
	return rect.GetTop();
}


int CMathDraw::GetNextExp(CString strExp)
{
	int i=0;
	char chr=strExp.GetAt(0);
	bool startByNumber=(chr>='0' && chr<='9');
	while ((chr>='0' && chr<='9') || (chr>='a' && chr<='z' && !startByNumber) || (chr>='A' && chr<='Z' && !startByNumber))
	{
		i++;
		if (i<strExp.GetLength()) chr=strExp.GetAt(i);
		else break;
	}

	
	if (chr=='{' || chr=='(' || chr=='[')
	{
		char cEnd, cBegin;
		cBegin=chr;
		cEnd=(chr=='{') ? '}' : ((chr=='(') ? ')' : ']');
		int d=1;i++;
		while (d!=0 && i<strExp.GetLength())
		{
			chr=strExp.GetAt(i);
			if (chr==cEnd) d--;
			else if (chr==cBegin) d++;
			i++;
		}
		return i;
	}
	return i;
// 	else
// 	{
// 		int i=0;
// 		chr=strExp.GetAt(0);
// 		while ((chr>='0' && chr<='9') || (chr>='a' && chr<='z') || (chr>='A' && chr<='Z') || chr=='(')
// 		{
// 			i++;
// 			if (i<strExp.GetLength()) chr=strExp.GetAt(i);
// 			else break;
// 		}
// 		return i;
// 	}
}