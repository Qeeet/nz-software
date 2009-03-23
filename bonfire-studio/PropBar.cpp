#include "stdafx.h"
#include "Bonfire.h"
#include "PropBar.h"
#include "MainFrm.h"

#include <string.h>
#include <io.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CPropBar, SIZEBAR_BASECLASS);

BEGIN_MESSAGE_MAP(CPropBar, SIZEBAR_BASECLASS)
    //{{AFX_MSG_MAP(CPropBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// Constructor
//////////////////////////////////////////////////////////////////////
CPropBar::CPropBar()
{
	
}

/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
CPropBar::~CPropBar()
{

}

/////////////////////////////////////////////////////////////////////////////
// called when project bar gets created
/////////////////////////////////////////////////////////////////////////////
int CPropBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (SIZEBAR_BASECLASS::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetSCBStyle(GetSCBStyle() | SCBS_SHOWEDGES);

    if (!m_wndList.Create (WS_DLGFRAME | 
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | 
		WS_VSCROLL | LVS_REPORT,CRect(0,0,0,0),this,
		AFX_IDW_PANE_FIRST))
	{
		TRACE0("Failed to create output edit window\n");
		return -1;
	}

	SetWindowLong(m_wndList.m_hWnd,GWL_EXSTYLE,
		GetWindowLong(m_wndList.m_hWnd,GWL_EXSTYLE) | WS_EX_CLIENTEDGE);

	m_wndList.SetFont(CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT)));

	//CRect rListRect;
	//m_wndList.GetWindowRect(rListRect);	
	CFont newFont;
	m_wndList.SetFont(&newFont, TRUE);
	m_wndList.InsertColumn(0, "Attribute", LVCFMT_LEFT, 70, -1);
	m_wndList.InsertColumn(1, "Value", LVCFMT_LEFT, 70, -1);
	m_wndList.SetItemText(m_wndList.InsertItem(0, "attr1"), 1, "val1");
	m_wndList.SetItemText(m_wndList.InsertItem(1, "attr2"), 1, "valueofitem2");

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void CPropBar::OnDestroy() 
{
	SIZEBAR_BASECLASS::OnDestroy();
}

/////////////////////////////////////////////////////////////////////////////
// event called onsizing of the control
/////////////////////////////////////////////////////////////////////////////
void CPropBar::OnSize(UINT nType, int cx, int cy) 
{
	SIZEBAR_BASECLASS::OnSize(nType, cx, cy);

	//RepositionBars(0,0x7fffff,AFX_IDW_PANE_FIRST,CWnd::reposDefault,NULL,NULL,TRUE);
	//RepositionBars(0,0x7fffff,AFX_IDW_PANE_FIRST,CWnd::reposDefault,NULL,NULL,TRUE);
	m_wndList.MoveWindow(0,0,cx,cy);
}
