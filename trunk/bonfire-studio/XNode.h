// XNode.h: interface for the CXNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XNODE_H__D3AE1422_F8B5_4BB8_AFAE_8EED7A9024CB__INCLUDED_)
#define AFX_XNODE_H__D3AE1422_F8B5_4BB8_AFAE_8EED7A9024CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <string.h>
#include <io.h>
#include "StdAfx.h"
#include "SortedArray.h"

enum NodeType
{
	XNODE_UNKNOWN,
	XNODE_FILE,
	XNODE_PROJECT,
	XNODE_FOLDER,
	XNODE_VIRTUALFOLDER
};

class CXFolder;
class CXProject;
class CBonfireDoc;
struct OutputMessage;
//template<class TYPE, class ARG_TYPE> class CSortedArray;

class CXNode : public CObject  
{
public:
	static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	CString			m_strName;
	CString			m_strPath;
	HTREEITEM		m_hTreeItem;
	CXFolder*		m_pParent;
	CXProject*		m_pProject;
	NodeType		m_enNodeType;

	CXNode();
	CXNode(const CXNode& _src);
	virtual ~CXNode();
	virtual CXNode& operator=(const CXNode& _src);
	CXNode* Clone();
	virtual CString FullPath();
	static int CompareNodes(CXNode* pNode1, CXNode* pNode2);
};

class CXFile : public CXNode  
{
public:
	bool			m_bSaved;
	bool			m_bNew;
	bool			m_bReadOnly;
	CBonfireDoc*	m_pDoc;

	CXFile();
	CXFile(const CXFile& _src);
	virtual ~CXFile();
	virtual CXFile& operator=(const CXFile& _src);
	CXFile* Clone();
	void Update(CString strFullPath, bool bSaved = TRUE); 
};

class CXFolder : public CXNode  
{
public:
	bool					m_bVirtual;
	std::vector<CXFolder*>	m_arrFolders;
	std::vector<CXFile*>	m_arrFiles;
	CSortedArray<CXNode*, CXNode*> m_arrFiles2;

	CXFolder();
	CXFolder(const CXFolder& _src);
	virtual ~CXFolder();
	virtual CXFolder& operator=(const CXFolder& _src);
	CXFolder* Clone();
	virtual CString FullPath();
	BOOL GenerateXML(CString &strSoFar);
	void SortChildren( CTreeCtrl* p_wndTreeCtrl );
	void UpdateChildren();
};

class CXProject : public CXFolder  
{
public:
	bool		m_bReadOnly;
	bool		m_bSaved;
	bool		m_bNew;
	bool		m_bMisc;

	CXProject();
	CXProject(bool bMisc, bool bNew, bool bSaved);
	CXProject(const CXProject& _src);
	virtual ~CXProject();
	virtual CXProject& operator=(const CXProject& _src);
	CXProject* Clone();
	CString FullPath();
	bool LoadProject(LPCTSTR strProjectPath, OutputMessage* pMsg);
	bool SaveProject();
	bool SaveProjectAs();
	bool SaveProjectToPath(LPCTSTR strProjectPath);
	void CreateProjectTree(IXMLDOMNode* pXNode, CXFolder* fParent);
	void SetModified(BOOL bModified = TRUE);
};

#endif // !defined(AFX_XNODE_H__D3AE1422_F8B5_4BB8_AFAE_8EED7A9024CB__INCLUDED_)
