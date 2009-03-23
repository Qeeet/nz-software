#ifndef PROPBAR_IS_DEFINED
#define PROPBAR_IS_DEFINED

#if _MSC_VER > 1000
#pragma once
#endif

#include "Downloaded Components\SizingCBar\SizeCBar.h"
#include "Globals.h"

class CPropBar : public SIZEBAR_BASECLASS  
{
	DECLARE_DYNAMIC(CPropBar);
private:
	CListCtrl		m_wndList;

public:
	CPropBar();
	virtual ~CPropBar();

// Overrides
public:
	// ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CPropBar)
	//}}AFX_VIRTUAL

// Generated message map functions
protected:
    //{{AFX_MSG(CPropBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	//}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

#endif
