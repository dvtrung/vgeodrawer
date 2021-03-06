#include "StdAfx.h"
#include "PropertyGradient.h"
#include "TextEditorDlg.h"
#include "GradientEditDlg.h"

CPropertyGradientPanel::CPropertyGradientPanel(void)
{
}

CPropertyGradientPanel::~CPropertyGradientPanel(void)
{
}

CPropertyGrid::EEditMode CPropertyGradientPanel::GetEditMode()
{
	return CPropertyGrid::EM_MODAL;
}

void CPropertyGradientPanel::DrawItem( CDC& dc, CRect rc, bool focused )
{
	dc.DrawText(L" <Tùy chọn>",rc,DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX);
// 	Rect rect(rc.left,rc.top,rc.Width(),rc.Height());
// 	rect.Inflate(-2,-2);
// 	
// 	Graphics gr(dc.m_hDC);
// 
// 	CVGGradientStyle* style=m_StyleArr[0];
// 	Brush* br=GetBrush(*style,rect);
// 	gr.FillRectangle(br,rect);
// 
// 	Pen pen(Color::Black);
// 	gr.DrawRectangle(&pen,rect);
// 	delete br;
}

bool CPropertyGradientPanel::OnEditItem()
{
	CGradientEditDlg dlg;
	dlg.m_grStyle=m_PanelArr[0]->m_gradientStyle;
	dlg.m_grTitleStyle=m_PanelArr[0]->m_gradientTitleStyle;
	dlg.m_btnBorderClr.m_Color=m_PanelArr[0]->m_BorderColor;
	dlg.m_bTitle=(BOOL)m_PanelArr[0]->m_bTitle;
	dlg.m_strTitle=m_PanelArr[0]->m_Text;
	dlg.m_nTitleHeight=m_PanelArr[0]->m_nTitleHeight;
	dlg.m_TextColor=m_PanelArr[0]->m_Color;
	memcpy(&dlg.m_Font,&m_PanelArr[0]->m_Font,sizeof(LOGFONT));

	if (dlg.DoModal()==IDOK);
	{
		for (int i=0;i<m_PanelArr.GetSize();i++)
		{
			m_PanelArr[i]->m_gradientStyle=dlg.m_grStyle;
			m_PanelArr[i]->m_gradientTitleStyle=dlg.m_grTitleStyle;
			m_PanelArr[i]->m_BorderColor=dlg.m_btnBorderClr.m_Color;
			m_PanelArr[i]->m_bTitle=dlg.m_bTitle;
			m_PanelArr[i]->m_nTitleHeight=dlg.m_nTitleHeight;
			m_PanelArr[i]->m_Color=dlg.m_TextColor;
			memcpy(&m_PanelArr[0]->m_Font,&dlg.m_Font,sizeof(LOGFONT));
		}
	}

	return TRUE;
}