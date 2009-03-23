// TabBar.h: interface for the CTabBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TABBAR_H__3E8BEFD7_3712_4555_8768_170CC09BE671__INCLUDED_)
#define AFX_TABBAR_H__3E8BEFD7_3712_4555_8768_170CC09BE671__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// NMTABBAR notification message structure

#define VTN_SELCHANGING		1

struct NMTABBAR
{
	NMHDR hdr;
	int prevtab;
	int tab;
	BOOL cancel;
};

// CTabBar class

class CTabBar : public CControlBar  
{
	DECLARE_DYNAMIC(CTabBar)
public:
	CTabBar();
	virtual ~CTabBar();

// Attributes
public:
	BOOL m_bBorderParentMaximized;
	int m_nSelTab;
	int m_nFirstTabOffset;
	int m_nScrollPos;
	int m_nHeight;
	CFont m_fntMain;
	CFont m_fntBold;
	CUIntArray m_arrTabWidths;
	CStringArray m_arrTabs;

	int GetHeight() const { return m_nHeight; }

// Implementation
public:
	CString GetTab(int iIndex) const;
	int GetSelTab() const;
	int GetTabCount() const;

	void SetTab(int nIndex, CString strCaption);
	void SetSelTab(int nIndex);

	int AddTab(LPCTSTR strTab, BOOL bRedraw);
	void RemoveTab(int nIndex);
	void ClearTabs(BOOL bRedraw);

	void HfnDrawTab(CDC* pDC, CFont* pFont, LPCTSTR strCaption,
		int x, int cx, BOOL bSelected = FALSE);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabBar)
	public:
	virtual BOOL Create(CWnd* pParentWnd, int nHeight, DWORD dwStyle, UINT nID);
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	virtual CSize CalcDynamicLayout(int nLength, DWORD dwMode);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	//}}AFX_VIRTUAL

// Generated message map functions
protected:
	//{{AFX_MSG(CTabBar)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_TABBAR_H__3E8BEFD7_3712_4555_8768_170CC09BE671__INCLUDED_)
