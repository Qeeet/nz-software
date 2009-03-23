// NZProjectTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "bonfire.h"
#include "NZProjectTreeCtrl.h"
#include "XNode.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CMainFrame* g_pMainFrame;

const DWORD EXPAND_TIMER = 2000;
const DWORD SCROLL_TIMER = 75;

/////////////////////////////////////////////////////////////////////////////
// CNZProjectTreeCtrl

CNZProjectTreeCtrl::CNZProjectTreeCtrl()
{
	m_bDragging		= FALSE;
	m_pDragImage	= FALSE;
	m_bRedraw		= TRUE;
}

CNZProjectTreeCtrl::~CNZProjectTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CNZProjectTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CNZProjectTreeCtrl)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBeginDrag)
	ON_WM_TIMER()
	ON_WM_SETFOCUS()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemExpanded)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemExpanding)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNZProjectTreeCtrl message handlers

void CNZProjectTreeCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	HTREEITEM		hitem;
	UINT			flags;

	if (m_bDragging)
	{
		//TRACE0("MOUSE MOVE/t");
		POINT pt = point;
		ClientToScreen( &pt );
		CImageList::DragMove(pt);
		if ((hitem = HitTest(point, &flags)) != NULL)
		{
			if( m_htiOldDrop == NULL )
				m_htiOldDrop = GetDropHilightItem();

			CXNode* pNode = (CXNode*)GetItemData(hitem);
			if (pNode->m_enNodeType == XNODE_FILE)
			{
				// use the file's parent (folder or project)
				hitem = pNode->m_pParent->m_hTreeItem;
				ASSERT(hitem);
			}

			// select the node to which we can drop
			CImageList::DragShowNolock(FALSE);
			SelectDropTarget(hitem);
			m_htiDrop = hitem;
			CImageList::DragShowNolock(TRUE);

			// kill the timer if selection changed
			if( m_nExpandTimer && hitem == m_htiOldDrop )
			{
				KillTimer( m_nExpandTimer );
				m_nExpandTimer = 0;
			}
			
			// start the timer again
			if( !m_nExpandTimer )
				m_nExpandTimer = SetTimer( 1000, EXPAND_TIMER, NULL );
		}
	}

	CTreeCtrl::OnMouseMove(nFlags, point);
}


void CNZProjectTreeCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CTreeCtrl::OnLButtonUp(nFlags, point);

	if (m_bDragging)
	{
		// end dragging
		m_bDragging = FALSE;

		try
		{
			CImageList::DragLeave(this);
			CImageList::EndDrag();
			ReleaseCapture();

			delete m_pDragImage;

			// Remove drop target highlighting
			SelectDropTarget(NULL);
			m_htiOldDrop = NULL;

			// If Drag item is a child of Drop item or they ar ethe same node then return
			if( m_htiDrag == m_htiDrop || GetParentItem(m_htiDrag) == m_htiDrop )
				throw "";
			
			CXNode* pNewNode = NULL;
			CXNode* pDragNode = (CXNode*)GetItemData(m_htiDrag);
			CXNode* pDropNode = (CXNode*)GetItemData(m_htiDrop);
			if (pDropNode->m_enNodeType == XNODE_FILE)
				throw"";
			
			this->MoveNode(pDragNode, (CXFolder*)pDropNode);
		}
		catch (...)
		{}

		// kill the expand timer
		if( m_nExpandTimer )
		{
			KillTimer( m_nExpandTimer );
			m_nExpandTimer = 0;
		}

		// kill the scroll timer
		if( m_nScrollTimer )
		{
			KillTimer( m_nScrollTimer );
			m_nScrollTimer = 0;
		}
	}
}

void CNZProjectTreeCtrl::OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here

	m_htiDrag = pNMTreeView->itemNew.hItem;
	m_htiDrop = NULL;

	CXNode* pNode = (CXNode*)GetItemData(m_htiDrag);
	// disallow dragging of projects
	if (!pNode || pNode->m_enNodeType == XNODE_PROJECT)
		return;

	m_pDragImage = CreateDragImage( m_htiDrag );
	if( !m_pDragImage )
		return;

	m_bDragging = true;

	CPoint pt(0,0);
	
	IMAGEINFO ii;
	m_pDragImage->GetImageInfo( 0, &ii );
	pt.x = (ii.rcImage.right - ii.rcImage.left) / 2;
	pt.y = (ii.rcImage.bottom - ii.rcImage.top) / 2;

	m_pDragImage->BeginDrag( 0, pt );
	pt = pNMTreeView->ptDrag;
	ClientToScreen( &pt );
	m_pDragImage->DragEnter(NULL,pt);
	
	SetCapture();

	*pResult = 0;

	// Set up the scroll timer
	m_nScrollTimer = SetTimer(1, SCROLL_TIMER, NULL);
}

// the timer event for expanding nodes in drag n drop procedure
void CNZProjectTreeCtrl::OnTimer(UINT nIDEvent) 
{
	if( nIDEvent == m_nExpandTimer )
	{
		HTREEITEM htiFloat = GetDropHilightItem();
		if( htiFloat && htiFloat == m_htiDrop )
		{
			if( ItemHasChildren( htiFloat ) )
				Expand( htiFloat, TVE_EXPAND );
		}
	}
	else if( nIDEvent == m_nScrollTimer )
	{
		// Doesn't matter that we didn't initialize m_timerticks
		m_timerticks++;

		POINT pt;
		GetCursorPos( &pt );
		RECT rect;
		GetClientRect( &rect );
		ClientToScreen( &rect );

		// NOTE: Screen coordinate is being used because the call
		// to DragEnter had used the Desktop window.
		//CImageList::DragMove(pt);

		HTREEITEM hitem = GetFirstVisibleItem();

		if( pt.y < rect.top + 10 ) // scroll up
		{
			// Scroll slowly if cursor near the treeview control
			int slowscroll = 6 - (rect.top + 10 - pt.y) / 20;
			if( 0 == ( m_timerticks % (slowscroll > 0? slowscroll : 1) ) )
			{
				CImageList::DragShowNolock(FALSE);
				SendMessage( WM_VSCROLL, SB_LINEUP);
				SelectDropTarget(hitem);
				m_htiDrop = hitem;
				CImageList::DragShowNolock(TRUE);
			}
		}
		else if( pt.y > rect.bottom - 10 ) // scroll down
		{
			// Scroll slowly if cursor near the treeview control
			int slowscroll = 6 - (pt.y - rect.bottom + 10 ) / 20;
			if( 0 == ( m_timerticks % (slowscroll > 0? slowscroll : 1) ) )
			{
				CImageList::DragShowNolock(FALSE);
				SendMessage( WM_VSCROLL, SB_LINEDOWN);
				int nCount = GetVisibleCount();
				for ( int i=0; i<nCount-1; ++i )
					hitem = GetNextVisibleItem(hitem);
				if( hitem )
					SelectDropTarget(hitem);
				m_htiDrop = hitem;
				CImageList::DragShowNolock(TRUE);
			}
		}
	}

	CTreeCtrl::OnTimer(nIDEvent);
}

////////////////////////////////////////////////////////////////
// place pNode under pNewParent
////////////////////////////////////////////////////////////////
void CNZProjectTreeCtrl::MoveNode(CXNode *pDragNode, CXFolder *pNewParent)
{
	CXNode* pNewNode	= NULL;
	CBonfireDoc* pDoc	= NULL;
	switch(pDragNode->m_enNodeType)
	{
	case XNODE_FILE :
	{
		pNewNode	= ((CXFile*)pDragNode)->Clone();
		pDoc		= ((CXFile*)pDragNode)->m_pDoc;
		g_pMainFrame->m_wndProjectBar.LinkTreeNodeToChild( (CXFile*)pNewNode, pDoc );

		break;
	}
	case XNODE_FOLDER :
		pNewNode	= ((CXFolder*)pDragNode)->Clone();
		break;
	case XNODE_PROJECT :
		pNewNode	= ((CXProject*)pDragNode)->Clone();
		break;
	}

	ASSERT(pNewNode);
	pNewNode->m_hTreeItem = NULL;

	switch(pDragNode->m_enNodeType)
	{ // both are very similar, keeping it separate for future, maybe they will be different
	case XNODE_FILE :
		// add the node to new parent
		if (g_pMainFrame->m_wndProjectBar.AddNodeToTree(pNewNode, pNewParent, CMA_MOVE, TRUE, FALSE))
		{
			// remove item from old parent, and make the old project UNsaved
			pDragNode->m_pProject->SetModified(TRUE);
			g_pMainFrame->m_wndProjectBar.DeleteNode(pDragNode, TRUE);

			// make the new project UNsaved
			pNewNode->m_pProject->SetModified(TRUE);

			pNewParent->UpdateChildren();

			// if document exists, rename (change dir) and add to MRU list
			// THIS IS DONE IN ADDNODETOTREE()
			//if (pDoc)
			//	pDoc->SetPathName(pNewNode->FullPath(), TRUE);

			// select the moved node
			g_pMainFrame->m_wndProjectBar.SelectTreeNode(pNewNode);
		}
		else
		{
			//delete pNewNode;
		}
		break;
	case XNODE_FOLDER :
		// add the node to new parent
		if (g_pMainFrame->m_wndProjectBar.AddNodeToTree(pNewNode, pNewParent, CMA_NONE, TRUE, FALSE))
		{
			// remove item from old parent, and make the old project UNsaved
			pDragNode->m_pProject->SetModified(TRUE);
			g_pMainFrame->m_wndProjectBar.DeleteNode(pDragNode, TRUE);

			// make the new project UNsaved
			pNewNode->m_pProject->SetModified(TRUE);

			pNewParent->UpdateChildren();

			// select the moved node
			g_pMainFrame->m_wndProjectBar.SelectTreeNode(pNewNode);
		}
		break;
	}
}

BOOL CNZProjectTreeCtrl::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	return CTreeCtrl::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CNZProjectTreeCtrl::PreTranslateMessage(MSG* pMsg) 
{
	// When editing labels, make sure the edit control receives important messages
	if( pMsg->message == WM_KEYDOWN && GetEditControl() )
	{
    	CWnd* pParent;
		if (pMsg->wParam == VK_RETURN)
		{
			pParent = GetParent();
			ASSERT(pParent);
			if(pParent)
				pParent->PostMessage(TVM_ENDEDITLABELNOW, FALSE, 0);
		}
		else if (pMsg->wParam == VK_ESCAPE)
		{
			pParent = GetParent();
			ASSERT(pParent);
			if(pParent)
				pParent->PostMessage(TVM_ENDEDITLABELNOW, TRUE, 0);
		}
		else
		{
			return CTreeCtrl::PreTranslateMessage(pMsg);
		}
		
        ::TranslateMessage(pMsg);
        ::DispatchMessage(pMsg);
        return TRUE;                            // DO NOT process further
    }

	if (pMsg->message == WM_KEYDOWN)
	{
		// allow handling of key down events for project tree
        ::TranslateMessage(pMsg);
        ::DispatchMessage(pMsg);
		return TRUE;
	}

	return CTreeCtrl::PreTranslateMessage(pMsg);
}

/*
void CNZProjectTreeCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	TRACE("%d\t%d\n",GetTickCount(),nChar);
	CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}
*/

void CNZProjectTreeCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	CTreeCtrl::OnSetFocus(pOldWnd);

	TRACE0("CNZProjectTreeCtrl::OnSetFocus\n");
}

void CNZProjectTreeCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	TRACE0("CNZProjectTreeCtrl::OnLButtonDblClk\n");
	
	CTreeCtrl::OnLButtonDblClk(nFlags, point);
}


void CNZProjectTreeCtrl::OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	SetRedrawFlag(FALSE);
	
	*pResult = 0;
}

void CNZProjectTreeCtrl::OnItemExpanded(NMHDR* pNMHDR, LRESULT* pResult) 
{
	SetRedrawFlag(TRUE);

	*pResult = 0;
}

DWORD iExpand2 = 0;
void CNZProjectTreeCtrl::SetRedrawFlag(BOOL bRedraw)
{
	if (bRedraw)
		iExpand2--;
	else
		iExpand2++;

	if (!bRedraw || iExpand2 == 0)
	//if (bRedraw || m_bRedraw)
	{
		m_bRedraw = bRedraw;
		SetRedraw(bRedraw);
	}
}
