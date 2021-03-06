#pragma once
#include "VGObject.h"
#include "CommandArray.h"
#include "VGeoDrawerView.h"
#include "VGGrid.h"
//#include "GeoObjListBox.h"

#define DRAG_MODE_DRAG_SELECTION 1
#define DRAG_MODE_DRAG_LABEL 2
#define DRAG_MODE_DRAG_OBJECT 3

#define UNREDO_TYPE_REMOVE 1
#define UNREDO_TYPE_CHANGE_PROPERTIES 2
#define UNREDO_TYPE_ADD 3

#define UNDO_MAX 20

using namespace std;
class CGeoObjTree;

class CVGObject;

class CVGObjLabel;
class CGeoExp;

class CVGPoint;
class CVGLineBase;
class CVGLine;
class CVGVector;
class CVGCircle;
class CVGExpression;

extern int numRound;
extern float nZoom;

class CVGDefaultValue
{
public:
	//////////////////////////////////////////////////////////////////////////
	//Default
	Color m_clrDefaultPoint;
	int m_nPointSize;
	bool m_bPointer; // Đường gióng
	int m_nCoordinateType;

	Color m_clrDefaultLine;
	int m_nLineStyle;
	int m_nLineWidth;
	bool m_bLineShowSeg; // Bỏ đoạn thừa ?
	int m_nLineFuncType;

	Color m_clrDefaultSeg;
	int m_nSegStyle;
	int m_nSegWidth;

	Color m_clrDefaultVector;
	int m_nVecStyle;
	int m_nVecWidth;

	Color m_clrDefaultCircle;
	int m_nCirStyle;
	int m_nCirWidth;
	int m_nCirFuncType;

	void Serialize(CArchive& ar)
	{
		if (ar.IsStoring())
		{
			ar << m_clrDefaultPoint.GetValue();
			ar << m_nPointSize;
			ar << m_bPointer;
			ar << m_nCoordinateType;

			ar << m_clrDefaultLine.GetValue();
			ar << m_nLineStyle;
			ar << m_nLineWidth;
			ar << m_bLineShowSeg;
			ar << m_nLineFuncType;

			ar << m_clrDefaultSeg.GetValue();
			ar << m_nSegStyle;
			ar << m_nSegWidth;

			ar << m_clrDefaultVector.GetValue();
			ar << m_nVecStyle;
			ar << m_nVecWidth;

			ar << m_clrDefaultCircle.GetValue();
			ar << m_nCirStyle;
			ar << m_nCirWidth;
			ar << m_nCirFuncType;
		}
		else
		{
			int clr;

			ar >> clr; m_clrDefaultPoint.SetValue(clr);
			ar >> m_nPointSize;
			ar >> m_bPointer;
			ar >> m_nCoordinateType;

			ar >> clr; m_clrDefaultLine.SetValue(clr);
			ar >> m_nLineStyle;
			ar >> m_nLineWidth;
			ar >> m_bLineShowSeg;
			ar >> m_nLineFuncType;

			ar >> clr; m_clrDefaultSeg.SetValue(clr);
			ar >> m_nSegStyle;
			ar >> m_nSegWidth;

			ar >> clr; m_clrDefaultVector.SetValue(clr);
			ar >> m_nVecStyle;
			ar >> m_nVecWidth;

			ar >> clr; m_clrDefaultCircle.SetValue(clr);
			ar >> m_nCirStyle;
			ar >> m_nCirWidth;
			ar >> m_nCirFuncType;
		}
	}
};

class CVGObjContainer
{
public:
	void Init();
	CVGObject* operator [](int nIndex);
	CVGObject* operator [](CString strName);
	//Hàm thêm đối tượng
	double* AddVariable(CString name, double dValue);
	BOOL AddCommand(CString strCmd, CArray<CVGObject*>* objOutputArray=NULL, CString* strOutputString=NULL);
	void AddCommands(CString strCommandLines);
	void AddRegularPolygon(CVGPoint* pt1, CVGPoint* pt2, int num);
	CVGObject* AddObject(ObjType objType);
	CVGObject* AddExpression(CString strExp, bool bTemp=true);

	void AddFunctionD();

	void DoCommand(CString strCmd, CArray<CVGObject*> &objInput, CStringArray &objOutputName, CArray<CVGObject*>* objOutput, CString* strOutputString);

	void Calc(); // Tính toán lại vị trí các đối tượng	
	void Calc(CVGObject* obj);
	void CalcAnimate();

	void CutSuperfluousLinePart();
	void Calc_Resize();
	void MoveSelection(int x, int y, CPoint startPt);
	void SelectAll(BOOL bSelect);
	void Select(CVGObject* obj, BOOL bInverse, BOOL bSelect=TRUE);
	void SelectInRect(Rect rect);
	//void CheckMouseOver( Point clientPoint, bool& bIsLabel, int& i1, int& i2 );
	void CheckMouseOver(Point clientPoint, bool& bIsLabel, bool& bIsBoundMark, bool& bIsMouseOverButton, CVGObject*& i1, CVGObject*& i2 ,int& nState, int& x, int& y);
	void CheckMouseOverObj(Point clientPoint, bool& bIsLabel, CVGObject*& i1, CVGObject*& i2);
	void CheckMouseOverGeoObj(Point clientPoint, bool& bIsLabel, CVGObject*& i1, CVGObject*& i2);
	int CheckPtInPanel(Point pt, CVGBound* except=NULL);
	int CheckRectInPanel(Rect rect, CVGBound* except);
	bool ResetButtonState();
	int ClickButton();
	void RefreshDocking();

	void DrawBackground(int width, int height);
	void DrawBackground(Graphics* gr, int width, int height);
	void Draw();
	void Draw(Graphics* gr, BOOL bTrace);
	void Draw(Graphics* gr);
	int GetSize();
	//void HighLightObj(Point point);
	BOOL HighLightObj(CVGObject* obj,BOOL bHighLight=TRUE,BOOL bAdd=FALSE);
	void SetSelection(int nIndex);
	void AddSelection(int nIndex);
	void DragLabel(CVGObject* obj, int x, int y);
	void ResetObjMoved();
	void SetObjParent(CVGBound* obj, CVGPanel* panel);


	CString m_Name;
	
	//Varible
	CAxisInfo m_AxisInfo;
	CArray<CVGObject*> m_Array;
	CCommandArray* m_cmdArray;
	int* m_PadWidth;
	int* m_PadHeight;

	CVGeoDrawerView* pView;
	CGeoObjTree* m_pObjTree;
	CScriptObject* m_ScriptObject;
	BOOL m_bTemp;

	Bitmap* m_bgBitmap;
	
	CVGObjContainer(bool bTemp=false);
	~CVGObjContainer(void);
	CString GetNextAvailableName(int nType, CStringArray* stringArr=NULL);
	void AddObjTreeItem(CVGObject* obj);
	CVGObject* GetObjByName(CString strName);
	CVGObject* GetAt(int nIndex);

	void ResetContent();
	int GetSelection(int iStart=0);
	void NextAnimatePos();
	void SetIntersection(CVGGeoObject* obj1, CVGGeoObject* obj2, CVGPoint* pt);
	BOOL CheckRegularName(CString strName);
	void Serialize(CArchive& ar);
	void DeleteSelection();
	void Undo();
	void Redo();
	int cmdCount;
	BOOL CanUndo();
	BOOL CanRedo();
	void OnAddObjs(CArray<int>* objArr);
	void OnRemoveObjs(CArray<CVGObject*>* objArr);
	void OnChangeProperties(CArray<CVGObject*>* objArr);
	BOOL RenameObject(CVGObject* obj, CString newName);
	void ResetUndoArray();
	void DeleteTestObj();
	void StartTestObj();
	void Copy(CVGObjContainer* c);
	bool AddWatch(CString strExp);
	CVGGrid m_graphAxis;
	int m_nTestStartPos;
	bool m_bIsTesting;
	bool m_bIsTestingPoint;
	CVGDefaultValue* m_defaultValue;

	Font* m_LabelFont;

	CArray<CGeoExp*> m_expWatchArray;

	//////////////////////////////////////////////////////////////////////////

	CStringArray* m_strReplacedNameArr;
private:
	void GetFuncAndParam(CString strFunc, CString& sFunc, CStringArray* aParam);
	void GetParam(CString strParam, CStringArray* aParam);
	int GetReturnValueType(CString cmd);
	void DeleteObjFrom(int nIndex);
	void SetName(CVGObject* obj,CString name);
	void ResetRedo();
	void RemoveUndoArrayElement(int nIndex);
	void FreeUndo();
	CString GetName(CStringArray &objOutputName,int index, int type);
	CString GetName(int nIndex);
	BOOL IsPointsArray(CArray<CVGObject*> &objArr);
	CString GetSimilarName(CString strName);
	bool GetExpStringValue(CString strExp, CString& str);
	CValue* GetExpValue(CString strExp);
	void DeleteObj(CVGObject* obj);
	int GetObjIndex(CVGObject* obj);
	void GetRelatedObjArray(CVGObject* obj, CArray<CVGObject*>& arr);
	void RefreshAxisLine();
	CArray<void*> m_UndoArray1;
	CArray<void*> m_UndoArray2;
	CArray<void*> m_UndoArray3;
	CArray<int> m_UndoType;
	int m_nUndoPos;
};


