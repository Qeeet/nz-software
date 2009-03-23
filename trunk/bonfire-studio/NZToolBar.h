#ifndef __NZTOOLBAR_H_
#define __NZTOOLBAR_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include <afxext.h>

class CNZToolBar : public CToolBar
{
	DECLARE_DYNAMIC(CNZToolBar)

public:
	CNZToolBar();

	void EnableDocking(DWORD dwDockStyle);
	void Draw3DBorders(CDC* pDC, CRect& rect);
	virtual void DrawGripper(CDC *pDC) const;
	virtual CSize CalcDynamicLayout (int nLength, DWORD nMode);

protected:
	//{{AFX_MSG(CNZToolBar)
	afx_msg void OnNcPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

const DWORD dwDockBarMap[4][2] =
{
	{ AFX_IDW_DOCKBAR_TOP,      CBRS_TOP    },
	{ AFX_IDW_DOCKBAR_BOTTOM,   CBRS_BOTTOM },
	{ AFX_IDW_DOCKBAR_LEFT,     CBRS_LEFT   },
	{ AFX_IDW_DOCKBAR_RIGHT,    CBRS_RIGHT  },
};

void NZEnableDocking(CFrameWnd* pFrame, DWORD dwDockStyle);

#endif
