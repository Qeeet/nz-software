// BrowserView.cpp : implementation file
//

#include "stdafx.h"
#include "Bonfire.h"
#include "BonfireDoc.h"
#include "BonfireView.h"
#include "BrowserView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CBonfireApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CBrowserView

IMPLEMENT_DYNCREATE(CBrowserView, CHtmlView)

CBrowserView::CBrowserView()
{
	//{{AFX_DATA_INIT(CBrowserView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CBrowserView::~CBrowserView()
{
}

void CBrowserView::DoDataExchange(CDataExchange* pDX)
{
	CHtmlView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBrowserView)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBrowserView, CHtmlView)
	//{{AFX_MSG_MAP(CBrowserView)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBrowserView diagnostics

#ifdef _DEBUG
void CBrowserView::AssertValid() const
{
	CHtmlView::AssertValid();
}

void CBrowserView::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBrowserView message handlers

BOOL CBrowserView::RefreshView()
{
	CBonfireDoc* pDoc = (CBonfireDoc*)GetDocument();
	CString docPath = pDoc->GetPathName();
	if (!pDoc->IsModified())
	{
		// display the file in the browser viewer
		Navigate(docPath);
	}
	else
	{
		CString	strAbsPath	= theApp.m_pszHelpFilePath;
		DWORD	nSize		= 128;
		char*	lpValue		= (char*)malloc( nSize );
		int		retVal		= GetEnvironmentVariable("TEMP", lpValue, nSize);
		if (retVal <= (int)nSize)
			strAbsPath = lpValue;
		else if (retVal == 0)
		{
			// try TMP
			retVal			= GetEnvironmentVariable("TMP", lpValue, nSize);
			if (retVal <= (int)nSize)
				strAbsPath = lpValue;
		}
		// generate a temp file
		CString navPath = GetFilePath( AddSlash(strAbsPath) ) + "btmp_" + GetFileTitle( docPath );
		pDoc->m_xTextBuffer.SaveToFile(navPath, CRLF_STYLE_AUTOMATIC, FALSE);

		// mark the file as temporary for later deletion
		theApp.AddTempFile( navPath );

		// display the file in the browser viewer
		Navigate(navPath);
	}

	return TRUE;
}

CString CBrowserView::GetViewName()
{
	return "Browser Preview";
}

int CBrowserView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CHtmlView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}
