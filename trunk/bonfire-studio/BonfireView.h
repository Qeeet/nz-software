// BonfireView.h: interface for the CBonfireView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BONFIREVIEW_H__BD39FED8_4025_4ED0_B079_E3A22CE7D733__INCLUDED_)
#define AFX_BONFIREVIEW_H__BD39FED8_4025_4ED0_B079_E3A22CE7D733__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "BonfireDoc.h"
class BonfireDoc;

class CBonfireView  
{
public:
	CBonfireView(CRuntimeClass* pClass, CString strCaption, BOOL bAllowsSplitter, BOOL bViewOnly);
	CBonfireView(CView* pView, CString strCaption, BOOL bAllowsSplitter, BOOL bViewOnly);
	CBonfireView(CBonfireView*);
	CBonfireView& operator=(const CBonfireView& _src);
	virtual ~CBonfireView();

// Attributes
public:
	BOOL			m_bModifiedSinceRefresh;
	BOOL			m_bAllowsSplitter;
	BOOL			m_bViewOnly;
	CString			m_strCaption;
	CView*			m_pView;
	CRuntimeClass*	m_pClass;

// Operations
public:
	void CopyFrom(CBonfireView* pViewFrom);
	virtual void RefreshView();
	virtual CString GetViewName();
};

#endif // !defined(AFX_BONFIREVIEW_H__BD39FED8_4025_4ED0_B079_E3A22CE7D733__INCLUDED_)
