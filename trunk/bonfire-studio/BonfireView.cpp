// BonfireView.cpp: implementation of the CBonfireView class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Bonfire.h"
#include "BonfireView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CBonfireApp theApp;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBonfireView::CBonfireView(CRuntimeClass* pClass, CString strCaption, BOOL bAllowsSplitter, BOOL bViewOnly)
{
	this->m_bModifiedSinceRefresh	= TRUE;
	this->m_bAllowsSplitter			= bAllowsSplitter;
	this->m_bViewOnly				= bViewOnly;
	this->m_strCaption				= strCaption;
	this->m_pClass					= pClass;
	this->m_pView					= NULL;
}

CBonfireView::CBonfireView(CView* pView, CString strCaption, BOOL bAllowsSplitter, BOOL bViewOnly)
{
	this->m_bModifiedSinceRefresh	= TRUE;
	this->m_bAllowsSplitter			= bAllowsSplitter;
	this->m_bViewOnly				= bViewOnly;
	this->m_strCaption				= strCaption;
	this->m_pClass					= pView->GetRuntimeClass();
	this->m_pView					= pView;
}

CBonfireView::CBonfireView(CBonfireView* pView)
{
	this->m_bModifiedSinceRefresh	= TRUE;
	this->m_bAllowsSplitter			= pView->m_bAllowsSplitter;
	this->m_bViewOnly				= pView->m_bViewOnly;
	this->m_strCaption				= pView->m_strCaption;
	this->m_pView					= pView->m_pView;
	this->m_pClass					= (pView->m_pView) ? pView->m_pView->GetRuntimeClass() : NULL;
}

CBonfireView& CBonfireView::operator=(const CBonfireView& _src)
{
	this->m_bModifiedSinceRefresh	= _src.m_bModifiedSinceRefresh;
	this->m_bAllowsSplitter			= _src.m_bAllowsSplitter;
	this->m_bViewOnly				= _src.m_bViewOnly;
	this->m_strCaption				= _src.m_strCaption;
	this->m_pClass					= _src.m_pClass;
	this->m_pView					= _src.m_pView;
	return *this;
}

CBonfireView::~CBonfireView()
{

}

/////////////////////////////////////////////////////////////////////////////
// CBonfireView message handlers

void CBonfireView::RefreshView()
{
	return;
}

CString CBonfireView::GetViewName()
{
	return "Default View";
}

void CBonfireView::CopyFrom(CBonfireView *pViewFrom)
{
	this->m_bModifiedSinceRefresh	= pViewFrom->m_bModifiedSinceRefresh;
	this->m_bAllowsSplitter			= pViewFrom->m_bAllowsSplitter;
	this->m_bViewOnly				= pViewFrom->m_bViewOnly;
	this->m_strCaption				= pViewFrom->m_strCaption;
}
