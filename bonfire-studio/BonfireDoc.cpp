// BonfireDoc.cpp : implementation of the CBonfireDoc class
//

#include "stdafx.h"
#include "Bonfire.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "FileWatch.h"
#include "GoToDlg.h"

#include "BonfireView.h"
#include "SourceView.h"
#include "XMLTreeView.h"
#include "BrowserView.h"

#include "BonfireDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define RELEASE_XML(pInterface)  \
if(pInterface) \
{\
	pInterface->Release();\
	pInterface = NULL;\
}\

/////////////////////////////////////////////////////////////////////////////
// CBonfireDoc

IMPLEMENT_DYNCREATE(CBonfireDoc, CDocument)

BEGIN_MESSAGE_MAP(CBonfireDoc, CDocument)
	//{{AFX_MSG_MAP(CBonfireDoc)
	ON_UPDATE_COMMAND_UI(ID_EDIT_READONLY, OnUpdateEditReadOnly)
	ON_COMMAND(ID_EDIT_GOTOLINE, OnGotoLine)
	ON_COMMAND(ID_EDIT_READONLY, OnEditReadOnly)
	ON_COMMAND(ID_TOOLS_VALIDATE_FILE, OnToolsValidateFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

extern CMainFrame* g_pMainFrame;
extern CBonfireApp theApp;
extern CMapStringToPtr g_Parsers;

/////////////////////////////////////////////////////////////////////////////
// CBonfireDoc construction/destruction

#pragma warning(disable:4355) // this is for the crystal edit
CBonfireDoc::CBonfireDoc() : m_xTextBuffer(this)
{
	m_pXFile		= NULL;
	m_fnParse		= NULL;
	m_pViews		= new CMapStringToPtr(3);
}

CBonfireDoc::~CBonfireDoc()
{
	try
	{
		if (m_pXFile)
			m_pXFile->m_pDoc = NULL;

		delete m_pViews;
	}
	catch (...) {}
}

/////////////////////////////////////////////////////////////////////////////
// CBonfireDoc serialization

void CBonfireDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CBonfireDoc diagnostics

#ifdef _DEBUG
void CBonfireDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CBonfireDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBonfireDoc functions

BOOL CBonfireDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	m_xTextBuffer.InitNew();

	((CMainFrame*)AfxGetMainWnd())->m_wndProjectBar.LoadFile(this);

	// MAYBE PUT IN SETPATHNAME() LATER
	// set the syntax highlighting parser
	this->m_fnParse = LookupParser( NULL );

	// draw icon
	POSITION pos = GetFirstViewPosition();
	CChildFrame* pChild = (CChildFrame*)GetNextView(pos)->GetParentFrame();
	pChild->UpdateIcon("");

	UpdateTabs( NULL );

	// initialize the document title
	UpdateDocumentTitle( NULL );

	SwitchToFirstView();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// switch to the first view under configuration settings - HACK
/////////////////////////////////////////////////////////////////////////////
void CBonfireDoc::SwitchToFirstView()
{
	POSITION pos			= GetFirstViewPosition();
	CChildFrame* pChild		= (CChildFrame*)GetNextView(pos)->GetParentFrame();
	if (pChild->m_wndTabBar.GetTabCount())
	{
		CString strFirstView	= pChild->m_wndTabBar.m_arrTabs[0];
		pChild->m_wndTabBar.SetSelTab(0);
		SwitchToView(strFirstView);
	}
}

/////////////////////////////////////////////////////////////////////////////
// switch to the new selected view tab
/////////////////////////////////////////////////////////////////////////////
BOOL CBonfireDoc::SwitchToView(CString strTab)
{
	if (strTab == "")
		return FALSE;

	CChildFrame*	pChild		= (CChildFrame*)g_pMainFrame->MDIGetActive();
	if (!pChild || pChild->GetActiveDocument() != this)
	{
		POSITION pos = GetFirstViewPosition();
		pChild	= (CChildFrame*)GetNextView(pos)->GetParentFrame();
	}

	if (!pChild)
		return FALSE;

	CView*			pViewAdd	= NULL;
	CBonfireView*	pGlobalView	= NULL;
	CBonfireView*	pBNewView	= NULL;
	
	// get the view to "hide"
	CView*			pViewDel	= pChild->GetActiveView();
	
	// if we're already displaying this kind of view, no need to go further.
	for (int i = 0; i < theApp.m_arAllViews.GetSize(); i++)
	{
		if ( ((CBonfireView*)theApp.m_arAllViews[i])->m_strCaption.Compare(strTab) == 0 )
		{
			pGlobalView = (CBonfireView*)theApp.m_arAllViews[i];
			break;
		}
	}

	if ( pViewDel && pViewDel->IsKindOf(pGlobalView->m_pClass) )
		return TRUE;

	// check if a view of this type already exists
 	if ( !m_pViews->Lookup(pGlobalView->m_strCaption, (void*&)pBNewView) )
	{
		// doesn't exist so create a new view
		pViewAdd = (CView*)pGlobalView->m_pClass->CreateObject();
		if (pViewAdd == NULL)
		{
			TRACE1("Warning: Dynamic create of view type %Fs failed\n", pGlobalView->m_pClass->m_lpszClassName);
			return FALSE;
		}

		// draw new view
		CCreateContext context;
		context.m_pNewViewClass		= pGlobalView->m_pClass;
		context.m_pCurrentDoc		= NULL;
		context.m_pNewDocTemplate	= this->GetDocTemplate();
		context.m_pLastView			= NULL;
		context.m_pCurrentFrame		= pChild;

		// draw new view
		if (!pViewAdd->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0),
			pChild, AFX_IDW_PANE_FIRST, &context))
		{
			TRACE0("Warning: couldn't create view for frame\n");
			delete pViewAdd;
			return FALSE;
		}

		// add the view to the map
		pBNewView			= new CBonfireView(pGlobalView);
		pBNewView->m_pView	= pViewAdd;
		pBNewView->m_pClass	= pViewAdd->GetRuntimeClass();

		// add this view to the document's map
		AddViewToMap(pBNewView);

		// WM_INITIALUPDATE is defined in afxpriv.h
		pViewAdd->SendMessage(WM_INITIALUPDATE,0,0);

		// connect the view to the document
		AddView(pViewAdd);
	}
	else
	{
		pViewAdd = pBNewView->m_pView;
	}

	// save file on switching views
	if (theApp.m_opOptions.general.dwSaveOnSwitch && !m_xTextBuffer.GetReadOnly() && pViewDel)
		DoFileSave();
	
	CSplitterWnd*	pSplitter	= NULL;
	POSITION pos				= GetFirstViewPosition();
	BOOL firstOne				= true;
	while (pos)
	{
		CView* pv	= GetNextView(pos);
		if (pv == pViewAdd)	
		{
			pv->SetDlgCtrlID(AFX_IDW_PANE_FIRST);

			// show the view
			if (pBNewView->m_bAllowsSplitter)
			{
				pSplitter	= (CSplitterWnd*)pViewAdd->GetParent();
				
				// show the splitter (show the children view(s) as well)
				pSplitter->ShowWindow(SW_SHOW);
			}
			else
				// show the view
				pViewAdd->ShowWindow(SW_SHOW);
		}
		else 
		{
			// check if the view is inside a splitter
			if (pv->GetParent()->GetRuntimeClass() == (CRuntimeClass*)RUNTIME_CLASS(CSplitterWnd))
			{
				pSplitter	= (CSplitterWnd*)pv->GetParent();
				
				// hide the splitter (hide all views with the splitter)
				pSplitter->ShowWindow(SW_HIDE);
			}
			else
			{
				// hide the view
				pv->ShowWindow(SW_HIDE);
				pv->SetDlgCtrlID(AFX_IDW_PANE_FIRST + 255);
			}
		}

		TRACE("%s (%d) ", pv->GetRuntimeClass()->m_lpszClassName, (unsigned)(pv->GetDlgCtrlID() - AFX_IDW_PANE_FIRST));
		if (pv == pViewDel)		TRACE("= D ");
		if (pv == pViewAdd)		TRACE("= A ");
		if (pv->GetRuntimeClass() == pGlobalView->m_pClass)
		{
			if (firstOne)
				firstOne = false;
			else
				TRACE("removing ");
		}
	}
	TRACE("\n");

	// get rid of extra views created by the splitter
	if (pSplitter)
	{
		for (int cols = 1; cols < pSplitter->GetColumnCount(); cols++)
			pSplitter->DeleteColumn(cols);
		for (int rows = 1; rows < pSplitter->GetRowCount(); rows++)
			pSplitter->DeleteRow(rows);

		pSplitter->RecalcLayout();
	}

	pViewAdd->UpdateWindow();
	pChild->SetActiveView(pViewAdd);
	pChild->RecalcLayout();

	if (pBNewView->m_bModifiedSinceRefresh)
	{
		pBNewView->m_bModifiedSinceRefresh = FALSE;
		if (pGlobalView->m_pClass == RUNTIME_CLASS(CXMLTreeView))
		{
			((CXMLTreeView*)pViewAdd)->RefreshView();
		}
		else if (pGlobalView->m_pClass == RUNTIME_CLASS(CBrowserView))
		{
			((CBrowserView*)pViewAdd)->RefreshView();
		}
		else
		{
			pBNewView->m_bModifiedSinceRefresh = TRUE;
		}
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CBonfireDoc commands

void CBonfireDoc::DeleteContents() 
{
	CDocument::DeleteContents();
	m_xTextBuffer.FreeAll();
}

void CBonfireDoc::SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU) 
{
	CDocument::SetPathName(lpszPathName, bAddToMRU);
	
	if (m_pXFile)
		m_pXFile->Update(lpszPathName,TRUE);

	// update the syntax highlighting parser
	this->m_fnParse = LookupParser( GetFileExtension(lpszPathName) );
	UpdateAllViews(NULL);
	
	// draw icon
	POSITION pos = GetFirstViewPosition();
	((CChildFrame*)GetNextView(pos)->GetParentFrame())->UpdateIcon(lpszPathName);

	UpdateDocumentTitle(lpszPathName);

	// update the tab list (changes when the file extension changes)
	UpdateTabs();
}

void CBonfireDoc::OnFileReload() 
{
	if (!GetPathName().IsEmpty())
	{
		SetModifiedFlag(FALSE);
		m_xTextBuffer.FreeAll();
		m_xTextBuffer.LoadFromFile(GetPathName(),
			CRLF_STYLE_AUTOMATIC,ExistsSourceView(GetPathName()));
		UpdateAllViews(NULL);
	}
}

BOOL CBonfireDoc::ExistsSourceView(LPCTSTR lpszPathName)
{
	CStringList pTabList;
	CString ext = (lpszPathName == NULL) 
				? GetFileExtension(GetPathName()) 
				: GetFileExtension(lpszPathName);
	
	// generate tabs based on the file extension
	CString strViews	= theApp.m_opOptions.views.vAssociations[0].strViews; // init to default views
	CString strExtList	= "";
	size_t nExt			= theApp.m_opOptions.views.vAssociations.size();
	
	// see if the current file's extension is in the list
	while (nExt-- > 0) // 0 is the default file extension
	{
		strExtList = (CString)theApp.m_opOptions.views.vAssociations[nExt].strExtensions;
		strExtList.Remove(' ');

		if (IsStringPresent(strExtList, ext))
		{
			strViews = theApp.m_opOptions.views.vAssociations[nExt].strViews;
			break;
		}
	}

	return (strViews[0] != '-' || strViews[0] == 's');
}


BOOL CBonfireDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	
	if (!m_xTextBuffer.LoadFromFile(lpszPathName, CRLF_STYLE_AUTOMATIC, ExistsSourceView(lpszPathName)))
		return FALSE;

	// add the file to modify watch
	AddToFileWatch();

	return TRUE;
}

BOOL CBonfireDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	if (m_pXFile->m_bSaved && !m_pXFile->m_bNew && GetPathName() == lpszPathName)
		return TRUE;
	else if (m_xTextBuffer.GetReadOnly())
		return FALSE;
	
	// temporarily remove the file from modify watch
	RemoveFileWatch();

	m_xTextBuffer.SaveToFile(lpszPathName);
	SetPathName(lpszPathName);
	
	// add the file back to modify watch
	AddToFileWatch();

	return TRUE;
}

void CBonfireDoc::OnCloseDocument() 
{
	// check if this new file has been at all modified
	if (m_pXFile && m_pXFile->m_bNew)
	{
		// remove the node from tree
		g_pMainFrame->m_wndProjectBar.DeleteFileNode(m_pXFile);
	}

	// remove the file from modify watch
	RemoveFileWatch();

	CDocument::OnCloseDocument();
}

void CBonfireDoc::OnUpdateEditReadOnly(CCmdUI* pCmdUI) 
{
	//pCmdUI->Enable(FALSE);
	pCmdUI->SetCheck(m_xTextBuffer.GetReadOnly());
}

void CBonfireDoc::OnEditReadOnly() 
{
	m_xTextBuffer.SetReadOnly(!m_xTextBuffer.GetReadOnly());
}

void CBonfireDoc::UpdateDocumentTitle(LPCTSTR lpszPathName /*=NULL*/)
{
	CString title;

	// get the name from the file node
	if (lpszPathName != NULL)
		title = GetFileTitle(lpszPathName);
	else if (m_pXFile != NULL) 
		title = m_pXFile->m_strName;
	else
		return;

	SetTitle(title);

	POSITION pos = GetFirstViewPosition();
	CView* pView;
	if (pos && (pView = GetNextView(pos)) && pView->GetParentFrame())
	{
		CString strTitle = title;
		if (IsModified())strTitle += " *";
		if (m_xTextBuffer.GetReadOnly())strTitle += " (Read Only)";
		pView->GetParentFrame()->SetWindowText(strTitle);
	}
}

void CBonfireDoc::OnGotoLine() 
{
	CGoToDlg	GoToDlg;

	if (GoToDlg.DoModal() == IDOK)
	{
		try
		{
			// get user input
			int line	= atoi(GoToDlg.m_strGoToValue);
			int max		= m_xTextBuffer.GetLineCount();
			
			// check min
			if (line <= 0)
				line = 1;

			// check max
			if (line >= max)
				line = max;
			
			CPoint pt1(0, line - 1);
		
			try
			{
				CBonfireView*	pBNewView	= NULL;
				CView*			pSourceView	= NULL;
			
				CString strCaption = ((CBonfireView)theApp.m_arAllViews[0]).m_strCaption;
				SwitchToView(strCaption);
				
				// check if a view of this type already exists
 				if ( !m_pViews->Lookup(strCaption, (void*&)pBNewView) )
					throw "No Such Viewer Found";
					
				pSourceView = pBNewView->m_pView;
				
				// go to the desired line
				((CSourceView*)pSourceView)->SetCursorPos(pt1);
				((CSourceView*)pSourceView)->EnsureVisible(pt1);
			}
			catch (...)
			{
				g_pMainFrame->AddOutputError("Error occurred while trying to seek to line.");
			}
		}
		catch (char* e)
		{
			CString strMessage;
			strMessage.Format("Error occurred while accessing the source view: %s", e);
			g_pMainFrame->AddOutputError(strMessage);
		}
		catch (...)
		{
			g_pMainFrame->AddOutputError("Invalid value inputted for \"Go To\".");
		}
	}
}

// generates the string list of view names to be used for the tabs
void CBonfireDoc::UpdateTabs(LPCTSTR lpszPathName /*= NULL*/)
{
	CStringList pTabList;
	CString ext = (lpszPathName == NULL) 
				? GetFileExtension(GetPathName()) 
				: GetFileExtension(lpszPathName);
	
	// generate tabs based on the file extension
	CString strViews	= theApp.m_opOptions.views.vAssociations[0].strViews; // init to default views
	CString strExtList	= "";
	size_t nExt			= theApp.m_opOptions.views.vAssociations.size();
	
	// see if the current file's extension is in the list
	while (nExt-- > 0) // 0 is the default file extension
	{
		strExtList = (CString)theApp.m_opOptions.views.vAssociations[nExt].strExtensions;
		strExtList.Remove(' ');

		if (IsStringPresent(strExtList, ext))
		{
			strViews = theApp.m_opOptions.views.vAssociations[nExt].strViews;
			break;
		}
	}

	POSITION pos		= GetFirstViewPosition();
	CChildFrame* pChild = ((CChildFrame*)GetNextView(pos)->GetParentFrame());
	CView* pView		= pChild->GetActiveView();
	BOOL bIsCurViewAvail = FALSE;

	// also make sure the current view is available, if not, close it
	// add the tabs to the tablist
	for (int i = 0; i < theApp.m_arAllViews.GetSize(); i++)
	{
		if (strViews[i] != '-')
		{
			pTabList.AddHead( ((CBonfireView*)theApp.m_arAllViews[i])->m_strCaption );
			if (pView && pView->IsKindOf(theApp.m_arAllViews[i]->m_pClass))
				bIsCurViewAvail = TRUE;
		}
	}

	// draw the tabs
	pChild->DrawTabs(&pTabList);

	if (!pView || !bIsCurViewAvail)
		SwitchToFirstView();
}

// adds the view to the Map of views
void CBonfireDoc::AddViewToMap(CBonfireView *pNewView)
{
	for (int i = 0; i < theApp.m_arAllViews.GetSize(); i++)
	{
		CBonfireView* pMapView = (CBonfireView*)theApp.m_arAllViews[i];
		if ( pNewView->m_pClass == pMapView->m_pClass )
		{
			void* nl;
			if ( !m_pViews->Lookup(pMapView->m_strCaption, (void*&)nl) )
				m_pViews->SetAt(pMapView->m_strCaption, pNewView);
			break;
		}
	}
}

// set the flag that will tell the views to refresh next time you switch to them
void CBonfireDoc::SetViewRefreshFlag()
{
	CString			strTab;
	CBonfireView*	pTempView	= NULL;
	POSITION		pos			= this->m_pViews->GetStartPosition();
	while (pos)
	{
		this->m_pViews->GetNextAssoc(pos, strTab, (void*&)pTempView);
		if (pTempView)
		{
			pTempView->m_bModifiedSinceRefresh = TRUE;
		}
	}
}

// dont delete returned value if sent to AddOutput of outputbar
// this function does not AddRef so no need to Release on pXError
OutputMessage* CBonfireDoc::CreateXMLErrorMessage(IXMLDOMParseError* pXError, bool bShowLine)
{
	OutputMessage* pMsg = new OutputMessage();
	pMsg->enType		= OUTPUT_XMLERROR;
	pMsg->enResult		= MSG_ERROR;

	BSTR reason	= NULL;
	try
	{
		// get the details about the error
		LONG code	= 0;
		LONG line	= 0;
		LONG pos	= 0;

		pXError->get_reason(&reason);
		pXError->get_errorCode(&code);
		if (bShowLine)
		{
			if (FAILED(pXError->get_line(&line)))
				line = 0;
			if (FAILED(pXError->get_linepos(&pos)))
				pos = 0;
		}

		BSTR bstr;
		if (SUCCEEDED(pXError->get_url(&bstr)))
		{
			// create the message object
			pMsg->strMessage	= reason;
			pMsg->nLine			= line;
			pMsg->nCol			= pos;

			if (bShowLine && line && pos)
			{
				CString str(bstr);
				if (str.CompareNoCase(GetPathName()) == 0)
				{
					int ln = m_xTextBuffer.GetLineLength(line - 1);
					m_xTextBuffer.GetText(line - 1, 0, line - 1, ln, pMsg->strLine);
				}
				else
				{
					pMsg->strLine = "(Outside Current File)";
				}
			}

			SysFreeString(bstr);
		}
		else
		{
			pMsg->strMessage = "There was an error validating the file or one of its linked files.";
			pMsg->nLine = 0;
			pMsg->nCol = 0;
		}
	}
	catch(...)
	{
		// create the message object
		pMsg->strMessage	= "Error loading text into XML DOM";
		pMsg->nLine			= 0;
		pMsg->nCol			= 0;
	}
	
	// free memory
	if (reason != NULL)
		SysFreeString(reason);

	return pMsg;
}

void CBonfireDoc::OnToolsValidateFile() 
{
	g_pMainFrame->m_wndOutputBar.ClearList(OUTPUT_XMLERROR);

	int nTextLength		= m_xTextBuffer.GetAllTextLength();
	char* data			= NULL;
	BSTR wcdata			= NULL;

	// show the wait cursor until everything is done
	CWaitCursor wc;

	try
	{
		data = (char*)malloc(nTextLength + 1);
		m_xTextBuffer.GetAllText(data);

		wcdata = (BSTR)calloc(nTextLength + 1,sizeof(OLECHAR));
		CreateBSTR(wcdata,nTextLength + 1,data,nTextLength + 1);

		free(data);
	}
	catch(...)
	{
		g_pMainFrame->AddOutputError("Unexpected error occurred while creating text buffer.");
		return;
	}

	// start timing
	DWORD tStart = GetTickCount();

	HRESULT				hr;
	VARIANT_BOOL		vbSuccess;
	HTREEITEM			tnRootNode	= TVI_LAST;
	IXMLDOMNode*		pXDN		= NULL;
	IXMLDOMElement*		pXE			= NULL;
	IXMLDOMDocument2*	pXDoc		= NULL;

	// initialize msxml.dll
	hr = CoInitialize(NULL); 
	if (CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMDocument, (void**)&pXDoc) == REGDB_E_CLASSNOTREG)
	{
		AfxMessageBox("Microsoft XML Library not found! Please make sure you have msxml3.dll registered.", MB_OK | MB_ICONEXCLAMATION,0);
		return;
	}

	try
	{		
		hr = pXDoc->QueryInterface(IID_IXMLDOMNode,(void**)&pXDN);

		// load the text into DOM
		hr = pXDoc->loadXML(wcdata, &vbSuccess);
		free(wcdata);

		IXMLDOMParseError* pXError;
		if (vbSuccess)
		{
			// validate
			if (pXDoc->validate(&pXError) == S_OK)
			{
				CString diff;
#ifdef _DEBUG
				diff.Format("Time: %d ms", GetTickCount() - tStart);
#endif

				// output "success"
				OutputMessage* pMsg = new OutputMessage();
				pMsg->enType		= OUTPUT_XMLERROR;
				pMsg->enResult		= MSG_SUCCESS;
				pMsg->strMessage	= "XML Validated. " + diff;
				pMsg->nLine			= 0;
				pMsg->nCol			= 0;
				g_pMainFrame->m_wndOutputBar.AddOutput(pMsg);
			}
			else
			{
				OutputMessage* pMsg = CreateXMLErrorMessage(pXError,true);
				g_pMainFrame->m_wndOutputBar.AddOutput(pMsg);
				RELEASE_XML(pXError);
			}
		}
		else
		{
			pXDoc->get_parseError(&pXError);
			OutputMessage* pMsg = CreateXMLErrorMessage(pXError,true);
			g_pMainFrame->m_wndOutputBar.AddOutput(pMsg);
			RELEASE_XML(pXError);
		}
	}
	catch (const char*) {}
	catch (...)
	{
		OutputMessage* pMsg = new OutputMessage();
		pMsg->enType		= OUTPUT_XMLERROR;
		pMsg->enResult		= MSG_ERROR;
		pMsg->strMessage	= "Internal Error (Exception)";
		pMsg->nLine			= 0;
		pMsg->nCol			= 0;
		g_pMainFrame->m_wndOutputBar.AddOutput(pMsg);
	}

	// free memory
	RELEASE_XML(pXDN);
	RELEASE_XML(pXDoc);
}

// modified from doccore.cpp
BOOL CBonfireDoc::DoFileSave()
{
	DWORD dwAttrib = GetFileAttributes(m_strPathName);
	if (dwAttrib & FILE_ATTRIBUTE_READONLY ||
		m_xTextBuffer.GetReadOnly())
	{
		// we do not have read-write access or the file does not (now) exist
		if (!DoSave(NULL))
		{
			TRACE0("Warning: File save with new name failed.\n");
			return FALSE;
		}
	}
	else
	{
		if (!DoSave(m_strPathName))
		{
			TRACE0("Warning: File save failed.\n");
			return FALSE;
		}
	}
	return TRUE;
}

// modified from doccore.cpp
BOOL CBonfireDoc::DoSave(LPCTSTR lpszPathName, BOOL bReplace)
	// Save the document data to a file
	// lpszPathName = path name where to save document file
	// if lpszPathName is NULL then the user will be prompted (SaveAs)
	// note: lpszPathName can be different than 'm_strPathName'
	// if 'bReplace' is TRUE will change file name if successful (SaveAs)
	// if 'bReplace' is FALSE will not change path name (SaveCopyAs)
{
	CString newName = lpszPathName;
	if (newName.IsEmpty())
	{
		CDocTemplate* pTemplate = GetDocTemplate();
		ASSERT(pTemplate != NULL);

		newName = m_strPathName;
		if (bReplace && newName.IsEmpty())
		{
			newName = m_strTitle;
			// check for dubious filename
			int iBad = newName.FindOneOf(_T(" #%;/\\"));
			if (iBad != -1)
				newName.ReleaseBuffer(iBad);

			// append the default suffix if there is one
			CString strExt;
			if (pTemplate->GetDocString(strExt, CDocTemplate::filterExt) &&
			  !strExt.IsEmpty())
			{
				ASSERT(strExt[0] == '.');
				newName += strExt;
			}
		}

		if (SaveFileDialog(AfxGetMainWnd(),newName,newName,NULL,
			GetFileFilter(FILTER_FILES),6,GetDefExt(FILTER_FILES)) <= 0)
			return FALSE; // don't even attempt to save
	}

	CWaitCursor wait;

	if (!OnSaveDocument(newName))
	{
		if (lpszPathName == NULL)
		{
			// be sure to delete the file
			TRY
			{
				CFile::Remove(newName);
			}
			CATCH_ALL(e)
			{
				TRACE0("Warning: failed to delete file after failed SaveAs.\n");
				do { e->Delete(); } while (0); //DELETE_EXCEPTION(e);
			}
			END_CATCH_ALL
		}
		return FALSE;
	}

	// reset the title and change the document name
	if (bReplace)
		SetPathName(newName);

	return TRUE;        // success
}

void CBonfireDoc::AddToFileWatch() 
{
	m_hFileWatch = CFileWatch::AddFileFolder(GetPathName(), NULL, this);
	BOOL bAutoReload = theApp.m_opOptions.general.dwReloadFiles;
	CFileWatch::SetAutoReload(m_hFileWatch, &bAutoReload);

	// set the value in case it was changed
	//theApp.m_opGenOptions.dwReloadFiles = bAutoReload;
}

void CBonfireDoc::RemoveFileWatch()
{
	CFileWatch::RemoveHandle(m_hFileWatch);
}
