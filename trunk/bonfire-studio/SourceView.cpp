// SourceView.cpp : implementation of the CSourceView class
//

#include "stdafx.h"
#include "Bonfire.h"

#include "MainFrm.h"
#include "BonfireDoc.h"
#include "BonfireView.h"

#include "SourceView.h"
#include "SyntaxParsers.h"
#include "ChildFrm.h"
#include "BCMenu.h"
#include "FileWatch.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CMainFrame* g_pMainFrame;
extern CBonfireApp theApp;
extern UINT g_msgFileWatchNotify;

/////////////////////////////////////////////////////////////////////////////
// CSourceView

IMPLEMENT_DYNCREATE(CSourceView, CCrystalEditView)

BEGIN_MESSAGE_MAP(CSourceView, CCrystalEditView)
	//{{AFX_MSG_MAP(CSourceView)
	ON_WM_CONTEXTMENU()
	ON_REGISTERED_MESSAGE(g_msgFileWatchNotify, OnFileWatchNotification)
	ON_WM_MOUSEWHEEL()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CCrystalEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CCrystalEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CCrystalEditView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSourceView construction/destruction

CSourceView::CSourceView()
{
	// TODO: add construction code here
}

CSourceView::~CSourceView()
{
}

BOOL CSourceView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CCrystalEditView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSourceView printing

BOOL CSourceView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	//return DoPreparePrinting(pInfo);
	return CCrystalEditView::OnPreparePrinting(pInfo);
}

void CSourceView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: add extra initialization before printing
	CCrystalEditView::OnBeginPrinting(pDC, pInfo);
}

void CSourceView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: add cleanup after printing
	CCrystalEditView::OnEndPrinting(pDC, pInfo);
}

/////////////////////////////////////////////////////////////////////////////
// CSourceView diagnostics

#ifdef _DEBUG
void CSourceView::AssertValid() const
{
	CCrystalEditView::AssertValid();
}

void CSourceView::Dump(CDumpContext& dc) const
{
	CCrystalEditView::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSourceView message handlers

void CSourceView::OnInitialUpdate() 
{
	CCrystalEditView::OnInitialUpdate();

	// check whether to make read-only or not based on file extension
	size_t nExt = theApp.m_opOptions.views.vAssociations.size();
	CString strExtList, strExt = GetFileExtension(GetDocument()->GetPathName());
	while (nExt-- > 0) // 0 is the default file extension
	{
		strExtList = theApp.m_opOptions.views.vAssociations[nExt].strExtensions;
		strExtList.Remove(' ');

		if (IsStringPresent(strExtList,strExt))
		{
			if (theApp.m_opOptions.views.vAssociations[nExt].strViews[0] == '-')
			{
				((CBonfireDoc*)GetDocument())->m_xTextBuffer.SetReadOnly(TRUE);
				break;
			}
		}
	}

	UpdateFont();
}

LPCTSTR CSourceView::GetSourceText()
{
	return NULL;//CCrystalEditView::m_pTextBuffer->GetText();
}

void CSourceView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	UpdateFont();
	((CBonfireDoc*)GetDocument())->m_fnParse = LookupParser(
		(GetDocument()->GetPathName() == "") ? NULL :
		(LPCTSTR)GetFileExtension(GetDocument()->GetPathName()));
	RedrawWindow();
	AfxGetMainWnd()->DrawMenuBar(); // if child frame is maximized
}

void CSourceView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	BCMenu popmenu;

	popmenu.LoadMenu(IDR_POPUP_EDITOR);

	// set the icons for the menu and show the popup
	g_pMainFrame->SetMenuIcons(&popmenu);
	ShowPopup(AfxGetMainWnd(),(BCMenu*)popmenu.GetSubMenu(0));

	// clean up
	popmenu.DestroyMenu();
}

DWORD CSourceView::ParseLine(DWORD dwCookie, int nLineIndex, TEXTBLOCK *pBuf, int &nActualItems)
{
	ParserFuncPtr fnParse = ((CBonfireDoc*)GetDocument())->m_fnParse;
	if (fnParse)
		return (*fnParse)(this, dwCookie, nLineIndex, pBuf, nActualItems);

	return 0;
}

CString CSourceView::GetViewName()
{
	return "Source View";
}

LRESULT CSourceView::OnFileWatchNotification(WPARAM wParam, LPARAM lParam)
{
	CString sPathName = CString((LPCTSTR)lParam);
	DWORD dwData = (DWORD)wParam;

	((CBonfireDoc*)GetDocument())->OnFileReload();

	return 0;
}

// set the current selection
void CSourceView::SelectText(CPoint pt1, CPoint pt2)
{
	SetSelection(pt1, pt2);
}

BOOL CSourceView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	UINT uiNumScrollLines;
	if (SystemParametersInfo(SPI_GETWHEELSCROLLLINES,0,&uiNumScrollLines,0))
	{
		int nNewPos = GetScrollPos(SB_VERT) - (zDelta / WHEEL_DELTA) * uiNumScrollLines;
		SetScrollPos(SB_VERT,nNewPos);
		SendMessage(WM_VSCROLL,MAKEWPARAM(SB_THUMBPOSITION,nNewPos),NULL);
		return TRUE;
	}

	return CCrystalEditView::OnMouseWheel(nFlags, zDelta, pt);
}

void CSourceView::OnKillFocus(CWnd* pNewWnd) 
{
	CCrystalEditView::OnKillFocus(pNewWnd);

	//TRACE0("CSourceView::OnKillFocus\n");
}

void CSourceView::OnSetFocus(CWnd* pOldWnd) 
{
	CCrystalEditView::OnSetFocus(pOldWnd);
	
	//TRACE0("CSourceView::OnSetFocus\n");
}

int CSourceView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CCrystalEditView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// add this view to the document's map
	for (int i = 0; i < theApp.m_arAllViews.GetSize(); i++)
	{
		CBonfireView* pMapView = (CBonfireView*)theApp.m_arAllViews[i];
		if ( this->GetRuntimeClass() == pMapView->m_pClass )
		{
			CBonfireView* pNewView	= new CBonfireView(pMapView);
			pNewView->m_pView		= this;
			pNewView->m_pClass		= pMapView->m_pClass;

			((CBonfireDoc*)GetDocument())->AddViewToMap(pNewView);
			break;
		}
	}

	return 0;
}

void CSourceView::UpdateFont()
{
	CBonfireApp* pApp = (CBonfireApp*)AfxGetApp();
	LOGFONT lf;
	memset(&lf,0,sizeof(lf));
	lf.lfWeight = FW_NORMAL;
	lf.lfCharSet = ANSI_CHARSET;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = FIXED_PITCH | FF_DONTCARE;

	CClientDC tdc(AfxGetMainWnd());
	lf.lfHeight = -MulDiv(pApp->m_opOptions.editor.dwFontSize,GetDeviceCaps(tdc,LOGPIXELSY),72);
	strcpy(lf.lfFaceName, pApp->m_opOptions.editor.strFont);
	SetFont( lf );
}

void CSourceView::UpdateView(CCrystalTextView *pSource, CUpdateContext *pContext, DWORD dwFlags, int nLineIndex)
{
	CCrystalEditView::UpdateView(pSource, pContext, dwFlags, nLineIndex);

	((CBonfireDoc*)GetDocument())->SetViewRefreshFlag();
}