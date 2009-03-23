// TabBar.cpp: implementation of the CTabBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TabBar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CTabBar

IMPLEMENT_DYNAMIC(CTabBar, CControlBar)

BEGIN_MESSAGE_MAP(CTabBar, CControlBar)
	//{{AFX_MSG_MAP(CTabBar)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CTabBar::CTabBar()
{
	m_nFirstTabOffset = 5;
	m_nScrollPos = 0;
	m_nSelTab = -1;
	m_nHeight = 0; // set in OnCreate
	m_bBorderParentMaximized = FALSE;
}

CTabBar::~CTabBar()
{

}

BOOL CTabBar::Create(CWnd* pParentWnd, int nHeight, DWORD dwStyle, UINT nID)
{
	m_dwStyle = dwStyle & CBRS_ALL; // save the control bar styles

	// register and create the window - skip CControlBar::Create()
	LPCTSTR strClass = AfxRegisterWndClass(CS_DBLCLKS,
		LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW)),
		NULL,NULL);

	dwStyle &= ~CBRS_ALL; // keep only the generic window styles
	dwStyle |= WS_CLIPCHILDREN; // prevents flashing
	m_nHeight = nHeight;

	return CWnd::Create(strClass,NULL,dwStyle,CRect(0,0,nHeight,0),
		pParentWnd,nID);
}


void CTabBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
	//CControlBar::OnUpdateCmdUI(pTarget,bDisableIfNoHndler);
}

CSize CTabBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	int n = GetParentFrame()->IsZoomed() ? 3 : -1;
	RECT rt;
	GetParentFrame()->GetClientRect(&rt);
	return CSize(rt.right - rt.left,m_nHeight + n);
}

CSize CTabBar::CalcDynamicLayout(int nLength, DWORD dwMode)
{
	return CalcFixedLayout(TRUE,TRUE);
}


BOOL CTabBar::PreCreateWindow(CREATESTRUCT& cs) 
{
	m_dwStyle &= ~(CBRS_BORDER_ANY | CBRS_BORDER_3D);
	return CControlBar::PreCreateWindow(cs);
}


/////////////////////////////////////////////////////////////////////////////
// CTabBar functions

void CTabBar::HfnDrawTab(CDC* pDC, CFont* pFont, LPCTSTR strCaption,
						  int x, int cx, BOOL bSelected)
{
	const int cy = m_nHeight - 2;

	// create the tab region
	POINT pts[4];
	pts[0].x = x;					pts[0].y = 2;
	pts[1].x = x + cy / 2;			pts[1].y = cy;
	pts[2].x = x + cx - cy / 2;		pts[2].y = cy;
	pts[3].x = x + cx;				pts[3].y = 2;

	CRgn rgnTab;
	rgnTab.CreatePolygonRgn(pts,4,ALTERNATE);

	// fill the region
	CBrush hbrMain; hbrMain.CreateSysColorBrush(bSelected ? COLOR_BTNFACE : COLOR_BTNSHADOW);
	pDC->FillRgn(&rgnTab,&hbrMain);

	// draw the lines
	CPen hpnMain; hpnMain.CreatePen(PS_SOLID,1,(COLORREF)GetSysColor(COLOR_3DDKSHADOW));
	CPen* pOldPen = pDC->SelectObject(&hpnMain);
	pDC->MoveTo(pts[0]); pDC->LineTo(pts[1]);
	pDC->LineTo(pts[2]); pDC->LineTo(pts[3]);
	if (!bSelected)pDC->LineTo(pts[0]); // don't draw top line if selected
	pDC->SelectObject(pOldPen); hpnMain.DeleteObject();

	// draw the text
	CRect rtText(x + cy / 2,2,x + cx - cy / 2,cy);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(GetSysColor(bSelected ? COLOR_BTNTEXT : COLOR_BTNHIGHLIGHT));
	pDC->DrawText(strCaption,&rtText,DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	/*if (bSelected)
	{
		// if selected, draw caption again one pixel to the right to make a "bold" effect
		OffsetRect(&rtText,1,0);
		pDC->DrawText(strCaption,&rtText,DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}*/

	rgnTab.DeleteObject();
}

CString CTabBar::GetTab(int nIndex) const
{
	if (nIndex < 0 || nIndex >= GetTabCount())return "";
	return m_arrTabs.GetAt(nIndex);
}

int CTabBar::GetSelTab() const
{	return m_nSelTab; }

int CTabBar::GetTabCount() const
{	return m_arrTabs.GetSize(); }

void CTabBar::SetTab(int nIndex, CString strCaption)
{	
	if (nIndex < 0 || nIndex >= GetTabCount())return;
	m_arrTabs.SetAt(nIndex,strCaption);
	if (m_hWnd)RedrawWindow();
}

void CTabBar::SetSelTab(int nIndex)
{
	if (nIndex >= 0 && nIndex != m_nSelTab)
	{
		// send the VTN_SELCHANGING notification message
		NMTABBAR nvt;
		nvt.hdr.code = VTN_SELCHANGING;
		nvt.hdr.hwndFrom = m_hWnd;
		nvt.hdr.idFrom = GetWindowLong(*this,GWL_ID);
		nvt.prevtab = m_nSelTab;
		nvt.tab = nIndex;
		nvt.cancel = FALSE;

		if (m_hWnd)GetParent()->SendMessage(WM_NOTIFY,(WPARAM)nvt.hdr.idFrom,(LPARAM)&nvt);

		if (nvt.cancel != TRUE)
		{
			m_nSelTab = nIndex;
			if (m_hWnd)RedrawWindow();
		}
	}
}

int CTabBar::AddTab(LPCTSTR strTab, BOOL bRedraw)
{
	if (m_arrTabs.GetSize() == 0)m_nSelTab = 0;
	return m_arrTabs.Add(strTab);
	if (m_hWnd && bRedraw)RedrawWindow();
}

void CTabBar::RemoveTab(int nIndex)
{
	if (nIndex < 0)return;
	if (nIndex >= m_nSelTab)m_nSelTab--;

	m_arrTabs.RemoveAt(nIndex);
	if (m_hWnd)RedrawWindow();
}

void CTabBar::ClearTabs(BOOL bRedraw)
{
	m_arrTabs.RemoveAll();
	if (m_hWnd && bRedraw)RedrawWindow();
}

/////////////////////////////////////////////////////////////////////////////
// CTabBar message handlers

void CTabBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect rt; GetClientRect(&rt);
	int nCx = rt.Width();
	int nCy = rt.Height();

	// double-buffer
	CDC hdc; hdc.CreateCompatibleDC(&dc);
	CBitmap bmp; bmp.CreateCompatibleBitmap(&dc,nCx,nCy);
	CBitmap* pOldBitmap = hdc.SelectObject(&bmp);

	// draw background
	CBrush hbrMain;
	CRect toprt;

	toprt = rt;
	//toprt.bottom = 2;
	hbrMain.CreateSysColorBrush(COLOR_BTNFACE);
	hdc.FillRect(toprt,&hbrMain);
	/*hbrMain.DeleteObject();

	toprt = rt;
	toprt.top = 3;
	hbrMain.CreateSysColorBrush(COLOR_APPWORKSPACE);
	hdc.FillRect(toprt,&hbrMain);*/

	if (m_bBorderParentMaximized && GetParentFrame()->IsZoomed())
	{
		hdc.DrawEdge(rt,EDGE_ETCHED,BF_BOTTOM);
	}

	// draw top line
	CPen hpnMain; hpnMain.CreatePen(PS_SOLID,1,(COLORREF)GetSysColor(COLOR_3DDKSHADOW));
	CPen* pOldPen = hdc.SelectObject(&hpnMain);
	hdc.MoveTo(0,2); hdc.LineTo(nCx + 1,2);
	hdc.SelectObject(pOldPen); hpnMain.DeleteObject();

	//fntMain.CreateStockObject(DEFAULT_GUI_FONT);
	CFont* pOldFont = hdc.SelectObject(&m_fntMain);
	m_arrTabWidths.RemoveAll();
	
	CString strCaption;
	int nTabWidth = 0;
	int nNextX = m_nFirstTabOffset - m_nScrollPos;
	int nSelTabX = 0;
	int nCount = m_arrTabs.GetSize();
	for (int i = 0; i < nCount; i++)
	{
		strCaption = m_arrTabs.GetAt(i);

		// calculate tab width and add to array
		nTabWidth = hdc.GetTextExtent(strCaption).cx + m_nHeight * 2;
		m_arrTabWidths.Add(nTabWidth);

		if (i != m_nSelTab)
			HfnDrawTab(&hdc,&m_fntMain,strCaption,nNextX,nTabWidth,FALSE); // draw tab if not selected
		else
			nSelTabX = nNextX; // if selected, save X position to draw later

		nNextX += nTabWidth - m_nHeight / 2;
	}

	if (m_nSelTab > -1 && m_nSelTab < nCount)
	{
		hdc.SelectObject(&m_fntBold);

		// draw the selected tab
		HfnDrawTab(&hdc,&m_fntMain,m_arrTabs.GetAt(m_nSelTab),nSelTabX,
			(int)m_arrTabWidths.GetAt(m_nSelTab),TRUE);
	}

	hdc.SelectObject(pOldFont);

	// draw to original dc and clean up
	dc.BitBlt(0,0,nCx,nCy,&hdc,0,0,SRCCOPY);

	hdc.SelectObject(pOldBitmap);
	bmp.DeleteObject();
	hdc.DeleteDC();
}

void CTabBar::OnSize(UINT nType, int cx, int cy)
{
	CControlBar::OnSize(nType, cx, cy);
	RedrawWindow();
}

void CTabBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	int nClickedTab = -1;
	int nActualX = point.x - m_nFirstTabOffset + m_nScrollPos;
	int nNextTabX = m_nHeight / 4;

	CClientDC dc(this);

	int nCount = m_arrTabWidths.GetSize();
	for (int i = 0; i < nCount; i++)
	{
		int nOldTabX = nNextTabX;
		nNextTabX += (int)m_arrTabWidths.GetAt(i) - m_nHeight / 2;

		if (nActualX >= nOldTabX && nActualX < nNextTabX)
		{
			nClickedTab = i;
			break;
		}
	}

	SetSelTab(nClickedTab);

	CControlBar::OnLButtonDown(nFlags, point);
}

int CTabBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CClientDC dc(AfxGetMainWnd());

	// create base font structure
	LOGFONT lf; memset(&lf,0,sizeof(lf));
	lf.lfHeight = -MulDiv(8,dc.GetDeviceCaps(LOGPIXELSY),72); 
	lf.lfWeight = FW_NORMAL; 
	lf.lfCharSet = DEFAULT_CHARSET; 
	strcpy(lf.lfFaceName,"Tahoma"); 

	// create base font
	m_fntMain.CreateFontIndirect(&lf);

	// create selected font
	lf.lfWeight = FW_BOLD;
	m_fntBold.CreateFontIndirect(&lf);

	// set height
	CFont* pOldFont = dc.SelectObject(&m_fntMain);
	m_nHeight = dc.GetTextExtent("0").cy + 8;
	dc.SelectObject(pOldFont);

	return 0;
}

void CTabBar::OnDestroy()
{
	CControlBar::OnDestroy();

	m_fntMain.DeleteObject();
	m_fntBold.DeleteObject();
}
