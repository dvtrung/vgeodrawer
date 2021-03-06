#include "PropertyGrid.h"
#include "VGObjContainer.h"
#include "VGeoDrawerDoc.h"
#pragma once

#define IDC_CATEGORY_TREE 1000

// COptionsDlg dialog

class COptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(COptionsDlg)

public:
	COptionsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COptionsDlg();

// Dialog Data
	enum { IDD = IDD_OPTIONS_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void LoadOptions();
	void SaveOptions();
	void SaveOptionsAsDefault();
	CPropertyGrid m_Grid;
	CTreeCtrl m_Tree;

	CVGObjContainer* m_ObjContainer;
	CVGeoDrawerDoc* m_pDoc;
private:
	int m_NumRound;
	bool m_Radian;
	bool m_Ox;
	bool m_Oy;
	bool m_ShowGrid;
	bool m_ShowNumber;
	int m_GridDashStyle;
	bool m_HasPadSize;
	int m_PadHeight;
	int m_PadWidth;

	CVGDefaultValue m_defaultValue;

	afx_msg void OnBnClickedApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSaveAsDefault();
};
