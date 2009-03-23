// XMLTreeView.cpp : implementation file
//

#include "stdafx.h"
#include "Globals.h"
#include "Bonfire.h"
#include "BonfireDoc.h"
#include "BonfireView.h"
#include "XMLTreeView.h"
#include "SourceView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CMainFrame* g_pMainFrame;
extern CBonfireApp theApp;

#define LEFT_MARGIN		4
#define RIGHT_MARGIN	4
#define TOP_MARGIN		4
#define BOTTOM_MARGIN	4

#define XMLTYPE_TAG		1
#define XMLTYPE_ATTR	2
#define XMLTYPE_COMMENT	3
#define XMLTYPE_TEXT	4
#define XMLTYPE_CDATA	10

#define RELEASE_XML(pInterface)  \
if(pInterface) \
{\
	pInterface->Release();\
	pInterface = NULL;\
}\

/////////////////////////////////////////////////////////////////////////////
// CXMLTreeView

IMPLEMENT_DYNCREATE(CXMLTreeView, CTreeView)

CXMLTreeView::CXMLTreeView()
{
	m_pXMLDoc		= NULL;
	m_bLinkToXML	= TRUE;
	m_editType		= 0;
	m_bRedraw		= TRUE;
}

CXMLTreeView::~CXMLTreeView()
{
}


BEGIN_MESSAGE_MAP(CXMLTreeView, CTreeView)
	//{{AFX_MSG_MAP(CXMLTreeView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, OnBeginEdit)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndEdit)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRClick)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_ATTRIBUTE_EDIT_NAME, OnAttributeEditName)
	ON_COMMAND(ID_ATTRIBUTE_EDIT_VALUE, OnAttributeEditValue)
	ON_COMMAND(ID_COMMENT_EDIT, OnCommentEdit)
	ON_COMMAND(ID_ELEMENT_EDIT_NAME, OnElementEditName)
	ON_UPDATE_COMMAND_UI(ID_ELEMENT_EDIT_NAME, OnUpdateElementEditName)
	ON_COMMAND(ID_ELEMENT_INSERT_ATTRIBUTE, OnElementInsertAttribute)
	ON_COMMAND(ID_ELEMENT_INSERT_COMMENT, OnElementInsertComment)
	ON_COMMAND(ID_ELEMENT_INSERT_ELEMENT, OnElementInsertElement)
	ON_COMMAND(ID_ELEMENT_INSERT_TEXT, OnElementInsertText)
	ON_UPDATE_COMMAND_UI(ID_ATTRIBUTE_EDIT_NAME, OnUpdateAttributeEditName)
	ON_COMMAND(ID_TEXTNODE_EDIT, OnTextNodeEditText)
	ON_COMMAND(ID_ELEMENT_EXPAND_COLLAPSE, OnElementExpandCollapse)
	ON_COMMAND(ID_ELEMENT_EXPANDALL, OnElementExpandAll)
	ON_COMMAND(ID_ELEMENT_COLLAPSEALL, OnElementCollapseAll)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemExpanding)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemExpanded)
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)	
	ON_COMMAND(ID_FILE_PRINT_DIRECT, OnFilePrint)	
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

	// damn class wizard doesnt like it so we have to temporarily remove these lines of code
	//ON_COMMAND(ID_FILE_PRINT_DIRECT, CTreeView::OnFilePrint)	
	//ON_COMMAND(ID_FILE_PRINT_PREVIEW, CTreeView::OnFilePrintPreview)
	
/////////////////////////////////////////////////////////////////////////////
// CXMLTreeView create/destroy

int CXMLTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pTree = &GetTreeCtrl();

	DWORD ln = GetWindowLong(GetTreeCtrl().m_hWnd,GWL_STYLE);
	SetWindowLong(GetTreeCtrl().m_hWnd,GWL_STYLE, ln |
		//WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | 
#ifdef BON_EDIT_XMLTREE
		TVS_EDITLABELS |
#endif
		TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | 
		TVS_SHOWSELALWAYS | TVS_DISABLEDRAGDROP);
	
	CBitmap bmpIcons; bmpIcons.LoadBitmap(IDB_XMLTREE_ICONS);
	m_imlTreeImages.Create(16,16,ILC_COLOR8 | ILC_MASK,4,2);
	m_imlTreeImages.Add(&bmpIcons,RGB(255,0,255));
	GetTreeCtrl().SetImageList(&m_imlTreeImages,TVSIL_NORMAL);
	bmpIcons.DeleteObject();

	UpdateFont();
	
	// add this view to the document's map
	//((CBonfireDoc*)GetDocument())->AddViewToMap(this);

	return 0;
}

void CXMLTreeView::OnDestroy() 
{
	CTreeView::OnDestroy();
	m_imlTreeImages.DeleteImageList();
	m_fntTree.DeleteObject();
}


/////////////////////////////////////////////////////////////////////////////
// CXMLTreeView diagnostics

#ifdef _DEBUG
void CXMLTreeView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CXMLTreeView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CXMLTreeView message handlers

BOOL CXMLTreeView::RefreshView()
{
	CTreeCtrl* pTree = &this->GetTreeCtrl();
	pTree->DeleteAllItems();

	CBonfireDoc* pDoc	= (CBonfireDoc*)GetDocument();
	int nTextLength		= pDoc->m_xTextBuffer.GetAllTextLength();
	char* data			= NULL;
	BSTR wcdata			= NULL;
	m_bLinkToXML		= theApp.m_opOptions.xmltree.dwEditXMLTree;

	// show the wait cursor until everything is done
	CWaitCursor wc;

	try
	{
		data = (char*)malloc(nTextLength + 1);
		pDoc->m_xTextBuffer.GetAllText(data);

		wcdata = (BSTR)calloc(nTextLength + 1,sizeof(OLECHAR));
		CreateBSTR(wcdata,nTextLength + 1,data,nTextLength + 1);
		free(data);
	}
	catch(...)
	{
		g_pMainFrame->AddOutputError("Unexpected error occurred while loading text buffer.");
	}

	if (m_bLinkToXML && nTextLength > (1024*16))
	{
		if (AfxMessageBox("This file is larger than 16Kb. Enabling of editing the nodes in the XML Tree might take a few minutes to initialize. Click \"Yes\" to proceed or \"No\" to disable editing of nodes."
				, MB_YESNO) == IDNO)
		{
			m_bLinkToXML	= FALSE;
		}
	}

	// start timing
	DWORD tStart = GetTickCount();

	HRESULT				hr;
	VARIANT_BOOL		vbSuccess;
	HTREEITEM			tnRootNode	= TVI_LAST;
	IXMLDOMNode*		pXDN		= NULL;
	IXMLDOMElement*		pXE			= NULL;
	try
	{		
		// initialize msxml.dll
		hr = CoInitialize(NULL); 
		if (CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMDocument, (void**)&m_pXMLDoc) == REGDB_E_CLASSNOTREG)
		{
			AfxMessageBox("Microsoft XML Library not found! Please make sure you have msxml3.dll registered.", MB_OK | MB_ICONEXCLAMATION,0);
			return FALSE;
		}
		hr = m_pXMLDoc->QueryInterface(IID_IXMLDOMNode,(void**)&pXDN);

		// load the text into DOM
		hr = m_pXMLDoc->loadXML(wcdata, &vbSuccess);
		free(wcdata);
		
		// check if loading was ok
		if (!vbSuccess)
		{
			IXMLDOMParseError*	pXError = NULL;
			BSTR				reason	= NULL;
			try
			{
				// get the details about the error
				LONG code	= NULL;
				LONG line	= NULL;
				LONG pos	= NULL;

				m_pXMLDoc->get_parseError(&pXError);
				pXError->get_reason(&reason);
				pXError->get_errorCode(&code);
				pXError->get_line(&line);
				pXError->get_linepos(&pos);

				// create the message object
				OutputMessage* pMsg = new OutputMessage();
				pMsg->enType		= OUTPUT_XMLERROR;
				pMsg->enResult		= MSG_ERROR;
				pMsg->strMessage	= reason;
				pMsg->nLine			= line;
				pMsg->nCol			= pos;
				if (line && pos)
				{
					int ln			= pDoc->m_xTextBuffer.GetLineLength(line - 1);
					pDoc->m_xTextBuffer.GetText(line - 1, 0, line - 1, ln, pMsg->strLine);
				}
				g_pMainFrame->m_wndOutputBar.AddOutput(pMsg);
			}
			catch(...)
			{
				// create the message object
				OutputMessage* pMsg = new OutputMessage();
				pMsg->enType		= OUTPUT_XMLERROR;
				pMsg->enResult		= MSG_ERROR;
				pMsg->strMessage	= "Error loading text into XML DOM";
				pMsg->nLine			= 0;
				pMsg->nCol			= 0;
				g_pMainFrame->m_wndOutputBar.AddOutput(pMsg);
			}
			
			// free memory
			RELEASE_XML(pXError);
			if (reason != NULL) SysFreeString(reason); reason = NULL;

			throw "";
		}

		// show xml tree
		if (m_pXMLDoc->get_documentElement(&pXE) != S_OK)
		{
			// create the message object
			OutputMessage* pMsg = new OutputMessage();
			pMsg->enType		= OUTPUT_XMLERROR;
			pMsg->enResult		= MSG_ERROR;
			pMsg->strMessage	= "Error getting Document Element";
			pMsg->nLine			= 0;
			pMsg->nCol			= 0;
			g_pMainFrame->m_wndOutputBar.AddOutput(pMsg);
			throw "";
		}

		// process root node
		{
			BSTR nodeName		= NULL;
			pXE->get_nodeName(&nodeName);
			CString strNodeName = nodeName;
			SysFreeString(nodeName);
			tnRootNode = pTree->InsertItem("<" + strNodeName + ">", 0, 0);
			pTree->SetItemState(tnRootNode, TVIS_BOLD, TVIS_BOLD);
			pTree->SetItemData(tnRootNode, (DWORD)pXE); // NOTE: why was this commented?
		}

		// show attributes
		if (theApp.m_opOptions.xmltree.dwShowAttributes)
		{
			ProcessAttributes(tnRootNode, pXE);
		}
		
		// create tree items and expand the root node
		if (!RefreshXMLViewRec(pXE, tnRootNode))
		{
			throw "";
		}

		pTree->Expand(tnRootNode, TVE_EXPAND);

		// clear old messages
		g_pMainFrame->m_wndOutputBar.ClearList(OUTPUT_XMLERROR);

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
	catch (const char*) {}
	catch (...)
	{
		OutputMessage* pMsg = new OutputMessage();
		pMsg->enType		= OUTPUT_XMLERROR;
		pMsg->enResult		= MSG_ERROR;
		pMsg->strMessage	= "Error (Exception)";
		pMsg->nLine			= 0;
		pMsg->nCol			= 0;
		g_pMainFrame->m_wndOutputBar.AddOutput(pMsg);

		return FALSE;
	}

	// free memory
	RELEASE_XML(pXDN);
	//RELEASE_XML(pXE);
	//RELEASE_XML(m_pXMLDoc);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// recursive function that creates the tree from xml
/////////////////////////////////////////////////////////////////////////////
BOOL CXMLTreeView::RefreshXMLViewRec(IXMLDOMNode *pXMLNode, HTREEITEM tnParentTreeNode)
{
	CTreeCtrl* pTree = &this->GetTreeCtrl();

	IXMLDOMNode*	pXMLNext		= NULL;
	IXMLDOMNode*	pXMLTemp		= NULL;
	HTREEITEM		tnLastSibling	= NULL;

	try
	{
		pXMLNode->get_firstChild(&pXMLNext);
		while (pXMLNext != NULL)
		{
			// get the name of the node
			BSTR nodeName = NULL;
			pXMLNext->get_nodeName(&nodeName);
			CString strNodeName = nodeName;
			SysFreeString(nodeName);

			if (strNodeName.Compare("#comment") == 0)
			{
				BSTR comment = NULL;
				pXMLNext->get_text(&comment);
				CString strComment = comment;
				SysFreeString(comment);
				strComment.TrimLeft();
				strComment.TrimRight();
				strComment.Replace('\n',' '); // beware -- could eat up processor time maybe?
				strComment.Replace('\r',' ');
				strComment.Replace('\t',' ');
				//strNodeText.TrimLeft(" "); strNodeText.TrimRight(" ");
				if (theApp.m_opOptions.xmltree.dwShowComments)
				{
					tnLastSibling = pTree->InsertItem(strComment, XMLTYPE_COMMENT, XMLTYPE_COMMENT, tnParentTreeNode, TVI_LAST);

					// link the node to XML
					if (m_bLinkToXML)
						pTree->SetItemData(tnLastSibling, (unsigned long)pXMLNext);
				}
			}
			else if (strNodeName.Compare("#text") == 0)
			{
				BSTR text = NULL;
				pXMLNext->get_text(&text);
				CString strText = text;
				SysFreeString(text);
				strText.TrimLeft();
				strText.TrimRight();
				strText.Replace('\n',' '); // beware -- could eat up processor time maybe?
				strText.Replace('\r',' ');
				strText.Replace('\t',' ');
				if (theApp.m_opOptions.xmltree.dwShowText)
				{
					tnLastSibling = pTree->InsertItem(strText, XMLTYPE_TEXT, XMLTYPE_TEXT, tnParentTreeNode, TVI_LAST);

					// link the node to XML
					if (m_bLinkToXML)
						pTree->SetItemData(tnLastSibling, (unsigned long)pXMLNext);
				}
			}
			else
			{
				strNodeName = "<" + strNodeName + ">";
				tnLastSibling = pTree->InsertItem(strNodeName, XMLTYPE_TAG, XMLTYPE_TAG, tnParentTreeNode, TVI_LAST);
				pTree->SetItemState(tnLastSibling,TVIS_BOLD,TVIS_BOLD);

				// link the node to XML
				if (m_bLinkToXML)
					pTree->SetItemData(tnLastSibling, (unsigned long)pXMLNext);
				
				// show attributes
				if (theApp.m_opOptions.xmltree.dwShowAttributes)
				{
					ProcessAttributes(tnLastSibling, pXMLNext);
				}
			}

			// recursively iterate through the new folder
			if (!RefreshXMLViewRec(pXMLNext, tnLastSibling)) return false;

			// go to the next xml sibling node
			pXMLTemp = pXMLNext;
			pXMLTemp->get_nextSibling(&pXMLNext);

			// free memory
			if (!m_bLinkToXML)
				pXMLTemp->Release();

			pXMLTemp = NULL;
		}
		return true;
	}
	catch (...)
	{
		OutputMessage* pMsg = new OutputMessage();
		pMsg->enType		= OUTPUT_XMLERROR;
		pMsg->enResult		= MSG_ERROR;
		pMsg->strMessage	= "Error (Exception)";
		pMsg->nLine			= 0;
		pMsg->nCol			= 0;
		g_pMainFrame->m_wndOutputBar.AddOutput(pMsg);
		return false;
	}
}

void CXMLTreeView::ProcessAttributes(HTREEITEM tnParent, IXMLDOMNode *pXMLNode)
{
	// get attributes
	IXMLDOMNamedNodeMap*	pXMLMap		= NULL;
	IXMLDOMNode*			pXMLAttrib	= NULL;
	CTreeCtrl*				pTree		= &this->GetTreeCtrl();
	pXMLNode->get_attributes(&pXMLMap);
	if (pXMLMap)
	{
		long iAttrLength = 0;
		pXMLMap->get_length(&iAttrLength);
		for (int attr=0; attr<iAttrLength; attr++)
		{
			CString strAttr;
			pXMLMap->get_item(attr, &pXMLAttrib);

			// get the attribute name and value
			BSTR attrName;
			BSTR attrVal;

			pXMLAttrib->get_nodeName(&attrName);
			pXMLAttrib->get_text(&attrVal);
			strAttr.Format("%s=\"%s\"", (CString)attrName, (CString)attrVal);
			HTREEITEM attrSibling = pTree->InsertItem(strAttr, 2, 2, tnParent, TVI_LAST);

			// link the node to XML
			if (m_bLinkToXML)
				pTree->SetItemData(attrSibling, (unsigned long)pXMLAttrib);

			// free memory
			SysFreeString(attrName);
			SysFreeString(attrVal);
		}
	}

	// free memory
	RELEASE_XML(pXMLMap);
	//RELEASE_XML(pXMLAttrib);
}

CString CXMLTreeView::GetViewName()
{
	return "XML Tree View";
}

void CXMLTreeView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	UpdateFont();

	if (lHint) RefreshView();

	RedrawWindow();
	AfxGetMainWnd()->DrawMenuBar(); // if child frame is maximized
}

void CXMLTreeView::UpdateFont()
{
	if (m_fntTree.GetSafeHandle() != NULL)
		m_fntTree.DeleteObject();

	CClientDC dcThis(this);
	LOGFONT lf;

	lf.lfWeight = FW_NORMAL;
	memset(&lf,0,sizeof(LOGFONT));
	lf.lfHeight = -MulDiv(theApp.m_opOptions.xmltree.dwFontSize, dcThis.GetDeviceCaps(LOGPIXELSY),72);
	strcpy(lf.lfFaceName, theApp.m_opOptions.xmltree.strFont);
	m_fntTree.CreateFontIndirect(&lf);
	GetTreeCtrl().SetFont(&m_fntTree);
}

// begin editing of a node
void CXMLTreeView::OnBeginEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

	*pResult = EditCurrentNode();
}

// edit the current XML node
BOOL CXMLTreeView::EditCurrentNode()
{
#ifdef BON_EDIT_XMLTREE
	//set the text to be only the name, not <xxx>
	//m_wndTree.EditLabel(m_pCurNode->m_hTreeItem);
	CTreeCtrl* pTree		= &GetTreeCtrl();
	HTREEITEM pTreeNode		= pTree->GetSelectedItem();
	IXMLDOMNode* pXMLNode	= NULL;
	BOOL retVal				= TRUE;
	
	try
	{
		pXMLNode = (IXMLDOMNode*)pTree->GetItemData(pTreeNode);
		if (pXMLNode)
		{
			CString strNodeText	= GetNodeText(pXMLNode);
				
			//pTree->SetItemText(m_pCurNode->m_hTreeItem, m_pCurNode->m_strName);
			CEdit* pEdit = pTree->GetEditControl();
			if (pEdit)
			{
				pEdit->SetWindowText(strNodeText);
			}
		}
		else
		{
			retVal = FALSE;
		}
	}
	catch (...)
	{
		g_pMainFrame->AddOutputError("Error retrieving XML Node");
		retVal = FALSE;
	}

	return !retVal;
#else
	return FALSE;
#endif
}

// end editing of a node
void CXMLTreeView::OnEndEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
#ifdef BON_EDIT_XMLTREE
	//TV_DISPINFO* pTVDispInfo	= (TV_DISPINFO*)pNMHDR;
	CTreeCtrl* pTree		= &GetTreeCtrl();
	HTREEITEM pTreeNode		= pTree->GetSelectedItem();
	IXMLDOMNode* pXMLNode	= NULL;
	LPTSTR temp				= ((LPNMTVDISPINFO)pNMHDR)->item.pszText;
	if (!temp)
	{
		m_editType			= 0;
		return;
	}

	try
	{
		// get the new label
		CString newtext		= temp;
		pXMLNode			= (IXMLDOMNode*)pTree->GetItemData(pTreeNode);
		if (pXMLNode)
		{
			CString strNodeText = GetNodeText(pXMLNode);

			// create the message object
			CString strOutput;
			strOutput.Format("Changing %s \"%s\" to \"%s\"", XMLTypeToString(pXMLNode), strNodeText, newtext);
			OutputMessage* pMsg = new OutputMessage();
			pMsg->enType		= OUTPUT_APPSTATUS;
			pMsg->enResult		= MSG_PROCESSING;
			pMsg->strMessage	= strOutput;
			g_pMainFrame->m_wndOutputBar.AddOutput(pMsg);
		
			// update the XML node with the new text
			if (UpdateNodeText(pXMLNode, pTreeNode, newtext, pMsg))
			{
/*
				BSTR xml	= NULL;
				m_pXMLDoc->get_xml(&xml);
				CString strXML		= xml;
				SysFreeString(xml);	
				AfxMessageBox(strXML, MB_OK);
*/    
				CBonfireDoc* pDoc	= (CBonfireDoc*)GetDocument();
				CString	strAbsPath	= theApp.m_pszHelpFilePath;
				DWORD	nSize		= 128;
				char*	lpValue		= (char*)malloc( nSize );
				int		retVal		= GetEnvironmentVariable("TEMP", lpValue, nSize);
				if (retVal <= nSize)
					strAbsPath		= lpValue;
				else if (retVal == 0)
				{
					// try TMP
					retVal			= GetEnvironmentVariable("TMP", lpValue, nSize);
					if (retVal <= nSize)
						strAbsPath	= lpValue;
				}
				// generate a temporary file
				CString strPath		= GetFilePath( AddSlash(strAbsPath) ) + "btmp_" + GetFileTitle( pDoc->GetPathName() );
				
				// save to temporary file
				_variant_t varPath	= strPath;
				if (m_pXMLDoc->save(varPath) != S_OK )
					throw "Error saving to temp file";

				// mark the file as temporary for later deletion
				theApp.AddTempFile( strPath );

				// load back from the temporary file
				// TO DO FOR ROMA
				pDoc->m_xTextBuffer.FreeAll(); // Roma fix
				if ( !pDoc->m_xTextBuffer.LoadFromFile(strPath))
					throw "Error loading from temp file";
				pDoc->m_xTextBuffer.SetModified();
			}

			*pResult	= FALSE;
		}
		else
		{
			*pResult	= TRUE;
		}
	}
	catch (char* e)
	{
		CString strMessage;
		strMessage.Format("Error updating tree node: %s", e);
		g_pMainFrame->AddOutputError(strMessage);
		*pResult = TRUE;
	}
	catch (...)
	{
		g_pMainFrame->AddOutputError("Error updating tree node");
		*pResult = TRUE;
	}

	m_editType	= 0;
#endif
}

// get the name of the XML node
CString CXMLTreeView::GetNodeText(IXMLDOMNode *pXMLNode)
{
	CString strNodeText	= "";
	BSTR bstrNodeText	= NULL;
	try
	{
		if (pXMLNode)
		{
			DOMNodeType pType;
			if (pXMLNode->get_nodeType(&pType) != S_OK)
				throw "Error : Get Node Type Exception";

			// use different functions depending on the node type
			switch (pType)
			{
			case NODE_DOCUMENT :
			case NODE_ELEMENT :
				pXMLNode->get_nodeName(&bstrNodeText);
				break;
			case NODE_ATTRIBUTE :
			{
				_variant_t val;
				if (m_editType == 1)
				{
					pXMLNode->get_nodeName(&bstrNodeText);
				}
				else
				{
					pXMLNode->get_nodeValue(&val);
					bstrNodeText = _bstr_t(val);
				}
			}
				break;
			case NODE_COMMENT :
			case NODE_TEXT :
				pXMLNode->get_text(&bstrNodeText);
				break;
			case NODE_CDATA_SECTION :
			default:
			case NODE_DOCUMENT_TYPE :
			case NODE_DOCUMENT_FRAGMENT :
			case NODE_NOTATION :
			case NODE_ENTITY_REFERENCE :
			case NODE_ENTITY :
			case NODE_PROCESSING_INSTRUCTION :
				return "";
			}
			
			strNodeText		= bstrNodeText;
			SysFreeString(bstrNodeText);
		}
	}
	catch (char* e)
	{
		g_pMainFrame->AddOutputError(e);
	}
	catch (...)
	{
		g_pMainFrame->AddOutputError("Error retrieving XML Node Text");
	}
	return strNodeText;
}

// get the name of the XML node
BOOL CXMLTreeView::UpdateNodeText(IXMLDOMNode* pXMLNode, HTREEITEM pTreeNode, CString strNewText, OutputMessage* pMsg)
{
	IXMLDOMNode* pNewNode	= NULL;
	IXMLDOMNode* pParent	= NULL;
	BSTR bstrOldText		= NULL;
	BSTR bstrOldName		= NULL;
	BOOL retVal				= FALSE;
	HRESULT hr				= FALSE;
	CTreeCtrl* pTree		= &GetTreeCtrl();
	
	try
	{
		DOMNodeType pType;
		
		pXMLNode->get_nodeName(&bstrOldName);
		//pXMLNode->get_text(&bstrOldText);
		pXMLNode->get_nodeType(&pType);
		VARIANT vType;
		vType.vt	= VT_I2;
		vType.iVal	= pType;

		switch (pType)
		{
		case NODE_DOCUMENT :
			break;
		case NODE_ELEMENT :
		{
			//hr = m_pXMLDoc->createNode(vType, bstrOldName, NULL, &pNewNode);

			IXMLDOMElement* pNewEl;
			_variant_t vSibling = pXMLNode;
			pXMLNode->get_parentNode(&pParent);
			//hr = pParent->insertBefore(pNewNode, vSibling, &pNewNode);
			IXMLDOMDocument* pXMLDoc = NULL;
			hr = pXMLNode->get_ownerDocument(&pXMLDoc);
			BSTR strTemp = strNewText.AllocSysString();
			pXMLDoc->createElement(strTemp,&pNewEl);

			// copy children
			IXMLDOMNodeList* pXMLChildren;
			pXMLNode->get_childNodes(&pXMLChildren);
			if (pXMLChildren)
			{
				long n = 0;
				pXMLChildren->get_length(&n);
				for (long i = 0; i < n; i++)
				{
					IXMLDOMNode* pChild, *pClone;
					pXMLChildren->get_item(i,&pChild);
					pChild->cloneNode(TRUE,&pClone);
					pNewEl->appendChild(pClone,NULL);
				}
			}

			// copy attributes

			IXMLDOMNamedNodeMap* pXMLAttributes, *pXMLDestAttributes;
			pXMLNode->get_attributes(&pXMLAttributes);
			pNewEl->get_attributes(&pXMLDestAttributes);
			if (pXMLAttributes)
			{
				long n = 0;
				pXMLAttributes->get_length(&n);
				for (long i = 0; i < n; i++)
				{
					IXMLDOMNode* pAttr, *pClone;
					pXMLAttributes->get_item(i,&pAttr);
					pAttr->cloneNode(TRUE,&pClone);
					pXMLDestAttributes->setNamedItem(pClone,NULL);
				}
			}

			SysFreeString(strTemp);
			hr = pParent->replaceChild(pNewEl,pXMLNode,NULL);
			if (hr != S_OK)
				throw "oh no?";

			// update the tree node text
			CString strNodeText;
			strNodeText.Format("<%s>", (CString)strNewText);
			pTree->SetItemText(pTreeNode, strNodeText);
			pTree->SetItemData(pTreeNode, (DWORD)pNewEl);
			
			break;
		}
		case NODE_ATTRIBUTE :
		{
			// get the attribute name and value
			BSTR attrName;
			BSTR attrVal;

			if (m_editType == 0)
			{
				// edit value
				_variant_t newVal = strNewText;
				hr = pXMLNode->put_nodeValue(newVal);
				if (hr != S_OK)
					throw "Update Failed.";
				pXMLNode->get_nodeName(&attrName);
				attrVal = strNewText.AllocSysString();
			}
			else
			{
				// edit name
				hr = pXMLNode->get_parentNode(&pParent);
				IXMLDOMAttribute* attr	= NULL;
				IXMLDOMNode* attrNode	= NULL;
				_variant_t oldNode		= (void*)0;//pXMLNode;
				attrName				= strNewText.AllocSysString();
				hr = m_pXMLDoc->createAttribute(attrName, &attr);
				if (hr != S_OK)
					throw "Error creating duplicate";

				pParent->insertBefore(attr, oldNode, &attrNode);
				break;
			}
			
			// update the tree node text
			CString strNodeText;
			strNodeText.Format("%s=\"%s\"", (CString)attrName, (CString)attrVal);
			pTree->SetItemText(pTreeNode, strNodeText);
			
			// free memory
			SysFreeString(attrName);
			SysFreeString(attrVal);
		}
			break;
		case NODE_COMMENT :
		case NODE_TEXT :
		{
			_variant_t newVal = strNewText;
			hr = pXMLNode->put_nodeValue(newVal);
			if (hr != S_OK)
				throw "Update Failed.";

			strNewText.Replace("\r\n"," ");
			pTree->SetItemText(pTreeNode, strNewText);
		}
			break;
		case NODE_CDATA_SECTION :
			break;
		}

		// current output is success
		pMsg->enResult	= MSG_SUCCESS;
		pMsg->strResult	= ("Success");
		g_pMainFrame->m_wndOutputBar.UpdateLast();

		retVal			= TRUE;
	}
	catch (char* e)
	{
		pMsg->enResult	= MSG_ERROR;
		pMsg->strResult.Format("Error: %s", e);
		g_pMainFrame->m_wndOutputBar.UpdateLast();
	}
	catch (...)
	{
		pMsg->enResult	= MSG_ERROR;
		pMsg->strResult = "Error";
		g_pMainFrame->m_wndOutputBar.UpdateLast();
	}

	// free memory
	SysFreeString(bstrOldText);
	SysFreeString(bstrOldName);

	return retVal;
}

// get the friendly name for the XML type
CString CXMLTreeView::XMLTypeToString(IXMLDOMNode *pXMLNode)
{
	CString strReturn	= "";
	DOMNodeType pType;
	pXMLNode->get_nodeType(&pType);

	switch (pType)
	{
	case NODE_DOCUMENT :
		strReturn		= "XML Document Element";
		break;
	case NODE_ELEMENT :
		strReturn		= "XML Tag";
		break;
	case NODE_ATTRIBUTE :
		strReturn		= (m_editType == 0) ? "Attribute (Value)" : "Attribute (Name)";
		break;
	case NODE_COMMENT :
		strReturn		= "Comment";
		break;
	case NODE_TEXT :
		strReturn		= "Text";
		break;
	case NODE_CDATA_SECTION :
		strReturn		= "CData Section";
		break;
	default:
		return "";
	}
	return strReturn;
}


void CXMLTreeView::OnRClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 1;

	// get the current node in the tree
	POINT pt;
	GetCursorPos(&pt);
	this->ScreenToClient(&pt);
		
	// the reason we do hit test here is because
	// when we right-click on an item we want to manually select it
	// since the GetSelectedItem() might actually give us an old node
	HTREEITEM hSelItem = this->GetTreeCtrl().HitTest(pt);
	if (hSelItem)
	{
		this->GetTreeCtrl().SelectItem(hSelItem);
		this->ClientToScreen(&pt);
		OnContextMenu(this,pt);
	}
}

void CXMLTreeView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	HTREEITEM hSelItem = this->GetTreeCtrl().GetSelectedItem();
	if (!hSelItem)
		return;

	IXMLDOMNode* pSelNode = (IXMLDOMNode*)this->GetTreeCtrl().GetItemData(hSelItem);
	if (pSelNode)
	{
		BCMenu popmenu;

		// get the appropriate right-click menu
		DOMNodeType ntype;
		pSelNode->get_nodeType(&ntype);
		switch (ntype)
		{
#ifdef BON_EDIT_XMLTREE
		case NODE_ATTRIBUTE :		
			popmenu.LoadMenu(IDR_POPUP_XMLATTRIBUTE);
			break;
		case NODE_TEXT :
			popmenu.LoadMenu(IDR_POPUP_XMLTEXT);
			break;
		case NODE_COMMENT :
			popmenu.LoadMenu(IDR_POPUP_XMLCOMMENT);
			break;
#endif
		case NODE_ELEMENT :
		{
			popmenu.LoadMenu(IDR_POPUP_XMLELEMENT);

#ifndef BON_EDIT_XMLTREE
			CMenu* pM = popmenu.GetSubMenu(0);
			for (int i = 14; i >= 5; i--)
				pM->DeleteMenu(i,MF_BYPOSITION);

			pM->DeleteMenu(1,MF_BYPOSITION);
			pM->DeleteMenu(0,MF_BYPOSITION);
#endif

			break;
		}
		default:
			return;
		}

		// set the icons for the menu and show the popup
		g_pMainFrame->SetMenuIcons(&popmenu);
		ShowPopup(AfxGetMainWnd(),(BCMenu*)popmenu.GetSubMenu(0));

		// clean up
		popmenu.DestroyMenu();
	}
}

///////////////////////////////////////////////////////////
// Attribute node menu 
///////////////////////////////////////////////////////////
void CXMLTreeView::OnUpdateAttributeEditName(CCmdUI* pCmdUI) 
{
#ifdef BON_EDIT_XMLTREE
	pCmdUI->Enable(TRUE);
#else
	pCmdUI->Enable(FALSE);
#endif
}

void CXMLTreeView::OnAttributeEditName() 
{
	CTreeCtrl* pTree	= &GetTreeCtrl();
	HTREEITEM pTreeNode	= pTree->GetSelectedItem();
	m_editType			= 1;
	pTree->EditLabel(pTreeNode);
}

void CXMLTreeView::OnAttributeEditValue() 
{
	CTreeCtrl* pTree	= &GetTreeCtrl();
	HTREEITEM pTreeNode	= pTree->GetSelectedItem();
	m_editType			= 0;
	pTree->EditLabel(pTreeNode);
}

///////////////////////////////////////////////////////////
// Text node menu 
///////////////////////////////////////////////////////////
void CXMLTreeView::OnTextNodeEditText() 
{
#ifdef BON_EDIT_XMLTREE
	CTreeCtrl* pTree	= &GetTreeCtrl();
	HTREEITEM pTreeNode	= pTree->GetSelectedItem();
	m_editType			= 0;
	pTree->EditLabel(pTreeNode);
#endif
}

///////////////////////////////////////////////////////////
// Comment node menu 
///////////////////////////////////////////////////////////
void CXMLTreeView::OnCommentEdit() 
{
#ifdef BON_EDIT_XMLTREE
	CTreeCtrl* pTree	= &GetTreeCtrl();
	HTREEITEM pTreeNode	= pTree->GetSelectedItem();
	m_editType			= 0;
	pTree->EditLabel(pTreeNode);
#endif
}

///////////////////////////////////////////////////////////
// Element node menu 
///////////////////////////////////////////////////////////
void CXMLTreeView::OnElementEditName() 
{
#ifdef BON_EDIT_XMLTREE
	CTreeCtrl* pTree	= &GetTreeCtrl();
	HTREEITEM pTreeNode	= pTree->GetSelectedItem();
	m_editType			= 0;
	pTree->EditLabel(pTreeNode);
#endif
}

void CXMLTreeView::OnUpdateElementEditName(CCmdUI* pCmdUI) 
{
	// WHY DO WE NEED THIS FUNCTION?
	// - IN CASE THE MENU IS SHOWN BY ACCIDENT, OR IF THERE IS A SHORTCUT TO THIS ACTION
#ifdef BON_EDIT_XMLTREE
	pCmdUI->Enable(TRUE);
#else
	pCmdUI->Enable(FALSE);
#endif
}

///////////////////////////////////////////////////////////
// Insert nodes menu 
///////////////////////////////////////////////////////////
void CXMLTreeView::OnElementInsertAttribute() 
{
	// TODO: Add your command handler code here
	
}

void CXMLTreeView::OnElementInsertComment() 
{
	// TODO: Add your command handler code here
	
}

void CXMLTreeView::OnElementInsertElement() 
{
	// TODO: Add your command handler code here
	
}

void CXMLTreeView::OnElementInsertText() 
{
#ifdef BON_EDIT_XMLTREE
	CTreeCtrl* pTree		= &GetTreeCtrl();
	HTREEITEM pTreeNode		= pTree->GetSelectedItem();
	IXMLDOMNode* pXMLNode	= NULL;
	BOOL retVal				= TRUE;
	HRESULT hr				= FALSE;
	
	try
	{
		pXMLNode = (IXMLDOMNode*)pTree->GetItemData(pTreeNode);
		if (pXMLNode)
		{
			// TO DO:
			// Save the children first, because the manipulating of text 
			// modifies children text nodes as well
			
			CString strText				= "HELLO";
			BSTR strTemp				= strText.AllocSysString();
			IXMLDOMDocument* pXMLDoc	= NULL;
			IXMLDOMText* pNewXMLNode	= NULL;
			IXMLDOMNode* pTempXMLNode	= NULL;
			pXMLNode->get_ownerDocument(&pXMLDoc);
			pXMLDoc->createTextNode(strTemp, &pNewXMLNode);
			pXMLNode->appendChild(pNewXMLNode, &pTempXMLNode);

			BSTR xml					= NULL;
			m_pXMLDoc->get_xml(&xml);
			CString strXML				= xml;
			SysFreeString(xml);	
			AfxMessageBox(strXML, MB_OK);
		}
		else
		{
			retVal = FALSE;
		}
	}
	catch (...)
	{
		g_pMainFrame->AddOutputError("Error Inserting Text");
		retVal = FALSE;
	}

#endif
}

/////////////////////////////////////////////////////////
// EXPAND/COLLAPSE code
/////////////////////////////////////////////////////////
DWORD iExpand = 0;
void CXMLTreeView::SetRedrawFlag(BOOL bRedraw)
{
	if (bRedraw)
		iExpand--;
	else
		iExpand++;

	if (!bRedraw || iExpand == 0)
	//if (bRedraw || m_bRedraw)
	{
		m_bRedraw = bRedraw;
		GetTreeCtrl().SetRedraw(bRedraw);
	}
}

void CXMLTreeView::OnElementExpandCollapse() 
{
	SetRedrawFlag(FALSE);
	HTREEITEM hSel = GetTreeCtrl().GetSelectedItem();
	if (hSel)
		GetTreeCtrl().Expand(hSel,TVE_TOGGLE);
	SetRedrawFlag(TRUE);

	GetTreeCtrl().EnsureVisible(GetTreeCtrl().GetSelectedItem());
}

void CXMLTreeView::ExpandChildren(HTREEITEM parent, BOOL bExpand)
{
	//SetRedrawFlag(FALSE);
	HTREEITEM hCur = GetTreeCtrl().GetChildItem(parent);
	while (hCur)
	{
		GetTreeCtrl().Expand(hCur,bExpand ? TVE_EXPAND : TVE_COLLAPSE); // expand/collapse this child
		ExpandChildren(hCur,bExpand); // expand/collapse all children of this child
		hCur = GetTreeCtrl().GetNextSiblingItem(hCur);
	}
	//SetRedrawFlag(TRUE);
}

void CXMLTreeView::OnElementExpandAll() 
{
	// start timing
	DWORD tStart = GetTickCount();

	SetRedrawFlag(FALSE);
	HTREEITEM hSel = GetTreeCtrl().GetSelectedItem();
	if (hSel)
	{
		GetTreeCtrl().Expand(hSel,TVE_EXPAND);
		ExpandChildren(hSel,TRUE);
	}
	SetRedrawFlag(TRUE);

	GetTreeCtrl().EnsureVisible(GetTreeCtrl().GetSelectedItem());

	CString diff;
	diff.Format("%d", GetTickCount() - tStart);

	OutputMessage* pMsg = new OutputMessage();
	pMsg->enType		= OUTPUT_APPSTATUS;
	pMsg->enResult		= MSG_NONE;
	pMsg->strMessage	= "Expand All operation took " + diff + " ms.";
	pMsg->nLine			= 0;
	pMsg->nCol			= 0;
	//g_pMainFrame->m_wndOutputBar.AddOutput(pMsg);
}

void CXMLTreeView::OnElementCollapseAll() 
{
	// start timing
	DWORD tStart = GetTickCount();
	
	SetRedrawFlag(FALSE);
	HTREEITEM hSel = GetTreeCtrl().GetSelectedItem();
	if (hSel)
	{
		GetTreeCtrl().Expand(hSel,TVE_COLLAPSE);
		ExpandChildren(hSel,FALSE);
	}
	SetRedrawFlag(TRUE);
	
	GetTreeCtrl().EnsureVisible(GetTreeCtrl().GetSelectedItem());

	CString diff;
	diff.Format("%d", GetTickCount() - tStart);

	OutputMessage* pMsg = new OutputMessage();
	pMsg->enType		= OUTPUT_APPSTATUS;
	pMsg->enResult		= MSG_NONE;
	pMsg->strMessage	= "Collapse All operation took " + diff + " ms.";
	pMsg->nLine			= 0;
	pMsg->nCol			= 0;
	//g_pMainFrame->m_wndOutputBar.AddOutput(pMsg);
}

void CXMLTreeView::OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	SetRedrawFlag(FALSE);

	*pResult = 0;
}

void CXMLTreeView::OnItemExpanded(NMHDR* pNMHDR, LRESULT* pResult) 
{
	SetRedrawFlag(TRUE);
	
	*pResult = 0;
}

/////////////////////////////////////////////////////////
// Printing Code
//	Code Project Article "A Print Enabled Tree View"
//	by Koay Kah Hoe 
/////////////////////////////////////////////////////////
BOOL CXMLTreeView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	//return CTreeView::OnPreparePrinting(pInfo);
	return DoPreparePrinting(pInfo);
}

void CXMLTreeView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	//CTreeView::OnBeginPrinting(pDC, pInfo);
	CTreeCtrl* pTree	= &GetTreeCtrl();
	HTREEITEM hItem		= pTree->GetRootItem();
	pTree->GetItemRect(hItem,rcBounds,TRUE);
	m_nRowHeight = rcBounds.Height();

	// Find the total number of visible items & the right most coordinate
	int ItemCount=0;
	do
	{
		ItemCount++;
		CRect rc;
		pTree->GetItemRect(hItem,rc,TRUE);
		if (rc.right>rcBounds.right)
			rcBounds.right=rc.right;
		hItem = pTree->GetNextItem(hItem,TVGN_NEXTVISIBLE);
	}
	while (hItem);

	// Find the entire print boundary
	int ScrollMin,ScrollMax;
	GetScrollRange(SB_HORZ,&ScrollMin,&ScrollMax);
	rcBounds.left=0;
	if (ScrollMax>rcBounds.right)
		rcBounds.right=ScrollMax+1;
	rcBounds.top=0;
	rcBounds.bottom=m_nRowHeight*ItemCount;

	// Get text width
	CDC *pCtlDC = pTree->GetDC();
	if (NULL == pCtlDC) return;
	TEXTMETRIC tm;
	pCtlDC->GetTextMetrics(&tm);
	m_nCharWidth = tm.tmAveCharWidth;
	double d = (double)pDC->GetDeviceCaps(LOGPIXELSY)/(double)pCtlDC->GetDeviceCaps(LOGPIXELSY);
	ReleaseDC(pCtlDC);

	// Find rows per page
	int nPageHeight = pDC->GetDeviceCaps(VERTRES);
	m_nRowsPerPage = (int)((double)nPageHeight/d)/m_nRowHeight-TOP_MARGIN-BOTTOM_MARGIN;

	// Set maximum pages
	int pages=(ItemCount-1)/m_nRowsPerPage+1;
	pInfo->SetMaxPage(pages);

	// Create a memory DC compatible with the paint DC
	CPaintDC dc(this);
	CDC MemDC;
	MemDC.CreateCompatibleDC(&dc);

	// Select a compatible bitmap into the memory DC
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(&dc, rcBounds.Width(), rcBounds.Height() );
	MemDC.SelectObject(&bitmap);

	// Enlarge window size to include the whole print area boundary
	GetWindowPlacement(&WndPlace);
	MoveWindow(0,0,::GetSystemMetrics(SM_CXEDGE)*2+rcBounds.Width(),
		::GetSystemMetrics(SM_CYEDGE)*2+rcBounds.Height(),FALSE);
	ShowScrollBar(SB_BOTH,FALSE);

	// Call the default printing
	pTree->EnsureVisible(pTree->GetRootItem());
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)MemDC.m_hDC, 0 );

	// Now create a mask
	CDC MaskDC;
	MaskDC.CreateCompatibleDC(&dc);
	CBitmap maskBitmap;

	// Create monochrome bitmap for the mask
	maskBitmap.CreateBitmap( rcBounds.Width(), rcBounds.Height(), 1, 1, NULL );
	MaskDC.SelectObject( &maskBitmap );
	MemDC.SetBkColor( ::GetSysColor( COLOR_WINDOW ) );

	// Create the mask from the memory DC
	MaskDC.BitBlt( 0, 0, rcBounds.Width(), rcBounds.Height(), &MemDC,
		rcBounds.left, rcBounds.top, SRCCOPY );

	// Copy image to clipboard
	CBitmap clipbitmap;
	clipbitmap.CreateCompatibleBitmap(&dc, rcBounds.Width(), rcBounds.Height() );
	CDC clipDC;
	clipDC.CreateCompatibleDC(&dc);
	CBitmap* pOldBitmap = clipDC.SelectObject(&clipbitmap);
	clipDC.BitBlt( 0, 0, rcBounds.Width(), rcBounds.Height(), &MemDC,
		rcBounds.left, rcBounds.top, SRCCOPY);
	OpenClipboard();
	EmptyClipboard();
	SetClipboardData(CF_BITMAP, clipbitmap.GetSafeHandle());
	CloseClipboard();
	clipDC.SelectObject(pOldBitmap);
	clipbitmap.Detach();

	// Copy the image in MemDC transparently
	MemDC.SetBkColor(RGB(0,0,0));
	MemDC.SetTextColor(RGB(255,255,255));
	MemDC.BitBlt(rcBounds.left, rcBounds.top, rcBounds.Width(), rcBounds.Height(),
		&MaskDC, rcBounds.left, rcBounds.top, MERGEPAINT);

	CPalette pal;
	hDIB=DDBToDIB(bitmap, BI_RGB, &pal );
}

void CXMLTreeView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
	CTreeView::OnPrepareDC(pDC, pInfo);

	// Map logical unit of screen to printer unit
	pDC->SetMapMode(MM_ANISOTROPIC);
	CClientDC dcScreen(NULL);
	pDC->SetWindowExt(dcScreen.GetDeviceCaps(LOGPIXELSX),dcScreen.GetDeviceCaps(LOGPIXELSX));
	pDC->SetViewportExt(pDC->GetDeviceCaps(LOGPIXELSX),pDC->GetDeviceCaps(LOGPIXELSX));

}

void CXMLTreeView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	//CTreeView::OnPrint(pDC, pInfo);

	// Save dc state
	int nSavedDC = pDC->SaveDC();

	// Set font
	CFont Font;
	LOGFONT lf;
	CFont *pOldFont = GetFont();
	pOldFont->GetLogFont(&lf);
	lf.lfHeight=m_nRowHeight-1;
	lf.lfWidth=0;
	Font.CreateFontIndirect(&lf);
	pDC->SelectObject(&Font);

	PrintHeadFoot(pDC,pInfo);
	pDC->SetWindowOrg(-1*(LEFT_MARGIN*m_nCharWidth),-m_nRowHeight*TOP_MARGIN);

	int height;
	if (pInfo->m_nCurPage==pInfo->GetMaxPage())
		height=rcBounds.Height()-((pInfo->m_nCurPage-1)*m_nRowsPerPage*m_nRowHeight);
	else
		height=m_nRowsPerPage*m_nRowHeight;
	int top=(pInfo->m_nCurPage-1)*m_nRowsPerPage*m_nRowHeight;

	pDC->SetBkColor(RGB(255,255,255));
	pDC->SetTextColor(RGB(0,0,0));

	LPBITMAPINFOHEADER lpbi;
	lpbi = (LPBITMAPINFOHEADER)hDIB;
	int nColors = lpbi->biClrUsed ? lpbi->biClrUsed : 1 << lpbi->biBitCount;
	BITMAPINFO &bmInfo = *(LPBITMAPINFO)hDIB;
	LPVOID lpDIBBits;
	if( bmInfo.bmiHeader.biBitCount > 8 )
		lpDIBBits = (LPVOID)((LPDWORD)(bmInfo.bmiColors + 
			bmInfo.bmiHeader.biClrUsed) + 
			((bmInfo.bmiHeader.biCompression == BI_BITFIELDS) ? 3 : 0));
	else
		lpDIBBits = (LPVOID)(bmInfo.bmiColors + nColors);
	HDC hDC=pDC->GetSafeHdc();
	StretchDIBits(hDC,				// hDC
		0,							// DestX
		0,							// DestY
		rcBounds.Width(),			// nDestWidth
		height,						// nDestHeight
		rcBounds.left,				// SrcX
		rcBounds.Height()-top-height,	// SrcY
		rcBounds.Width(),			// wSrcWidth
		height,						// wSrcHeight
		lpDIBBits,					// lpBits
		&bmInfo,					// lpBitsInfo
		DIB_RGB_COLORS,				// wUsage
		SRCCOPY);					// dwROP

	pDC->SelectObject(pOldFont);
	pDC->RestoreDC( nSavedDC );

}

void CXMLTreeView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	//CTreeView::OnEndPrinting(pDC, pInfo);
	GlobalFree(hDIB);
	SetWindowPlacement(&WndPlace);
	RedrawWindow();

}

void CXMLTreeView::PrintHeadFoot(CDC *pDC, CPrintInfo *pInfo)
{
	CClientDC dcScreen(NULL);
	CRect rc;
	rc.top=m_nRowHeight*(TOP_MARGIN-2);
	rc.bottom = (int)((double)(pDC->GetDeviceCaps(VERTRES)*dcScreen.GetDeviceCaps(LOGPIXELSY))
		/(double)pDC->GetDeviceCaps(LOGPIXELSY));
	rc.left = LEFT_MARGIN*m_nCharWidth;
	rc.right = (int)((double)(pDC->GetDeviceCaps(HORZRES)*dcScreen.GetDeviceCaps(LOGPIXELSX))
		/(double)pDC->GetDeviceCaps(LOGPIXELSX))-RIGHT_MARGIN*m_nCharWidth;

	CString sTemp;
	/*// Print App title on top left corner
	sTemp = GetDocument()->GetTitle();
	CRect header(rc);
	header.bottom=header.top+m_nRowHeight;
	pDC->DrawText(sTemp, header, DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER);
	*/
	
	// Print draw page number at bottom center
	rc.top = rc.bottom - m_nRowHeight*(BOTTOM_MARGIN-1);
	rc.bottom = rc.top + m_nRowHeight;
	
	sTemp.Format("Page %d/%d",pInfo->m_nCurPage,pInfo->GetMaxPage());
	pDC->DrawText(sTemp,rc, DT_CENTER | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER);
}

HANDLE CXMLTreeView::DDBToDIB( CBitmap& bitmap, DWORD dwCompression, CPalette* pPal )
{
	BITMAP bm;
	BITMAPINFOHEADER bi;
	LPBITMAPINFOHEADER lpbi;
	DWORD dwLen;
	HANDLE hDIB;
	HANDLE handle;
	HDC hDC;
	HPALETTE hPal;

	ASSERT( bitmap.GetSafeHandle() );

	// The function has no arg for bitfields
	if ( dwCompression == BI_BITFIELDS )
		return NULL;

	// If a palette has not been supplied use defaul palette
	hPal = (HPALETTE) pPal->GetSafeHandle();
	if (hPal==NULL)
		hPal = (HPALETTE) GetStockObject(DEFAULT_PALETTE);

	// Get bitmap information
	bitmap.GetObject(sizeof(bm),(LPSTR)&bm);

	// Initialize the bitmapinfoheader
	bi.biSize               = sizeof(BITMAPINFOHEADER);
	bi.biWidth              = bm.bmWidth;
	bi.biHeight             = bm.bmHeight;
	bi.biPlanes             = 1;
	bi.biBitCount           = bm.bmPlanes * bm.bmBitsPixel;
	bi.biCompression        = dwCompression;
	bi.biSizeImage          = 0;
	bi.biXPelsPerMeter      = 0;
	bi.biYPelsPerMeter      = 0;
	bi.biClrUsed            = 0;
	bi.biClrImportant       = 0;

	// Compute the size of the  infoheader and the color table
	int nColors = (1 << bi.biBitCount);
	if ( nColors > 256 ) 
		nColors = 0;
	dwLen = bi.biSize + nColors * sizeof(RGBQUAD);

	// We need a device context to get the DIB from
	hDC = ::GetDC(NULL);
	hPal = SelectPalette(hDC,hPal,FALSE);
	RealizePalette(hDC);

	// Allocate enough memory to hold bitmapinfoheader and color table
	hDIB = GlobalAlloc(GMEM_FIXED,dwLen);

	if (!hDIB)
	{
		SelectPalette(hDC,hPal,FALSE);
		::ReleaseDC(NULL,hDC);
		return NULL;
	}

	lpbi = (LPBITMAPINFOHEADER)hDIB;

	*lpbi = bi;

	// Call GetDIBits with a NULL lpBits param, so the device driver 
	// will calculate the biSizeImage field 
	GetDIBits(hDC, (HBITMAP)bitmap.GetSafeHandle(), 0L, (DWORD)bi.biHeight,
		(LPBYTE)NULL, (LPBITMAPINFO)lpbi, (DWORD)DIB_RGB_COLORS);

	bi = *lpbi;

	// If the driver did not fill in the biSizeImage field, then compute it
	// Each scan line of the image is aligned on a DWORD (32bit) boundary
	if (bi.biSizeImage == 0)
	{
		bi.biSizeImage = ((((bi.biWidth * bi.biBitCount) + 31) & ~31) / 8) 
			* bi.biHeight;

		// If a compression scheme is used the result may infact be larger
		// Increase the size to account for this.
		if (dwCompression != BI_RGB)
			bi.biSizeImage = (bi.biSizeImage * 3) / 2;
	}

	// Realloc the buffer so that it can hold all the bits
	dwLen += bi.biSizeImage;
	if (handle = GlobalReAlloc(hDIB, dwLen, GMEM_MOVEABLE))
		hDIB = handle;
	else
	{
		GlobalFree(hDIB);

		// Reselect the original palette
		SelectPalette(hDC,hPal,FALSE);
		::ReleaseDC(NULL,hDC);
		return NULL;
	}

	// Get the bitmap bits
	lpbi = (LPBITMAPINFOHEADER)hDIB;

	// FINALLY get the DIB
	BOOL bGotBits = GetDIBits( hDC, (HBITMAP)bitmap.GetSafeHandle(),
							0L,                             // Start scan line
							(DWORD)bi.biHeight,             // # of scan lines
							(LPBYTE)lpbi                    // address for bitmap bits
							+ (bi.biSize + nColors * sizeof(RGBQUAD)),
							(LPBITMAPINFO)lpbi,             // address of bitmapinfo
							(DWORD)DIB_RGB_COLORS);         // Use RGB for color table

	if( !bGotBits )
	{
		GlobalFree(hDIB);

		SelectPalette(hDC,hPal,FALSE);
		::ReleaseDC(NULL,hDC);
		return NULL;
	}

	SelectPalette(hDC,hPal,FALSE);
	::ReleaseDC(NULL,hDC);
	return hDIB;
}


// these messages never get called, they are forwarded to parent class
void CXMLTreeView::OnFilePrint() 
{
	CTreeView::OnFilePrint();
}

void CXMLTreeView::OnFilePrintPreview() 
{
	CTreeView::OnFilePrintPreview();
}


/////////////////////////////////////////////////////////
// End printing code
/////////////////////////////////////////////////////////

