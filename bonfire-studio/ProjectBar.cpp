// ProjectBar.cpp: implementation of the CProjectBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Bonfire.h"
#include "Globals.h"
#include "ProjectBar.h"
#include "MainFrm.h"
#include "AddFolderDlg.h"
#include <direct.h>
#include <errno.h>
#include <comdef.H>
#include <cderr.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CProjectBar, SIZEBAR_BASECLASS);

BEGIN_MESSAGE_MAP(CProjectBar, SIZEBAR_BASECLASS)
    //{{AFX_MSG_MAP(CProjectBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

extern CMainFrame* g_pMainFrame;
extern CBonfireApp theApp;
//extern CToolsOptionsDialog g_dlgOptions;

//static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

//////////////////////////////////////////////////////////////////////
// Constructor
//////////////////////////////////////////////////////////////////////
CProjectBar::CProjectBar()
{
	m_pMiscProject			= NULL;
	m_pCurProject			= NULL;
	m_pCurNode				= NULL;
	nNewProjects			= 1;
	nNewFolders				= 1;
	m_bExpanding			= FALSE;
	m_imgNewFile			= 5;
}

/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
CProjectBar::~CProjectBar()
{
	//delete all the CXNodes
	int i = 0;
}

/////////////////////////////////////////////////////////////////////////////
// called when project bar gets created
/////////////////////////////////////////////////////////////////////////////
int CProjectBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (SIZEBAR_BASECLASS::OnCreate(lpCreateStruct) == -1)
		return -1;

	//SetSCBStyle(GetSCBStyle() | SCBS_SHOWEDGES);

    if (!m_wndTree.Create(WS_DLGFRAME | 
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | 
		TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | 
		TVS_SHOWSELALWAYS | TVS_EDITLABELS,CRect(0,0,0,0),
		this,ID_PROJECT_TREE))return -1;

	// set up tree view
	m_wndTree.SetIndent(0); // set to minimum indent
	m_wndTree.ModifyStyleEx(0,WS_EX_CLIENTEDGE);

	CBitmap bmpIcons; bmpIcons.LoadBitmap(IDB_PROJBAR_ICONS);
	m_imlTreeImages.Create(16,16,ILC_COLOR32 | ILC_MASK,4,2);
	m_imlTreeImages.Add(&bmpIcons,RGB(255,0,255));
	m_imlTreeImages.SetOverlayImage(ICON_LINK_OVERLAY,ICON_LINK_OVERLAY + 1);
	m_wndTree.SetImageList(&m_imlTreeImages,TVSIL_NORMAL);
	m_wndTree.SetImageList(&m_imlTreeImages,TVSIL_STATE);
	bmpIcons.DeleteObject();

	HICON hIcon = (HICON)LoadImage(AfxGetResourceHandle(),MAKEINTRESOURCE(IDR_BONFIRE_TYPE),
			IMAGE_ICON,GetSystemMetrics(SM_CXSMICON),GetSystemMetrics(SM_CYSMICON),0);
	m_imgNewFile = m_imlTreeImages.Add(hIcon);
	DestroyIcon(hIcon);

	/*if (!m_wndButtons.CreateEx(this,TBSTYLE_FLAT, 
		WS_VISIBLE | WS_CHILD | CBRS_ALIGN_TOP | CCS_NOMOVEY | CBRS_TOOLTIPS,CRect(0,0,0,0),
		AFX_IDW_TOOLBAR + 40) ||
		!m_wndButtons.LoadToolBar(IDR_TOOLBAR_PROJECT))
		return -1;

	m_wndButtons.ModifyStyleEx(0,WS_EX_CLIENTEDGE,SWP_FRAMECHANGED);*/

	//m_wndButtons.LoadImages(IDR_TOOLBAR_PROJECT,AfxGetInstanceHandle());

	CreateMiscProject();
	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void CProjectBar::OnDestroy() 
{
	SIZEBAR_BASECLASS::OnDestroy();
	m_imlTreeImages.DeleteImageList();
}

/////////////////////////////////////////////////////////////////////////////
// event called onsizing of the control
/////////////////////////////////////////////////////////////////////////////
void CProjectBar::OnSize(UINT nType, int cx, int cy) 
{
	SIZEBAR_BASECLASS::OnSize(nType, cx, cy);

	CRect rc;
	GetClientRect(&rc);

	/*int nToolbarHeight = HIWORD(m_wndButtons.GetToolBarCtrl().GetButtonSize());
	m_wndButtons.MoveWindow(rc.left,rc.top,rc.Width(),nToolbarHeight,TRUE);
	rc.top += nToolbarHeight;*/

	m_wndTree.MoveWindow(&rc);
}

/////////////////////////////////////////////////////////////////////////////
// this function gets called when tree bar isnotified of an event
/////////////////////////////////////////////////////////////////////////////
BOOL CProjectBar::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	NMHDR* pNotify = (NMHDR*)lParam;

	//TRACE("From: %d ... Code: %d\n",pNotify->idFrom,pNotify->code);
		
	if (pNotify->idFrom == ID_PROJECT_TREE)
	{
		//TRACE("%d\t-",pNotify->code);
		switch (pNotify->code)
		{
		case TVN_BEGINLABELEDIT : // determine if the node can be edited
			if (m_pCurNode)
			{
				if (IsEditingAllowed(m_pCurNode))
				{
					*pResult = FALSE;

					//set the text to be only the name, not the fullpath
					m_wndTree.SetItemText(m_pCurNode->m_hTreeItem, m_pCurNode->m_strName);
					CEdit* pEdit = this->m_wndTree.GetEditControl();
					if (pEdit)
					{
						pEdit->SetWindowText(m_pCurNode->m_strName);
					}				
				}
				else // disallow
					*pResult = TRUE;
			}
			break;

		case TVN_ENDLABELEDIT :
			if (m_pCurNode)
			{
				LPTSTR temp = ((LPNMTVDISPINFO)lParam)->item.pszText;
				if (!temp)
					return FALSE;

				// get the new label
				CString newtext = temp;

				// create the message object
				CString strOutput;
				strOutput.Format("Renaming %s to %s", m_pCurNode->m_strName, newtext);
				OutputMessage* pMsg	= new OutputMessage();
				pMsg->enType		= OUTPUT_APPSTATUS;
				pMsg->enResult		= MSG_PROCESSING;
				pMsg->strMessage	= strOutput;

				g_pMainFrame->m_wndOutputBar.AddOutput(pMsg);

				if (m_pCurNode->m_enNodeType == XNODE_FILE && ((CXFile*)m_pCurNode)->m_bNew)
				{
					pMsg->enResult	= MSG_ERROR;
					pMsg->strResult	= "Cannot rename a new, unsaved file";
					g_pMainFrame->m_wndOutputBar.UpdateLast();
					return FALSE;
				}
				
				CString strCompare = AddSlash(m_pCurNode->m_strPath) + newtext;
				
				// if its a folder, then add a / at the end for correct comparison
				if (m_pCurNode->m_enNodeType == XNODE_FOLDER)
					strCompare = AddSlash(strCompare);

				// see if there were any changes
				if (m_pCurNode->FullPath().Compare(strCompare) == 0)
				{
					pMsg->enResult	= MSG_SUCCESS;
					pMsg->strResult	= "User cancelled operation";
					g_pMainFrame->m_wndOutputBar.UpdateLast();

					// simply update the node
					UpdateNode(m_pCurNode, FALSE);
					break;
				}

				CBonfireDoc* pDoc	= NULL;
				if (m_pCurNode->m_enNodeType == XNODE_FILE) 
					pDoc			= ((CXFile*)m_pCurNode)->m_pDoc;

				if (pDoc) pDoc->RemoveFileWatch();

				int retVal = rename((const char *)m_pCurNode->FullPath(), 
								(const char *)(AddSlash(m_pCurNode->m_strPath) + newtext));
				
				if (retVal == 0)
				{
					m_pCurNode->m_strName = newtext;
					switch (m_pCurNode->m_enNodeType)
					{
						case XNODE_PROJECT :
							theApp.AddToRecentProjectList(m_pCurNode->FullPath());
						case XNODE_FOLDER :
							// simply update the node
							UpdateNode(m_pCurNode, FALSE);
							break;
						case XNODE_FILE :
						{
							CString strNewName	= AddSlash(m_pCurNode->m_strPath) + newtext;
							
							if (pDoc)
								pDoc->SetPathName(strNewName, TRUE);
							else
								((CXFile*)m_pCurNode)->Update(strNewName, ((CXFile*)m_pCurNode)->m_bSaved);
							
							break;
						}
					}

					// set project to modified if its not actually th project node
					if (m_pCurNode->m_enNodeType != XNODE_PROJECT)
						m_pCurNode->m_pProject->SetModified();

					pMsg->enResult = MSG_SUCCESS;
					pMsg->strResult = "Success";
					g_pMainFrame->m_wndOutputBar.UpdateLast();
				}
				else
				{
					// get the error and prnt it out
					extern int errno;

					pMsg->enResult = MSG_ERROR;
					pMsg->strResult = strerror(errno);
					g_pMainFrame->m_wndOutputBar.UpdateLast();
				}

				if (pDoc) pDoc->AddToFileWatch();
			}
			break;

		case TVN_ITEMEXPANDING :
		{
			m_bExpanding = TRUE;
			
			m_wndTree.SelectItem(((NMTREEVIEW*)lParam)->itemNew.hItem);
			SetCurrentNode(FALSE);
			BOOL retVal = CWnd::OnNotify(wParam, lParam, pResult);

			m_bExpanding = FALSE;

			return retVal;

			break;
		}

		case TVN_KEYDOWN :
		{
			//TRACE("(%d)\n",((LPNMTVKEYDOWN)lParam)->wVKey);
			switch( ((LPNMTVKEYDOWN)lParam)->wVKey )
			{
			case VK_DELETE:
				ProjectRemoveFromProject();
				break;
			default:
				return FALSE;
			}
			break;
		}

		case NM_RETURN:
		case NM_DBLCLK:	
			SetCurrentNode(FALSE);
			// get the appropriate right-click menu
			if (m_pCurNode->m_enNodeType == XNODE_FILE)
			{
				OpenCurrentDocument();
				//*pResult = 1;
				g_pMainFrame->GetActiveFrame()->SetFocus();
			}
			else
				return CWnd::OnNotify(wParam, lParam, pResult);
			
			break;

		case NM_RCLICK :
			SetCurrentNode(TRUE);
			if (m_pCurNode)
			{
				BCMenu popmenu;

				// get the appropriate right-click menu
				switch (m_pCurNode->m_enNodeType)
				{
				case XNODE_PROJECT :
					popmenu.LoadMenu(IDR_POPUP_PROJECT);
					break;
				case XNODE_FOLDER :		
					popmenu.LoadMenu(IDR_POPUP_FOLDER);
					break;
				case XNODE_FILE :
					popmenu.LoadMenu(IDR_POPUP_FILE);
					break;
				default :
					return TRUE;
				}

				// set the icons for the menu and show the popup
				g_pMainFrame->SetMenuIcons(&popmenu);
				ShowPopup(AfxGetMainWnd(),(BCMenu*)popmenu.GetSubMenu(0));

				// clean up
				popmenu.DestroyMenu();
			}
			break;

		case TVN_SELCHANGED:
		case NM_CLICK:
			SetCurrentNode(FALSE);
			//TRACE0("NM_CLICK or TVN_SELCHANGED\n");
			break;
				
		default :
			return CWnd::OnNotify(wParam, lParam, pResult);
		}
	}
	else
	{
		return CWnd::OnNotify(wParam, lParam, pResult);
	}
	
	return TRUE; 
}

/////////////////////////////////////////////////////////////////////////////
// set the m_pCurNode when clicking inside the project tree
/////////////////////////////////////////////////////////////////////////////
bool CProjectBar::SetCurrentNode(BOOL bHitTest /* = FALSE */)
{
	// get the current node in the tree
	POINT pt;
	GetCursorPos(&pt);
	m_wndTree.ScreenToClient(&pt);
		
	// the reason we do hit test here is because
	// when we right-click on an item we want to manually select it
	// since the GetSelectedItem() might actually give us an old node
	HTREEITEM hSelItem = NULL;
	if (bHitTest)
		hSelItem = m_wndTree.HitTest(pt);
	if (hSelItem == NULL)
		hSelItem = this->m_wndTree.GetSelectedItem();
	if (hSelItem != NULL)
	{
		this->SelectTreeNode( (CXNode*)m_wndTree.GetItemData(hSelItem) );
	}
	return (m_pCurNode != NULL);
}

/////////////////////////////////////////////////////////////////////////////
// callback function for the sorting of tree nodes
/////////////////////////////////////////////////////////////////////////////
static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	return 1;
}

/////////////////////////////////////////////////////////////////////////////
// create miscellaneous project
/////////////////////////////////////////////////////////////////////////////
void CProjectBar::CreateMiscProject()
{
	CXProject* xMiscProj;
	if (m_pMiscProject != NULL)
	{
		xMiscProj = m_pMiscProject;
	}
	else
	{
		xMiscProj = new CXProject(true, true, true);
		xMiscProj->m_pParent = NULL;
		xMiscProj->m_strName = "Miscellaneous Files";
		xMiscProj->m_strPath = "";
		PopulateFolderTree(xMiscProj);
		m_arrProjects.push_back(xMiscProj);
	}

	// set the appropriate pointers
	m_pMiscProject	= xMiscProj;
	SelectTreeNode(m_pMiscProject);
	SetCurrentProject(xMiscProj);
}

/////////////////////////////////////////////////////////////////////////////
// adds a CXNode node to the tree
/////////////////////////////////////////////////////////////////////////////
BOOL CProjectBar::AddNodeToTree(CXNode* pNode, CXFolder* pParent, int nCopyMove, BOOL bDraw/* = TRUE*/, BOOL bOpen/* = FALSE*/)
{
	bool bExists	= false;
			
	if (!pParent)
	{
		CreateMiscProject();
		pParent = m_pMiscProject;
	}

	CString addUnderPath = AddSlash((pParent->m_enNodeType == XNODE_FOLDER) ? pParent->FullPath() : pParent->m_strPath) ;
	
	if (pNode->m_enNodeType == XNODE_FILE)
	{
		try
		{
			// see if the file already exists in the folder/project
			CXFile* pExistFile	= NULL;
			int size			= pParent->m_arrFiles.size();
			for (int i = 0; i < size; i++)
			{
				CXFile* pFile = (CXFile*)pParent->m_arrFiles[i];
				if (pFile->FullPath().CompareNoCase(pNode->FullPath()) == 0)
				{
					// simply select the existing file node
					if (bOpen)
						OpenDocument( pFile );
					SelectTreeNode( pFile );
					return FALSE;
				}
				else if (pFile->m_strName.CompareNoCase(pNode->m_strName) == 0)
				{
					pExistFile	= pFile;
					bExists		= true;
					break;
				}
			}

			switch (nCopyMove)
			{
			case CMA_COPY :
				// make sure its a different path			
				if (//pParent->m_enNodeType != XNODE_PROJECT &&
					pParent != m_pMiscProject &&
					AddSlash(pNode->m_strPath).Compare(addUnderPath))
				{
					CString newFilePath = addUnderPath + pNode->m_strName;
					if (bExists || !CopyFile(pNode->FullPath(), newFilePath, TRUE))
					{
						CString msg = "The file \"" + pNode->m_strName + "\" already exists in ";
						msg += addUnderPath + ". Do you want to overwrite the existing file?\nPress OK to overwrite.\nPress Cancel to abort the operation.";
						switch (AfxMessageBox(msg, MB_OKCANCEL | MB_ICONEXCLAMATION, 0))
						{
						case IDCANCEL : 
							throw "User aborted overwrite operation.";
						case IDOK : 
							if (!CopyFile(pNode->FullPath(), newFilePath, FALSE))
								throw "Error occured during copying of file.";
							break;
						}
					}
					pNode->m_strPath = addUnderPath;
				}
				break;
			case CMA_MOVE :
				// make sure its a different path			
				if (pParent == m_pMiscProject)
					throw "Moving to Miscellanouse Files is not allowed.";
					
				if (AddSlash(pNode->m_strPath).Compare(addUnderPath) == 0)
					throw "The destination path is the same as the source.";

				CString newFilePath = addUnderPath + pNode->m_strName;
				
				if (!bExists)
				{
					CBonfireDoc* pDoc	= ((CXFile*)pNode)->m_pDoc;
					BOOL bSuccess		= FALSE;
					if (pDoc)
					{
						pDoc->RemoveFileWatch();
						bSuccess		= MoveFile(pNode->FullPath(), newFilePath);
						pDoc->SetPathName(newFilePath, TRUE);
						pDoc->AddToFileWatch();
					}
					else
						bSuccess		= MoveFile(pNode->FullPath(), newFilePath);

					if (!bSuccess)
						throw "Cannot move file.";
				}
				else
				{
					throw "Cannot move file because it already exists in the destination path.";
				}
				pNode->m_strPath = addUnderPath;
				break;
			}

			pParent->m_arrFiles.push_back((CXFile*)pNode);
			if (bExists)
			{
				LinkTreeNodeToChild((CXFile*)pNode, pExistFile->m_pDoc);
				DeleteNode(pExistFile, TRUE);
			}
		}
		catch (char* e)
		{
			g_pMainFrame->AddOutputError(e);
			return FALSE;
		}
		catch (...)
		{
			g_pMainFrame->AddOutputError("Unexpected Error (Exception)");
			return FALSE;
		}
	}
	else
	{
		try
		{
			// see if the file already exists in the folder/project
			int size = pParent->m_arrFolders.size();
			for (int i = 0; i < size; i++)
			{
				CXFolder* pFolder = (CXFolder*)pParent->m_arrFolders[i];
				if (pFolder->FullPath().CompareNoCase(pNode->FullPath()) == 0)
				{
					// simply select the existing folder node
					SelectTreeNode( pFolder );
					return FALSE;
				}
			}

			pParent->m_arrFolders.push_back((CXFolder*)pNode);
		}
		catch (...)
		{
			g_pMainFrame->AddOutputError("Unexpected Error (Exception)");
			return FALSE;
		}
	}

	pNode->m_pParent	= pParent;
	pNode->m_pProject	= pParent->m_pProject;

	if (bDraw) 
		PopulateFolderTree(pParent);
	
	if (pParent && pParent->m_hTreeItem)
	{
		// expand parent if it's not already expanded
		if (m_wndTree.GetItemState(pParent->m_hTreeItem, TVIF_STATE) != TVIS_EXPANDED)
			m_wndTree.Expand(pParent->m_hTreeItem, TVE_EXPAND);
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// link a tree node to a child frame
/////////////////////////////////////////////////////////////////////////////
void CProjectBar::LinkTreeNodeToChild(CXFile* pFile, CBonfireDoc* pChildDoc)
{
	if (pChildDoc)
	{
		// remove the old pointer first
		if (pChildDoc->m_pXFile && pChildDoc->m_pXFile != pFile)
		{
			pChildDoc->m_pXFile->m_pDoc = NULL;

			// mark it as saved so not to bother with it
			pChildDoc->m_pXFile->m_bSaved = true;

			//pChildDoc->m_pXFile->m_bReadOnly = (pChildDoc->m_xTextBuffer.GetReadOnly() != 0);
		}

		pFile->m_pDoc		= pChildDoc;
		pChildDoc->m_pXFile = pFile;
		pChildDoc->UpdateDocumentTitle();
	}
}

/////////////////////////////////////////////////////////////////////////////
// load the file from given path
/////////////////////////////////////////////////////////////////////////////
CXFile* CProjectBar::LoadFile(CBonfireDoc* pNewDocument)
{
	HICON hIcon;
	CXFile* pNewFile	= new CXFile();
	CString strPath		= pNewDocument->GetPathName();

	try
	{
		if (strPath != "") //loading an existing file
		{
			pNewFile->m_bNew	= false;
			hIcon = GetIconForFile(strPath);
		}
		else // loading a new file
		{		
			pNewFile->m_bNew	= true;
			hIcon = m_imlTreeImages.ExtractIcon(5);
		}
		
		pNewFile->m_strName		= (pNewFile->m_bNew) ? pNewDocument->GetTitle() : GetFileTitle(strPath);
		pNewFile->m_strPath		= GetFilePath(strPath);
		pNewFile->m_bSaved		= true;

		if (!AddNodeToTree(pNewFile, NULL, CMA_NONE, TRUE, TRUE))
			return NULL;

		LinkTreeNodeToChild(pNewFile, pNewDocument);

		pNewFile->m_bReadOnly = (pNewDocument->m_xTextBuffer.GetReadOnly() != 0);

		SelectTreeNode(pNewFile);

		// set the appropriate pointers
		SetCurrentProject(m_pMiscProject);
		m_pMiscProject->SetModified();

		return pNewFile;
	}
	catch (...)
	{
		AfxMessageBox("Error in LoadFile()", MB_OK | MB_ICONEXCLAMATION,0);
		return NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
// load the project from given path
/////////////////////////////////////////////////////////////////////////////
bool CProjectBar::LoadProject(LPCTSTR strProjectPath)
{
	bool retVal = true;

	CString strOutput;
	strOutput.Format("Loading project: \"%s\" (%s)", GetFileTitle(strProjectPath), strProjectPath);
	
	// create the message object
	OutputMessage* pMsg	= new OutputMessage();
	pMsg->enType		= OUTPUT_APPSTATUS;
	pMsg->enResult		= MSG_PROCESSING;
	pMsg->strMessage	= strOutput;
	g_pMainFrame->m_wndOutputBar.AddOutput(pMsg);

	try
	{
		CXProject* xNewProj = new CXProject();
		CWaitCursor wc;
		if ( xNewProj->LoadProject(strProjectPath, pMsg) )
		{		
			// add project to project list
			m_arrProjects.push_back(xNewProj);
			
			// draw the project in the tree
			m_wndTree.SetRedrawFlag(FALSE);
			PopulateFolderTree(xNewProj);
			m_wndTree.SetRedrawFlag(TRUE);

			// set the appropriate pointer
			SetCurrentProject(xNewProj);
			
			xNewProj->m_bNew = false;
			xNewProj->SetModified(FALSE);

			// add project to MRU list
			theApp.AddToRecentProjectList(xNewProj->FullPath());

			//select the project tree node
			SelectTreeNode(xNewProj);

			pMsg->enResult = MSG_SUCCESS;
			pMsg->strResult = "Success";
			g_pMainFrame->m_wndOutputBar.UpdateLast();
		}	
		else
		{
			// the message was already set
			g_pMainFrame->m_wndOutputBar.UpdateLast();
			retVal = false;
		}
	}
	catch (...)
	{
		pMsg->enResult = MSG_ERROR;
		pMsg->strResult = "Error (Exception)";
		g_pMainFrame->m_wndOutputBar.UpdateLast();
		retVal = false;
	}

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////
// creates the tree items associated with a node
/////////////////////////////////////////////////////////////////////////////
void CProjectBar::PopulateFolderTree(CXFolder* pNode)
{
	if (pNode == NULL) return;
	
	// draw the node first
	DrawNode(pNode);
	
	// create folder items
	int size = pNode->m_arrFolders.size();
	for (int i = 0; i < size; i++)
	{
		CXFolder* pCurFolder = (CXFolder*)pNode->m_arrFolders[i];

		// recursively populate subfolders
		PopulateFolderTree( pCurFolder );
	}

	// create file items
	size = pNode->m_arrFiles.size();
	for (i = 0; i < size; i++)
		DrawNode( (CXFile*)pNode->m_arrFiles[i] );

	// sort the child nodes (not-recursive)
	pNode->SortChildren( &this->m_wndTree );
}

/////////////////////////////////////////////////////////////////////////////////
// draws the node if it doesnt have a tree item yet, otherwise update
/////////////////////////////////////////////////////////////////////////////////
void CProjectBar::DrawNode(CXNode *pNode)
{
	HTREEITEM		hParent;
	HTREEITEM		hInsertAfter;
	int				iImg;
	CString			caption	= GetFileTitle(pNode->m_strName);
	CBonfireApp*	pApp = (CBonfireApp*)AfxGetApp();

	// display path if optioned
	switch (pNode->m_enNodeType)
	{
	case XNODE_PROJECT :
		if( !((CXProject*)pNode)->m_bMisc && pApp->m_opOptions.projbar.dwShowPathProjects)
			caption += " - " + pNode->m_strPath;
		break;
	case XNODE_FOLDER :
		if(pApp->m_opOptions.projbar.dwShowPathFolders)
			caption += " - " + pNode->m_strPath;
		break;
	case XNODE_FILE :
		if(pApp->m_opOptions.projbar.dwShowPathFiles && !((CXFile*)pNode)->m_bNew)
			caption += " - " + pNode->m_strPath;
		break;
	}

	// if tree item doesnt exist, draw it
	if (!pNode->m_hTreeItem)
	{
		// set the parent of the node
		CXFolder* pParent = pNode->m_pParent;
		
		// if parent node exits then create root item, if not create as child of parent's item
		if (pParent)
		{
			hParent			= pParent->m_hTreeItem;
			hInsertAfter	= TVI_LAST;
			if (pNode->m_enNodeType == XNODE_FILE)
			{
				// get the index of the icon in the image list
				iImg = GetIconIndex(pNode->FullPath(), &m_imlTreeImages);
				if (iImg == -1)
					iImg = (((CXFile*)pNode)->m_bNew) ? ICON_FILE_NEW : ICON_FILE_NOTFOUND;
			}
			else
			{
				iImg = ICON_FOLDER_CLOSED;
			}
		}
		else
		{
			hParent			= NULL;
			hInsertAfter	= (((CXProject*)pNode)->m_bMisc) ? TVI_LAST : TVI_FIRST;
			iImg			= ICON_PROJECT;
		}

		// insert the node into the tree
		pNode->m_hTreeItem = m_wndTree.InsertItem(caption, iImg, iImg, hParent, hInsertAfter);

		// check if its virtual, and draw overlay if needed
		if ( pNode->m_enNodeType == XNODE_FOLDER && ((CXFolder*)pNode)->m_bVirtual)
		{
			m_wndTree.SetItemState(pNode->m_hTreeItem, 
				INDEXTOOVERLAYMASK(ICON_LINK_OVERLAY + 1),
				TVIS_OVERLAYMASK);
		}
		m_wndTree.SetItemData(pNode->m_hTreeItem, (DWORD)pNode);
	}
	else // change some properties according to options
	{
		UpdateNode(pNode, FALSE);
	}
}

/////////////////////////////////////////////////////////////////////////////
// updates tree node
/////////////////////////////////////////////////////////////////////////////
void CProjectBar::UpdateNode(CXNode *pNode, BOOL bRecurse)
{
	HTREEITEM		pTreeItem;

	if (pNode && (pTreeItem = pNode->m_hTreeItem))
	{
		int iImg;
		CString	caption = pNode->m_strName;
		switch (pNode->m_enNodeType)
		{
		case XNODE_PROJECT :
			// set the caption variable
			if( !((CXProject*)pNode)->m_bMisc && theApp.m_opOptions.projbar.dwShowPathProjects)
				caption += " - " + pNode->m_strPath;
			break;
		case XNODE_FOLDER :
			// set/remove the virtual mask
			if ( ((CXFolder*)pNode)->m_bVirtual )
				m_wndTree.SetItemState(pNode->m_hTreeItem, 
					INDEXTOOVERLAYMASK(ICON_LINK_OVERLAY + 1), TVIS_OVERLAYMASK);
			else
				m_wndTree.SetItemState(pNode->m_hTreeItem,0,TVIS_OVERLAYMASK);
			
			// set the caption variable
			if(theApp.m_opOptions.projbar.dwShowPathFolders)
				caption += " - " + pNode->m_strPath;
			break;
		case XNODE_FILE :
			// change the icon in case the file was saved as different type
			iImg = GetIconIndex(pNode->FullPath(), &m_imlTreeImages);
			if (iImg == -1)
				iImg = (((CXFile*)pNode)->m_bNew) ? ICON_FILE_NEW : ICON_FILE_NOTFOUND;
			m_wndTree.SetItemImage(pTreeItem, iImg, iImg);

			// set the caption variable, display path if file not new
			if(theApp.m_opOptions.projbar.dwShowPathFiles && !((CXFile*)pNode)->m_bNew)
				caption += " - " + pNode->m_strPath;

			// see if the file already exists in the folder/project
			int size = pNode->m_pParent->m_arrFiles.size();
			for (int i = 0; i < size; i++)
			{
				CXFile* pFile = (CXFile*)pNode->m_pParent->m_arrFiles[i];
				if (pNode != pFile && pFile->FullPath().CompareNoCase(pNode->FullPath()) == 0)
				{
					DeleteNode(pFile, FALSE);
					break;
				}
			}
			break;
		}

		// set the caption
		m_wndTree.SetItemText(pTreeItem, caption);

		// recursively update child nodes
		if(pNode->m_enNodeType != XNODE_FILE && bRecurse)
		{
			// create folder items
			int i = 0, size = 0;
			std::vector<CXFolder*>*	arrFolders = &((CXFolder*)pNode)->m_arrFolders;
			size = arrFolders->size();
			for (i = 0; i < size; i++)
				UpdateNode( (*arrFolders)[i], bRecurse );

			// create file items
			std::vector<CXFile*>* arrFiles = &((CXFolder*)pNode)->m_arrFiles;
			size = arrFiles->size();
			for (i = 0; i < size; i++)
				UpdateNode( (*arrFiles)[i], bRecurse );
		}

		if (pNode->m_pParent)
			// sort the sibling nodes (not-recursive)
			pNode->m_pParent->SortChildren( &this->m_wndTree );
	}
}

/////////////////////////////////////////////////////////////////////////////
// select a specific node in the tree
/////////////////////////////////////////////////////////////////////////////
void CProjectBar::SelectTreeNode(CXNode* pCurNode)
{
	if (pCurNode && pCurNode->m_hTreeItem)
	{
		/*
		if (m_pCurNode && m_pCurNode != pCurNode)
		{
			//set the icon of the OLD NODE'S parent folder to "open"
			CXFolder* pOldNode = (CXFolder*)((m_pCurNode->m_enNodeType == XNODE_FOLDER) ? m_pCurNode : m_pCurNode->m_pParent);
			if (pOldNode && pOldNode->m_enNodeType == XNODE_FOLDER)
			{
				this->m_wndTree.SetItemImage(pOldNode->m_hTreeItem, 1, 1);
			}
		}
		*/

		// select the current node
		m_wndTree.SelectItem(pCurNode->m_hTreeItem);
		m_pCurNode = pCurNode;
		
		// if the current node is a folder then use it, otherwise try the parent
		CXFolder* pNode = NULL;
		if (pCurNode->m_enNodeType == XNODE_FOLDER)
		{
			pNode = (CXFolder*)pCurNode;
		}
		else if (pCurNode->m_pParent && pCurNode->m_pParent->m_enNodeType == XNODE_FOLDER)
		{
			pNode = (CXFolder*)pCurNode->m_pParent;
		}

		// change the folder icon to "open" if the node is expanded or expanding
		// NOTE : This method can be called after notification of EXPANDING
		if (pNode)
		{
			BOOL bExpanded = (m_wndTree.GetItemState(pNode->m_hTreeItem, TVIS_EXPANDED) & TVIS_EXPANDED) 
						? TRUE : FALSE;
			
			int iImg = (m_bExpanding && bExpanded) || (!m_bExpanding && !bExpanded)
						? ICON_FOLDER_CLOSED : ICON_FOLDER_OPEN;

			m_wndTree.SetItemImage(pNode->m_hTreeItem, iImg, iImg);
		}

		// set the current project
		SetCurrentProject( pCurNode->m_pProject );
	}
}

/////////////////////////////////////////////////////////////////////////////
// creates new project andputs it into the array of projects
/////////////////////////////////////////////////////////////////////////////
void CProjectBar::CreateNewProject() 
{
	CString projectFullPath;

	// create the message object
	OutputMessage* pMsg = new OutputMessage();
	
	try
	{
		int nResult = SaveFileDialog(AfxGetMainWnd(),projectFullPath,NULL,"New Project (Set Location)",
			GetFileFilter(FILTER_PROJECTS),1,GetDefExt(FILTER_PROJECTS));

		if (nResult > 0)
		{
			// get the filename
			//if (projectFullPath.Find('.', 0) < 1)
			//	projectFullPath += ".xpr";

			pMsg->enType = OUTPUT_APPSTATUS;
			pMsg->enResult = MSG_PROCESSING;
			pMsg->strMessage = "Creating new project " + projectFullPath;
			
			g_pMainFrame->m_wndOutputBar.AddOutput(pMsg);

			// create new project
			CXProject* pNewProj		= new CXProject(false, false, true);
			pNewProj->m_pParent		= NULL;
			
			// update the new name for the project
			pNewProj->m_strPath = GetFilePath(projectFullPath);
			pNewProj->m_strName = GetFileTitle(projectFullPath);
			
			//xNewProj->m_strName.Format("Project%d.xpr",this->nNewProjects++);
			m_arrProjects.push_back(pNewProj);
			PopulateFolderTree(pNewProj);
			
			// create the file on disk
			pNewProj->SaveProjectToPath(projectFullPath);

			// set the appropriate pointers
			SetCurrentProject(pNewProj);
			SelectTreeNode(pNewProj);

			// add project to MRU list
			theApp.AddToRecentProjectList(projectFullPath);
		
			pMsg->enResult = MSG_SUCCESS;
			pMsg->strResult = "Success";
			g_pMainFrame->m_wndOutputBar.UpdateLast();
		}
		else
		{
			delete pMsg;	
		}
	}
	catch (...)
	{
		pMsg->enResult = MSG_ERROR;
		pMsg->strResult = "Error (Exception)";
		g_pMainFrame->m_wndOutputBar.UpdateLast();
	}
}

/////////////////////////////////////////////////////////////////////////////
// adds selected files to current object
/////////////////////////////////////////////////////////////////////////////
void CProjectBar::ProjectAddFilesToProject() 
{
	CXNode*		t_pCurNode		= m_pCurNode;
	CXProject*	t_pCurProject	= m_pCurProject;

	CStringArray arrStrFiles;
	int result = OpenFilesDialog(this,arrStrFiles,TRUE,NULL,"Add Files to Project",
		GetFileFilter(FILTER_FILES),6); // don't use default extension

	int nNumFiles = arrStrFiles.GetSize();
	if (result > 0 && nNumFiles > 0)
	{
		CXFolder* pAddUnder = t_pCurProject;
		// get the node under which to add the file(s)
		if (t_pCurNode != NULL)
		{
			if (t_pCurNode->m_enNodeType == XNODE_FILE)
				pAddUnder = t_pCurNode->m_pParent;
			else
				pAddUnder = (CXFolder*)t_pCurNode;
		}
		
		if (nNumFiles == 1)
		{
			// one file - open it
			CXFile* pNewFile		= new CXFile();
			pNewFile->m_bNew		= false;
			pNewFile->m_bSaved		= true;
			pNewFile->m_strPath		= GetFilePath(arrStrFiles[0]);
			pNewFile->m_strName		= GetFileTitle(arrStrFiles[0]);

			AddNodeToTree(pNewFile, pAddUnder, CMA_COPY, TRUE);
			SelectTreeNode(pNewFile);
		}
		else
		{
			// multiple files - first get directory
			CString dir = AddSlash(arrStrFiles[0]);
			for(int i = 1; i < nNumFiles; i++)
			{
				CXFile* pNewFile		= new CXFile();
				pNewFile->m_bNew		= false;
				pNewFile->m_bSaved		= true;
				pNewFile->m_strName		= arrStrFiles[i];
				pNewFile->m_strPath		= dir;

				AddNodeToTree(pNewFile, pAddUnder, CMA_COPY, TRUE);
			}

			// select the new file's node
			SelectTreeNode(pAddUnder);
		}
		t_pCurProject->SetModified();
	}
}

// recursive function that traverses a folder, creating CXNodes for subfolders
BOOL CProjectBar::AddChildren(CXFolder* pParentFolder, CString strFileFilter, BOOL bSubfolders)
{
	// go through each file/folder
	struct _finddata_t c_file;
	long hFile = 0;
	CString strFind = AddSlash(pParentFolder->FullPath());
	
	hFile = _findfirst(strFind + "*", &c_file);
	if (hFile == -1L)
		return FALSE;

	//first do directories
	if (bSubfolders)
	{
		do
		{
			if (strcmp(c_file.name, ".") == 0 || strcmp(c_file.name, "..") == 0) continue;
			
			if (c_file.attrib & _A_SUBDIR) //folder
			{
				CXFolder* pNewFolder = new CXFolder();
				pNewFolder->m_strName = GetFileTitle(c_file.name);
				pNewFolder->m_strPath = strFind;
				pNewFolder->m_bVirtual = false;

				// add the folder to the tree
				if (!AddNodeToTree(pNewFolder, pParentFolder, CMA_NONE, FALSE))
					return FALSE;

				// call the function recursively
				AddChildren(pNewFolder, strFileFilter, bSubfolders);
			}
		}
		while (_findnext(hFile, &c_file ) != -1L );
	}

	// go through all file filters to find files
	strFileFilter += ";";
	int i = 0;
	while( ((i = strFileFilter.FindOneOf(",")) > -1) ||
			((i = strFileFilter.FindOneOf(";")) > -1) )
	{
		CString ext = strFileFilter.Left(i);
		strFileFilter = strFileFilter.Right(strFileFilter.GetLength() - i - 1);
		hFile = _findfirst(strFind + ext, &c_file);
		if (hFile == -1L)
			continue;

		do
		{
			if (strcmp(c_file.name, ".") == 0 || strcmp(c_file.name, "..") == 0) continue;
			
			if (! (c_file.attrib & _A_SUBDIR) ) //files
			{
				CXFile* pNewFile = new CXFile();
				pNewFile->m_strName = GetFileTitle(c_file.name);
				pNewFile->m_strPath = strFind;
				pNewFile->m_bNew = false;
				pNewFile->m_bSaved = true;

				// add the folder to the tree
				AddNodeToTree(pNewFile, pParentFolder, CMA_NONE, FALSE);
			}
		}
		while (_findnext(hFile, &c_file ) != -1L );
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// add folder to project
/////////////////////////////////////////////////////////////////////////////
void CProjectBar::ProjectAddDirToProject() 
{
	CAddFolderDlg dlg;
	if (m_pCurNode != NULL)
	{
		CXNode* t_pCurNode = m_pCurNode;
		CXProject* t_pCurProject = m_pCurProject;

		if (dlg.DoModal() == IDOK)
		{
			CString strNewFolderPath = dlg.m_strPath;
			if (strNewFolderPath.GetLength() < 1)
				return;

			// show the wait cursor until everything is done
			CWaitCursor wc;

			BOOL bSubFolders = dlg.m_bAddSubFolders;
			
			// get the node under which to add the folder(s)
			CXFolder* pAddUnder = NULL;
			if (t_pCurNode->m_enNodeType == XNODE_FILE)
				pAddUnder = t_pCurNode->m_pParent;
			else
				pAddUnder = (CXFolder*)t_pCurNode;

			// determine the full path of the addUnder node
			CString strAddUnderPath = (pAddUnder->m_enNodeType == XNODE_FOLDER 
								? pAddUnder->FullPath()
								: pAddUnder->m_strPath);

			// the new folder:
			CXFolder* pNewFolder = new CXFolder();
			pNewFolder->m_strName = GetFileTitle(strNewFolderPath);
			pNewFolder->m_strPath = GetFilePath(strNewFolderPath);
			
			// "virtual" folder goes under project node
			if(GetFilePath(strNewFolderPath).CompareNoCase(AddSlash(strAddUnderPath)) != 0)
				pNewFolder->m_bVirtual = true;
			
			CString msg = "Adding Folder " + strNewFolderPath;
			if (bSubFolders) msg += " and subfolders";
			msg += " under " + pAddUnder->m_strName + "...";
				
			// add the folder to the tree 
			if (!AddNodeToTree(pNewFolder, pAddUnder))
				return;

			AddChildren(pNewFolder, dlg.m_strAddFiles, bSubFolders);
			
			// draw all the new nodes
			m_wndTree.SetRedrawFlag(FALSE);
			PopulateFolderTree(pNewFolder);
			m_wndTree.SetRedrawFlag(TRUE);

			t_pCurProject->SetModified();

			// select its node
			SelectTreeNode(pNewFolder);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// updates to see if there is a project
/////////////////////////////////////////////////////////////////////////////
void CProjectBar::UpdateProjectNewFolder(CCmdUI *pCmdUI)
{
	bool tr = (m_pCurNode != NULL && m_pCurProject != NULL && m_pCurProject != m_pMiscProject);
	pCmdUI->Enable( tr ? TRUE : FALSE );
}

/////////////////////////////////////////////////////////////////////////////
// create a new folder in the project
/////////////////////////////////////////////////////////////////////////////
void CProjectBar::ProjectNewFolder()
{
	if (m_pCurNode != NULL)
	{
		// get the node under which to add the folder(s)
		CXFolder* pAddUnder = NULL;
		if (m_pCurNode->m_enNodeType == XNODE_FILE)
			pAddUnder = m_pCurNode->m_pParent;
		else
			pAddUnder = (CXFolder*)m_pCurNode;

		// determine the full path of the addUnder node
		CString strAddUnderPath = (pAddUnder->m_enNodeType == XNODE_FOLDER 
			? pAddUnder->FullPath()
			: pAddUnder->m_strPath);

		// create the new folder:
		CXFolder* pNewFolder = new CXFolder();
		pNewFolder->m_strName.Format("NewFolder%d", this->nNewFolders++);
		pNewFolder->m_strPath = strAddUnderPath;
		pNewFolder->m_bVirtual = false;

		extern int errno;
		while(true)
		{
			if ( _mkdir(pNewFolder->FullPath()) == 0) //success
			{
				break;
			}
			else //failure
				if (errno != EEXIST)
				{
					g_pMainFrame->AddOutputError("Error creating new folder in " + strAddUnderPath);
					delete pNewFolder;
					return;
				}

			//try next NewFolderX
			pNewFolder->m_strName.Format("NewFolder%d", this->nNewFolders++);
		}

		// add the folder to the tree 
		AddNodeToTree(pNewFolder, pAddUnder, TRUE);

		//set current project modified
		pAddUnder->m_pProject->SetModified();

		// select its node
		SelectTreeNode(pNewFolder);

		//let the user edit the name of the folder
		this->m_wndTree.EditLabel(pNewFolder->m_hTreeItem);
	}
}

/////////////////////////////////////////////////////////////////////////////
// updates to see if the current project is not miscellaneous
/////////////////////////////////////////////////////////////////////////////
void CProjectBar::UpdateProjectAddFilesToProject(CCmdUI* pCmdUI) 
{
	bool tr = (m_pCurNode != NULL && m_pCurNode->m_enNodeType != XNODE_FILE && m_pCurProject != NULL);
	pCmdUI->Enable( tr ? TRUE : FALSE );
}

/////////////////////////////////////////////////////////////////////////////
// updates to see if the current project is not miscellaneous
/////////////////////////////////////////////////////////////////////////////
void CProjectBar::UpdateProjectAddDirToProject(CCmdUI* pCmdUI) 
{
	bool tr = (m_pCurNode != NULL && m_pCurNode->m_enNodeType != XNODE_FILE && m_pCurProject != NULL);
	pCmdUI->Enable( tr ? TRUE : FALSE );
}

/////////////////////////////////////////////////////////////////////////////
// updates to see if the current node is a file
/////////////////////////////////////////////////////////////////////////////
void CProjectBar::UpdateProjectRemoveFromProject(CCmdUI* pCmdUI) 
{
	bool tr = (m_pCurNode != NULL && (m_pCurNode->m_enNodeType == XNODE_FILE || 
							m_pCurNode->m_enNodeType == XNODE_FOLDER ||
							m_pCurNode->m_enNodeType == XNODE_VIRTUALFOLDER) );
	pCmdUI->Enable( tr ? TRUE : FALSE );
}

/////////////////////////////////////////////////////////////////////////////
// updates to see if the current project is not miscellaneous
/////////////////////////////////////////////////////////////////////////////
void CProjectBar::ProjectRemoveFromProject() 
{
	if (m_pCurNode != m_pMiscProject)
	{
		m_wndTree.SetRedrawFlag(FALSE);
		DeleteNode(m_pCurNode);
		m_wndTree.SetRedrawFlag(TRUE);
	}
}

/////////////////////////////////////////////////////////////////////////////
// updates to see if the current project is not miscellaneous
/////////////////////////////////////////////////////////////////////////////
void CProjectBar::UpdateProjectClose(CCmdUI* pCmdUI) 
{
	bool tr = (m_pCurProject != NULL && !m_pCurProject->m_bMisc);
	pCmdUI->Enable( tr ? TRUE : FALSE );
}

/////////////////////////////////////////////////////////////////////////////
// updates to see if the current project is not saved
/////////////////////////////////////////////////////////////////////////////
void CProjectBar::UpdateProjectSave(CCmdUI *pCmdUI)
{
	bool tr = (m_pCurProject != NULL && !m_pCurProject->m_bMisc && !m_pCurProject->m_bSaved );
	pCmdUI->Enable( tr ? TRUE : FALSE );
}

void CProjectBar::UpdateProjectSaveAs(CCmdUI *pCmdUI)
{
	bool tr = (m_pCurProject != NULL);// && !m_pCurProject->m_bSaved );
	pCmdUI->Enable( tr ? TRUE : FALSE );
	//UpdateProjectSave(pCmdUI);
}

/////////////////////////////////////////////////////////////////////////////
// update the opening(showing) of file
/////////////////////////////////////////////////////////////////////////////
void CProjectBar::UpdateFileShow(CCmdUI *pCmdUI)
{
	bool tr = (m_pCurNode != NULL && m_pCurNode->m_enNodeType == XNODE_FILE );
	pCmdUI->Enable( tr ? TRUE : FALSE );	
}

/////////////////////////////////////////////////////////////////////////////
// remove File node from tree (has tobe separate to be called from CDocument::OnClose...
/////////////////////////////////////////////////////////////////////////////
void CProjectBar::DeleteFileNode(CXFile *pFile)
{
	if (pFile)
	{
		if (pFile->m_hTreeItem)
		{
			// delete the tree node
			this->m_wndTree.DeleteItem(pFile->m_hTreeItem);
		}
		
		//delete the file from its parent
		CXFolder* pParent = pFile->m_pParent;
		int size = pParent->m_arrFiles.size();
		for (int i = 0; i < size; i++)
		{
			CXFile* pCurFile = (CXFile*)pParent->m_arrFiles[i];
			if (pCurFile == pFile)
			{
				//delete the CXFile from the parent
				pParent->m_arrFiles.erase(pParent->m_arrFiles.begin()+i);
				break;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// delete a CXNode from the tree structure (recurses down to childen)
/////////////////////////////////////////////////////////////////////////////
BOOL CProjectBar::DeleteNode(CXNode* pDelNode, BOOL bMoving/* = FALSE*/)
{
	int			i		= 0;
	int			size	= 0;
	CXFolder*	pParent = NULL;

	if (pDelNode == NULL/* || m_pCurNode->m_enNodeType == XNODE_PROJECT*/)
		return FALSE;

	try
	{
		switch ( pDelNode->m_enNodeType)
		{
		case XNODE_FILE :
		{
			// is it ok to delete? (save?)
			CBonfireDoc* pDoc = ((CXFile*)pDelNode)->m_pDoc;
			if (pDoc && !bMoving)
			{
				if (!pDoc->SaveModified())
					return FALSE;
				else
					//close the child
					pDoc->OnCloseDocument();
			}

			// delete the tree node
			DeleteFileNode((CXFile*)pDelNode);
			break;
		}
		case XNODE_PROJECT :
			//if ( !((CXProject*)pDelNode)->m_bSaved )
			//	return FALSE;
			// keep processing as if it's a folder
		case XNODE_FOLDER :
			//delete all files from the folder that is being deleted
			int filessize = ((CXFolder*)pDelNode)->m_arrFiles.size();
			for (int j = filessize-1; j >= 0; j--)
				if (!DeleteNode((CXFile*)((CXFolder*)pDelNode)->m_arrFiles[j]))
					return FALSE;

			//delete all folders from the folder that is being deleted
			int foldersssize = ((CXFolder*)pDelNode)->m_arrFolders.size();
			for (int k = foldersssize-1; k >= 0; k--)
				if (!DeleteNode((CXNode*)((CXFolder*)pDelNode)->m_arrFolders[k]))
					return FALSE;

			// delete the tree node
			this->m_wndTree.DeleteItem(pDelNode->m_hTreeItem);

			//delete the folder from its parent
			pParent = pDelNode->m_pParent;
			if (pParent)
			{
				int pfolderssize = pParent->m_arrFolders.size();
				for (int i = 0; i < pfolderssize; i++)
				{
					CXFolder* pCurFolder = (CXFolder*)pParent->m_arrFolders[i];
					if (pCurFolder == pDelNode)
					{
						//delete the CXFolder from the parent
						pParent->m_arrFolders.erase(pParent->m_arrFolders.begin()+i);
						break;
					}
				}
			}
			break;
		}

		// select its parent after deleting
		SelectTreeNode(pDelNode->m_pParent);

		// the project has been "modified"
		pDelNode->m_pProject->SetModified();

		delete pDelNode;
	}
	catch (...) 
	{
		return FALSE;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// save the current project
/////////////////////////////////////////////////////////////////////////////
void CProjectBar::ProjectSave()
{
	if (m_pCurProject)
	{
		if (m_pCurProject->SaveProject() && m_pCurProject == m_pMiscProject)
			// update the ponter for misc project
			m_pMiscProject = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
// save the current project as another file name
/////////////////////////////////////////////////////////////////////////////
void CProjectBar::ProjectSaveAs()
{
	if (m_pCurProject)
	{
		if(m_pCurProject == m_pMiscProject)
		{
			// update the ponter for misc project
			if(!m_pCurProject->SaveProjectAs())
				return;

			// store the pointer to the current project
			CXProject* pProject = m_pCurProject;

			m_pMiscProject = NULL;
			
			// re-create the misc project
			CreateMiscProject();

			// select the project that was just saved
			SelectTreeNode(pProject);
		}
		else
		{
			// simply call the function
			m_pCurProject->SaveProjectAs();
		}

		// add project to MRU list
		theApp.AddToRecentProjectList(m_pCurProject->FullPath());
	}
}

/////////////////////////////////////////////////////////////////////////////
// open the child frame with the current document
/////////////////////////////////////////////////////////////////////////////
void CProjectBar::OpenCurrentDocument()
{
	if (m_pCurNode != NULL && m_pCurNode->m_enNodeType == XNODE_FILE)
		OpenDocument( (CXFile*)m_pCurNode );
}

/////////////////////////////////////////////////////////////////////////////
// open the child frame with the specified document
/////////////////////////////////////////////////////////////////////////////
CBonfireDoc* CProjectBar::OpenDocument(CXFile *pFileNode)
{
	if (pFileNode != NULL && pFileNode->m_enNodeType == XNODE_FILE)
	{
		try
		{
			CBonfireDoc* selDoc = pFileNode->m_pDoc;
			BOOL bExisted		= FALSE;
			if (selDoc != NULL)
			{
				// a CBonfireDoc is available
				bExisted		= TRUE;
				CView* pAcView	= NULL;
				if (!g_pMainFrame->GetActiveFrame() || !g_pMainFrame->GetActiveFrame()->GetActiveView())
					return FALSE;

				if ( g_pMainFrame->GetActiveFrame()->GetActiveView()->GetDocument() == selDoc)
				{
					// if the current view is a view of the selected document, activate it
					pAcView			= g_pMainFrame->GetActiveFrame()->GetActiveView();
				}
				else
				{
					// if not, then find the first view and activate that
					POSITION pos	= selDoc->GetFirstViewPosition();
					pAcView			= selDoc->GetNextView(pos);
					pAcView			= pAcView->GetParentFrame()->GetActiveView();
				}

				// set focus on the child frame
				pAcView->GetParentFrame()->ActivateFrame();
			}
			else
			{
				// a CBonfireDoc is not available, open the document from strPath
				CString strFullPath = pFileNode->FullPath();
				selDoc = theApp.OpenExistingDocumentFile(strFullPath);

				if (selDoc != NULL && selDoc->IsKindOf(RUNTIME_CLASS(CBonfireDoc)))
				{
					LinkTreeNodeToChild((CXFile*)pFileNode, selDoc);
				}
				else
				{
					throw "Error creating document";
				}
			}

			selDoc->m_xTextBuffer.SetReadOnly(pFileNode->m_bReadOnly);
			selDoc->UpdateDocumentTitle();

			// switch to the first view under configuration settings - HACK
			if (!bExisted)
				selDoc->SwitchToFirstView();

			g_pMainFrame->GetActiveFrame()->GetActiveView()->SetFocus();

			return selDoc;
		}
		catch(...)
		{						
			CString output = "Error occured during loading of document window for "
							+ m_pCurNode->FullPath() + "!";
			g_pMainFrame->AddOutputError(output);
		}
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// close all projects
//////////////////////////////////////////////////////////////////////////
BOOL CProjectBar::CloseAllProjects()
{
	int size = this->m_arrProjects.size();
	for (int j = 0; j < size; j++)
	{
		CXProject* pProject = (CXProject*)m_arrProjects[j];
		if (!pProject->m_bMisc && !ProjectClose(pProject) )
			return FALSE;
	}

	if (!ProjectClose(m_pMiscProject) )
		return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// save all projects
//////////////////////////////////////////////////////////////////////////
BOOL CProjectBar::SaveAllProjects()
{
	int size = this->m_arrProjects.size();
	for (int j = 0; j < size; j++)
	{
		CXProject* pProject = (CXProject*)m_arrProjects[j];
		if (!pProject->m_bMisc && !pProject->SaveProject() )
			return FALSE;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// close the current project
/////////////////////////////////////////////////////////////////////////////
bool CProjectBar::CloseCurrentProject()
{
	return ProjectClose(m_pCurProject);
}

/////////////////////////////////////////////////////////////////////////////
// close the specified project
/////////////////////////////////////////////////////////////////////////////
bool CProjectBar::ProjectClose(CXProject *pProject)
{
	try
	{
		if (!pProject->m_bMisc && !pProject->m_bSaved)
		{
			// ask to save project
			switch (AfxMessageBox("Do you want to save the project " + pProject->FullPath() + "?", 
				MB_YESNOCANCEL | MB_ICONQUESTION, 0))
			{
			case IDCANCEL : return false;
			case IDYES : 
				if (!pProject->SaveProject())
					return false;
				break;
			case IDNO : break;
			}
		}

		// find the project node in the top array
		int size	= m_arrProjects.size();
		int i		= 0;
		for (; i < size; i++)
		{
			CXProject* pArrProject = (CXProject*)m_arrProjects[i];
			if (pArrProject == pProject)
			{
				break;
			}
		}

		// clean up all the nodes, last chance to save open documents
		m_wndTree.SetRedrawFlag(FALSE);
		BOOL retVal = this->DeleteNode(pProject);
		m_wndTree.SetRedrawFlag(TRUE);
		if (!retVal)
			return FALSE;

		// finally remove the project node
		if (i < size)
			m_arrProjects.erase(m_arrProjects.begin()+i);

		// set the appropriate current project
		if ( m_arrProjects.size() )
		{
			CXProject* pProject = (CXProject*)m_arrProjects[0];
			SetCurrentProject(pProject);
		}
	}
	catch (...)
	{
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// set the pointer tothe current project
/////////////////////////////////////////////////////////////////////////////
void CProjectBar::SetCurrentProject(CXProject *pProject)
{
	// check if we are actually switching to another project
	if (pProject != m_pCurProject)
	{
		if (m_pCurProject && m_pCurProject->m_hTreeItem)
			m_wndTree.SetItemState(m_pCurProject->m_hTreeItem, 0, TVIS_BOLD);

		if (pProject && pProject->m_hTreeItem)
		{
			m_pCurProject = pProject;
			m_wndTree.SetItemState(pProject->m_hTreeItem, TVIS_BOLD, TVIS_BOLD);
		}
	}
}

// retrieve private member (m_pCurProject)
CXProject* CProjectBar::GetCurrentProject()
{
	return this->m_pCurProject;
}

CXNode* CProjectBar::GetCurrentNode()
{
	return this->m_pCurNode;
}

// show the current file's properties
void CProjectBar::ShowFileProperties(BOOL bProj) 
{
	struct _finddata_t c_file;
	CXNode* pNode = (bProj) ? m_pCurNode->m_pProject : m_pCurNode;
	
	if (pNode->m_enNodeType == XNODE_PROJECT && ((CXProject*)pNode)->m_bMisc )
	{
		AfxMessageBox("The Miscellaneous project is not a project that exists as a file on your hard disk.\nInstead, it is a logical gathering of files which CAN be saved as a project file with a different name.", MB_OK | MB_ICONINFORMATION, 0);
		return;
	}

	if( _findfirst(pNode->FullPath(), &c_file) == -1L )
	{
		AfxMessageBox("The file was not found", MB_OK | MB_ICONEXCLAMATION, 0);
	}
	else
	{
		// get the file attributes and print them to a modal dialog box
		CTime tmCreated(c_file.time_create);
		CTime tmModified(c_file.time_write);
		CString strAttributes;
		strAttributes.Format("File:\t\t %s\nSize:\t\t %d Bytes\nDate Created:\t %s\nDate Modified:\t %s\nFlags:\t\t ",
					pNode->FullPath(), (unsigned long)c_file.size,
					tmCreated.Format("%#c"), tmModified.Format("%#c"));			
		if (c_file.attrib & _A_RDONLY) strAttributes += "Read-Only, ";
		if (c_file.attrib & _A_SYSTEM) strAttributes += "System, ";
		if (c_file.attrib & _A_HIDDEN) strAttributes += "Hidden, ";
		if (c_file.attrib & _A_ARCH) strAttributes += "Archive, ";
		int end = strAttributes.ReverseFind(',');
		if (end > 0)
			strAttributes = strAttributes.Left(end);
		strAttributes += "\n";
		AfxMessageBox(strAttributes, MB_OK | MB_ICONINFORMATION, 0);
	}
}

void CProjectBar::EditCurNodeText()
{
	if (m_pCurNode)
		// call the treectrl's edit node function
		m_wndTree.EditLabel(m_pCurNode->m_hTreeItem);
}

// determine whether editing/renaming of current node is allowed
BOOL CProjectBar::IsEditingAllowed(CXNode *pNode)
{
	switch (pNode->m_enNodeType)
	{
	case XNODE_PROJECT :
		return !((CXProject*)pNode)->m_bMisc;
	case XNODE_FOLDER :
		return TRUE;
	case XNODE_FILE :
		return !((CXFile*)pNode)->m_bNew;
	}	
	return FALSE;
}

void CProjectBar::UpdateRenameFile(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(IsEditingAllowed(m_pCurNode));
}

void CProjectBar::UpdateRenameFolder(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(IsEditingAllowed(m_pCurNode));
}

void CProjectBar::UpdateRenameProject(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(IsEditingAllowed(m_pCurNode));
}

void CProjectBar::RefreshCurrentProject()
{
	// show the wait cursor until everything is done
	CWaitCursor wc;

	m_wndTree.SetRedrawFlag(FALSE);
	UpdateNode(m_pCurProject, TRUE);
	m_wndTree.SetRedrawFlag(TRUE);
}

void CProjectBar::UpdateProjectRefresh(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pCurProject != NULL);
}

void CProjectBar::RefreshProjectManagerTree()
{
	m_wndTree.SetRedrawFlag(FALSE);
	
	int size = m_arrProjects.size();
	for (int i = 0; i < size; i++)
	{
		UpdateNode( m_arrProjects[i], TRUE );
	}

	m_wndTree.SetRedrawFlag(TRUE);
}

void CProjectBar::UpdateMoveFileToProject(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
    CMenu* pMenu = pCmdUI->m_pMenu;

    if (pMenu == NULL)
		return;

	int size = this->m_arrProjects.size();
	if (size <= 1)
	{
		pCmdUI->Enable(FALSE);
		//return;
	}

    ASSERT(pMenu == pCmdUI->m_pMenu);  // make sure preceding code didn't mess with it
    ASSERT(pMenu->m_hMenu);

    // look for a submenu to use instead
    CMenu *pSubMenu = pMenu->GetSubMenu(pCmdUI->m_nIndex);
	if (pSubMenu) 
	{
		ASSERT(pSubMenu->m_hMenu);
		pMenu = pSubMenu;
		ASSERT(pMenu->m_hMenu);
    }

	// remove the old items
    for (int iOldItems = 0; iOldItems < 8; iOldItems++) 
	{
        pMenu->RemoveMenu(pCmdUI->m_nID + iOldItems, MF_BYCOMMAND);
    }

	// create the new items
	for (int iProj = 0; iProj < size; iProj++)
	{
		CXProject* pProject = (CXProject*)m_arrProjects[iProj];

		// insert the name of the project
		pMenu->InsertMenu(pCmdUI->m_nIndex++,
			MF_STRING | MF_BYPOSITION, pCmdUI->m_nID++,
			pProject->m_strName);
		
		// disable menu item if its not the current project
		//DOESNT WORK! WHY?
		if (pProject == m_pCurProject)
		{
			pMenu->EnableMenuItem(pCmdUI->m_nID, FALSE);
		}
    }

	// update end menu count
	pCmdUI->m_nIndex--; // point to last menu added
	pCmdUI->m_nIndexMax = pMenu->GetMenuItemCount();
	//pCmdUI->m_bEnableChanged = TRUE;    // all the added items are enabled
}

void CProjectBar::MoveFileToProject(UINT nID)
{
	if (!m_pCurNode)
		return;

	// get the project to which to move
	int nIndex = nID - ID_FILE_MOVETO_PROJECT1;
	ASSERT(nIndex >= 0 && nIndex <= m_arrProjects.size());

	// get the selected project
	CXProject*	pProject = (CXProject*)m_arrProjects[nIndex];

	// move the current node to the project
	this->m_wndTree.MoveNode(m_pCurNode, pProject);
}

BOOL CProjectBar::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (!m_wndTree.OnCmdMsg(nID,nCode,pExtra,pHandlerInfo))
		return FALSE;

	return SIZEBAR_BASECLASS::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CProjectBar::PreTranslateMessage(MSG* pMsg) 
{
	return CControlBar::PreTranslateMessage(pMsg);
}
