// SourceView.h : interface of the CSourceView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOURCEVIEW_H__14155E94_15C5_43BE_BAB3_029AD3A44F3E__INCLUDED_)
#define AFX_SOURCEVIEW_H__14155E94_15C5_43BE_BAB3_029AD3A44F3E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Downloaded Components\CrystalEdit\CCrystalEditView.h"
#include "BonfireDoc.h"

class CSourceView : public CCrystalEditView
{
protected: // create from serialization only
	CSourceView();
	DECLARE_DYNCREATE(CSourceView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSourceView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SelectText(CPoint pt1, CPoint pt2);
	CString GetViewName();
	LPCTSTR GetSourceText();
	CCrystalTextBuffer* LocateTextBuffer()
	{ return &((CBonfireDoc*)GetDocument())->m_xTextBuffer; }
	DWORD ParseLine(DWORD dwCookie, int nLineIndex, TEXTBLOCK *pBuf, int &nActualItems);
	virtual void UpdateView(CCrystalTextView *pSource, CUpdateContext *pContext, DWORD dwFlags, int nLineIndex = -1);

	virtual ~CSourceView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	void UpdateFont();
	//{{AFX_MSG(CSourceView)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg LRESULT OnFileWatchNotification(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOURCEVIEW_H__14155E94_15C5_43BE_BAB3_029AD3A44F3E__INCLUDED_)
