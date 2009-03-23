#if !defined(AFX_NZPROJECTTREECTRL_H__A9C739CE_B512_426C_A973_3DD362C0C5E4__INCLUDED_)
#define AFX_NZPROJECTTREECTRL_H__A9C739CE_B512_426C_A973_3DD362C0C5E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NZProjectTreeCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNZProjectTreeCtrl window

class CProjectBar;
class CXNode;
class CXFolder;

class CNZProjectTreeCtrl : public CTreeCtrl
{
// Construction
public:
	CNZProjectTreeCtrl();

// Attributes
public:
	BOOL			m_bDragging;
	CImageList*		m_pDragImage;
protected:
	HTREEITEM		m_htiDrag;
	HTREEITEM		m_htiDrop;
	HTREEITEM		m_htiOldDrop;
	UINT			m_nExpandTimer;
	UINT			m_nScrollTimer;
	UINT			m_timerticks;
	BOOL			m_bRedraw;

// Operations
public:
	void MoveNode(CXNode* pDragNode, CXFolder* pNewParent);
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNZProjectTreeCtrl)
	public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNZProjectTreeCtrl();
	void SetRedrawFlag(BOOL bRedraw);
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CNZProjectTreeCtrl)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnItemExpanded(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NZPROJECTTREECTRL_H__A9C739CE_B512_426C_A973_3DD362C0C5E4__INCLUDED_)
