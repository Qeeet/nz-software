// OptionsPages.cpp : implementation file
//

#include "stdafx.h"
#include "Bonfire.h"
#include "MainFrm.h"
#include "OptionsDlg.h"
#include "OptionsPages.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CMainFrame* g_pMainFrame;
extern CBonfireApp theApp;

int CALLBACK EnumFontFamExProc(ENUMLOGFONTEX *lpelfe,
							   NEWTEXTMETRICEX *lpntme,
							   DWORD FontType, LPARAM lParam)
{
	EnumFontData* pParam = (EnumFontData*)lParam;

	BOOL bFirstCheck	= (!pParam->bFixed || (pParam->bFixed && lpelfe->elfLogFont.lfPitchAndFamily & FIXED_PITCH));
	BOOL bSecondCheck	= (lpelfe->elfLogFont.lfCharSet == ANSI_CHARSET ||
							lpelfe->elfLogFont.lfCharSet == OEM_CHARSET);
	// only fixed pitch
	if (bFirstCheck && bSecondCheck)
		pParam->pCombo->AddString(lpelfe->elfLogFont.lfFaceName);
	return 1;
}

/////////////////////////////////////////////////////////////////////////////
// CGeneralPageDlg dialog
/////////////////////////////////////////////////////////////////////////////

CGeneralPageDlg::CGeneralPageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGeneralPageDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGeneralPageDlg)
	m_bClearOutput		= theApp.m_opOptions.general.dwClearOutput;
	m_bReloadModified	= theApp.m_opOptions.general.dwReloadFiles;
	m_bSaveOnSwitch		= theApp.m_opOptions.general.dwSaveOnSwitch;
	//}}AFX_DATA_INIT
}


void CGeneralPageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGeneralPageDlg)
	DDX_Check(pDX, IDC_CHK_CLEAR_OUTPUT, m_bClearOutput);
	DDX_Check(pDX, IDC_CHK_RELOAD_MODIFIED, m_bReloadModified);
	DDX_Check(pDX, IDC_CHK_SAVE_ON_SWITCH, m_bSaveOnSwitch);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGeneralPageDlg, CDialog)
	//{{AFX_MSG_MAP(CGeneralPageDlg)
	ON_MESSAGE(WM_DOAPPLY, OnApply)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGeneralPageDlg message handlers

BOOL CGeneralPageDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LONG CGeneralPageDlg::OnApply(UINT, LONG)
{
	if (!UpdateData())
		return FALSE;

	// save changes to application variables
	theApp.m_opOptions.general.dwClearOutput = m_bClearOutput;
	theApp.m_opOptions.general.dwReloadFiles = m_bReloadModified;
	theApp.m_opOptions.general.dwSaveOnSwitch = m_bSaveOnSwitch;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CEditorPageDlg dialog
/////////////////////////////////////////////////////////////////////////////

CEditorPageDlg::CEditorPageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditorPageDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditorPageDlg)
	m_nFontSize = theApp.m_opOptions.editor.dwFontSize;
	//}}AFX_DATA_INIT
}


void CEditorPageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditorPageDlg)
	DDX_Control(pDX, IDC_CBO_FONT, m_wndFontCombo);
	DDX_Control(pDX, IDC_LIST_SYNTAX, m_wndListSyntax);
	DDX_Control(pDX, IDC_COLOR_CHOOSE, m_wndColorBtn);
	DDX_Text(pDX, IDC_FONT_SIZE, m_nFontSize);
	DDV_MinMaxInt(pDX, m_nFontSize, 5, 120);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditorPageDlg, CDialog)
	//{{AFX_MSG_MAP(CEditorPageDlg)
	ON_WM_DRAWITEM()
	ON_LBN_SELCHANGE(IDC_LIST_SYNTAX, OnSelChangeListSyntax)
	ON_MESSAGE(CPN_SELENDOK, OnColorSelEndOK)
	ON_MESSAGE(WM_DOAPPLY, OnApply)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditorPageDlg message handlers

BOOL CEditorPageDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// copy settings from global app settings
	m_edOptions = theApp.m_opOptions.editor;
	
	m_wndListSyntax.SetItemData(m_wndListSyntax.AddString("Background"),
		APP_COLOR_BACKGROUND);
	m_wndListSyntax.SetItemData(m_wndListSyntax.AddString("Text"),
		APP_COLOR_TEXT);
	m_wndListSyntax.SetItemData(m_wndListSyntax.AddString("Selection Bk"),
		APP_COLOR_SEL_BACKGROUND);
	m_wndListSyntax.SetItemData(m_wndListSyntax.AddString("Selection Text"),
		APP_COLOR_SEL_TEXT);
	m_wndListSyntax.SetItemData(m_wndListSyntax.AddString("Element"),
		APP_COLOR_ELEMENT);
	m_wndListSyntax.SetItemData(m_wndListSyntax.AddString("Element Name"),
		APP_COLOR_EL_NAME);
	m_wndListSyntax.SetItemData(m_wndListSyntax.AddString("Element Attr."),
		APP_COLOR_EL_ATTRIB);
	m_wndListSyntax.SetItemData(m_wndListSyntax.AddString("String"),
		APP_COLOR_STRING);
	m_wndListSyntax.SetItemData(m_wndListSyntax.AddString("Comment"),
		APP_COLOR_COMMENT);
	m_wndListSyntax.SetItemData(m_wndListSyntax.AddString("Preprocessor"),
		APP_COLOR_PREPROCESSOR);
	m_wndListSyntax.SetItemData(m_wndListSyntax.AddString("Keyword"),
		APP_COLOR_KEYWORD);
	m_wndListSyntax.SetItemData(m_wndListSyntax.AddString("Number"),
		APP_COLOR_NUMBER);
	m_wndListSyntax.SetItemData(m_wndListSyntax.AddString("ASP"),
		APP_COLOR_ASP);

	//m_wndListSyntax.SetCurSel(0);
	OnSelChangeListSyntax();

	CClientDC dc(this);

	// enumerate all fonts
	LOGFONT lf; memset(&lf,0,sizeof(lf));
	lf.lfCharSet = DEFAULT_CHARSET;
	EnumFontData pParam; pParam.pCombo = &m_wndFontCombo; pParam.bFixed = TRUE;
	EnumFontFamiliesEx(dc,&lf,(FONTENUMPROC)EnumFontFamExProc,(LPARAM)&pParam,0);

	// set font
	m_wndFontCombo.SelectString(-1,m_edOptions.strFont);
	m_nFontSize = m_edOptions.dwFontSize;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


LONG CEditorPageDlg::OnApply(UINT, LONG)
{
	if (!UpdateData())
		return FALSE;

	// save changes to application variables
	CString str;
	m_wndFontCombo.GetLBText(m_wndFontCombo.GetCurSel(),str);

	theApp.m_opOptions.editor = m_edOptions;
	strcpy(theApp.m_opOptions.editor.strFont,str);

	theApp.m_opOptions.editor.dwFontSize = m_nFontSize;

	return TRUE;
}

void CEditorPageDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	if (nIDCtl == IDC_LIST_SYNTAX)
	{
		CRect rc = lpDrawItemStruct->rcItem;
		HDC hdc = lpDrawItemStruct->hDC;

		BOOL bSelected = lpDrawItemStruct->itemState & ODS_SELECTED;

		// draw background
		HGDIOBJ oldpen = SelectObject(hdc,CreatePen(PS_NULL,0,0));
		HGDIOBJ oldbrush = SelectObject(hdc,GetSysColorBrush(
			bSelected ? COLOR_HIGHLIGHT : COLOR_WINDOW));

		Rectangle(hdc,rc.left + rc.Height() + 3,rc.top,rc.right + 1,rc.bottom + 1);

		DeleteObject(SelectObject(hdc,oldpen));
		SelectObject(hdc,oldbrush);

		// draw color
		oldpen = SelectObject(hdc,CreatePen(PS_SOLID,1,0));
		oldbrush = SelectObject(hdc,CreateSolidBrush(
			theApp.GetAppColor(lpDrawItemStruct->itemData,&m_edOptions)));

		Rectangle(hdc,rc.left + 2,rc.top + 1,rc.left + rc.Height() + 1, rc.bottom - 1);

		DeleteObject(SelectObject(hdc,oldpen));
		DeleteObject(SelectObject(hdc,oldbrush));

		// draw text
		rc.left += rc.Height() + 5;
		CString strText;
		m_wndListSyntax.GetText(lpDrawItemStruct->itemID,strText);

		SetBkMode(hdc,TRANSPARENT);
		SetTextColor(hdc,GetSysColor(bSelected ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT));
		DrawText(hdc,strText,-1,rc,DT_SINGLELINE | DT_VCENTER);
	}
	else
		CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CEditorPageDlg::OnSelChangeListSyntax() 
{
	int n = m_wndListSyntax.GetItemData(m_wndListSyntax.GetCurSel());

	if (n < 0)
		m_wndColorBtn.EnableWindow(FALSE);
	else
	{
		m_wndColorBtn.EnableWindow(TRUE);

		// get the color (don't get the default color, only if it is indeed default)
		COLORREF clr = theApp.GetAppColor(n,&m_edOptions,FALSE);

		// first create a dummy EDITOR_OPTIONS structure, set all of the values to
		// default and then get the default value using GetAppColor
		EDITOR_OPTIONS temp; memset(&temp,255,sizeof(temp));
		m_wndColorBtn.SetDefaultColor(theApp.GetAppColor(n,&temp,TRUE));

		if (clr == 0xFFFFFFFF) // default
			m_wndColorBtn.SetColor(CLR_DEFAULT); // default color constant for the button class
		else
			m_wndColorBtn.SetColor(clr);
	}
}

LONG CEditorPageDlg::OnColorSelEndOK(UINT, LONG)
{
	int n = m_wndListSyntax.GetItemData(m_wndListSyntax.GetCurSel());
	COLORREF clr = m_wndColorBtn.GetColor();

	if (clr == CLR_DEFAULT)
		theApp.SetAppColor(n,0xFFFFFFFF,&m_edOptions);
	else
		// note: switch red and blue
		theApp.SetAppColor(n,
			RGB(GetBValue(clr),GetGValue(clr),GetRValue(clr)),&m_edOptions);

	m_wndListSyntax.RedrawWindow();

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// COutputPageDlg dialog
/////////////////////////////////////////////////////////////////////////////

COutputPageDlg::COutputPageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COutputPageDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COutputPageDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void COutputPageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COutputPageDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COutputPageDlg, CDialog)
	//{{AFX_MSG_MAP(COutputPageDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutputPageDlg message handlers

BOOL COutputPageDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



/////////////////////////////////////////////////////////////////////////////
// CProjectMgrPageDlg dialog
/////////////////////////////////////////////////////////////////////////////

CProjectMgrPageDlg::CProjectMgrPageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProjectMgrPageDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProjectMgrPageDlg)
	m_bDispFullPathFiles	= theApp.m_opOptions.projbar.dwShowPathFiles;
	m_bDispFullPathFolders	= theApp.m_opOptions.projbar.dwShowPathFolders;
	m_bDispFullPathProjects	= theApp.m_opOptions.projbar.dwShowPathProjects;
	m_bFoldersFirst			= theApp.m_opOptions.projbar.dwShowFoldersBeforeFiles;
	//}}AFX_DATA_INIT
}


void CProjectMgrPageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProjectMgrPageDlg)
	DDX_Check(pDX, IDC_CHK_DFP_FILES, m_bDispFullPathFiles);
	DDX_Check(pDX, IDC_CHK_DFP_FOLDERS, m_bDispFullPathFolders);
	DDX_Check(pDX, IDC_CHK_DFP_PROJECTS, m_bDispFullPathProjects);
	DDX_Check(pDX, IDC_CHK_FOLDERS_FIRST, m_bFoldersFirst);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProjectMgrPageDlg, CDialog)
	//{{AFX_MSG_MAP(CProjectMgrPageDlg)
	ON_MESSAGE(WM_DOAPPLY, OnApply)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProjectMgrPageDlg message handlers

BOOL CProjectMgrPageDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LONG CProjectMgrPageDlg::OnApply(UINT, LONG)
{
	if (!UpdateData())
		return FALSE;

	// save changes to application variables
	theApp.m_opOptions.projbar.dwShowPathProjects = m_bDispFullPathProjects;
	theApp.m_opOptions.projbar.dwShowPathFolders = m_bDispFullPathFolders;
	theApp.m_opOptions.projbar.dwShowPathFiles = m_bDispFullPathFiles;
	theApp.m_opOptions.projbar.dwShowFoldersBeforeFiles = m_bFoldersFirst;

	g_pMainFrame->m_wndProjectBar.RefreshProjectManagerTree();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CXMLTreePageDlg dialog
/////////////////////////////////////////////////////////////////////////////

CXMLTreePageDlg::CXMLTreePageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CXMLTreePageDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CXMLTreePageDlg)
	m_bShowAttributes	= theApp.m_opOptions.xmltree.dwShowAttributes;
	m_bShowComments		= theApp.m_opOptions.xmltree.dwShowComments;
	m_bShowText			= theApp.m_opOptions.xmltree.dwShowText;
	m_nFontSize			= theApp.m_opOptions.xmltree.dwFontSize;
	m_bEditXMLTree		= theApp.m_opOptions.xmltree.dwEditXMLTree;
	//}}AFX_DATA_INIT
}


void CXMLTreePageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXMLTreePageDlg)
	DDX_Control(pDX, IDC_OPTIONS_XMLTREE_FONT, m_wndFontCombo);
	DDX_Check(pDX, IDC_CHK_SHOW_XML_ATTRIBUTES, m_bShowAttributes);
	DDX_Check(pDX, IDC_CHK_SHOW_XML_COMMENTS, m_bShowComments);
	DDX_Check(pDX, IDC_CHK_SHOW_XML_TEXT, m_bShowText);
	DDX_Text(pDX, IDC_OPTIONS_XMLTREE_FONTSIZE, m_nFontSize);
	DDV_MinMaxInt(pDX, m_nFontSize, 5, 120);
	DDX_Check(pDX, IDC_CHK_EDIT_XMLTREE, m_bEditXMLTree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CXMLTreePageDlg, CDialog)
	//{{AFX_MSG_MAP(CXMLTreePageDlg)
	ON_MESSAGE(WM_DOAPPLY, OnApply)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXMLTreePageDlg message handlers

BOOL CXMLTreePageDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// copy settings from global app settings
	m_edOptions = theApp.m_opOptions.xmltree;
	
	CClientDC dc(this);

	// enumerate all fonts
	LOGFONT lf; memset(&lf,0,sizeof(lf));
	lf.lfCharSet = DEFAULT_CHARSET;
	EnumFontData pParam; pParam.pCombo = &m_wndFontCombo; pParam.bFixed = FALSE;
	EnumFontFamiliesEx(dc,&lf,(FONTENUMPROC)EnumFontFamExProc,(LPARAM)&pParam,0);

	// set font
	m_wndFontCombo.SelectString(-1,m_edOptions.strFont);
	m_nFontSize = m_edOptions.dwFontSize;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LONG CXMLTreePageDlg::OnApply(UINT, LONG)
{
	if (!UpdateData())
		return FALSE;

	// save changes to application variables
	theApp.m_opOptions.xmltree.dwShowAttributes	= m_bShowAttributes;
	theApp.m_opOptions.xmltree.dwShowText		= m_bShowText;
	theApp.m_opOptions.xmltree.dwShowComments	= m_bShowComments;
	theApp.m_opOptions.xmltree.dwEditXMLTree	= m_bEditXMLTree;

	// save changes to application variables
	CString str;
	m_wndFontCombo.GetLBText(m_wndFontCombo.GetCurSel(),str);
	strcpy(theApp.m_opOptions.xmltree.strFont,str);
	theApp.m_opOptions.xmltree.dwFontSize = m_nFontSize;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CViewsPageDlg dialog
/////////////////////////////////////////////////////////////////////////////

CViewsPageDlg::CViewsPageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CViewsPageDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CViewsPageDlg)
	m_strFileExt = _T("");
	//}}AFX_DATA_INIT
}


void CViewsPageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewsPageDlg)
	DDX_Control(pDX, IDC_COMBO_HIGHLIGHTING, m_wndHighlighting);
	DDX_Control(pDX, IDC_LIST_VIEWS, m_wndViews);
	DDX_Control(pDX, IDC_LIST_FILEEXT, m_wndFileExt);
	DDX_Text(pDX, IDC_FILEEXT, m_strFileExt);
	DDV_MaxChars(pDX, m_strFileExt, 255);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CViewsPageDlg, CDialog)
	//{{AFX_MSG_MAP(CViewsPageDlg)
	ON_MESSAGE(WM_DOAPPLY, OnApply)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_ADD_EDIT_FILEEXT, OnAddEditFileExt)
	ON_BN_CLICKED(IDC_DELETE_FILEEXT, OnDeleteFileExt)
	ON_LBN_SELCHANGE(IDC_LIST_FILEEXT, OnSelChangeListFileExt)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_VIEWS, OnItemChangedListViews)
	ON_CBN_SELCHANGE(IDC_COMBO_HIGHLIGHTING, OnSelChangeComboHighlighting)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewsPageDlg message handlers

LONG CViewsPageDlg::OnApply(UINT, LONG)
{
	if (!UpdateData())
		return FALSE;

	// clear original associations
	theApp.m_opOptions.views.vAssociations.erase(
		theApp.m_opOptions.views.vAssociations.begin(),
		theApp.m_opOptions.views.vAssociations.end());

	// save changes to application variables
	size_t nNumAssoc = m_vwsOptions.vAssociations.size();
	for (size_t i = 0; i < nNumAssoc; i++)
	{
		VIEWS_OPTIONS_ASSOC temp;

		strcpy(temp.strExtensions,m_vwsOptions.vAssociations[i].strExtensions);
		strcpy(temp.strViews,m_vwsOptions.vAssociations[i].strViews);
		temp.nHighlightType = m_vwsOptions.vAssociations[i].nHighlightType;

		theApp.m_opOptions.views.vAssociations.push_back(temp);
	}

	return TRUE;
}

BOOL CViewsPageDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CClientDC tmpdc(this);

	m_wndFileExt.ResetContent();

	// load options from app's main options
	size_t nNumAssoc = theApp.m_opOptions.views.vAssociations.size();
	for (size_t i = 0; i < nNumAssoc; i++)
	{
		VIEWS_OPTIONS_ASSOC temp;

		strcpy(temp.strExtensions,theApp.m_opOptions.views.vAssociations[i].strExtensions);
		strcpy(temp.strViews,theApp.m_opOptions.views.vAssociations[i].strViews);
		temp.nHighlightType = theApp.m_opOptions.views.vAssociations[i].nHighlightType;

		if (strcmp(temp.strExtensions,"") == 0)
			m_wndFileExt.AddString("(Default)");
		else
			m_wndFileExt.AddString(temp.strExtensions);

		m_vwsOptions.vAssociations.push_back(temp);
	}

	/*m_fntUpDownButtons.CreateFont(-MulDiv(13,tmpdc.GetDeviceCaps(LOGPIXELSY),72),0,
		0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,"Webdings");

	GetDlgItem(IDC_MOVE_UP)->SetFont(&m_fntUpDownButtons);
	GetDlgItem(IDC_MOVE_DOWN)->SetFont(&m_fntUpDownButtons);*/

	// set up views list
	m_wndViews.ModifyStyle(WS_HSCROLL,0);
	ListView_SetExtendedListViewStyleEx(m_wndViews.GetSafeHwnd(),0,
		LVS_EX_CHECKBOXES);

	CRect rcTemp; m_wndViews.GetWindowRect(rcTemp);

	m_wndViews.InsertColumn(0,(LPSTR)NULL,LVCFMT_LEFT,rcTemp.Width() - 4);
	m_wndViews.InsertItem(0,"Source/Text View");
	m_wndViews.InsertItem(1,"XML Tree View");
	m_wndViews.InsertItem(2,"Browser Preview");

	m_wndHighlighting.AddString("(None)");
	m_wndHighlighting.AddString("HTML/XML");
	m_wndHighlighting.AddString("C++");
	m_wndHighlighting.AddString("ASP");
	m_wndHighlighting.AddString("CSS (Style Sheet)");
	m_wndHighlighting.AddString("SQL");

	// select the first item by default
	m_wndFileExt.SetSel(0);
	OnSelChangeListFileExt();
	
	return TRUE;
}

void CViewsPageDlg::OnDestroy() 
{
	CDialog::OnDestroy();
}

/*void CViewsPageDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CListBox* pListBox = (CListBox*)GetDlgItem(nIDCtl);
	if (pListBox)
	{
		int nCheckState = 0;
		switch (lpDrawItemStruct->itemData)
		{
			case 1: nCheckState = DFCS_PUSHED; break;
			case 2: nCheckState = DFCS_PUSHED | DFCS_BUTTON3STATE; break;
		}

		CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
		CRect rcDraw = lpDrawItemStruct->rcItem;
		int nItem = lpDrawItemStruct->itemID;

		CBrush brush; brush.CreateSysColorBrush(
			(lpDrawItemStruct->itemState & ODS_SELECTED) ? COLOR_HIGHLIGHT : COLOR_WINDOW);
		pDC->FillRect(rcDraw,&brush);

		if (lpDrawItemStruct->itemState & ODS_FOCUS)
			pDC->DrawFocusRect(rcDraw);

		CRect rcTemp = rcDraw; rcTemp.DeflateRect(2,2);
		rcTemp.right = rcTemp.bottom - rcTemp.top + rcTemp.left;
		pDC->DrawFrameControl(rcTemp,DFC_BUTTON,DFCS_BUTTONCHECK | nCheckState);

		rcTemp = rcDraw; rcTemp.DeflateRect(2,2);
		rcTemp.left = rcTemp.bottom - rcTemp.top + 4;

		int oldbkmode = pDC->SetBkMode(TRANSPARENT);
		COLORREF oldclr = pDC->SetTextColor(GetSysColor(
			(lpDrawItemStruct->itemState & ODS_SELECTED) ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT));

		CString strText; pListBox->GetText(nItem,strText);
		pDC->DrawText(strText,rcTemp,DT_VCENTER | DT_WORD_ELLIPSIS);

		pDC->SetTextColor(oldclr);
		pDC->SetBkMode(oldbkmode);
	}

	//CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CViewsPageDlg::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	CListBox* pListBox = (CListBox*)GetDlgItem(nIDCtl);
	if (pListBox)
	{
		CClientDC tmpdc(pListBox);
		lpMeasureItemStruct->itemHeight = tmpdc.GetTextExtent("0").cy + 2;
	}

	CDialog::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}
*/

void CViewsPageDlg::OnAddEditFileExt() 
{
	if (!UpdateData())
		return;

	// ensure string is valid and there are no duplicate file extensions
	std::vector<CString> arr;
	if (!SplitString(m_strFileExt,arr))
		return;

	size_t nCheck = m_vwsOptions.vAssociations.size();
	size_t nExt = arr.size();
	while (nExt--)
		for (size_t i = 0; i < nCheck; i++)
			if (IsStringPresent(m_vwsOptions.vAssociations[i].strExtensions,arr[nExt]))
			{
				AfxMessageBox(IDS_ERROR_VIEWS_FILEEXT_EXISTS,MB_ICONEXCLAMATION);
				return;
			}

	VIEWS_OPTIONS_ASSOC temp;
	strcpy(temp.strExtensions,m_strFileExt);
	strcpy(temp.strViews,"s--"); // all three views aren't visible
	temp.nHighlightType = 0;
	m_vwsOptions.vAssociations.push_back(temp);
	m_wndFileExt.AddString(m_strFileExt);
}

void CViewsPageDlg::OnDeleteFileExt() 
{
	if (m_wndFileExt.GetSelCount() == 0)
		return;

	int arrSelItems[60];
	int nNumSel = m_wndFileExt.GetSelItems(60,arrSelItems);
	
	for (int i = 0; i < nNumSel; i++)
		if (arrSelItems[i] != 0)
		{
			// this loop ensures all indices are valid after a deletion
			for (int j = i + 1; j < nNumSel; j++)
				if (arrSelItems[j] > arrSelItems[i])
					arrSelItems[j]--;

			m_wndFileExt.DeleteString((UINT)arrSelItems[i]);
			m_vwsOptions.vAssociations.erase(
				m_vwsOptions.vAssociations.begin() + arrSelItems[i]);
		}
}

void CViewsPageDlg::OnSelChangeListFileExt() 
{
	// NOTE: some of this code can be used for tri-state list boxes, but for now,
	// only two-state checkbox listviews are used

	int arrSelItems[60];
	int nNumSel = m_wndFileExt.GetSelItems(60,arrSelItems);
	
	BOOL bSV, bXV, bBV; // at least one checked S/C/B has been encountered
	BOOL bNSV, bNXV, bNBV; // at least one unchecked S/C/B has been encountered

	bSV = bXV = bBV = bNSV = bNXV = bNBV = FALSE;

	int nHighlight = -2;

	// figure out the checked state of S/C/B views of the selection
	CString strTest;
	for (int i = 0; i < nNumSel; i++)
	{
		if (nHighlight == -2)
			nHighlight = m_vwsOptions.vAssociations[arrSelItems[i]].nHighlightType;
		else if (nHighlight != -1)
		{
			if (m_vwsOptions.vAssociations[arrSelItems[i]].nHighlightType != nHighlight)
				nHighlight = -1;
		}

		strTest = m_vwsOptions.vAssociations[arrSelItems[i]].strViews;
		if (strTest.GetLength() < 3)
			continue;

		if (strTest[0] == 's') bSV = TRUE;
		else bNSV = TRUE;

		if (strTest[1] == 'x') bXV = TRUE;
		else bNXV = TRUE;

		if (strTest[2] == 'b') bBV = TRUE;
		else bNBV = TRUE;
	}

	::SetWindowLong(m_wndViews,GWL_USERDATA,1); // prevent loop-back
	m_wndViews.SetCheck(0,bSV && !bNSV);
	m_wndViews.SetCheck(1,bXV && !bNXV);
	m_wndViews.SetCheck(2,bBV && !bNBV);
	::SetWindowLong(m_wndViews,GWL_USERDATA,0);

	m_wndHighlighting.SetCurSel(nHighlight);
}

// handles check-box clicks
void CViewsPageDlg::OnItemChangedListViews(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLISTVIEW* pNMListView = (NMLISTVIEW*)pNMHDR;

	*pResult = 0;

	if (::GetWindowLong(m_wndViews,GWL_USERDATA))
		return;

	if (pNMListView->uChanged == LVIF_STATE)
	{
		UINT nOldStateImg = (pNMListView->uOldState & LVIS_STATEIMAGEMASK) >> 12;
		UINT nNewStateImg = (pNMListView->uNewState & LVIS_STATEIMAGEMASK) >> 12;

		// undo if trying to disable the source view
		/*if (pNMListView->iItem == 0 && nNewStateImg == 1)
		{
			//AfxMessageBox("You cannot disable the Source View", MB_OK, 0);
			::SetWindowLong(m_wndViews,GWL_USERDATA,1); // prevent loop-back
			m_wndViews.SetCheck(0, 1);
			::SetWindowLong(m_wndViews,GWL_USERDATA,0);
			return;
		}*/

		if (nOldStateImg != nNewStateImg)
		{
			char* strFilled = "sxb";

			// a checkbox was clicked
			if (m_wndFileExt.GetSelCount() == 0)
				return;

			int arrSelItems[60];
			int nNumSel = m_wndFileExt.GetSelItems(60,arrSelItems);
			
			for (int i = 0; i < nNumSel; i++)
			{
				m_vwsOptions.vAssociations[arrSelItems[i]].
					strViews[pNMListView->iItem] = (nNewStateImg == 1) ? '-' : 
					strFilled[pNMListView->iItem];
			}
		}
	}
}

// handles syntax highlighting type changes
void CViewsPageDlg::OnSelChangeComboHighlighting() 
{
	if (m_wndHighlighting.GetCurSel() == -1)
		return;

	int arrSelItems[60];
	int nNumSel = m_wndFileExt.GetSelItems(60,arrSelItems);
	
	for (int i = 0; i < nNumSel; i++)
		m_vwsOptions.vAssociations[arrSelItems[i]].nHighlightType = m_wndHighlighting.GetCurSel();
}