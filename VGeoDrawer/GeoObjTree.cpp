// GeoObjTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "VGeoDrawer.h"
#include "GeoObjTree.h"
#include "appfunc.h"

// CGeoObjTreeCtrl

IMPLEMENT_DYNAMIC(CGeoObjTree, CTreeCtrl)

CGeoObjTree::CGeoObjTree()
{
	m_GroupBy=GROUP_BY_TYPE;
	for (int i=0;i<ROOT_COUNT;i++) bRoot[i]=false;
}

CGeoObjTree::~CGeoObjTree()
{

}

void CGeoObjTree::Reset()
{
	DeleteAllItems();
	for (int i=0;i<ROOT_COUNT;i++) bRoot[i]=false;
}


BEGIN_MESSAGE_MAP(CGeoObjTree, CTreeCtrl)
	ON_WM_CREATE()
END_MESSAGE_MAP()

void CGeoObjTree::AddObj(CVGObject* obj)
{
	SetRedraw(FALSE);
	if (obj->m_Type==OBJ_POINT_HIDDEN_CHILD) return;
	if (!bRoot[obj->m_Type])
	{
		int i;
		for (i=obj->m_Type+1;i<OBJ_COUNT;i++)
		{
			if (bRoot[i]) break;
		}
		if (i==OBJ_COUNT) 
			root[obj->m_Type]=InsertItem(GetObjTypeName(obj->m_Type),GetIconIndex(obj->m_Type),GetIconIndex(obj->m_Type));
		else 
			root[obj->m_Type]=InsertItem(GetObjTypeName(obj->m_Type),GetIconIndex(obj->m_Type),GetIconIndex(obj->m_Type),TVI_ROOT,root[i]);
		bRoot[obj->m_Type]=true;
		InsertItem(obj->m_Name,GetIconIndex(obj->m_Type),GetIconIndex(obj->m_Type),root[obj->m_Type]);
		Expand(root[obj->m_Type],TVE_EXPAND);
	}
	else
		InsertItem(obj->m_Name,GetIconIndex(obj->m_Type),GetIconIndex(obj->m_Type),root[obj->m_Type]);
	SetRedraw();
}

void CGeoObjTree::FillList()
{
	SetRedraw(FALSE);
	DeleteAllItems();
	for (int i=0;i<OBJ_COUNT;i++) bRoot[i]=false;
	for (int i=0;i<OBJ_COUNT;i++)
	{
		if (i==OBJ_POINT_HIDDEN_CHILD) continue;
		for (int j=0;j<m_Array->GetSize();j++)
		{
			if (m_Array->GetAt(j)->m_Type==i)
			{
				if (!bRoot[i])
				{
					root[i]=InsertItem(GetObjTypeName(i),GetIconIndex(m_Array->GetAt(j)->m_Type),GetIconIndex(m_Array->GetAt(j)->m_Type));
					bRoot[i]=true;
				}
				InsertItem(m_Array->GetAt(j)->m_Name,GetIconIndex(m_Array->GetAt(j)->m_Type),GetIconIndex(m_Array->GetAt(j)->m_Type),root[i]);
			}
		}
	}

	SetRedraw(TRUE);
}

void CGeoObjTree::Refresh()
{
	SetRedraw(FALSE);

	//HTREEITEM h=GetFirstVisibleItem();
	Reset();
	FillList();
	//EnsureVisible(h);
	
	SetRedraw(TRUE);
}

int CGeoObjTree::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_imgList.Create(16,16,ILC_COLOR32 | ILC_MASK,0,0);
	m_imgList.Add(AfxGetApp()->LoadIcon(IDI_POINT));
	m_imgList.Add(AfxGetApp()->LoadIcon(IDI_LINE));
	m_imgList.Add(AfxGetApp()->LoadIcon(IDI_RAY));
	m_imgList.Add(AfxGetApp()->LoadIcon(IDI_SEGMENT));
	m_imgList.Add(AfxGetApp()->LoadIcon(IDI_VECTOR));
	m_imgList.Add(AfxGetApp()->LoadIcon(IDI_CIRCLE));
	m_imgList.Add(AfxGetApp()->LoadIcon(IDI_GRAPH));


	SetImageList(&m_imgList,TVSIL_NORMAL);

	return 0;
}

int CGeoObjTree::GetIconIndex(ObjType type)
{
	switch (type)
	{
	case OBJ_POINT: return 0;
	case OBJ_LINE: return 1;
	case OBJ_RAY: return 2;
	case OBJ_SEGMENT: return 3;
	case OBJ_VECTOR: return 4;
	case OBJ_CIRCLE: return 5;
	case OBJ_FUNCTION: return 6;
	default: return -1;
	}
}