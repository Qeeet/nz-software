// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Bonfire.h"
#include "MainFrm.h"
#include "SourceView.h"
#include "OptionsDlg.h"
#include "FileWatch.h"
#include "FindInFiles.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CBonfireApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_PROJECT_ADDFILESTOPROJECT, OnProjectAddFilesToProject)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_ADDFILESTOPROJECT, OnUpdateProjectAddFilesToProject)
	ON_COMMAND(ID_PROJECT_REMOVEFROMPROJECT, OnProjectRemoveFromProject)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_REMOVEFROMPROJECT, OnUpdateProjectRemoveFromProject)
	ON_COMMAND(ID_PROJECT_ADDDIRTOPROJECT, OnProjectAddDirToProject)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_ADDDIRTOPROJECT, OnUpdateProjectAddDirToProject)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_SAVE, OnUpdateProjectSave)
	ON_COMMAND(ID_PROJECT_SAVE, OnProjectSave)
	ON_COMMAND(ID_VIEW_OUTPUTWINDOW, OnViewOutput)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OUTPUTWINDOW, OnUpdateViewOutput)
	ON_COMMAND(ID_VIEW_PROJECTEXPLORER, OnViewProjectExplorer)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROJECTEXPLORER, OnUpdateViewProjectExplorer)
	ON_COMMAND(ID_VIEW_TAGPROPERTIES, OnViewProperties)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TAGPROPERTIES, OnUpdateViewProperties)
	ON_COMMAND(ID_FILE_SHOW, OnFileShow)
	ON_UPDATE_COMMAND_UI(ID_FILE_SHOW, OnUpdateFileShow)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_POS, OnUpdateCurPosIndicator)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_OVR, OnUpdateOverwriteIndicator)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_READ, OnUpdateReadonlyIndicator)
	ON_COMMAND(ID_PROJECT_CLOSE, OnProjectClose)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_CLOSE, OnUpdateProjectClose)
	ON_COMMAND(ID_FILE_PROPERTIES, OnFileProperties)
	ON_COMMAND(ID_TOOLS_OPTIONS, OnToolsOptions)
	ON_COMMAND(ID_PROJECT_SAVEAS, OnProjectSaveas)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_SAVEAS, OnUpdateProjectSaveas)
	ON_COMMAND(ID_PROJECT_PROPERTIES, OnProjectProperties)
	ON_COMMAND(ID_VIEW_FULLSCREEN, OnViewFullscreen)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FULLSCREEN, OnUpdateViewFullscreen)
	ON_WM_MEASUREITEM()
	ON_WM_MENUCHAR()
	ON_WM_INITMENUPOPUP()
	ON_COMMAND(ID_VIEW_TOOLBAR_FIND, OnViewToolbarFind)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBAR_FIND, OnUpdateViewToolbarFind)
	ON_COMMAND(ID_VIEW_TOOLBAR_PROJECT, OnViewToolbarProject)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBAR_PROJECT, OnUpdateViewToolbarProject)
	ON_COMMAND(ID_FILE_SAVEALL, OnFileSaveall)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVEALL, OnUpdateFileSaveall)
	ON_COMMAND(ID_PROJECT_NEWFOLDER, OnProjectNewFolder)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_NEWFOLDER, OnUpdateProjectNewFolder)
	ON_UPDATE_COMMAND_UI(ID_FILE_RENAME, OnUpdateFileRename)
	ON_COMMAND(ID_FILE_RENAME, OnFileRename)
	ON_COMMAND(ID_PROJECT_RENAME, OnProjectRename)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_RENAME, OnUpdateProjectRename)
	ON_COMMAND(ID_FOLDER_RENAME, OnFolderRename)
	ON_UPDATE_COMMAND_UI(ID_FOLDER_RENAME, OnUpdateFolderRename)
	ON_COMMAND(ID_PROJECT_REFRESH, OnProjectRefresh)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_REFRESH, OnUpdateProjectRefresh)
	ON_COMMAND(ID_WINDOW_CLOSEALL, OnWindowCloseAll)
	ON_COMMAND(ID_TOOLS_LOCATEINEXPLORER, OnToolsLocateInExplorer)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_LOCATEINEXPLORER, OnUpdateToolsLocateInExplorer)
	ON_COMMAND(ID_EDIT_FINDINFILES, OnFindInFiles)
	ON_COMMAND_EX_RANGE(ID_FILE_MOVETO_PROJECT1, ID_FILE_MOVETO_PROJECT8, OnFileMoveToProject)
	ON_UPDATE_COMMAND_UI(ID_FILE_MOVETO_PROJECT1, OnUpdateFileMoveToProject)
	ON_COMMAND(ID_TOOLS_OPENDEFAULT, OnToolsOpenDefault)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_OPENDEFAULT, OnUpdateToolsOpenDefault)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_POS,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
	ID_INDICATOR_READ,
	ID_INDICATOR_OVR,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_bFullScreen = FALSE;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	//lpCreateStruct->style &= ~FWS_ADDTOTITLE;
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

#ifdef _DEBUG
	SetTitle(GetTitle() + " (DEBUG VERSION)");
	this->InitialUpdateFrame(NULL,FALSE);
#endif

	this->ModifyStyleEx(0, WS_EX_ACCEPTFILES);

	{
		// create main menu image list
		m_imlMenuIcons.Create(16,16,ILC_COLOR8 | ILC_MASK,6,1);

		CBitmap bmp, bmp2;

		bmp.LoadBitmap(IDB_TOOLBAR_STANDARD);
		m_imlMenuIcons.Add(&bmp,RGB(255,0,255));
		bmp.DeleteObject();

		bmp.LoadBitmap(IDB_TOOLBAR_PROJECT);
		m_imlMenuIcons.Add(&bmp,RGB(255,0,255));
		bmp.DeleteObject();

		bmp.LoadBitmap(IDB_TOOLBAR_FIND);
		m_imlMenuIcons.Add(&bmp,RGB(255,0,255));
		bmp.DeleteObject();

		bmp.LoadBitmap(IDB_MENU_ICONS);
		m_imlMenuIcons.Add(&bmp,RGB(255,0,255));
		bmp.DeleteObject();

		// from magenta to the system button face
		//COLORMAP colormap = {RGB(255,0,255),GetSysColor(COLOR_BTNFACE)};

		const int imlcolor = ILC_COLOR32;

		// set up standard toolbar image list
		//LoadMappedBitmap(IDB_TOOLBAR_STANDARD,0,&colormap,1);
		bmp.LoadBitmap(IDB_TOOLBAR_STANDARD);
		bmp2.Attach(::LoadImage(AfxGetResourceHandle(),
			MAKEINTRESOURCE(IDB_TOOLBAR_STANDARD),IMAGE_BITMAP,0,0,LR_CREATEDIBSECTION));
		::CreateDisabledImage(bmp2);
		m_imlStandardTB.Create(16,16,imlcolor | ILC_MASK,4,2);
		m_imlStandardTB.Add(&bmp,RGB(255,0,255));
		m_imlStandardTBDisabled.Create(16,16,imlcolor | ILC_MASK,4,2);
		m_imlStandardTBDisabled.Add(&bmp2,RGB(255,0,255));
		bmp.DeleteObject(); bmp2.DeleteObject();
		
		// set up project toolbar image list
		//bmp.LoadMappedBitmap(IDB_TOOLBAR_PROJECT,0,&colormap,1);
		bmp.LoadBitmap(IDB_TOOLBAR_PROJECT);
		bmp2.Attach(::LoadImage(AfxGetResourceHandle(),
			MAKEINTRESOURCE(IDB_TOOLBAR_PROJECT),IMAGE_BITMAP,0,0,LR_CREATEDIBSECTION));
		::CreateDisabledImage(bmp2);
		m_imlProjectTB.Create(16,16,imlcolor | ILC_MASK,4,2);
		m_imlProjectTB.Add(&bmp,RGB(255,0,255));
		m_imlProjectTBDisabled.Create(16,16,imlcolor | ILC_MASK,4,2);
		m_imlProjectTBDisabled.Add(&bmp2,RGB(255,0,255));
		bmp.DeleteObject(); bmp2.DeleteObject();

		// set up find toolbar image list
		//bmp.LoadMappedBitmap(IDB_TOOLBAR_FIND,0,&colormap,1);
		bmp.LoadBitmap(IDB_TOOLBAR_FIND);
		bmp2.Attach(::LoadImage(AfxGetResourceHandle(),
			MAKEINTRESOURCE(IDB_TOOLBAR_FIND),IMAGE_BITMAP,0,0,LR_CREATEDIBSECTION));
		::CreateDisabledImage(bmp2);
		m_imlFindTB.Create(16,16,imlcolor | ILC_MASK,4,2);
		m_imlFindTB.Add(&bmp,RGB(255,0,255));
		m_imlFindTBDisabled.Create(16,16,imlcolor| ILC_MASK,4,2);
		m_imlFindTBDisabled.Add(&bmp2,RGB(255,0,255));
		bmp.DeleteObject(); bmp2.DeleteObject();
	}

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT | TBSTYLE_WRAPABLE, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create MAIN toolbar\n");
		return -1;      // fail to create
	}

	//m_wndToolBar.SetBitmap(m_bmpToolBar);
	m_wndToolBar.GetToolBarCtrl().SetImageList(&m_imlStandardTB);
	m_wndToolBar.GetToolBarCtrl().SetDisabledImageList(&m_imlStandardTBDisabled);
	m_wndToolBar.SetWindowText("Standard");
	m_wndToolBar.SetBorders(TBB_L,TBB_T,TBB_R,TBB_B);
	
	if (!m_wndProjectToolBar.CreateEx(this, TBSTYLE_FLAT | TBSTYLE_WRAPABLE, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC,CRect(0,0,0,0),
		AFX_IDW_CONTROLBAR_FIRST + 36) ||
		!m_wndProjectToolBar.LoadToolBar(IDR_TOOLBAR_PROJECT))
	{
		TRACE0("Failed to create PROJECT toolbar\n");
		return -1;      // fail to create
	}

	//m_wndProjectToolBar.SetBitmap(m_bmpProjectToolBar);
	m_wndProjectToolBar.GetToolBarCtrl().SetImageList(&m_imlProjectTB);
	m_wndProjectToolBar.GetToolBarCtrl().SetDisabledImageList(&m_imlProjectTBDisabled);
	m_wndProjectToolBar.SetWindowText("Project");
	m_wndProjectToolBar.SetBorders(TBB_L,TBB_T,TBB_R,TBB_B);
	
	if (!m_wndFindToolBar.CreateEx(this, TBSTYLE_FLAT | TBSTYLE_WRAPABLE, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC,CRect(0,0,0,0),
		AFX_IDW_CONTROLBAR_FIRST + 32) ||
		!m_wndFindToolBar.LoadToolBar(IDR_TOOLBAR_FIND))
	{
		TRACE0("Failed to create FIND toolbar\n");
		return -1;      // fail to create
	}

	//m_wndFindToolBar.SetBitmap(m_bmpFindToolBar);
	m_wndFindToolBar.GetToolBarCtrl().SetImageList(&m_imlFindTB);
	m_wndFindToolBar.GetToolBarCtrl().SetDisabledImageList(&m_imlFindTBDisabled);
	m_wndFindToolBar.SetWindowText("Find");
	m_wndFindToolBar.SetBorders(TBB_L,TBB_T,TBB_R,TBB_B);
	
	if (!m_wndFullScreenToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC,CRect(0,0,0,0),
		AFX_IDW_CONTROLBAR_FIRST + 34) ||
		!m_wndFullScreenToolBar.LoadToolBar(IDR_TOOLBAR_FULLSCREEN))
	{
		TRACE0("Failed to create FullScreen toolbar\n");
		return -1;      // fail to create
	}
	m_wndFullScreenToolBar.SetWindowText("Full Screen");
	
	if (!m_wndProjectBar.Create("Project Explorer",this,CSize(175,375),TRUE,
		AFX_IDW_CONTROLBAR_FIRST + 33,WS_CHILD | WS_VISIBLE | CBRS_LEFT
		| CBRS_SIZE_DYNAMIC))
	{
		TRACE0("Failed to create project explorer\n");
		return -1;
	}

	/*
	if (!m_wndPropBar.Create("Properties",this,CSize(150,375),TRUE,
		AFX_IDW_CONTROLBAR_FIRST + 34,WS_CHILD | WS_VISIBLE | CBRS_RIGHT
		| CBRS_SIZE_DYNAMIC))
	{
		TRACE0("Failed to create properties window\n");
		return -1;
	}
	*/

	if (!m_wndOutputBar.Create("Output",this,CSize(300,150),TRUE,
		AFX_IDW_CONTROLBAR_FIRST + 35,WS_CHILD | WS_VISIBLE | CBRS_BOTTOM
		| CBRS_SIZE_DYNAMIC))
	{
		TRACE0("Failed to create output window\n");
		return -1;
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	//NZEnableDocking(this,CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	m_wndProjectToolBar.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndProjectToolBar);

	m_wndFindToolBar.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndFindToolBar);

	m_wndFullScreenToolBar.EnableDocking(CBRS_NOALIGN);
	FloatControlBar(&m_wndFullScreenToolBar, CPoint(50,50));

	// enable docking for the dockable bars
	m_wndProjectBar.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndProjectBar);
	//m_wndPropBar.EnableDocking(CBRS_ORIENT_VERT);
	//DockControlBar(&m_wndPropBar);
	m_wndOutputBar.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndOutputBar);

	return 0;
}

void CMainFrame::OnClose() 
{
	this->EscapeFullScreen();
	SaveUIState();

	CMDIFrameWnd::OnClose();
}

void CMainFrame::OnDestroy() 
{
	m_mnMainFrame.DestroyMenu();
	m_mnBonfireType.DestroyMenu();

	m_imlMenuIcons.DeleteImageList();

	m_imlStandardTB.DeleteImageList();
	m_imlProjectTB.DeleteImageList();
	m_imlFindTB.DeleteImageList();

	m_imlStandardTBDisabled.DeleteImageList();
	m_imlProjectTBDisabled.DeleteImageList();
	m_imlFindTBDisabled.DeleteImageList();
	
	// the default menu has been destroyed :-D
	m_hMenuDefault = NULL;

	// don't need to delete bitmaps... deleted by toolbar controls
	/*m_bmpToolBar.DeleteObject();
	m_bmpProjectToolBar.DeleteObject();
	m_bmpFindToolBar.DeleteObject();*/
	
	// stop the file watching thread
	CFileWatch::Stop();

	CMDIFrameWnd::OnDestroy();
}

BOOL CMainFrame::VerifyBarState(LPCTSTR lpszProfileName)
{
	CDockState state; state.LoadState(lpszProfileName);

	for (int i = 0; i < state.m_arrBarInfo.GetSize(); i++)
	{
		CControlBarInfo* pInfo = (CControlBarInfo*)state.m_arrBarInfo[i];
		ASSERT(pInfo != NULL);

		int nDockedCount = pInfo->m_arrBarID.GetSize();
		if (nDockedCount > 0)
		{
			// dockbar
			for (int j = 0; j < nDockedCount; j++)
			{
				UINT nID = (UINT) pInfo->m_arrBarID[j];
				if (nID == 0) continue; // row separator
				if (nID > 0xFFFF)nID &= 0xFFFF; // placeholder - get the ID
				if (GetControlBar(nID) == NULL)return FALSE;
			}
		}

		if (!pInfo->m_bFloating) // floating dockbars can be created later
			if (GetControlBar(pInfo->m_nBarID) == NULL)
				return FALSE; // invalid bar ID
	}

	return TRUE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnProjectAddFilesToProject() 
{	this->m_wndProjectBar.ProjectAddFilesToProject();}

void CMainFrame::OnUpdateProjectAddFilesToProject(CCmdUI* pCmdUI) 
{	this->m_wndProjectBar.UpdateProjectAddFilesToProject(pCmdUI);}

void CMainFrame::OnProjectRemoveFromProject() 
{	this->m_wndProjectBar.ProjectRemoveFromProject();}

void CMainFrame::OnUpdateProjectRemoveFromProject(CCmdUI* pCmdUI) 
{	this->m_wndProjectBar.UpdateProjectRemoveFromProject(pCmdUI);}

void CMainFrame::OnProjectAddDirToProject() 
{	m_wndProjectBar.ProjectAddDirToProject();}

void CMainFrame::OnUpdateProjectAddDirToProject(CCmdUI* pCmdUI) 
{	this->m_wndProjectBar.UpdateProjectAddDirToProject(pCmdUI);}

void CMainFrame::OnUpdateProjectSave(CCmdUI* pCmdUI) 
{	this->m_wndProjectBar.UpdateProjectSave(pCmdUI);}

void CMainFrame::OnProjectSave() 
{	this->m_wndProjectBar.ProjectSave();}

// save all modified documents and projects
void CMainFrame::OnFileSaveall() 
{
	// traverse the document templates and the documents and do save on each
	POSITION posDocT = theApp.GetFirstDocTemplatePosition();
	CDocTemplate* pTemplate = NULL;
	while ( posDocT != NULL)
	{
		pTemplate = theApp.GetNextDocTemplate( posDocT );
		ASSERT_KINDOF(CDocTemplate, pTemplate);
		POSITION posDoc = pTemplate->GetFirstDocPosition();
		while (posDoc != NULL)
		{
			CBonfireDoc* pDoc = (CBonfireDoc*)pTemplate->GetNextDoc( posDoc );
			//if ( !pDoc->m_xTextBuffer.GetReadOnly() )
			if (!pDoc->m_pXFile->m_bSaved || pDoc->m_pXFile->m_bNew)
				pDoc->DoFileSave();
		}
	}
	
	//AfxGetApp()->SaveAllModified();
	m_wndProjectBar.SaveAllProjects();
}

void CMainFrame::OnUpdateFileSaveall(CCmdUI* pCmdUI) 
{
	BOOL tr = FALSE;
	// check all open documents

	POSITION posDocT = AfxGetApp()->GetFirstDocTemplatePosition();
	while (posDocT != NULL)
	{
		CDocTemplate* pTemplate = AfxGetApp()->GetNextDocTemplate(posDocT);
		ASSERT_KINDOF(CDocTemplate, pTemplate);
		
		POSITION posDoc = pTemplate->GetFirstDocPosition();
		/*
		// this will actually check if a file is modified
		while (posDoc != NULL)
		{
			CBonfireDoc* pDoc = (CBonfireDoc*)pTemplate->GetNextDoc(posDoc);
			if (pDoc->IsModified() || pDoc->m_pXFile->m_bNew)
			{
				tr = TRUE;
			}
		}
		*/
		if (posDoc != NULL)
			tr = TRUE;
		
	}

	// if tr hasnt been set to true already lets see if there are modified projects
	if (!tr)
	{
		int size = this->m_wndProjectBar.m_arrProjects.size();
		for (int j = 0; j < size; j++)
		{
			CXProject* pProject = (CXProject*)this->m_wndProjectBar.m_arrProjects[j];
			if (!pProject->m_bSaved)
			{
				tr = TRUE;
				break;
			}
		}
	}

	pCmdUI->Enable( tr );

	//this->m_wndProjectBar.UpdateProjectSave(pCmdUI);
}

void CMainFrame::OnProjectRefresh() 
{	this->m_wndProjectBar.RefreshCurrentProject(); }

void CMainFrame::OnUpdateProjectRefresh(CCmdUI* pCmdUI) 
{	this->m_wndProjectBar.UpdateProjectRefresh(pCmdUI); }

void CMainFrame::OnUpdateProjectSaveas(CCmdUI* pCmdUI) 
{	this->m_wndProjectBar.UpdateProjectSaveAs(pCmdUI); }

void CMainFrame::OnProjectSaveas() 
{	this->m_wndProjectBar.ProjectSaveAs(); }

void CMainFrame::OnProjectClose() 
{	this->m_wndProjectBar.CloseCurrentProject(); }

void CMainFrame::OnUpdateProjectClose(CCmdUI* pCmdUI) 
{	this->m_wndProjectBar.UpdateProjectClose(pCmdUI); }

void CMainFrame::OnProjectNewFolder() 
{	this->m_wndProjectBar.ProjectNewFolder(); }

void CMainFrame::OnUpdateProjectNewFolder(CCmdUI* pCmdUI) 
{	this->m_wndProjectBar.UpdateProjectNewFolder(pCmdUI); }

void CMainFrame::AddOutputError(CString message)
{	
	OutputMessage* pMsg	= new OutputMessage();
	pMsg->enType		= OUTPUT_APPSTATUS;
	pMsg->enResult		= MSG_ERROR;
	pMsg->strMessage	= message;
	m_wndOutputBar.AddOutput(pMsg);
}

///////////////////////////////////////////////////////////////////////////
// show/hide and update toolbars
///////////////////////////////////////////////////////////////////////////

// show/hide output window
void CMainFrame::OnViewOutput() 
{	ShowControlBar(&m_wndOutputBar,!m_wndOutputBar.IsWindowVisible(),FALSE); }

void CMainFrame::OnUpdateViewOutput(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck(m_wndOutputBar.IsWindowVisible()); }

// show/hide project explorer window
void CMainFrame::OnViewProjectExplorer() 
{	ShowControlBar(&m_wndProjectBar,!m_wndProjectBar.IsWindowVisible(),FALSE); }

void CMainFrame::OnUpdateViewProjectExplorer(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck(m_wndProjectBar.IsWindowVisible()); }

// show/hide properties window
void CMainFrame::OnViewProperties() 
{	ShowControlBar(&m_wndPropBar,!m_wndPropBar.IsWindowVisible(),FALSE); }

void CMainFrame::OnUpdateViewProperties(CCmdUI* pCmdUI) 
{	
	//pCmdUI->SetCheck(m_wndPropBar.IsWindowVisible());
	pCmdUI->Enable(FALSE);
	pCmdUI->SetCheck(FALSE);
}

// show/hide find toolbar
void CMainFrame::OnViewToolbarFind() 
{	ShowControlBar(&m_wndFindToolBar,!m_wndFindToolBar.IsWindowVisible(),FALSE); }

void CMainFrame::OnUpdateViewToolbarFind(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck(m_wndFindToolBar.IsWindowVisible()); }

// show/hide project toolbar
void CMainFrame::OnViewToolbarProject() 
{	ShowControlBar(&m_wndProjectToolBar,!m_wndProjectToolBar.IsWindowVisible(),FALSE); }

void CMainFrame::OnUpdateViewToolbarProject(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck(m_wndProjectToolBar.IsWindowVisible()); }

///////////////////////////////////////////////////////////////////////////
// show/hide and update toolbars
///////////////////////////////////////////////////////////////////////////


// display current file's text window
void CMainFrame::OnFileShow() 
{	this->m_wndProjectBar.OpenCurrentDocument();}

void CMainFrame::OnUpdateFileShow(CCmdUI* pCmdUI) 
{	this->m_wndProjectBar.UpdateFileShow(pCmdUI);}

void CMainFrame::OnProjectRename() 
{	this->m_wndProjectBar.EditCurNodeText(); }

void CMainFrame::OnUpdateProjectRename(CCmdUI* pCmdUI)
{	this->m_wndProjectBar.UpdateRenameProject(pCmdUI); }

void CMainFrame::OnFolderRename() 
{	this->m_wndProjectBar.EditCurNodeText(); }

void CMainFrame::OnUpdateFolderRename(CCmdUI* pCmdUI) 
{	this->m_wndProjectBar.UpdateRenameFolder(pCmdUI); }

void CMainFrame::OnFileRename() 
{	this->m_wndProjectBar.EditCurNodeText(); }

void CMainFrame::OnUpdateFileRename(CCmdUI* pCmdUI) 
{	this->m_wndProjectBar.UpdateRenameFile(pCmdUI); }

void CMainFrame::OnUpdateCurPosIndicator(CCmdUI *pCmdUI)
{
	try
	{
		CView* pView = NULL;
		if (GetActiveFrame() && (pView = GetActiveFrame()->GetActiveView()) &&
			pView->IsKindOf(RUNTIME_CLASS(CSourceView)))
		{
			CSourceView* pCurView = (CSourceView*)pView;
			CString strCurPos;
			CPoint ptStart, ptEnd;

			pCurView->GetSelection(ptStart,ptEnd);

			CString strCols, strRows;
			if (ptStart.x != ptEnd.x || ptStart.y != ptEnd.y)
			{
				char tmp[20];
				_itoa(ptStart.y+1,tmp,10); strCols = tmp; strCols += "-";
				_itoa(ptEnd.y+1,tmp,10); strCols += tmp;

				_itoa(ptStart.x+1,tmp,10); strRows = tmp; strRows += "-";
				_itoa(ptEnd.x+1,tmp,10); strRows += tmp;
			}
			else
			{
				char tmp[20];
				_itoa(ptStart.y+1,tmp,10); strCols = tmp;
				_itoa(ptStart.x+1,tmp,10); strRows = tmp;
			}

			strCurPos.Format(ID_INDICATOR_POS,strCols,strRows);

			m_wndStatusBar.SetPaneText(
				m_wndStatusBar.CommandToIndex(ID_INDICATOR_POS),
				strCurPos);
		}
		else
		{
			m_wndStatusBar.SetPaneText(
				m_wndStatusBar.CommandToIndex(ID_INDICATOR_POS),
				" ");
		}
	}
	catch(...){}
}

void CMainFrame::OnUpdateOverwriteIndicator(CCmdUI *pCmdUI)
{
	try
	{
		CView* pView = NULL;
		if (GetActiveFrame() && (pView = GetActiveFrame()->GetActiveView()) &&
			pView->IsKindOf(RUNTIME_CLASS(CSourceView)) &&
			((CSourceView*)pView)->GetOverwriteMode())
		{
			m_wndStatusBar.SetPaneText(
				m_wndStatusBar.CommandToIndex(ID_INDICATOR_OVR),
				"OVR");
		}
		else
		{
			m_wndStatusBar.SetPaneText(
				m_wndStatusBar.CommandToIndex(ID_INDICATOR_OVR),
				" ");
		}
	}
	catch(...){}
}

void CMainFrame::OnUpdateReadonlyIndicator(CCmdUI *pCmdUI)
{
	try
	{
		CView* pView = NULL;
		if (GetActiveFrame() && (pView = GetActiveFrame()->GetActiveView()) &&
			((CBonfireDoc*)pView->GetDocument())->m_xTextBuffer.GetReadOnly())
		{
			m_wndStatusBar.SetPaneText(
				m_wndStatusBar.CommandToIndex(ID_INDICATOR_READ),
				"READ");
		}
		else
		{
			m_wndStatusBar.SetPaneText(
				m_wndStatusBar.CommandToIndex(ID_INDICATOR_READ),
				" ");
		}
	}
	catch(...){}
}

void CMainFrame::OnFileProperties() 
{	this->m_wndProjectBar.ShowFileProperties();	}

void CMainFrame::OnToolsOptions() 
{	
	//CToolsOptionsDialog dlgOptions;
	COptionsDlg dlgOptions;
	dlgOptions.DoModal();

	// refresh the Project Manager window
	//this->m_wndProjectBar.RefreshProjectManagerTree();
}


void CMainFrame::OnProjectProperties() 
{	this->m_wndProjectBar.ShowFileProperties(TRUE);	}

//To make your application fill the whole screen 
void CMainFrame::OnViewFullscreen() 
{
	if (m_bFullScreen)
		EscapeFullScreen();
	else
	{
		SaveUIState();

		this->ShowWindow(SW_SHOWMAXIMIZED);
		this->ModifyStyle(WS_CAPTION|WS_SYSMENU,WS_MAXIMIZE,SWP_FRAMECHANGED);
		//this->SetMenu(NULL); //Use this line if you dont need the Menu too.
		
		// hide the toolbars

		ShowControlBar(&m_wndOutputBar,FALSE,FALSE);
		ShowControlBar(&m_wndProjectBar,FALSE,FALSE);
		ShowControlBar(&m_wndToolBar,FALSE,FALSE);
		ShowControlBar(&m_wndFindToolBar,FALSE,FALSE);
		ShowControlBar(&m_wndProjectToolBar,FALSE,FALSE);
		ShowControlBar(&m_wndStatusBar,FALSE,FALSE);
		
		//if (!dlgFS)
		//	dlgFS = new CFullScreenDlg(this);
		//dlgFS->ShowWindow(SW_SHOWNORMAL);
		
		// show the "Full Screen" popup
		ShowControlBar(&m_wndFullScreenToolBar,TRUE,FALSE);

		this->UpdateWindow();

		m_bFullScreen = TRUE;
	}
}

//To make your application to be displayed normally 
//	return CFrameWnd::PreTranslateMessage(pMsg);
BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		EscapeFullScreen();
	}
	return CMDIFrameWnd::PreTranslateMessage(pMsg);
}

void CMainFrame::EscapeFullScreen()
{
	if (!m_bFullScreen)
		return;

	LPBYTE data;
	UINT wplsize = sizeof(WINDOWPLACEMENT);
	AfxGetApp()->GetProfileBinary("Workspace\\Main Window","WindowPlacement",&data,&wplsize);
	if (wplsize == sizeof(WINDOWPLACEMENT))
		::SetWindowPlacement(m_hWnd,(WINDOWPLACEMENT*)data);

	if (data)
		delete[] data;

	this->ModifyStyle(0,WS_CAPTION|WS_SYSMENU,SWP_FRAMECHANGED);
	// Add the following lines if you have removed the menu in the previous function.

	//CMenu pMenu;
	//pMenu.LoadMenu(IDR_MAINFRAME);
	//this->SetMenu(&pMenu);

	// show the toolbars and set the main frame initial position
	LoadUIState(TRUE);

	// hide the "Full Screen" popup
	//dlgFS->ShowWindow(SW_HIDE);
	ShowControlBar(&m_wndFullScreenToolBar,FALSE,FALSE);

	this->UpdateWindow();

	m_bFullScreen = FALSE;
}

void CMainFrame::OnUpdateViewFullscreen(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bFullScreen ? 1 : 0);
}

// save the state of all the UI components
void CMainFrame::SaveUIState()
{
	WINDOWPLACEMENT wpl; memset(&wpl,0,sizeof(wpl));
	wpl.length = sizeof(wpl);
	::GetWindowPlacement(m_hWnd,&wpl);

	AfxGetApp()->WriteProfileBinary("Workspace\\Main Window","WindowPlacement",(LPBYTE)&wpl,
		sizeof(WINDOWPLACEMENT));

	SaveBarState("Workspace\\Docking");

	m_wndProjectBar.SaveState("Workspace\\Docking");
	//m_wndPropBar.SaveState("Workspace\\Docking");
	m_wndOutputBar.SaveState("Workspace\\Docking");
}

// load the state of all the UI components
void CMainFrame::LoadUIState(BOOL bLoadWindowPlacement)
{
	if (bLoadWindowPlacement)
	{
		// set window position from registry
		LPBYTE data;
		UINT wplsize = sizeof(WINDOWPLACEMENT);
		AfxGetApp()->GetProfileBinary("Workspace\\Main Window","WindowPlacement",&data,&wplsize);
		if (wplsize == sizeof(WINDOWPLACEMENT))
		{
			::SetWindowPlacement(m_hWnd,(WINDOWPLACEMENT*)data);
			if (AfxGetApp()->m_nCmdShow== SW_SHOWDEFAULT ||
				AfxGetApp()->m_nCmdShow == SW_SHOWNORMAL)
				AfxGetApp()->m_nCmdShow = ((WINDOWPLACEMENT*)data)->showCmd;
		}

		if (data)
			delete[] data;
	}

	if (VerifyBarState("Workspace\\Docking"))
	{
		LoadBarState("Workspace\\Docking");

		m_wndProjectBar.LoadState("Workspace\\Docking");
		//m_wndPropBar.LoadState("Workspace\\Docking");
		m_wndOutputBar.LoadState("Workspace\\Docking");
	}
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// BCMenu code
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

HMENU CMainFrame::SetMenuIcons(BCMenu* pMenu/*BOOL bChild*/)
{
	/*
	new,open,save,saveall,print,undo,redo,cut,copy,paste,fullscreen,about
	newproject,openproject,saveproject,newfolder,addfile,addfolder,remove
	find,findprev,findnext,replace,findinfiles
	*/
	int i = 0; // for image index increments
	struct { 
		UINT id; // item id
		int image; // image list index
	} arrItems[] = {
		ID_FILE_NEW,			i++,
		ID_FILE_OPEN,			i++,
		ID_FILE_SAVE,			i++,
		ID_FILE_SAVEALL,		i++,
		ID_FILE_PRINT,			i++,
		ID_EDIT_UNDO,			i++,
		ID_EDIT_REDO,			i++,
		ID_EDIT_CUT,			i++,
		ID_EDIT_COPY,			i++,
		ID_EDIT_PASTE,			i++,
		ID_VIEW_FULLSCREEN,		i++,
		ID_HELP_CONTENTS,		i++,

		ID_PROJECT_NEW,			i++,
		ID_PROJECT_OPEN,		i++,
		ID_PROJECT_SAVE,		i++,
		ID_PROJECT_NEWFOLDER,	i++,
		ID_PROJECT_ADDFILESTOPROJECT,i++,
		ID_PROJECT_ADDDIRTOPROJECT,i++,
		ID_PROJECT_REMOVEFROMPROJECT,i++,

		ID_EDIT_FIND,			i++,
		ID_EDIT_FIND_PREVIOUS,	i++,
		ID_EDIT_REPEAT,			i++,
		ID_EDIT_REPLACE,		i++,
		ID_EDIT_FINDINFILES,	i++,

		ID_APP_ABOUT,			i++,		
		ID_HELP_SEARCH,			i++,
		ID_HELP_HOMEPAGE,		i++,
		ID_PROJECT_PROPERTIES,	i++,
		ID_TOOLS_OPTIONS,		i++,
		ID_PROJECT_CLOSE,		i++,
		ID_TOOLS_LOCATEINEXPLORER,		i++,
		
		0,0 // end of list
	};

	for (i = 0; /* use a break statement to exit loop */ ; i++)
	{
		if (arrItems[i].id == 0)
			break;

		/*if (arrItems[i].where == 0 ||
			( bChild && arrItems[i].where == 1) ||
			(!bChild && arrItems[i].where == 2))*/
		pMenu->ModifyODMenu(NULL,arrItems[i].id,&m_imlMenuIcons,arrItems[i].image);
	}

	return pMenu->m_hMenu;//Detach();
}

void CMainFrame::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	// TODO: Add your message handler code here and/or call default
	BOOL setflag = FALSE;
	if(lpMeasureItemStruct->CtlType==ODT_MENU)
	{
		if(IsMenu((HMENU)lpMeasureItemStruct->itemID))
		{
			CMenu* cmenu = CMenu::FromHandle((HMENU)lpMeasureItemStruct->itemID);
			if(m_mnBonfireType.IsMenu(cmenu)||m_mnMainFrame.IsMenu(cmenu))
			{
				m_mnBonfireType.MeasureItem(lpMeasureItemStruct);
				setflag = TRUE;
			}
		}
	}

	if(!setflag)
		// this was here before without the IF
		CMDIFrameWnd::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

LRESULT CMainFrame::OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu) 
{
	// TODO: Add your message handler code here and/or call default
	LRESULT lresult;
	if(m_mnBonfireType.IsMenu(pMenu)||m_mnMainFrame.IsMenu(pMenu))
		lresult = BCMenu::FindKeyboardShortcut(nChar, nFlags, pMenu);
	else
		lresult = CMDIFrameWnd::OnMenuChar(nChar, nFlags, pMenu);
	
	return lresult;

	return CMDIFrameWnd::OnMenuChar(nChar, nFlags, pMenu);
}

void CMainFrame::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) 
{
	CMDIFrameWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
	
	// TODO: Add your message handler code here
	if(!bSysMenu)
	{
		if(m_mnBonfireType.IsMenu(pPopupMenu)||m_mnMainFrame.IsMenu(pPopupMenu))
			BCMenu::UpdateMenu(pPopupMenu);
	}

}

void CMainFrame::OnWindowCloseAll() 
{
	// hack to prevent closing projects
	theApp.m_bClosing = FALSE;

	// try save all modified documents, and close them
	theApp.SaveAllModified();

	theApp.m_bClosing = TRUE;
}

// view file in windows explorer
void CMainFrame::OnToolsLocateInExplorer() 
{	
	CXNode* pNode = this->m_wndProjectBar.GetCurrentNode();
	CString path = (pNode->m_enNodeType == XNODE_FOLDER) ? pNode->FullPath() : pNode->m_strPath ;
	ShellExecute(NULL, "open", "explorer", path, NULL, SW_SHOWDEFAULT);
}

void CMainFrame::OnUpdateToolsLocateInExplorer(CCmdUI* pCmdUI) 
{
	CXNode* pNode = this->m_wndProjectBar.GetCurrentNode();
	pCmdUI->Enable( pNode != NULL );
}

void CMainFrame::OnToolsOpenDefault() 
{
	CXNode* pNode = this->m_wndProjectBar.GetCurrentNode();
	ShellExecute(NULL, "open", pNode->FullPath(), NULL,  NULL, SW_SHOWDEFAULT);
}

void CMainFrame::OnUpdateToolsOpenDefault(CCmdUI* pCmdUI) 
{
	CXNode* pNode = this->m_wndProjectBar.GetCurrentNode();
	pCmdUI->Enable( pNode != NULL && pNode->m_enNodeType != XNODE_PROJECT );
}

void CMainFrame::OnFindInFiles() 
{
	CFindInFiles f;
	f.FindInFiles();
}

BOOL CMainFrame::OnFileMoveToProject(UINT nID) 
{
	this->m_wndProjectBar.MoveFileToProject(nID);
	return TRUE;
}

void CMainFrame::OnUpdateFileMoveToProject(CCmdUI* pCmdUI)
{
	this->m_wndProjectBar.UpdateMoveFileToProject(pCmdUI);
}
