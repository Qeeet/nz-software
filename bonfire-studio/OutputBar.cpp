#include "stdafx.h"
#include "Bonfire.h"
#include "OutputBar.h"
#include "SourceView.h"
#include "MainFrm.h"

#include <string.h>
#include <io.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CMainFrame* g_pMainFrame;
extern CBonfireApp theApp;

IMPLEMENT_DYNAMIC(COutputBar, SIZEBAR_BASECLASS);

BEGIN_MESSAGE_MAP(COutputBar, SIZEBAR_BASECLASS)
    //{{AFX_MSG_MAP(COutputBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// Constructor
//////////////////////////////////////////////////////////////////////
COutputBar::COutputBar()
{
	m_lsCurrentList = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
COutputBar::~COutputBar()
{

}

/////////////////////////////////////////////////////////////////////////////
// called when project bar gets created
/////////////////////////////////////////////////////////////////////////////
int COutputBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (SIZEBAR_BASECLASS::OnCreate(lpCreateStruct) == -1)
		return -1;

	//SetSCBStyle(GetSCBStyle() | SCBS_SHOWEDGES);

	CClientDC tmpdc(this);

	/*m_fntEdit.CreateFont(-MulDiv(10,tmpdc.GetDeviceCaps(LOGPIXELSY),72),0,
		0,0,FW_NORMAL,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,"Courier");

    if (!m_wndEdit.Create (
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | 
		WS_HSCROLL | WS_VSCROLL | ES_MULTILINE | ES_READONLY | 
		ES_DISABLENOSCROLL,CRect(0,0,0,0),this,
		AFX_IDW_PANE_FIRST))
	{
		TRACE0("Failed to create output edit window\n");
		return -1;
	}*/

	if (!m_wndList.Create (
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
		LVS_SHOWSELALWAYS |
		WS_HSCROLL | WS_VSCROLL | LVS_REPORT,CRect(0,0,0,0),this,
		AFX_IDW_PANE_FIRST))
	{
		TRACE0("Failed to create output list window\n");
		return -1;
	}

	m_wndList.SetExtendedStyle(	//LVS_EX_FLATSB | 
								LVS_EX_HEADERDRAGDROP | 
								LVS_EX_FULLROWSELECT);

	SetWindowLong(m_wndList.m_hWnd,GWL_EXSTYLE,
		GetWindowLong(m_wndList.m_hWnd,GWL_EXSTYLE) | WS_EX_CLIENTEDGE);

	CBitmap bmpIcons; bmpIcons.LoadBitmap(IDB_OUTPUT_ICONS);
	m_imlListImages.Create(16,16,ILC_COLOR8 | ILC_MASK,4,2);
	m_imlListImages.Add(&bmpIcons,RGB(255,0,255));
	m_wndList.SetImageList(&m_imlListImages,LVSIL_SMALL);
	bmpIcons.DeleteObject();

	//m_wndEdit.SetFont(&m_fntEdit);

	if (!m_wndTabBar.Create(this,18,
		WS_VISIBLE | WS_CHILD | CBRS_BOTTOM,
		AFX_IDW_CONTROLBAR_FIRST + 32))
	{
		TRACE0("Failed to create tab bar for output bar\n");
		return -1;
	}

	ChangeView(0,FALSE);

	m_wndTabBar.AddTab("Application Output",FALSE);
	m_wndTabBar.AddTab("Find in Files",FALSE);
	m_wndTabBar.AddTab("XML Output",TRUE);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void COutputBar::OnDestroy() 
{
	SaveColumnSizes(m_wndTabBar.GetSelTab());

	// free memory
	POSITION nPos = m_arrAppMessages.GetTailPosition();
	while (nPos != NULL) delete (OutputMessage*)m_arrAppMessages.GetPrev(nPos);

	nPos = m_arrFIFMessages.GetTailPosition();
	while (nPos != NULL) delete (OutputMessage*)m_arrFIFMessages.GetPrev(nPos);

	nPos = m_arrXMLMessages.GetTailPosition();
	while (nPos != NULL) delete (OutputMessage*)m_arrXMLMessages.GetPrev(nPos);

	SIZEBAR_BASECLASS::OnDestroy();
	m_imlListImages.DeleteImageList();
	//m_fntEdit.DeleteObject();
}

/////////////////////////////////////////////////////////////////////////////
// this function gets called when tree bar isnotified of an event
/////////////////////////////////////////////////////////////////////////////
BOOL COutputBar::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (wParam == (WPARAM)GetWindowLong(m_wndTabBar,GWL_ID))
	{
		NMTABBAR* pMessage = (NMTABBAR*)lParam;
		if (pMessage->tab == 0 && m_wndTabBar.GetSelTab() != 0)
			;//m_wndEdit.SetWindowText("hey");
		else if (pMessage->tab == 1 && m_wndTabBar.GetSelTab() != 1)
			;//m_wndEdit.SetWindowText("hey2");// switch to view
		else
			;//pMessage->cancel = TRUE;

		if (!pMessage->cancel)
		{
			SaveColumnSizes(pMessage->prevtab);
			ChangeView(pMessage->tab,FALSE);
		}
	}
	else if (wParam == AFX_IDW_PANE_FIRST)
	{
		NMHDR* pNotify = (NMHDR*)lParam;
		switch (pNotify->code)
		{
		case NM_RETURN : //FOR ROMA
		{
			int nItem = 0;
			POSITION pos = m_wndList.GetFirstSelectedItemPosition();
			if (pos == NULL)
			   TRACE0("No items were selected!\n");
			else
				nItem = m_wndList.GetNextSelectedItem(pos);
			OpenItem(nItem);

			break;
		}
		case NM_DBLCLK :
		{
			// get the current node in the tree
			POINT pt;
			GetCursorPos(&pt);
			m_wndList.ScreenToClient(&pt);
			OpenItem(m_wndList.HitTest(pt));
			break;
		}
		//default :
			//TRACE("%d\t", pNotify->code);
		}

	}

	return SIZEBAR_BASECLASS::OnNotify(wParam, lParam, pResult);
}

void COutputBar::OpenItem(int nIndex)
{
	if (nIndex != -1)
	{
		OutputMessage* pMsg = (OutputMessage*)m_wndList.GetItemData(nIndex);

		// dont do anything with NULL nodes
		if (pMsg == NULL || pMsg->enResult == MSG_NONE)
			return;

		if (pMsg->enType == OUTPUT_FIF_RESULT)
		{
			// open the file
			CBonfireDoc* pDoc = (CBonfireDoc*)theApp.OpenDocumentFile(pMsg->strMessage);
			if (pDoc && pMsg->nLine >= 0)
			{
				try
				{
					CBonfireView*	pBNewView	= NULL;
					CView*			pSourceView	= NULL;
				
					CString strCaption = ((CBonfireView)theApp.m_arAllViews[0]).m_strCaption;
					pDoc->SwitchToView(strCaption);
					
					// check if a view of this type already exists
 					if ( !pDoc->m_pViews->Lookup(strCaption, (void*&)pBNewView) )
						throw "";
						
					pSourceView = pBNewView->m_pView;
					
					// go to the desired line
					CPoint pt1(pMsg->nCol - 1, pMsg->nLine - 1);
					CPoint pt2(pMsg->nCol - 1 + pMsg->nSelSize, pMsg->nLine - 1);
				
					((CSourceView*)pSourceView)->SetCursorPos(pt1);
					((CSourceView*)pSourceView)->SelectText(pt1, pt2);
					((CSourceView*)pSourceView)->EnsureVisible(pt1);
				}
				catch (...)
				{
					// add the message to output
					OutputMessage* pMsg = new OutputMessage();
					pMsg->enType = OUTPUT_APPSTATUS;
					pMsg->enResult = MSG_ERROR;
					pMsg->strMessage = "Error occurred while trying to seek to line.";
					g_pMainFrame->m_wndOutputBar.AddOutput(pMsg);
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// event called onsizing of the control
/////////////////////////////////////////////////////////////////////////////
void COutputBar::OnSize(UINT nType, int cx, int cy) 
{
	SIZEBAR_BASECLASS::OnSize(nType, cx, cy);

	//RepositionBars(0,0x7fffff,AFX_IDW_PANE_FIRST,CWnd::reposDefault,NULL,NULL,TRUE);
	//RepositionBars(0,0x7fffff,AFX_IDW_PANE_FIRST,CWnd::reposDefault,NULL,NULL,TRUE);
	m_wndList.MoveWindow(0,0,cx,cy - m_wndTabBar.GetHeight());
	m_wndTabBar.MoveWindow(0,cy - m_wndTabBar.GetHeight(),cx,m_wndTabBar.GetHeight());
}

// defined only here
void FormatOutputString(CString& str)
{
	str.TrimLeft();
	str.TrimRight();
	str.Replace("\r\n"," ");
}

/////////////////////////////////////////////////////////////////////////////
// add the message to the appropriate output view
/////////////////////////////////////////////////////////////////////////////
void COutputBar::AddOutput(OutputMessage *pMessage)
{
	// format the strings in the message
	FormatOutputString(pMessage->strMessage);
	FormatOutputString(pMessage->strResult);
	FormatOutputString(pMessage->strLine);

	int iLast = m_wndList.GetItemCount();

	switch(pMessage->enType)
	{
	case OUTPUT_APPSTATUS :
		if (theApp.m_opOptions.general.dwClearOutput)
			ClearList(OUTPUT_APPSTATUS, TRUE);

		m_arrAppMessages.AddHead(pMessage);
		if (m_lsCurrentList != &m_arrAppMessages)
		{
			// swtich to the view
			ChangeView(pMessage->enType);
		}
		else
		{
			// simply add the new message to the current view
			m_wndList.InsertItem(0, pMessage->strMessage, pMessage->enResult);
			m_wndList.SetItemData(0, (DWORD)pMessage);
			UpdateItem(0);
		}
		break;
	case OUTPUT_FIF_RESULT :
		m_arrFIFMessages.AddTail(pMessage);
		if (m_lsCurrentList != &m_arrFIFMessages)
		{
			// swtich to the view
			ChangeView(pMessage->enType);
		}
		else
		{
			// simply add the new message to the current view
			m_wndList.InsertItem(iLast, pMessage->strMessage, pMessage->enResult);
			m_wndList.SetItemData(iLast, (DWORD)pMessage);
			UpdateItem(iLast);
		}
		
		// show the output bar if it was hidden
		g_pMainFrame->ShowControlBar(&g_pMainFrame->m_wndOutputBar, TRUE, FALSE);
		m_wndList.EnsureVisible(iLast, FALSE);

		break;
	case OUTPUT_XMLERROR :
		// remove the "success" message if its there
		if ( m_arrXMLMessages.GetCount() )
		{
			OutputMessage* pFirstMsg = (OutputMessage*)m_arrXMLMessages.GetHead();
			if (pFirstMsg && pFirstMsg->enResult == MSG_SUCCESS)
				ClearList(OUTPUT_XMLERROR);
		}

		m_arrXMLMessages.AddHead(pMessage);
		if (m_lsCurrentList != &m_arrXMLMessages)
		{
			// swtich to the view
			ChangeView(pMessage->enType);
		}
		else
		{
			// simply add the new message to the current view
			m_wndList.InsertItem(0, pMessage->strMessage, pMessage->enResult);
			m_wndList.SetItemData(0, (DWORD)pMessage);
			UpdateItem(0);
			//UpdateLast(pMessage);
		}
		
		// show the output bar if it was hidden
		g_pMainFrame->ShowControlBar(&g_pMainFrame->m_wndOutputBar, TRUE, FALSE);

		break;
	}

	this->m_wndList.SetRedraw();
}

/////////////////////////////////////////////////////////////////////////////
// update the last message in the list using the last message in the datastructure
/////////////////////////////////////////////////////////////////////////////
void COutputBar::UpdateLast()
{
	//OutputMessage* pMessage = (OutputMessage*)m_lsCurrentList->GetTail();
	UpdateItem(0);
	//UpdateLast(pMessage);
}

/////////////////////////////////////////////////////////////////////////////
// update the nth message in the list
/////////////////////////////////////////////////////////////////////////////
void COutputBar::UpdateItem(int iIndex)
{
	OutputMessage* pMessage = (OutputMessage*)m_wndList.GetItemData(iIndex);
	if (pMessage && pMessage->enResult != MSG_NONE)
	{
		switch(pMessage->enType)
		{
		case OUTPUT_APPSTATUS :
			// set the result text and the new image
			m_wndList.SetItem(iIndex, 1, LVIF_TEXT, pMessage->strResult, pMessage->enResult, 0, 0, NULL);
			m_wndList.SetItem(iIndex, 0, LVIF_IMAGE, pMessage->strResult, pMessage->enResult, 0, 0, NULL);
			break;
		case OUTPUT_FIF_RESULT :
		case OUTPUT_XMLERROR :
			// set the line #
			if (pMessage->nLine != 0 && pMessage->nCol != 0)
			{
				CString strNLine;
				strNLine.Format("%d, %d", pMessage->nLine, pMessage->nCol);
				m_wndList.SetItem(iIndex, 1, LVIF_TEXT, strNLine, pMessage->enResult, pMessage->enResult, 0, NULL);
			}

			// set the line text
			m_wndList.SetItem(iIndex, 2, LVIF_TEXT, pMessage->strLine, pMessage->enResult, pMessage->enResult, 0, NULL);
			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// update the last message in the list with the given object
/////////////////////////////////////////////////////////////////////////////
void COutputBar::UpdateLast(OutputMessage* pMessage)
{
	switch(pMessage->enType)
	{
	case OUTPUT_APPSTATUS :
		// set the result text and the new image
		m_wndList.SetItem(0, 1, LVIF_TEXT, pMessage->strResult, pMessage->enResult, 0, 0, NULL);
		m_wndList.SetItem(0, 0, LVIF_IMAGE, pMessage->strResult, pMessage->enResult, 0, 0, NULL);
		break;
	case OUTPUT_FIF_RESULT :
	case OUTPUT_XMLERROR :
		// set the line #
		if (pMessage->nLine != 0 && pMessage->nCol != 0)
		{
			CString strNLine;
			strNLine.Format("%d, %d", pMessage->nLine, pMessage->nCol);
			m_wndList.SetItem(0, 1, LVIF_TEXT, strNLine, pMessage->enResult, pMessage->enResult, 0, NULL);
		}

		// set the line text
		m_wndList.SetItem(0, 2, LVIF_TEXT, pMessage->strLine, pMessage->enResult, pMessage->enResult, 0, NULL);
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////
// change the current output view
/////////////////////////////////////////////////////////////////////////////
void COutputBar::ChangeView(int nView, BOOL bUpdateTabs)
{
	if (!m_wndList.GetHeaderCtrl())
		return;

	// clear the old messages
	m_wndList.DeleteAllItems();

	// delete the columns
	int nColumnCount = m_wndList.GetHeaderCtrl()->GetItemCount();
	for (int i = 0; i < nColumnCount; i++)
		m_wndList.DeleteColumn(0);

	switch (nView)
	{
	case OUTPUT_APPSTATUS:
		m_wndList.InsertColumn(0,"Message",LVCFMT_LEFT,GetColSize(0,0));
		m_wndList.InsertColumn(1,"Result",LVCFMT_LEFT,GetColSize(0,1));

		m_lsCurrentList = &m_arrAppMessages;
		break;

	case OUTPUT_FIF_RESULT:
		m_wndList.InsertColumn(0,"File",LVCFMT_LEFT,GetColSize(1,0));
		m_wndList.InsertColumn(1,"Line #, Position",LVCFMT_LEFT,GetColSize(1,1));
		m_wndList.InsertColumn(2,"Line",LVCFMT_LEFT,GetColSize(1,2));

		m_lsCurrentList = &m_arrFIFMessages;
		break;

	case OUTPUT_XMLERROR:
		m_wndList.InsertColumn(0,"Error",LVCFMT_LEFT,GetColSize(2,0));
		m_wndList.InsertColumn(1,"Line #, Position",LVCFMT_LEFT,GetColSize(2,1));
		m_wndList.InsertColumn(2,"Line",LVCFMT_LEFT,GetColSize(2,2));

		m_lsCurrentList = &m_arrXMLMessages;
		break;
	}

	// populate the output list
	int size = m_lsCurrentList->GetCount();
	POSITION nPos = m_lsCurrentList->GetTailPosition();
	while (nPos != NULL)
	{
		// get the next message
		OutputMessage* pMessage = (OutputMessage*)m_lsCurrentList->GetPrev(nPos);
		m_wndList.InsertItem(0, pMessage->strMessage, pMessage->enResult);
		
		// set the data of the item to point to the message object
		m_wndList.SetItemData(0, (DWORD)pMessage);

		// update the message in the list: add the other columns
		UpdateItem(0);
		//UpdateLast(pMessage);
	}

	if (bUpdateTabs)
		m_wndTabBar.SetSelTab(nView);
}

/////////////////////////////////////////////////////////////////////////////
// set the column width
/////////////////////////////////////////////////////////////////////////////
int COutputBar::GetColSize(int nView, int nColumn)
{
	CString strVal;
	strVal.Format("ColSize_%d.%d",nView,nColumn);

	return AfxGetApp()->GetProfileInt("OutputBar",strVal,150);
}

/////////////////////////////////////////////////////////////////////////////
// return the number of FIF results
/////////////////////////////////////////////////////////////////////////////
int COutputBar::GetFIFCount()
{
	return this->m_arrFIFMessages.GetCount();
}

/////////////////////////////////////////////////////////////////////////////
// save column sizes in registry profile
/////////////////////////////////////////////////////////////////////////////
void COutputBar::SaveColumnSizes(int nView)
{
	CString strVal;

	int nColumnCount = m_wndList.GetHeaderCtrl()->GetItemCount();
	for (int i = 0; i < nColumnCount; i++)
	{
		strVal.Format("ColSize_%d.%d",nView,i);
		AfxGetApp()->WriteProfileInt("OutputBar",strVal,
			m_wndList.GetColumnWidth(i));
	}
}

HBRUSH COutputBar::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	/*if (nCtlColor == CTLCOLOR_STATIC && pWnd == &m_wndEdit)
	{
		pDC->SetBkColor(GetSysColor(COLOR_WINDOW));
		pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
		return GetSysColorBrush(COLOR_WINDOW);
	}
	else*/
		return SIZEBAR_BASECLASS::OnCtlColor(pDC, pWnd, nCtlColor);
}

/////////////////////////////////////////////////////////////////////////////
// clear the messages from the specified list
/////////////////////////////////////////////////////////////////////////////
void COutputBar::ClearList(int nList, BOOL bSwitchTab /*= TRUE*/)
{
	CList<OutputMessage*, OutputMessage*>* lsList;

	switch (nList)
	{
	case OUTPUT_APPSTATUS:
		lsList = &m_arrAppMessages;
		break;

	case OUTPUT_FIF_RESULT:
		lsList = &m_arrFIFMessages;
		break;

	case OUTPUT_XMLERROR:
		lsList = &m_arrXMLMessages;
		break;
	}

	POSITION pos = lsList->GetHeadPosition();
	while(pos != NULL)
	{
		OutputMessage* pMsg = (OutputMessage*)lsList->GetNext(pos);
		delete pMsg;
	}
	lsList->RemoveAll();

	// update the control
	if (bSwitchTab)
		ChangeView(nList, TRUE);

	return;
}

BOOL COutputBar::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		// allow handling of key down events for output bar
        ::TranslateMessage(pMsg);
        ::DispatchMessage(pMsg);
		return TRUE;
	}

	return SIZEBAR_BASECLASS::PreTranslateMessage(pMsg);
}


