// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__0D1E4CE1_14FC_4F5A_BCC7_0D46388D735E__INCLUDED_)
#define AFX_MAINFRM_H__0D1E4CE1_14FC_4F5A_BCC7_0D46388D735E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProjectBar.h"
#include "PropBar.h"
#include "OutputBar.h"
//#include "ToolBarEx.h"
#include "NZToolBar.h"
#include "BCMenu.h"

// toolbar stuff
#define TOOLBARCLS		CNZToolBar
#define TBB_L			2
#define TBB_T			2
#define TBB_R			3
#define TBB_B			2

// main frame class
class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL VerifyBarState(LPCTSTR lpszProfileName);
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	// image list for the menus (256 color)
	CImageList m_imlMenuIcons;

	// bitmaps for the toolbars (256 color)
	//CBitmap m_bmpToolBar;
	//CBitmap m_bmpProjectToolBar;
	//CBitmap m_bmpFindToolBar;

	// imagelists for the toolbars (256 color)
	CImageList m_imlStandardTB,		m_imlStandardTBDisabled;
	CImageList m_imlProjectTB,		m_imlProjectTBDisabled;
	CImageList m_imlFindTB,			m_imlFindTBDisabled;

	// BCMenu code
	//HMENU NewMenu();
	//HMENU NewDefaultMenu();
	//HMENU NewMenu(BOOL bChild);
	HMENU SetMenuIcons(BCMenu* pMenu);

	BCMenu m_mnMainFrame;
	BCMenu m_mnBonfireType;

public:  // control bar embedded members
	void LoadUIState(BOOL bLoadWindowPlacement = FALSE);
	void SaveUIState();
	void EscapeFullScreen();
	void AddOutputError(CString message);

	CStatusBar	m_wndStatusBar;
	TOOLBARCLS	m_wndToolBar;
	TOOLBARCLS	m_wndProjectToolBar;
	TOOLBARCLS	m_wndFindToolBar;
	TOOLBARCLS	m_wndFullScreenToolBar;
	CProjectBar	m_wndProjectBar;
	CPropBar	m_wndPropBar;
	COutputBar	m_wndOutputBar;
	BOOL		m_bFullScreen;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnProjectAddFilesToProject();
	afx_msg void OnUpdateProjectAddFilesToProject(CCmdUI* pCmdUI);
	afx_msg void OnProjectRemoveFromProject();
	afx_msg void OnUpdateProjectRemoveFromProject(CCmdUI* pCmdUI);
	afx_msg void OnProjectAddDirToProject();
	afx_msg void OnUpdateProjectAddDirToProject(CCmdUI* pCmdUI);
	afx_msg void OnUpdateProjectSave(CCmdUI* pCmdUI);
	afx_msg void OnProjectSave();
	afx_msg void OnViewOutput();
	afx_msg void OnUpdateViewOutput(CCmdUI* pCmdUI);
	afx_msg void OnViewProjectExplorer();
	afx_msg void OnUpdateViewProjectExplorer(CCmdUI* pCmdUI);
	afx_msg void OnViewProperties();
	afx_msg void OnUpdateViewProperties(CCmdUI* pCmdUI);
	afx_msg void OnFileShow();
	afx_msg void OnUpdateFileShow(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCurPosIndicator(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOverwriteIndicator(CCmdUI* pCmdUI);
	afx_msg void OnUpdateReadonlyIndicator(CCmdUI* pCmdUI);
	afx_msg void OnProjectClose();
	afx_msg void OnUpdateProjectClose(CCmdUI* pCmdUI);
	afx_msg void OnFileProperties();
	afx_msg void OnToolsOptions();
	afx_msg void OnProjectSaveas();
	afx_msg void OnUpdateProjectSaveas(CCmdUI* pCmdUI);
	afx_msg void OnProjectProperties();
	afx_msg void OnViewFullscreen();
	afx_msg void OnUpdateViewFullscreen(CCmdUI* pCmdUI);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg LRESULT OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnViewToolbarFind();
	afx_msg void OnUpdateViewToolbarFind(CCmdUI* pCmdUI);
	afx_msg void OnViewToolbarProject();
	afx_msg void OnUpdateViewToolbarProject(CCmdUI* pCmdUI);
	afx_msg void OnFileSaveall();
	afx_msg void OnUpdateFileSaveall(CCmdUI* pCmdUI);
	afx_msg void OnProjectNewFolder();
	afx_msg void OnUpdateProjectNewFolder(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileRename(CCmdUI* pCmdUI);
	afx_msg void OnFileRename();
	afx_msg void OnProjectRename();
	afx_msg void OnUpdateProjectRename(CCmdUI* pCmdUI);
	afx_msg void OnFolderRename();
	afx_msg void OnUpdateFolderRename(CCmdUI* pCmdUI);
	afx_msg void OnProjectRefresh();
	afx_msg void OnUpdateProjectRefresh(CCmdUI* pCmdUI);
	afx_msg void OnWindowCloseAll();
	afx_msg void OnToolsLocateInExplorer();
	afx_msg void OnUpdateToolsLocateInExplorer(CCmdUI* pCmdUI);
	afx_msg void OnFindInFiles();
	afx_msg BOOL OnFileMoveToProject(UINT nID);
	afx_msg void OnUpdateFileMoveToProject(CCmdUI* pCmdUI);
	afx_msg void OnToolsOpenDefault();
	afx_msg void OnUpdateToolsOpenDefault(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__0D1E4CE1_14FC_4F5A_BCC7_0D46388D735E__INCLUDED_)
