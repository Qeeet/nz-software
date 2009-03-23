// ProjectBar.h: interface for the CProjectBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROJECTBAR_H__F8BCC367_030F_4871_9CC5_525257C67CF4__INCLUDED_)
#define AFX_PROJECTBAR_H__F8BCC367_030F_4871_9CC5_525257C67CF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "XNode.h"
#include "BonfireDoc.h"
#include "NZProjectTreeCtrl.h"

#include <fstream>
#include <vector>
//#include "msxml2.h"

#define ICON_PROJECT			0
#define ICON_FOLDER_CLOSED		1
//#define ICON_LINK_OVERLAY		2
#define ICON_FOLDER_OPEN		3
#define ICON_FILE_NOTFOUND		4
#define ICON_FILE_NEW			5
#define ICON_FOLDER_NOTFOUND	6
#define ICON_FILE_NONE			7
#define ICON_LINK_OVERLAY		8

enum CopyMoveAction
{
	CMA_NONE,
	CMA_COPY,
	CMA_MOVE
};


class CProjectBar : public SIZEBAR_BASECLASS  
{
	DECLARE_DYNAMIC(CProjectBar);
private:
	bool		SetCurrentNode(BOOL bHitTest = FALSE);
	int			m_imgNewFile;
	int			nNewProjects;
	int			nNewFolders;
	BOOL		m_bExpanding;
	CXProject*	m_pCurProject;
	CXNode*		m_pCurNode;
	CXProject*	m_pMiscProject;
	CNZProjectTreeCtrl m_wndTree;
	//CTreeCtrl	m_wndTree;
	CImageList	m_imlTreeImages;
	//CToolBar	m_wndButtons;

public:
	std::vector<CXProject*> m_arrProjects;

	CProjectBar();
	virtual ~CProjectBar();
// Overrides
public:
	void MoveFileToProject(UINT nID);
	void RefreshProjectManagerTree();
	void RefreshCurrentProject();
	BOOL IsEditingAllowed(CXNode* pNode);
	void EditCurNodeText();
	void UpdateProjectNewFolder(CCmdUI* pCmdUI);
	void DeleteFileNode(CXFile* pFile);
	void DrawNode(CXNode* pNode);
	void UpdateNode(CXNode* pNode, BOOL bRecurse);
	//void ShowToolsOptions();
	CXProject* GetCurrentProject();
	CXNode* GetCurrentNode();
	void SetCurrentProject(CXProject* pProject);
	bool CloseCurrentProject();
	BOOL CloseAllProjects();
	BOOL SaveAllProjects();
	void UpdateFileShow(CCmdUI* pCmdUI);
	// ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CProjectBar)
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL
	void SelectTreeNode(CXNode* pCurNode);
	void CreateMiscProject();
	void CreateNewProject();
	BOOL AddNodeToTree(CXNode* pNode, CXFolder* pParent, int nCopyMove = CMA_NONE, BOOL bDraw = TRUE, BOOL bOpen = FALSE);
	void LinkTreeNodeToChild(CXFile* pFile, CBonfireDoc* pChildDoc);	
	CXFile* LoadFile(CBonfireDoc* pNewDocument);
	bool LoadProject(LPCTSTR strProjectPath);
	void PopulateFolderTree(CXFolder* pNode);
	BOOL AddChildren(CXFolder* pParentFolder, CString strFileFilter, BOOL bSubfolders);
	void ProjectAddFilesToProject();
	void ProjectAddDirToProject();
	void ProjectRemoveFromProject();
	void ProjectSave();
	void ProjectSaveAs();
	bool ProjectClose(CXProject* pProject);
	void ProjectNewFolder();

	void UpdateProjectRefresh(CCmdUI *pCmdUI);
	void UpdateProjectAddFilesToProject(CCmdUI* pCmdUI);
	void UpdateProjectAddDirToProject(CCmdUI* pCmdUI);
	void UpdateProjectRemoveFromProject(CCmdUI* pCmdUI);
	void UpdateProjectClose(CCmdUI* pCmdUI);
	void UpdateProjectSave(CCmdUI* pCmdUI);
	void UpdateProjectSaveAs(CCmdUI* pCmdUI);
	void UpdateMoveFileToProject(CCmdUI* pCmdUI);
	void UpdateRenameProject(CCmdUI* pCmdUI);
	void UpdateRenameFolder(CCmdUI* pCmdUI);
	void UpdateRenameFile(CCmdUI* pCmdUI);
	
	BOOL DeleteNode(CXNode* pDelNode, BOOL bMoving = FALSE);
	CBonfireDoc* OpenDocument(CXFile* pFileNode);
	void OpenCurrentDocument();
	void ShowFileProperties(BOOL bProj = FALSE);
		
// Generated message map functions
protected:
    //{{AFX_MSG(CProjectBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	//}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_PROJECTBAR_H__F8BCC367_030F_4871_9CC5_525257C67CF4__INCLUDED_)
