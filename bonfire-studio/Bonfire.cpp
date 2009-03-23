// Bonfire.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Globals.h"
#include "Bonfire.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "BonfireDoc.h"
#include "BonfireView.h"
#include "SourceView.h"
#include "VIRecentFileList.h"
#include "SyntaxParsers.h"
#include "OpenFromURLDlg.h"
#include "XMLTreeView.h"
#include "BrowserView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBonfireApp

BEGIN_MESSAGE_MAP(CBonfireApp, CWinApp)
	//{{AFX_MSG_MAP(CBonfireApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_PROJECT_OPEN, OnProjectOpen)
	ON_COMMAND(ID_PROJECT_NEW, OnProjectNew)
	ON_COMMAND(ID_HELP_HOMEPAGE, OnHelpHomepage)
	ON_COMMAND(ID_HELP_CONTENTS, OnHelpContents)
	ON_COMMAND(ID_HELP_INDEX, OnHelpIndex)
	ON_COMMAND(ID_HELP_SEARCH, OnHelpSearch)
	ON_COMMAND_EX_RANGE(ID_FILE_MRU_PROJECT1, ID_FILE_MRU_PROJECT4, OnOpenRecentProject)
	ON_UPDATE_COMMAND_UI(ID_FILE_MRU_PROJECT1, OnUpdateProjectMRU)
	ON_COMMAND(ID_FILE_OPENFROMURL, OnFileOpenFromUrl)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

//AFX_STATIC_DATA 
const TCHAR m_strFileSection[] = _T("Recent File List");
const TCHAR m_strFileEntry[] = _T("File%d");
const TCHAR m_strProjectSection[] = _T("Recent Project List");
const TCHAR m_strProjectEntry[] = _T("Project%d");
const TCHAR m_strPreviewSection[] = _T("Settings");
const TCHAR m_strPreviewEntry[] = _T("PreviewPages");

/////////////////////////////////////////////////////////////////////////////
// CBonfireApp construction
/////////////////////////////////////////////////////////////////////////////
CBonfireApp::CBonfireApp()
{
	// TODO: add construction code here,
	m_arAllViews.Add(new CBonfireView(RUNTIME_CLASS(CSourceView), "Source", TRUE, FALSE));
	m_arAllViews.Add(new CBonfireView(RUNTIME_CLASS(CXMLTreeView), "XML Tree", FALSE, TRUE));
	m_arAllViews.Add(new CBonfireView(RUNTIME_CLASS(CBrowserView), "Browser", FALSE, TRUE));
	
	// this is a HACK because MFC sucks. Inorder to determine whether
	// to close or simply save projects in SaveAllModified we need this
	m_bClosing = TRUE;
	
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CBonfireApp object
/////////////////////////////////////////////////////////////////////////////
CBonfireApp theApp;

/////////////////////////////////////////////////////////////////////////////
// global pointer to the main form
/////////////////////////////////////////////////////////////////////////////
CMainFrame* g_pMainFrame;

/////////////////////////////////////////////////////////////////////////////
// global file watch notification message
/////////////////////////////////////////////////////////////////////////////
UINT g_msgFileWatchNotify;

/////////////////////////////////////////////////////////////////////////////
// CBonfireApp initialization
/////////////////////////////////////////////////////////////////////////////
BOOL CBonfireApp::InitInstance()
{
	AfxEnableControlContainer();

	//CoInitialize(NULL);
	//OleInitialize(NULL); // for drag and drop in source view

	// the MFC way of doing OLE initialization
	if (!AfxOleInit())
		return FALSE;

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.


#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif


	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("NZ Software"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)
	LoadCustomProfileSettings();

///////////////////////////////////////////////////////////////////////////
	// Load the parsers
	InitParsers();

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_BONFIRETYPE,
		//IDR_MAINFRAME,
		RUNTIME_CLASS(CBonfireDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CSourceView));
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	g_pMainFrame = new CMainFrame;
	
	if (!g_pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;

	m_pMainWnd = g_pMainFrame;

///////////////////////////////////////////////////////////////////////////
	// create main MDI Frame window

	//BOOL b = IsWindow(g_pMainFrame->m_wndProjectToolBar);

	// This code replaces the MFC created menus with the Ownerdrawn versions 
	//pDocTemplate->m_hMenuShared		= g_pMainFrame->NewMenu();
	//g_pMainFrame->m_hMenuDefault	= g_pMainFrame->NewDefaultMenu();

	g_pMainFrame->m_mnMainFrame.LoadMenu(IDR_MAINFRAME);
	g_pMainFrame->m_mnBonfireType.LoadMenu(IDR_BONFIRETYPE);

	pDocTemplate->m_hMenuShared =  g_pMainFrame->SetMenuIcons(&g_pMainFrame->m_mnBonfireType);
	g_pMainFrame->m_hMenuDefault = g_pMainFrame->SetMenuIcons(&g_pMainFrame->m_mnMainFrame);

	// This simulates a window being opened if you don't have
	// a default window displayed at startup
	g_pMainFrame->OnUpdateFrameMenu(g_pMainFrame->m_hMenuDefault);

	// load the user interface settings
	g_pMainFrame->LoadUIState(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew)
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;

	// ******* see if need to open a project (.xpr extension) *******
	if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen &&
		GetFileExtension(cmdInfo.m_strFileName) == "xpr")
	{		
		((CMainFrame*)AfxGetMainWnd())->m_wndProjectBar.LoadProject(cmdInfo.m_strFileName);
	}
	else
	{
		// Dispatch commands specified on the command line
		if (!ProcessShellCommand(cmdInfo))
			return FALSE;
	}

	// The main window has been initialized, so show and update it.
	g_pMainFrame->ShowWindow(m_nCmdShow);
	g_pMainFrame->UpdateWindow();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// when the application is exiting
/////////////////////////////////////////////////////////////////////////////
int CBonfireApp::ExitInstance() 
{
	SaveCustomProfileSettings();

	if (m_pCmdInfo == NULL ||
		m_pCmdInfo->m_nShellCommand != CCommandLineInfo::AppUnregister)
	{
		if (!afxContextIsDLL)
			SaveStdProfileSettings();
	}
	
	// do some major cleanup
	DeleteTempFiles();

	try
	{
		for (int i = 0; i < m_arAllViews.GetSize(); i++)
			delete m_arAllViews[i];
	} catch(...) {}

	delete m_pRecentProjectList;

	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// load MRU lists from registry
/////////////////////////////////////////////////////////////////////////////
void CBonfireApp::LoadStdProfileSettings(UINT nMaxMRU)
{
	ASSERT_VALID(this);
	ASSERT(m_pRecentFileList == NULL);

	// load the MRU lists from registry
	if (nMaxMRU != 0)
	{
		// Here's the important part--overriding CRecentFileList
		m_pRecentFileList = new VIRecentFileList(0, m_strFileSection, m_strFileEntry, nMaxMRU);
		m_pRecentProjectList = new VIRecentFileList(0, m_strProjectSection, m_strProjectEntry, nMaxMRU);

		m_pRecentFileList->ReadList();
		m_pRecentProjectList->ReadList();
	}

	// 0 by default means not set
	m_nNumPreviewPages = GetProfileInt(m_strPreviewSection, m_strPreviewEntry, 0);
}

/////////////////////////////////////////////////////////////////////////////
// load UI and other preferences from regisry
/////////////////////////////////////////////////////////////////////////////
void CBonfireApp::LoadCustomProfileSettings()
{
	CString strFont							= GetProfileString("Editor","Font","Courier New");
	strcpy(m_opOptions.editor. strFont, strFont);
	m_opOptions.editor.dwFontSize			= GetProfileInt("Editor","FontSize",10);
	m_opOptions.editor.dwColorBackground	= GetProfileInt("Editor\\Colors","Background",0xFFFFFFFF);
	m_opOptions.editor.dwColorText			= GetProfileInt("Editor\\Colors","Text",0xFFFFFFFF);
	m_opOptions.editor.dwColorSelBackground	= GetProfileInt("Editor\\Colors","SelBackground",0xFFFFFFFF);
	m_opOptions.editor.dwColorSelText		= GetProfileInt("Editor\\Colors","SelText",0xFFFFFFFF);
	m_opOptions.editor.dwColorElement		= GetProfileInt("Editor\\Colors","Element",0xFFFFFFFF);
	m_opOptions.editor.dwColorElementName	= GetProfileInt("Editor\\Colors","ElementName",0xFFFFFFFF);
	m_opOptions.editor.dwColorAttribute		= GetProfileInt("Editor\\Colors","Attribute",0xFFFFFFFF);
	m_opOptions.editor.dwColorString		= GetProfileInt("Editor\\Colors","String",0xFFFFFFFF);
	m_opOptions.editor.dwColorComment		= GetProfileInt("Editor\\Colors","Comment",0xFFFFFFFF);
	m_opOptions.editor.dwColorPreprocessor	= GetProfileInt("Editor\\Colors","Preprocessor",0xFFFFFFFF);
	m_opOptions.editor.dwColorKeyword		= GetProfileInt("Editor\\Colors","Keyword",0xFFFFFFFF);
	m_opOptions.editor.dwColorNumber		= GetProfileInt("Editor\\Colors","Number",0xFFFFFFFF);
	m_opOptions.editor.dwColorASP			= GetProfileInt("Editor\\Colors","ASP",0xFFFFFFFF);

	m_opOptions.projbar.dwShowPathFiles		= GetProfileInt("ProjectBar","spFiles",0x00000001);
	m_opOptions.projbar.dwShowPathFolders	= GetProfileInt("ProjectBar","spFolders",0x00000001);
	m_opOptions.projbar.dwShowPathProjects	= GetProfileInt("ProjectBar","spProjects",0x00000001);
	m_opOptions.projbar.dwShowFoldersBeforeFiles = GetProfileInt("ProjectBar","spFoldersFirst",0x00000001);

	m_opOptions.general.dwClearOutput		= GetProfileInt("General","clearOutput",0x00000001);
	m_opOptions.general.dwReloadFiles		= GetProfileInt("General","reloadFiles",0x00000001);
	m_opOptions.general.dwSaveOnSwitch		= GetProfileInt("General","saveOnSwitch",0x00000001);

	CString strXMLTreeFont					= GetProfileString("XMLTree","Font","Tahoma");
	strcpy(m_opOptions.xmltree. strFont, strXMLTreeFont);
	m_opOptions.xmltree.dwFontSize			= GetProfileInt("XMLTree","FontSize",8);
	m_opOptions.xmltree.dwShowAttributes	= GetProfileInt("XMLTree","showAttributes",0x00000001);
	m_opOptions.xmltree.dwShowText			= GetProfileInt("XMLTree","showText",0x00000001);
	m_opOptions.xmltree.dwShowComments		= GetProfileInt("XMLTree","showComments",0x00000001);
	m_opOptions.xmltree.dwEditXMLTree		= GetProfileInt("XMLTree","editXMLTree",0x00000001);

	m_opOptions.views.vAssociations.erase(
		m_opOptions.views.vAssociations.begin(),
		m_opOptions.views.vAssociations.end());

	// load views options
	HKEY hKeyViews = GetSectionKey("Views");
	if (hKeyViews)
	{
		char strName[1000], strData[1000];
		DWORD dwNameSize, dwDataSize, dwDataType;
		DWORD dwIndex = 0;

		while (1)
		{
			dwNameSize = sizeof(strName);
			dwDataSize = sizeof(strData);

			if (RegEnumValue(hKeyViews,dwIndex,strName,&dwNameSize,
				NULL,&dwDataType,(BYTE*)strData,&dwDataSize) == ERROR_NO_MORE_ITEMS)
				break;

			if (dwDataType == REG_SZ)
			{
				VIEWS_OPTIONS_ASSOC temp;
				if (dwNameSize)
					strcpy(temp.strExtensions,strName);
				else
					temp.strExtensions[0] = 0;

				if (dwDataSize >= 2)
				{
					strcpy(temp.strViews,strData + 1);
					temp.nHighlightType = strData[0] - '0';
				}
				else
				{
					strcpy(temp.strViews,"s--");
					temp.nHighlightType = SYNHIGHLIGHT_NONE;
				}

				if (strcmp(strName,"") == 0)
					m_opOptions.views.vAssociations.insert(
						m_opOptions.views.vAssociations.begin(),temp);
				else
					m_opOptions.views.vAssociations.push_back(temp);
			}

			dwIndex++;
		}

		// must have default
		if (m_opOptions.views.vAssociations.size() == 0)
		{
			VIEWS_OPTIONS_ASSOC temp;
			temp.strExtensions[0] = 0;
			strcpy(temp.strViews,"sxb");
			temp.nHighlightType = SYNHIGHLIGHT_NONE;
			m_opOptions.views.vAssociations.push_back(temp);
		}

		RegCloseKey(hKeyViews);
	}
}

/////////////////////////////////////////////////////////////////////////////
// save MRU lists to registry
/////////////////////////////////////////////////////////////////////////////
void CBonfireApp::SaveStdProfileSettings()
{
	ASSERT_VALID(this);

	// write the MRU lists to registry
	if (m_pRecentFileList != NULL)
		m_pRecentFileList->WriteList();

	if (m_pRecentProjectList != NULL)
		m_pRecentProjectList->WriteList();

	if (m_nNumPreviewPages != 0)
		WriteProfileInt(m_strPreviewSection, m_strPreviewEntry, m_nNumPreviewPages);
}

/////////////////////////////////////////////////////////////////////////////
// save UI and other preferences to regisry
/////////////////////////////////////////////////////////////////////////////
void CBonfireApp::SaveCustomProfileSettings()
{
	WriteProfileString("Editor","Font",m_opOptions.editor.strFont);
	WriteProfileInt("Editor","FontSize",(int)m_opOptions.editor.dwFontSize);

	WriteProfileInt("Editor\\Colors","Background",(int)m_opOptions.editor.dwColorBackground);
	WriteProfileInt("Editor\\Colors","Text",(int)m_opOptions.editor.dwColorText);
	WriteProfileInt("Editor\\Colors","SelBackground",(int)m_opOptions.editor.dwColorSelBackground);
	WriteProfileInt("Editor\\Colors","SelText",(int)m_opOptions.editor.dwColorSelText);
	WriteProfileInt("Editor\\Colors","Element",(int)m_opOptions.editor.dwColorElement);
	WriteProfileInt("Editor\\Colors","ElementName",(int)m_opOptions.editor.dwColorElementName);
	WriteProfileInt("Editor\\Colors","Attribute",(int)m_opOptions.editor.dwColorAttribute);
	WriteProfileInt("Editor\\Colors","String",(int)m_opOptions.editor.dwColorString);
	WriteProfileInt("Editor\\Colors","Comment",(int)m_opOptions.editor.dwColorComment);
	WriteProfileInt("Editor\\Colors","Preprocessor",(int)m_opOptions.editor.dwColorPreprocessor);
	WriteProfileInt("Editor\\Colors","Keyword",(int)m_opOptions.editor.dwColorKeyword);
	WriteProfileInt("Editor\\Colors","Number",(int)m_opOptions.editor.dwColorNumber);
	WriteProfileInt("Editor\\Colors","ASP",(int)m_opOptions.editor.dwColorASP);

	WriteProfileInt("ProjectBar","spFiles",(int)m_opOptions.projbar.dwShowPathFiles);
	WriteProfileInt("ProjectBar","spFolders",(int)m_opOptions.projbar.dwShowPathFolders);
	WriteProfileInt("ProjectBar","spProjects",(int)m_opOptions.projbar.dwShowPathProjects);
	WriteProfileInt("ProjectBar","spFoldersFirst",(int)m_opOptions.projbar.dwShowFoldersBeforeFiles);

	WriteProfileInt("General","clearOutput",(int)m_opOptions.general.dwClearOutput);
	WriteProfileInt("General","reloadFiles",(int)m_opOptions.general.dwReloadFiles);
	WriteProfileInt("General","saveOnSwitch",(int)m_opOptions.general.dwSaveOnSwitch);

	WriteProfileInt("XMLTree","showAttributes",(int)m_opOptions.xmltree.dwShowAttributes);
	WriteProfileInt("XMLTree","showText",(int)m_opOptions.xmltree.dwShowText);
	WriteProfileInt("XMLTree","showComments",(int)m_opOptions.xmltree.dwShowComments);
	WriteProfileString("XMLTree","Font",m_opOptions.xmltree.strFont);
	WriteProfileInt("XMLTree","FontSize",(int)m_opOptions.xmltree.dwFontSize);
	WriteProfileInt("XMLTree","editXMLTree",(int)m_opOptions.xmltree.dwEditXMLTree);
	// save views options

	// first delete key, then recreate it (clears the list of extensions)
	HKEY hAppKey = GetAppRegistryKey();
	if (hAppKey)
	{
		RegDeleteKey(hAppKey,"Views");
		RegCloseKey(hAppKey);
	}

	HKEY hKeyViews = GetSectionKey("Views"); // creates the Views key
	if (hKeyViews)
	{
		char str[256];

		size_t nNumAssoc = m_opOptions.views.vAssociations.size();
		while (nNumAssoc--)
		{
			sprintf(str,"%d%s",m_opOptions.views.vAssociations[nNumAssoc].nHighlightType,
				m_opOptions.views.vAssociations[nNumAssoc].strViews);
			DWORD dwValLen = strlen(str);
			RegSetValueEx(hKeyViews,
				m_opOptions.views.vAssociations[nNumAssoc].strExtensions,0,
				REG_SZ,(const BYTE*)str,dwValLen);
		}

		RegCloseKey(hKeyViews);
	}
}

COLORREF CBonfireApp::GetAppColor(int nColorIndex, EDITOR_OPTIONS* opt, BOOL bFixDefault)
{
	if (opt == NULL)
		opt = &m_opOptions.editor;

	DWORD dwVal;
	COLORREF clrDefault;

	switch (nColorIndex)
	{
	case APP_COLOR_BACKGROUND:
		dwVal = opt->dwColorBackground;
		clrDefault = GetSysColor(COLOR_WINDOW); break;

	case APP_COLOR_TEXT:
		dwVal = opt->dwColorText;
		clrDefault = GetSysColor(COLOR_WINDOWTEXT); break;

	case APP_COLOR_SEL_BACKGROUND:
		dwVal = opt->dwColorSelBackground;
		clrDefault = GetSysColor(COLOR_HIGHLIGHT); break;

	case APP_COLOR_SEL_TEXT:
		dwVal = opt->dwColorSelText;
		clrDefault = GetSysColor(COLOR_HIGHLIGHTTEXT); break;

	case APP_COLOR_ELEMENT:
		dwVal = opt->dwColorElement;
		clrDefault = RGB(0,0,255); break;

	case APP_COLOR_EL_NAME:
		dwVal = opt->dwColorElementName;
		clrDefault = RGB(128,0,0); break;

	case APP_COLOR_EL_ATTRIB:
		dwVal = opt->dwColorAttribute;
		clrDefault = RGB(255,0,0); break;

	case APP_COLOR_STRING:
		dwVal = opt->dwColorString;
		clrDefault = RGB(0,0,255); break;

	case APP_COLOR_COMMENT:
		dwVal = opt->dwColorComment;
		clrDefault = RGB(164,164,164); break;
	
	case APP_COLOR_PREPROCESSOR:
		dwVal = opt->dwColorPreprocessor;
		clrDefault = RGB(0,128,0); break;
	
	case APP_COLOR_KEYWORD:
		dwVal = opt->dwColorKeyword;
		clrDefault = RGB(0,0,255); break;
	
	case APP_COLOR_NUMBER:
		dwVal = opt->dwColorNumber;
		clrDefault = RGB(0,0,0); break;

	case APP_COLOR_ASP:
		dwVal = opt->dwColorASP;
		clrDefault = RGB(255,255,0); break;
	}

	if (dwVal == 0xFFFFFFFF)
		return (bFixDefault ? clrDefault : dwVal);
	else
	{
		RGBQUAD* pRGB = (RGBQUAD*)&dwVal;
		return RGB(pRGB->rgbRed,pRGB->rgbGreen,pRGB->rgbBlue);
	}
}

void CBonfireApp::SetAppColor(int nColorIndex, COLORREF clr, EDITOR_OPTIONS* opt)
{
	if (opt == NULL)
		opt = &m_opOptions.editor;

	switch (nColorIndex)
	{
	case APP_COLOR_BACKGROUND:		opt->dwColorBackground = clr; break;
	case APP_COLOR_TEXT:			opt->dwColorText = clr; break;
	case APP_COLOR_SEL_BACKGROUND:	opt->dwColorSelBackground = clr; break;
	case APP_COLOR_SEL_TEXT:		opt->dwColorSelText = clr; break;
	case APP_COLOR_ELEMENT:			opt->dwColorElement = clr; break;
	case APP_COLOR_EL_NAME:			opt->dwColorElementName = clr; break;
	case APP_COLOR_EL_ATTRIB:		opt->dwColorAttribute = clr; break;
	case APP_COLOR_STRING:			opt->dwColorString = clr; break;
	case APP_COLOR_COMMENT:			opt->dwColorComment = clr; break;
	case APP_COLOR_PREPROCESSOR:	opt->dwColorPreprocessor = clr; break;
	case APP_COLOR_KEYWORD:			opt->dwColorKeyword = clr; break;
	case APP_COLOR_NUMBER:			opt->dwColorNumber = clr; break;
	case APP_COLOR_ASP:				opt->dwColorASP = clr; break;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
/////////////////////////////////////////////////////////////////////////////
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CBonfireApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CBonfireApp message handlers

////////////////////////////////////////////////////////////////////////////////
// Open a document
////////////////////////////////////////////////////////////////////////////////
void CBonfireApp::OnFileOpen() 
{
	CStringArray arrStrFiles;
	BOOL bReadOnly;
	int nResult = OpenFilesDialog(AfxGetMainWnd(),arrStrFiles,TRUE,NULL,NULL,
		GetFileFilter(FILTER_FILES),6,GetDefExt(FILTER_FILES),&bReadOnly);

	// NOTE: the opened document may start out read only, so don't set the flag
	// to false if bReadOnly is false... bReadOnly only specifies whether or not
	// the user manually chose the option

	int nNumFiles = arrStrFiles.GetSize();
	CBonfireDoc* pDoc;
	if (nResult > 0 && nNumFiles > 0)
	{
		// show the wait cursor until everything is done
		CWaitCursor wc;

		if (nNumFiles == 1)
		{
			pDoc = (CBonfireDoc*)theApp.OpenDocumentFile(arrStrFiles[0]);
			if (pDoc && bReadOnly)
				pDoc->m_xTextBuffer.SetReadOnly();
		}
		else
		{
			CString strDir = AddSlash(arrStrFiles[0]);
			for(int i = 1; i < nNumFiles; i++)
			{
				pDoc = (CBonfireDoc*)theApp.OpenDocumentFile(strDir + arrStrFiles[i]);
				if (pDoc && bReadOnly)
					pDoc->m_xTextBuffer.SetReadOnly();
			}
		}		
	}	
}

////////////////////////////////////////////////////////////////////////////////
// Code to download a file from a URL and open it in Bonfire
////////////////////////////////////////////////////////////////////////////////
void CBonfireApp::OnFileOpenFromUrl() 
{
	COpenFromURLDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		// show the wait cursor until everything is done
		CWaitCursor wc;

		char strTempFile[MAX_PATH];
		if (SUCCEEDED(URLDownloadToCacheFile(NULL,dlg.m_strURL,strTempFile,MAX_PATH,0,NULL)))
		{
			// open a new document (Untitled) and put the data from the temp
			// file into the document
			POSITION pos = GetFirstDocTemplatePosition();
			CDocTemplate* pTemplate = (CDocTemplate*)GetNextDocTemplate(pos);
			ASSERT(pTemplate != NULL);
			ASSERT_KINDOF(CDocTemplate, pTemplate);
			CBonfireDoc* pDoc = (CBonfireDoc*)pTemplate->OpenDocumentFile(NULL,FALSE);

			pDoc->m_xTextBuffer.FreeAll();
			if (!pDoc->m_xTextBuffer.LoadFromFile(strTempFile))
			{
				pDoc->OnCloseDocument();
				return;
			}

			pDoc->SetModifiedFlag();

			// show the doc and initialize all views
			pos = pDoc->GetFirstViewPosition();
			if (pos)
			{
				CView* pView = pDoc->GetNextView(pos);

				pView->GetParentFrame()->ActivateFrame();
				pView->OnInitialUpdate();
				while (pView = pDoc->GetNextView(pos))
					pView->OnInitialUpdate();
			}
			
			((CMainFrame*)AfxGetMainWnd())->m_wndProjectBar.LoadFile(pDoc);
			//pDoc->m_xTextBuffer.UpdateViews(NULL,NULL,UPDATE_RESET);
		}
		else
		{
			AfxMessageBox("There was an error downloading from the specified URL.",MB_ICONEXCLAMATION);
		}
	}
}

// open the file and create project nodes, views, etc
CDocument* CBonfireApp::OpenDocumentFile(LPCTSTR lpszFileName) 
{
	CDocument* pDoc;
	if ((pDoc = CWinApp::OpenDocumentFile(lpszFileName)) == NULL)
		return NULL;

	if (pDoc->IsKindOf(RUNTIME_CLASS(CBonfireDoc)))
	{
		((CMainFrame*)AfxGetMainWnd())->m_wndProjectBar.LoadFile((CBonfireDoc*)pDoc);
		
		// switch to the first view under configuration settings - HACK
		//((CBonfireDoc*)pDoc)->SwitchToFirstView();
	}

	return pDoc;
}

CBonfireDoc* CBonfireApp::OpenExistingDocumentFile(LPCTSTR lpszFileName) 
{
	return (CBonfireDoc*)CWinApp::OpenDocumentFile(lpszFileName);
}

void CBonfireApp::OnProjectOpen() 
{
	CStringArray arrStrFiles;
	int nResult = OpenFilesDialog(AfxGetMainWnd(),arrStrFiles,FALSE,NULL,
		"Open Project",GetFileFilter(FILTER_PROJECTS),1,GetDefExt(FILTER_PROJECTS),
		NULL);

	int nNumFiles = arrStrFiles.GetSize();
	if (nResult > 0 && nNumFiles > 0)
	{
		// show the wait cursor until everything is done
		CWaitCursor wc;

		CProjectBar* pProjectBar = &((CMainFrame*)AfxGetMainWnd())->m_wndProjectBar;
		if (nNumFiles == 1)
		{
			pProjectBar->LoadProject(arrStrFiles[0]);
		}
		else
		{
			CString strDir = AddSlash(arrStrFiles[0]);
			for(int i = 1; i < nNumFiles; i++)
			{
				pProjectBar->LoadProject(arrStrFiles[0]);
			}
		}		
	}
}

void CBonfireApp::OnProjectNew() 
{
	((CMainFrame*)AfxGetMainWnd())->m_wndProjectBar.CreateNewProject();	
}

BOOL CBonfireApp::SaveAllModified() 
{
	// ask to save all modified files
	if (CWinApp::SaveAllModified())
	{
		// close all documents if they are all saved by now
		this->CloseAllDocuments(FALSE);

		// m_bClosing will be set to FALSE if SaveAllModified is called from places
		// other than closing the whole app (e.g. MainFrm::WindowCloseAll() )
		if (this->m_bClosing)
			return ((CMainFrame*)AfxGetMainWnd())->m_wndProjectBar.CloseAllProjects();
		
		return TRUE;
	}

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// go to company homepage
////////////////////////////////////////////////////////////////////////////////
void CBonfireApp::OnHelpHomepage() 
{
	ShellExecute(NULL, "open", "http://www.nzworks.com/index.asp", NULL, NULL, SW_SHOWDEFAULT);
}

////////////////////////////////////////////////////////////////////////////////
// open up help contents
////////////////////////////////////////////////////////////////////////////////
void CBonfireApp::OnHelpContents() 
{
	CString path = AddSlash(GetFilePath(theApp.m_pszHelpFilePath)) + "bonfire.chm::/welcome.html";
	HtmlHelp(NULL, path, HH_DISPLAY_TOC, 0);
}

////////////////////////////////////////////////////////////////////////////////
// open up help index
////////////////////////////////////////////////////////////////////////////////
void CBonfireApp::OnHelpIndex() 
{
	CString path = AddSlash(GetFilePath(theApp.m_pszHelpFilePath)) + "bonfire.chm";
	HtmlHelp(NULL, path, HH_DISPLAY_INDEX, 0);
}

////////////////////////////////////////////////////////////////////////////////
// open up help search
////////////////////////////////////////////////////////////////////////////////
void CBonfireApp::OnHelpSearch() 
{
	HH_FTS_QUERY query;
	query.cbStruct = sizeof(query);
	query.fUniCodeStrings = FALSE;
	query.pszSearchQuery;
	query.iProximity = HH_FTS_DEFAULT_PROXIMITY;
	query.fStemmedSearch = FALSE;
	query.fTitleOnly = FALSE;
	query.fExecute = TRUE;
	query.pszSearchQuery = NULL;
	query.pszWindow = NULL;
	CString path = AddSlash(GetFilePath(theApp.m_pszHelpFilePath)) + "bonfire.chm";
	HtmlHelp(NULL, path, HH_DISPLAY_SEARCH, (DWORD_PTR)&query);
}

////////////////////////////////////////////////////////////////////////////////
// add the given project to MRU list
////////////////////////////////////////////////////////////////////////////////
void CBonfireApp::AddToRecentProjectList(LPCTSTR lpszPathName)
{
	ASSERT_VALID(this);
	ASSERT(lpszPathName != NULL);
	ASSERT(AfxIsValidString(lpszPathName));

	if (m_pRecentProjectList != NULL)
		m_pRecentProjectList->Add(lpszPathName);
}

/////////////////////////////////////////////////////////////////////////////
// MRU project list implementation
/////////////////////////////////////////////////////////////////////////////
void CBonfireApp::OnUpdateProjectMRU(CCmdUI* pCmdUI) 
{
	if (m_pRecentProjectList == NULL) // no MRU files
		pCmdUI->Enable(FALSE);
	else
		m_pRecentProjectList->UpdateMenu(pCmdUI);
}

/////////////////////////////////////////////////////////////////////////////
// when opening project from recent menu
/////////////////////////////////////////////////////////////////////////////
BOOL CBonfireApp::OnOpenRecentProject(UINT nID)
{
	ASSERT_VALID(this);
	ASSERT(m_pRecentProjectList != NULL);

	ASSERT(nID >= ID_FILE_MRU_PROJECT1);
	ASSERT(nID < ID_FILE_MRU_PROJECT1 + (UINT)m_pRecentProjectList->GetSize());
	int nIndex = nID - ID_FILE_MRU_PROJECT1;
	ASSERT((*m_pRecentFileList)[nIndex].GetLength() != 0);

	TRACE2("MRU: open file (%d) '%s'.\n", (nIndex) + 1, (LPCTSTR)(*m_pRecentProjectList)[nIndex]);

	CProjectBar* pProjectBar = &((CMainFrame*)AfxGetMainWnd())->m_wndProjectBar;
	if (pProjectBar->LoadProject((*m_pRecentProjectList)[nIndex]) == NULL)
		m_pRecentProjectList->Remove(nIndex);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Delete all temporary files created by the application
////////////////////////////////////////////////////////////////////////////////
void CBonfireApp::DeleteTempFiles()
{
	// delete temp files
	/*
	CString strCurFile	= "";
	int nTempFiles		= m_pTempFiles.GetCount();
	POSITION posCurFile = m_pTempFiles.GetStartPosition();
	while (posCurFile != NULL)
	{
		//CString strCurFile = m_pTempFiles.GetNext( posCurFile );
		m_pTempFiles.GetNextAssoc( posCurFile, strCurFile, &strCurFile );
		//m_pTempFiles.RemoveAt( posCurFile );
	}
	*/
}

////////////////////////////////////////////////////////////////////////////////
// Add a file to the temporary files list
////////////////////////////////////////////////////////////////////////////////
void CBonfireApp::AddTempFile(CString strTempFile)
{
	//m_pTempFiles.AddTail( strTempFile );
}

BOOL CBonfireApp::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	return CWinApp::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
