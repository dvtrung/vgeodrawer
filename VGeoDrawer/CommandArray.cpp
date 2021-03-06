#include "StdAfx.h"
#include "CommandArray.h"
#include "resource.h"
#include "VGObject.h"
#include "appfunc.h"

CCommandArray::CCommandArray(void)
{
}

CCommandArray::~CCommandArray(void)
{
	for (int i=0;i<m_CmdArray.GetSize();i++)
		delete m_CmdArray[i];
}

CCommandArray::CCmd* CCommandArray::operator [](int nIndex)
{
	return m_CmdArray[nIndex];
}

CCommandArray::CCmd* CCommandArray::operator [](CString str)
{
	return GetCommandByString(str);
}

void CCommandArray::LoadCommands()
{
	m_ImageList.DeleteImageList();
	for (int i=0;i<m_CmdArray.GetSize();i++)
		delete m_CmdArray[i];
	m_CmdArray.RemoveAll();

	CFile file;
	file.Open(GetAppDirectory()+L"\\data\\cmd.dat",CFile::modeRead);
	CArchive archive(&file,CArchive::load);
	Serialize(archive);
	archive.Close();
	file.Close();
}

void CCommandArray::SaveCommands()
{
	CFile file;
	file.Open(GetAppDirectory()+L"\\data\\cmd.dat",CFile::modeWrite);
	CArchive archive(&file,CArchive::store);
	Serialize(archive);
	archive.Close();
	file.Close();
}

CCommandArray::CCmd* CCommandArray::GetCommandByString(CString str)
{
	for (int i=0;i<m_CmdArray.GetSize();i++)
	{
		if (m_CmdArray[i]->m_CommandName==str)	return m_CmdArray[i];
	}
	return NULL;
}

void CCommandArray::Serialize( CArchive& ar )
{
	if (ar.IsStoring()) //Lưu
	{
		ar << m_CmdStdArray.GetSize();
		for (int i=0;i<m_CmdStdArray.GetSize();i++)
		{
			ar << m_CmdStdArray[i].strName;
			ar << m_CmdStdArray[i].strDescription;
			ar << m_CmdStdArray[i].strInput;
		}

		ar << m_CmdExpArray.GetSize();
		for (int i=0;i<m_CmdExpArray.GetSize();i++)
		{
			ar << m_CmdExpArray[i].strName;
			ar << m_CmdExpArray[i].strDescription;
			ar << m_CmdExpArray[i].strInput;
			ar << m_CmdExpArray[i].strOutput;
		}

		ar << m_CmdArray.GetSize();
		CCmd* cmd;
		for (int i=0;i<m_CmdArray.GetSize();i++)
		{
			cmd=(CCmd*)m_CmdArray[i];
			cmd->Serialize(ar);
		}

		m_ImageList.Write(&ar);
	}
	else //Mở
	{
		//m_ImageList.Create(32,32,ILC_COLOR32,0,0);
		int size;
		ar >> size;
		CString str1,str2,str3,str4;
		for (int i=0;i<size;i++)
		{
			ar >> str1;
			ar >> str2;
			ar >> str3;
			AddStdCommand(str1,str2,str3);
		}

		ar >> size;
		for (int i=0;i<size;i++)
		{
			ar >> str1;
			ar >> str2;
			ar >> str3;
			ar >> str4;
			AddExpCommand(str1,str2,str3,str4);
		}

		ar >> size;
		for (int i=0;i<size;i++)
		{
			CCmd* cmd=new CCmd();
			cmd->Serialize(ar);
			m_CmdArray.Add(cmd);
		}
		//AddCommand(L"PerpendicularBisector",L"",L"Point I:MidPoint(A,B)\n\nLine d:PerpendicularLine(I,Line d2(A,B)))",L"Point A,Point B",L"d",L"Perpendicular Bisector",0);
		m_ImageList.Read(&ar);
	}
}

void CCommandArray::AddCommand(CString m_Name,CString m_Des,CString m_Cmd,CString m_In,CString m_Out,CString m_ToolName,int m_imgIndex)
{
	CCmd* cmd;
	int i=0;

	for (i;i<m_CmdArray.GetSize();i++)
	{
		cmd=(CCmd*)m_CmdArray[i];
		if (cmd->m_CommandName==m_Name) break;
	}

	if (i==m_CmdArray.GetSize())
	{
		cmd=new CCmd();
		cmd->m_CommandName=m_Name;
	}

	SubCommand subCmd;
	subCmd.m_Command=m_Cmd;
	subCmd.m_Input=m_In;
	subCmd.m_Output=m_Out;
	subCmd.m_Description=m_Des;
	subCmd.m_ImageIndex=m_imgIndex;
	subCmd.m_ToolName=m_ToolName;

	cmd->m_SubCmdArray.Add(subCmd);

	if (i==m_CmdArray.GetSize()) m_CmdArray.Add(cmd);
}

void CCommandArray::AddStdCommand(CString strName,CString strDes,CString strIn)
{
	CStdCmd cmd;
	cmd.strName=strName;
	cmd.strDescription=strDes;
	cmd.strInput=strIn;
	m_CmdStdArray.Add(cmd);
}

void CCommandArray::AddExpCommand( CString strName,CString strDes,CString strIn,CString strOut )
{
	CExpCmd cmd;
	cmd.strName=strName;
	cmd.strDescription=strDes;
	cmd.strInput=strIn;
	cmd.strOutput=strOut;
	m_CmdExpArray.Add(cmd);
}

int CCommandArray::GetStdIndex(CString strName)
{
	for (int i=0;i<m_CmdStdArray.GetSize();i++)
	{
		if (m_CmdStdArray[i].strName==strName) return i;
	}
	return -1;
}

int CCommandArray::GetExpIndex(CString strName)
{
	for (int i=0;i<m_CmdExpArray.GetSize();i++)
	{
		if (m_CmdExpArray[i].strName==strName) return i;
	}
	return -1;
}

int CCommandArray::GetSize()
{
	return (int)m_CmdArray.GetSize();
}

int CCommandArray::GetToolImageIndex(CString toolName)
{
	for (int i=0;i<m_CmdArray.GetSize();i++)
	{
		for (int j=0;j<m_CmdArray[i]->GetSize();j++)
			if (m_CmdArray[i]->m_SubCmdArray[j].m_ToolName==toolName) return m_CmdArray[i]->m_SubCmdArray[j].m_ImageIndex;
	}
	return -1;
}

void CCommandArray::Delete(CString strName, CString strInput)
{
	int i=0;
	for (i;i<m_CmdArray.GetSize();i++)
	{
		if (m_CmdArray[i]->m_CommandName==strName) break;
	}
	
	int j=m_CmdArray[i]->GetIndexByInput(strInput);
	m_CmdArray[i]->m_SubCmdArray.RemoveAt(j);
	if (m_CmdArray[i]->m_SubCmdArray.GetSize()==0)
	{
		delete m_CmdArray[i];
		m_CmdArray.RemoveAt(i);
	}

}

CString CCommandArray::GetParam(CString strIn)
{
	CString strParam;
	CString strObjType;

	int i1=0;
	int i2=strIn.Find(',');
	while (i2!=-1)
	{
		strObjType=strIn.Mid(i1,i2-i1);
		int pos=strObjType.Find(' ');
		if (pos!=-1) strObjType=strObjType.Left(pos);
		strParam+=strObjType+',';
		i1=i2+1;
		i2=strIn.Find(',',i1+1);
	}
	strObjType=strIn.Mid(i1,strIn.GetLength()-i1);
	int pos=strObjType.Find(' ');
	if (pos!=-1) strObjType=strObjType.Left(pos);
	strParam+=strObjType;

	strParam.Replace(L",",L", ");
	return strParam;
}

CCommandArray::SubCommand* CCommandArray::GetSubCommandByToolName(CString strToolName)
{
	for (int i=0;i<m_CmdArray.GetSize();i++)
	{
		for (int j=0;j<m_CmdArray[i]->GetSize();j++)
		{
			if (m_CmdArray[i]->m_SubCmdArray[j].m_ToolName==strToolName) return &m_CmdArray[i]->m_SubCmdArray[j];
		}
	}
	return NULL;
}

// Tìm command
// Dạng có dấu cách
// VD : PerpendicularBisector (Point, Line)
void CCommandArray::GetCommandByDeclaration(CString strCmd, CCmd** cmd, SubCommand** subCmd)
{
	int pos=strCmd.Find(L" ");

	*cmd = GetCommandByString(strCmd.Left(pos));
	*subCmd=&(*cmd)->m_SubCmdArray[(*cmd)->GetIndexByInput(strCmd.Mid(pos+2,strCmd.GetLength()-pos-3))];
}

void CCommandArray::CCmd::Serialize(CArchive& ar)
{
	if (ar.IsStoring()) //Lưu
	{
		m_CommandName.Remove(' ');
		ar << m_CommandName;
		ar << m_SubCmdArray.GetSize();
		for (int i=0;i<m_SubCmdArray.GetSize();i++)
		{
			ar << m_SubCmdArray[i].m_Description;
			ar << m_SubCmdArray[i].m_Command;
			ar << m_SubCmdArray[i].m_Input;
			ar << m_SubCmdArray[i].m_Output;
			ar << m_SubCmdArray[i].m_ToolName;
			ar << m_SubCmdArray[i].m_ImageIndex;
		}
// 		*ar << m_CommandDes;
// 		m_Command.Serialize(*ar);
// 		m_Input.Serialize(*ar);
// 		m_Output.Serialize(*ar);
// 		m_ToolName.Serialize(*ar);
// 		//*ar << m_imageIndex;
// 		for (int i=0;i<m_Command.GetSize();i++)
// 			*ar << m_imageIndex[i];
	}
	else //Mở
	{
		ar >> m_CommandName;
		int count;
		ar >> count;
		for (int i=0;i<count;i++)
		{
			SubCommand subCmd;
			ar >> subCmd.m_Description;
			ar >> subCmd.m_Command;
			ar >> subCmd.m_Input;
			ar >> subCmd.m_Output;
			ar >> subCmd.m_ToolName;
			ar >> subCmd.m_ImageIndex;
			CString strParam;
			CString strObjType;

			m_SubCmdArray.Add(subCmd);
		}

// 		int i;
// 		*ar >> m_CommandName;
// 		*ar >> m_CommandDes;
// 		m_Command.Serialize(*ar);
// 		m_Input.Serialize(*ar);
// 		m_Output.Serialize(*ar);
// 		//m_imageIndex=0;
// 		*ar >> i;
// 		for (int i=0;i<m_Command.GetSize();i++)
// 		{
// 			SubCommand subCmd;
// 			subCmd.m_Command=m_Command[i];
// 			subCmd.m_Input=m_Input[i];
// 			subCmd.m_Output=m_Output[i];
// 			subCmd.m_ImageIndex=0;
// 			m_SubCmdArray.push_back(subCmd);
// 			//m_imageIndex.push_back(0);
// 		}
	}
}

//Trả về mảng kiểu dữ liệu
//VD Point A,Point B
// -> OBJ_POINT, OBJ_POINT
void CCommandArray::GetParamType(CString strParam, CArray<int>* m_TypeArray)
{
	int i1=0;
	int i2=strParam.Find(',');
	CString strObjType;
	while (i2!=-1)
	{
		strObjType=strParam.Mid(i1,i2-i1);
		int pos=strObjType.Find(' ');
		if (pos!=-1) strObjType=strObjType.Left(pos);

		m_TypeArray->Add(GetObjType(strObjType));
		
		i1=i2+1;
		i2=strParam.Find(',',i1+1);
	}
	strObjType=strParam.Mid(i1,strParam.GetLength()-i1);
	int pos=strObjType.Find(' ');
	if (pos!=-1) strObjType=strObjType.Left(pos);
	m_TypeArray->Add(GetObjType(strObjType));
}

void CCommandArray::GetParamText(CString strParam, vector<CString>& m_strArray)
{
	int i1=0;
	int i2=strParam.Find(',');
	CString strObjType;
	while (i2!=-1)
	{
		strObjType=strParam.Mid(i1,i2-i1);
		int pos=strObjType.Find(' ');
		if (pos!=-1) strObjType=strObjType.Left(pos);

		m_strArray.push_back(strObjType);

		i1=i2+1;
		i2=strParam.Find(',',i1+1);
	}
	strObjType=strParam.Mid(i1,strParam.GetLength()-i1);
	int pos=strObjType.Find(' ');
	if (pos!=-1) strObjType=strObjType.Left(pos);
	m_strArray.push_back(strObjType);
}

int CCommandArray::GetParamCount( CString strParam )
{
	int count=1;
	for (int i=0;i<strParam.GetLength();i++)
		if (strParam[i]==',') count++;
	return count;
}

int CCommandArray::CCmd::GetIndexByInput(CString input)
{
	for (int i=0;i<GetSize();i++)
		if (GetParam(m_SubCmdArray[i].m_Input)==input) return i;
	return -1;
}

int CCommandArray::CCmd::GetSize()
{
	return m_SubCmdArray.GetSize();
}

CCommandArray::SubCommand* CCommandArray::CCmd::operator [](int nIndex)
{
	return &m_SubCmdArray[nIndex];
}