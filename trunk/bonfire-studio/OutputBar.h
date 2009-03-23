#ifndef OUTPUTBAR_IS_DEFINED
#define OUTPUTBAR_IS_DEFINED

#if _MSC_VER > 1000
#pragma once
#endif

#include "StdAfx.h"
#include "Globals.h"
#include "TabBar.h"

class COutputBar : public SIZEBAR_BASECLASS  
{
	DECLARE_DYNAMIC(COutputBar);
private:
	CList<OutputMessage*, OutputMessage*> m_arrAppMessages;
	CList<OutputMessage*, OutputMessage*> m_arrFIFMessages;
	CList<OutputMessage*, OutputMessage*> m_arrXMLMessages;
	CList<OutputMessage*, OutputMessage*>* m_lsCurrentList;

	//CEdit			m_wndEdit;
	CListCtrl		m_wndList;
	CTabBar			m_wndTabBar;
	CImageList		m_imlListImages;
	CFont			m_fntEdit;
	
public:
	COutputBar();
	virtual ~COutputBar();
	void UpdateLast();
	void UpdateLast(OutputMessage* pMessage);
	void AddOutput(OutputMessage* pMessage);
	void ChangeView(int nView, BOOL bUpdateTabs = TRUE);
	int GetColSize(int nView, int nColumn);
	void ClearList(int nList, BOOL bSwitchTab = TRUE);
	int GetFIFCount();
	void SaveColumnSizes(int nView);

// Overrides
public:
	void OpenItem(int nIndex);
	void UpdateItem(int iIndex);
	// ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(COutputBar)
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Generated message map functions
protected:
    //{{AFX_MSG(COutputBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

#endif
