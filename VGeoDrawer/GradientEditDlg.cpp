// GradientEditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VGeoDrawer.h"
#include "GradientEditDlg.h"
#include "InputDlg.h"


// CGradientEditDlg dialog

IMPLEMENT_DYNAMIC(CGradientEditDlg, CDialog)

CGradientEditDlg::CGradientEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGradientEditDlg::IDD, pParent)
	, m_bTitle(FALSE)
	, m_nTitleHeight(0)
{
	m_Font.lfCharSet=DEFAULT_CHARSET;
	m_Font.lfStrikeOut=FALSE;
	m_Font.lfItalic=FALSE;
	m_Font.lfUnderline=FALSE;
	m_Font.lfWeight=FW_NORMAL;
	wcsncpy(m_Font.lfFaceName, _T("Tahoma"), LF_FACESIZE);
	m_Font.lfHeight=-12;
}

CGradientEditDlg::~CGradientEditDlg()
{
}

void CGradientEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_COLOR1, m_btnColor1);
	DDX_Control(pDX, IDC_BTN_COLOR2, m_btnColor2);
	DDX_Control(pDX, IDC_BTN_TITLE_COLOR1, m_btnTitleColor1);
	DDX_Control(pDX, IDC_BTN_TITLE_COLOR2, m_btnTitleColor2);
	DDX_Control(pDX, IDC_BTN_COLOR_BORDER, m_btnBorderClr);
	DDX_Check(pDX, IDC_CHECK_TITLE, m_bTitle);
	DDX_Text(pDX, IDC_EDIT2, m_nTitleHeight);
}


BEGIN_MESSAGE_MAP(CGradientEditDlg, CDialog)
	ON_MESSAGE(WM_BTN_COLOR_CHANGED, &CGradientEditDlg::OnColorChanged)
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_COMBO_GR_MODE, &CGradientEditDlg::OnCbnSelchangeComboGrMode)
	ON_CBN_SELCHANGE(IDC_COMBO_GR_MODE_TITLE, &CGradientEditDlg::OnCbnSelchangeComboGrMode)
	ON_BN_CLICKED(IDOK, &CGradientEditDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_SELECT_FONT, &CGradientEditDlg::OnBnClickedBtnSelectFont)
	ON_BN_CLICKED(IDCANCEL, &CGradientEditDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_CHOOSE_STYLE, &CGradientEditDlg::OnBnClickedBtnChooseStyle)
END_MESSAGE_MAP()


// CGradientEditDlg message handlers

LRESULT CGradientEditDlg::OnColorChanged(WPARAM wParam, LPARAM lParam)
{
	m_grStyle.m_Color1=m_btnColor1.m_Color;
	m_grStyle.m_Color2=m_btnColor2.m_Color;
	m_grTitleStyle.m_Color1=m_btnTitleColor1.m_Color;
	m_grTitleStyle.m_Color2=m_btnTitleColor2.m_Color;

	OnPaint();
	return 0;
}

void CGradientEditDlg::OnPaint()
{
	CDialog::OnPaint();

	UpdateData(TRUE);

	Graphics gr(GetDlgItem(IDC_STATIC_VIEW)->m_hWnd);
	CRect rc;
	GetDlgItem(IDC_STATIC_VIEW)->GetClientRect(rc);
	Rect rect(rc.left,rc.top,rc.Width()-1,rc.Height()-1);

	Brush* br=GetBrush(m_grStyle,rect);
	//LinearGradientBrush br(rect,m_Color,Color::White,LinearGradientModeVertical);
	gr.FillRectangle(br,rect);

	Pen pen(m_btnBorderClr.m_Color);
	gr.DrawRectangle(&pen,rect);

	if (m_bTitle)
	{
		Rect rectTitle=Rect(rect.X,rect.Y,rect.Width,m_nTitleHeight);
		Brush* brTitle=GetBrush(m_grTitleStyle,rectTitle);
		gr.FillRectangle(brTitle,rectTitle);
		gr.DrawRectangle(&pen,rectTitle);
		
		Font font(AfxGetMainWnd()->GetDC()->m_hDC,&m_Font);
		SolidBrush textBr(m_TextColor);
		RectF rcf=RectF(5,0,rectTitle.Width,rectTitle.Height);
		StringFormat stringFormat;
		stringFormat.SetLineAlignment(StringAlignmentCenter);
		gr.DrawString(m_strTitle,-1,&font, rcf, &stringFormat, &textBr);
	}

	delete br;
}

void CGradientEditDlg::OnCbnSelchangeComboGrMode()
{
	m_grStyle.m_Mode=(LinearGradientMode)((CComboBox*)GetDlgItem(IDC_COMBO_GR_MODE))->GetCurSel();
	m_grTitleStyle.m_Mode=(LinearGradientMode)((CComboBox*)GetDlgItem(IDC_COMBO_GR_MODE_TITLE))->GetCurSel();
	OnPaint();
}

BOOL CGradientEditDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_btnColor1.m_Color=m_grStyle.m_Color1;
	m_btnColor2.m_Color=m_grStyle.m_Color2;
	((CComboBox*)GetDlgItem(IDC_COMBO_GR_MODE))->SetCurSel(m_grStyle.m_Mode);

	m_btnTitleColor1.m_Color=m_grTitleStyle.m_Color1;
	m_btnTitleColor2.m_Color=m_grTitleStyle.m_Color2;
	((CComboBox*)GetDlgItem(IDC_COMBO_GR_MODE_TITLE))->SetCurSel(m_grTitleStyle.m_Mode);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CGradientEditDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CGradientEditDlg::OnBnClickedBtnSelectFont()
{
	CFontDialog dlg(&m_Font,CF_EFFECTS | CF_SCREENFONTS);
	dlg.m_cf.rgbColors=m_TextColor.ToCOLORREF();
	if (dlg.DoModal()==IDOK)
	{
		memcpy(&m_Font, &dlg.m_lf, sizeof(LOGFONT));
		m_TextColor.SetFromCOLORREF(dlg.m_cf.rgbColors);
	}
	OnPaint();
}

void CGradientEditDlg::SetStyle(int nID)
{
	switch (nID)
	{
	case 0:
		m_grStyle.m_Color1=Color(255,255,255);
		m_grStyle.m_Color2=Color(212,212,212);
		m_grStyle.m_Mode=LinearGradientModeVertical;
		m_grTitleStyle.m_Color1=Color(255,255,255);
		m_grTitleStyle.m_Color2=Color(234,234,234);
		m_grTitleStyle.m_Mode=LinearGradientModeVertical;
		m_nTitleHeight=25;
		m_btnBorderClr.m_Color=Color(128,128,128);
		
		m_Font.lfCharSet=DEFAULT_CHARSET;
		m_Font.lfStrikeOut=FALSE;
		m_Font.lfItalic=FALSE;
		m_Font.lfUnderline=FALSE;
		m_Font.lfWeight=FW_BOLD;
		wcsncpy(m_Font.lfFaceName, _T("Tahoma"), LF_FACESIZE);
		m_Font.lfHeight=-12;

		m_TextColor=Color(100,100,100);

		break;
	}

	m_btnColor1.m_Color=m_grStyle.m_Color1;
	m_btnColor2.m_Color=m_grStyle.m_Color2;
	m_btnColor1.RedrawWindow();
	m_btnColor2.RedrawWindow();
	((CComboBox*)GetDlgItem(IDC_COMBO_GR_MODE))->SetCurSel(m_grStyle.m_Mode);

	m_btnTitleColor1.m_Color=m_grTitleStyle.m_Color1;
	m_btnTitleColor1.RedrawWindow();
	m_btnTitleColor2.m_Color=m_grTitleStyle.m_Color2;
	m_btnTitleColor2.RedrawWindow();
	((CComboBox*)GetDlgItem(IDC_COMBO_GR_MODE_TITLE))->SetCurSel(m_grTitleStyle.m_Mode);

	OnPaint();
}

void CGradientEditDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CGradientEditDlg::OnBnClickedBtnChooseStyle()
{
	CInputDlg dlg(L"Style",this);
	dlg.AddStatic(L"Kiểu :");
	CString strStyle;
	CStringArray arr;
	arr.Add(L"Xám");
	dlg.AddComboBox(&strStyle,&arr,WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST);
	if (dlg.DoModal()==IDOK)
	{
		for (int i=0;i<arr.GetSize();i++)
			if (arr[i]==strStyle) { SetStyle(i); return; }
	}
}
