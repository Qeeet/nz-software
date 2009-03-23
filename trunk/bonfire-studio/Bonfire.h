// Bonfire.h : main header file for the BONFIRE application
//

#if !defined(AFX_BONFIRE_H__43A640EC_B2FE_46BA_BE58_F0BB7FC33A82__INCLUDED_)
#define AFX_BONFIRE_H__43A640EC_B2FE_46BA_BE58_F0BB7FC33A82__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "commctrl.h"
#include "BonfireView.h"
#include <vector>

/////////////////////////////////////////////////////////////////////////////
// CBonfireApp:

// Constants
#define APP_COLOR_BACKGROUND		1
#define APP_COLOR_TEXT				2
#define APP_COLOR_SEL_BACKGROUND	3
#define APP_COLOR_SEL_TEXT			4
#define APP_COLOR_ELEMENT			5
#define APP_COLOR_EL_NAME			6
#define APP_COLOR_EL_ATTRIB			7
#define APP_COLOR_STRING			8
#define APP_COLOR_COMMENT			9
#define APP_COLOR_PREPROCESSOR		10
#define APP_COLOR_KEYWORD			11
#define APP_COLOR_NUMBER			12
#define APP_COLOR_ASP				13

struct GENERAL_OPTIONS
{
	DWORD dwClearOutput;
	DWORD dwReloadFiles;
	DWORD dwSaveOnSwitch;
};

struct PROJECT_BAR_OPTIONS
{
	DWORD dwShowPathFiles;
	DWORD dwShowPathFolders;
	DWORD dwShowPathProjects;
	DWORD dwShowFoldersBeforeFiles;
};

struct EDITOR_OPTIONS
{
	char strFont[260];
	DWORD dwFontSize;

	DWORD dwColorBackground;
	DWORD dwColorText;
	DWORD dwColorSelBackground;
	DWORD dwColorSelText;
	DWORD dwColorElement;
	DWORD dwColorElementName;
	DWORD dwColorAttribute;
	DWORD dwColorString;
	DWORD dwColorComment;
	DWORD dwColorPreprocessor;
	DWORD dwColorKeyword;
	DWORD dwColorNumber;
	DWORD dwColorASP;
};

struct XMLTREE_OPTIONS
{
	char strFont[260];
	DWORD dwFontSize;

	DWORD dwShowAttributes;
	DWORD dwShowText;
	DWORD dwShowComments;
	DWORD dwEditXMLTree;
};

#define SYNHIGHLIGHT_NONE		0
#define SYNHIGHLIGHT_XML		1
#define SYNHIGHLIGHT_CPP		2
#define SYNHIGHLIGHT_ASP		3
#define SYNHIGHLIGHT_CSS		4
#define SYNHIGHLIGHT_SQL		5

struct VIEWS_OPTIONS_ASSOC
{
	char strExtensions[260];
	char strViews[10];
	int nHighlightType;
};

struct VIEWS_OPTIONS
{
	std::vector<VIEWS_OPTIONS_ASSOC> vAssociations;
};

struct APP_OPTIONS
{
	GENERAL_OPTIONS general;
	PROJECT_BAR_OPTIONS projbar;
	EDITOR_OPTIONS editor;
	XMLTREE_OPTIONS xmltree;
	VIEWS_OPTIONS views;
};

enum enMsgType
{
	MSG_NONE		= -1,
	MSG_SUCCESS		= 0,
	MSG_ERROR		= 1,
	MSG_PROCESSING	= 2,
	MSG_FIF			= 3
};

enum enOutputType
{
	OUTPUT_APPSTATUS = 0,
	OUTPUT_FIF_RESULT = 1,
	OUTPUT_XMLERROR = 2
};

struct OutputMessage
{
	enOutputType	enType;
	CString			strMessage;
	enMsgType		enResult;
	//union
	//{
	//	struct
	//	{
			CString strResult;
	//	};
	//	struct
	//	{
			int nLine;
			int nCol;
			int nSelSize;
			CString strLine;
	//	};
	//};
	DWORD dwData;
};

class CBonfireDoc;

class CBonfireApp : public CWinApp
{
// operations
public:
	CBonfireApp();
	void LoadCustomProfileSettings();
	void SaveCustomProfileSettings();
	CBonfireDoc* OpenExistingDocumentFile(LPCTSTR lpszFileName);
	COLORREF GetAppColor(int nColorIndex, EDITOR_OPTIONS* opt = NULL,
		BOOL bFixDefault = TRUE);
	void SetAppColor(int nColorIndex, COLORREF clr, EDITOR_OPTIONS* opt = NULL);
	void AddToRecentProjectList(LPCTSTR lpszPathName);

// member variables
private:
	CRecentFileList*		m_pRecentProjectList;
	//CMap<CString, CString&, CString, CString&>	m_pTempFiles;
public:
	void AddTempFile(CString strTempFile);
	void DeleteTempFiles();
	APP_OPTIONS				m_opOptions;
	BOOL					m_bClosing;

	//CBonfireView vwSource(RUNTIME_CLASS(CSourceView), "Source", TRUE, FALSE);
	//CBonfireView vwXMLTree(RUNTIME_CLASS(CXMLTreeView), "XML Tree", FALSE, TRUE);
	//CBonfireView vwBrowser(RUNTIME_CLASS(CBrowserView), "Browser", FALSE, TRUE);
	//CBonfireView m_arAllViews[] = {vwSource, vwXMLTree, vwBrowser};
	//const int nViews = 3;
	CArray <CBonfireView*, CBonfireView*> m_arAllViews;
	//m_arAllViews2.Add(vwSource);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBonfireApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
	virtual BOOL SaveAllModified();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CBonfireApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileOpen();
	afx_msg void OnProjectOpen();
	afx_msg void OnProjectNew();
	afx_msg void OnHelpHomepage();
	afx_msg void OnHelpContents();
	afx_msg void OnHelpIndex();
	afx_msg void OnHelpSearch();
	afx_msg BOOL OnOpenRecentProject(UINT nID);
	afx_msg void OnUpdateProjectMRU(CCmdUI* pCmdUI);
	afx_msg void OnFileOpenFromUrl();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void LoadStdProfileSettings(UINT nMaxMRU = _AFX_MRU_COUNT);
	void SaveStdProfileSettings();
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BONFIRE_H__43A640EC_B2FE_46BA_BE58_F0BB7FC33A82__INCLUDED_)
