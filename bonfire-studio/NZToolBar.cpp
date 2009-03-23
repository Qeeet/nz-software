#include "stdafx.h"
#include "NZToolBar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "NZDockContext.h"
#include "NZDockBar.h"
#include "afximpl.h"

IMPLEMENT_DYNAMIC(CNZToolBar, CToolBar)

BEGIN_MESSAGE_MAP(CNZToolBar, CToolBar)
	//{{AFX_MSG_MAP(CNZToolBar)
	ON_WM_NCPAINT()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CNZToolBar::CNZToolBar()
{
}

void CNZToolBar::EnableDocking(DWORD dwDockStyle)
{
	// must be CBRS_ALIGN_XXX or CBRS_FLOAT_MULTI only
	ASSERT((dwDockStyle & ~(CBRS_ALIGN_ANY|CBRS_FLOAT_MULTI)) == 0);
	// CBRS_SIZE_DYNAMIC toolbar cannot have the CBRS_FLOAT_MULTI style
	ASSERT(((dwDockStyle & CBRS_FLOAT_MULTI) == 0) || ((m_dwStyle & CBRS_SIZE_DYNAMIC) == 0));

	m_dwDockStyle = dwDockStyle;
	if (m_pDockContext == NULL)
		m_pDockContext = new CDockContext(this);//CNZDockContext(this);

	// permanently wire the bar's owner to its current parent
	if (m_hWndOwner == NULL)
		m_hWndOwner = ::GetParent(m_hWnd);
}

void CNZToolBar::OnNcPaint() 
{
	CWindowDC dc(this);
	CRect rectClient;
	GetClientRect(rectClient);
	CRect rectWindow;
	GetWindowRect(rectWindow);
	ScreenToClient(rectWindow);
	rectClient.OffsetRect(-rectWindow.left, -rectWindow.top);
	dc.ExcludeClipRect(rectClient);

	// draw borders in non-client area
	rectWindow.OffsetRect(-rectWindow.left, -rectWindow.top);

	Draw3DBorders(&dc,rectWindow);

	dc.IntersectClipRect(rectWindow);

	#ifdef _MEMDC_H_
		// You're using Keith Rule's CMemDC. In this case
		// we have to make sure that WM_ERASEBKGND
		// will not be sent.
		dc.FillSolidRect(rectWindow, m_clrBtnFace);
	#else
		// erase parts not drawn
		SendMessage(WM_ERASEBKGND, (WPARAM)dc.m_hDC);
	#endif

	DrawGripper(&dc);
}

void CNZToolBar::Draw3DBorders(CDC* pDC, CRect& rect)
{
	DrawBorders(pDC,rect);
	return;

	ASSERT_VALID(this);
	ASSERT_VALID(pDC);

	DWORD dwStyle = m_dwStyle;
	if (!(dwStyle & CBRS_BORDER_ANY))
		return;

	// prepare for dark lines
	ASSERT(rect.top == 0 && rect.left == 0);
	CRect rect1, rect2;
	rect1 = rect;
	rect2 = rect;
	COLORREF clr = afxData.bWin4 ? afxData.clrBtnShadow : afxData.clrWindowFrame;

	// draw dark line one pixel back/up
	if (dwStyle & CBRS_BORDER_3D)
	{
		rect1.right -= CX_BORDER;
		rect1.bottom -= CY_BORDER;
	}
	if (dwStyle & CBRS_BORDER_TOP)
		rect2.top += afxData.cyBorder2;
	if (dwStyle & CBRS_BORDER_BOTTOM)
		rect2.bottom -= afxData.cyBorder2;

	CRect rect3 = rect;
	pDC->Draw3dRect(rect3,afxData.clrBtnHilite,afxData.clrBtnShadow);
	rect3.InflateRect(-1,-1);
	pDC->Draw3dRect(rect3,afxData.clrBtnFace,afxData.clrBtnFace);

	// draw left and top
	if (dwStyle & CBRS_BORDER_LEFT)
		pDC->FillSolidRect(0, rect2.top, CX_BORDER, rect2.Height(), clr);
	if (dwStyle & CBRS_BORDER_TOP)
		pDC->FillSolidRect(0, 0, rect.right, CY_BORDER, clr);

	// draw right and bottom
	if (dwStyle & CBRS_BORDER_RIGHT)
		pDC->FillSolidRect(rect1.right, rect2.top, -CX_BORDER, rect2.Height(), clr);
	if (dwStyle & CBRS_BORDER_BOTTOM)
		pDC->FillSolidRect(0, rect1.bottom, rect.right, -CY_BORDER, clr);

	if (dwStyle & CBRS_BORDER_3D)
	{
		// prepare for hilite lines
		clr = afxData.clrBtnHilite;

		// draw left and top
		if (dwStyle & CBRS_BORDER_LEFT)
			pDC->FillSolidRect(1, rect2.top, CX_BORDER, rect2.Height(), clr);
		if (dwStyle & CBRS_BORDER_TOP)
			pDC->FillSolidRect(0, 1, rect.right, CY_BORDER, clr);

		// draw right and bottom
		if (dwStyle & CBRS_BORDER_RIGHT)
			pDC->FillSolidRect(rect.right, rect2.top, -CX_BORDER, rect2.Height(), clr);
		if (dwStyle & CBRS_BORDER_BOTTOM)
			pDC->FillSolidRect(0, rect.bottom, rect.right, -CY_BORDER, clr);
	}

	rect.left += 2;
	rect.top += 2;
	rect.right -= 2;
	rect.bottom -= 2;

	if (dwStyle & CBRS_BORDER_LEFT)
		rect.left += afxData.cxBorder2 - 1;
	if (dwStyle & CBRS_BORDER_TOP)
		rect.top += afxData.cyBorder2;
	if (dwStyle & CBRS_BORDER_RIGHT)
		rect.right -= afxData.cxBorder2;
	if (dwStyle & CBRS_BORDER_BOTTOM)
		rect.bottom -= afxData.cyBorder2;
}

void CNZToolBar::DrawGripper(CDC* pDC) const
{ 
    // Do not draw a gripper if the bar is floating or not
	// dockable.
	if( (m_dwStyle & CBRS_FLOATING) || m_dwDockStyle == 0 )
		return;

	CRect gripper;
	GetWindowRect(gripper);
	ScreenToClient(gripper);
	gripper.OffsetRect(-gripper.left, -gripper.top);

	if( m_dwStyle & CBRS_ORIENT_HORZ ) {
		// gripper at left
		gripper.DeflateRect(3, 4);
		gripper.right = gripper.left+3;
        pDC->Draw3dRect(
			gripper,
			GetSysColor(COLOR_BTNHIGHLIGHT),
            GetSysColor(COLOR_BTNSHADOW)
		);
		gripper.OffsetRect(4, 0);
        pDC->Draw3dRect(
			gripper,
			GetSysColor(COLOR_BTNHIGHLIGHT),
            GetSysColor(COLOR_BTNSHADOW)
		);
	} else {
		// gripper at top
		gripper.DeflateRect(4, 3);
		gripper.bottom = gripper.top+3;
		pDC->Draw3dRect(
			gripper,
			GetSysColor(COLOR_BTNHIGHLIGHT),
            GetSysColor(COLOR_BTNSHADOW)
		);
		gripper.OffsetRect(0, 4);
        pDC->Draw3dRect(
			gripper,
			GetSysColor(COLOR_BTNHIGHLIGHT),
            GetSysColor(COLOR_BTNSHADOW)
		);
	}
}

CSize CNZToolBar::CalcDynamicLayout (int nLength, DWORD nMode)
{
	// this code will keep the toolbar the same size as it was
	// when floating even when it's docked
	/*if ((nMode & LM_VERTDOCK))
		return CToolBar::CalcDynamicLayout(nLength,
			(nMode & ~LM_VERTDOCK) | (LM_MRUWIDTH));
	else*/
		return CToolBar::CalcDynamicLayout(nLength, nMode);
}

void NZEnableDocking(CFrameWnd* pFrame, DWORD dwDockStyle)
{
	ASSERT_VALID(pFrame);
	ASSERT((dwDockStyle & ~(CBRS_ALIGN_ANY|CBRS_FLOAT_MULTI)) == 0);

	pFrame->EnableDocking(dwDockStyle);
	for (int i = 0; i < 4; i++)
	{
		if (dwDockBarMap[i][1] & dwDockStyle & CBRS_ALIGN_ANY)
		{
			CDockBar* pDock = (CDockBar*)pFrame->GetControlBar(dwDockBarMap[i][0]);
			
			// make sure the dock bar is of correct type
			if(pDock == 0 || !pDock->IsKindOf(RUNTIME_CLASS(CNZDockBar)))
			{
				BOOL bNeedDelete = !pDock->m_bAutoDelete;
				pDock->m_pDockSite->RemoveControlBar(pDock);
				pDock->m_pDockSite = 0;	// avoid problems in destroying the dockbar
				pDock->DestroyWindow();
				if (bNeedDelete)
					delete pDock;
				pDock = 0;
			}

			if (pDock == 0)
			{
				pDock = new CNZDockBar;
				if (!pDock->Create(pFrame,
					WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CHILD | WS_VISIBLE |
					dwDockBarMap[i][1], dwDockBarMap[i][0]))
						AfxThrowResourceException();
			}
		}
	}
}

void CNZToolBar::OnMouseMove(UINT nFlags, CPoint point) 
{
	POINT pt = {point.x, point.y};
	int n = GetToolBarCtrl().HitTest(&pt);
	//TRACE1("Hit Toolbar Item: %d\n",n);

	if (n < 0 || n >= GetToolBarCtrl().GetButtonCount())
	{
		GetToolBarCtrl().SetHotItem(-1);
	}
	//else
	CToolBar::OnMouseMove(nFlags, point);
}
