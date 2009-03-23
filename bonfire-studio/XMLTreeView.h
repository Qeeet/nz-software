#if !defined(AFX_XMLTREEVIEW_H__D51FBF0E_3BA6_4C46_ACDC_C0765A9E1E1D__INCLUDED_)
#define AFX_XMLTREEVIEW_H__D51FBF0E_3BA6_4C46_ACDC_C0765A9E1E1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// XMLTreeView.h : header file
//
#include "StdAfx.h"

#include <afxcview.h>

/////////////////////////////////////////////////////////////////////////////
// CXMLTreeView view

class CXMLTreeView : public CTreeView
{
// Attributes
protected:
	IXMLDOMDocument*	m_pXMLDoc;
	BOOL				m_bLinkToXML;
	short				m_editType;
	BOOL				m_bRedraw;
	CTreeCtrl*			m_pTree;
public:
	CFont			m_fntTree;
	CImageList		m_imlTreeImages;

// Operations
public:
	BOOL EditCurrentNode();
	void ProcessAttributes(HTREEITEM tnParent, IXMLDOMNode* pXMLNode);
	CString XMLTypeToString(IXMLDOMNode *pXMLNode);
	CString GetNodeText(IXMLDOMNode* pXMLNode);
	BOOL UpdateNodeText(IXMLDOMNode *pXMLNode, HTREEITEM pTreeNode, CString strNewText, OutputMessage* pMsg);
	CString GetViewName();
	BOOL RefreshXMLViewRec(IXMLDOMNode* pXMLNode, HTREEITEM tnParentTreeNode);
	BOOL RefreshView();

// printing
private:
	CRect rcBounds;
	int m_nCharWidth;
	int m_nRowHeight;
	int m_nRowsPerPage;
	HANDLE hDIB;
	WINDOWPLACEMENT WndPlace;

// Operations
public:
	void PrintHeadFoot(CDC *pDC, CPrintInfo *pInfo);
	HANDLE DDBToDIB( CBitmap& bitmap, DWORD dwCompression, CPalette* pPal );
// end printing

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXMLTreeView)
	public:
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	protected:
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	CXMLTreeView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CXMLTreeView)
	virtual ~CXMLTreeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	void ExpandChildren(HTREEITEM parent, BOOL bExpand);
	void SetRedrawFlag(BOOL bRedraw);
	void UpdateFont();
	//{{AFX_MSG(CXMLTreeView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnBeginEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnAttributeEditName();
	afx_msg void OnAttributeEditValue();
	afx_msg void OnCommentEdit();
	afx_msg void OnElementEditName();
	afx_msg void OnUpdateElementEditName(CCmdUI* pCmdUI);
	afx_msg void OnElementInsertAttribute();
	afx_msg void OnElementInsertComment();
	afx_msg void OnElementInsertElement();
	afx_msg void OnElementInsertText();
	afx_msg void OnUpdateAttributeEditName(CCmdUI* pCmdUI);
	afx_msg void OnTextNodeEditText();
	afx_msg void OnElementExpandCollapse();
	afx_msg void OnElementExpandAll();
	afx_msg void OnElementCollapseAll();
	afx_msg void OnFilePrint();
	afx_msg void OnFilePrintPreview();
	afx_msg void OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemExpanded(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XMLTREEVIEW_H__D51FBF0E_3BA6_4C46_ACDC_C0765A9E1E1D__INCLUDED_)
