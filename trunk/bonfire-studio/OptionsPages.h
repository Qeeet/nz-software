#if !defined(AFX_OPTIONSPAGES_H__6B63DB85_B787_4B2C_870E_3E0B210D37F3__INCLUDED_)
#define AFX_OPTIONSPAGES_H__6B63DB85_B787_4B2C_870E_3E0B210D37F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsPages.h : header file
//

#include "Bonfire.h"
#include "resource.h"
#include "Downloaded Components\ColorButton\ColorButton.h"

struct EnumFontData
{
	CComboBox*	pCombo;
	BOOL		bFixed;
};

/////////////////////////////////////////////////////////////////////////////
// CGeneralPageDlg dialog

class CGeneralPageDlg : public CDialog
{
// Construction
public:
	CGeneralPageDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGeneralPageDlg)
	enum { IDD = IDD_OPTIONS_GENERAL_PAGE };
	BOOL	m_bClearOutput;
	BOOL	m_bReloadModified;
	BOOL	m_bSaveOnSwitch;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGeneralPageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGeneralPageDlg)
	virtual BOOL OnInitDialog();
	afx_msg LONG OnApply(UINT, LONG);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CEditorPageDlg dialog

class CEditorPageDlg : public CDialog
{
// Construction
public:
	CEditorPageDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditorPageDlg)
	enum { IDD = IDD_OPTIONS_EDITOR_PAGE };
	CComboBox	m_wndFontCombo;
	CListBox	m_wndListSyntax;
	CColorButton	m_wndColorBtn;
	int		m_nFontSize;
	//}}AFX_DATA

	EDITOR_OPTIONS m_edOptions;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditorPageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditorPageDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnSelChangeListSyntax();
	afx_msg LONG OnColorSelEndOK(UINT, LONG);
	afx_msg LONG OnApply(UINT, LONG);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

int CALLBACK EnumFontFamExProc(ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme,
	DWORD FontType, LPARAM lParam);

/////////////////////////////////////////////////////////////////////////////
// COutputPageDlg dialog

class COutputPageDlg : public CDialog
{
// Construction
public:
	COutputPageDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COutputPageDlg)
	enum { IDD = IDD_OPTIONS_OUTPUT_PAGE };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COutputPageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COutputPageDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CProjectMgrPageDlg dialog

class CProjectMgrPageDlg : public CDialog
{
// Construction
public:
	CProjectMgrPageDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProjectMgrPageDlg)
	enum { IDD = IDD_OPTIONS_PROJECTMGR_PAGE };
	BOOL	m_bDispFullPathFiles;
	BOOL	m_bDispFullPathFolders;
	BOOL	m_bDispFullPathProjects;
	BOOL	m_bFoldersFirst;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjectMgrPageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProjectMgrPageDlg)
	virtual BOOL OnInitDialog();
	afx_msg LONG OnApply(UINT, LONG);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CXMLTreePageDlg dialog

class CXMLTreePageDlg : public CDialog
{
// Construction
public:
	CXMLTreePageDlg(CWnd* pParent = NULL);   // standard constructor

	XMLTREE_OPTIONS m_edOptions;

// Dialog Data
	//{{AFX_DATA(CXMLTreePageDlg)
	enum { IDD = IDD_OPTIONS_XMLTREE_PAGE };
	CComboBox	m_wndFontCombo;
	BOOL	m_bShowAttributes;
	BOOL	m_bShowComments;
	BOOL	m_bShowText;
	int		m_nFontSize;
	BOOL	m_bEditXMLTree;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXMLTreePageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CXMLTreePageDlg)
	virtual BOOL OnInitDialog();
	afx_msg LONG OnApply(UINT, LONG);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CViewsPageDlg dialog

class CViewsPageDlg : public CDialog
{
// Construction
public:
	CViewsPageDlg(CWnd* pParent = NULL);   // standard constructor

	//CFont m_fntUpDownButtons;
	VIEWS_OPTIONS m_vwsOptions;

// Dialog Data
	//{{AFX_DATA(CViewsPageDlg)
	enum { IDD = IDD_OPTIONS_VIEWS_PAGE };
	CComboBox	m_wndHighlighting;
	CListCtrl	m_wndViews;
	CListBox	m_wndFileExt;
	CString	m_strFileExt;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewsPageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CViewsPageDlg)
	afx_msg LONG OnApply(UINT, LONG);
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnAddEditFileExt();
	afx_msg void OnDeleteFileExt();
	afx_msg void OnSelChangeListFileExt();
	afx_msg void OnItemChangedListViews(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelChangeComboHighlighting();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSPAGES_H__6B63DB85_B787_4B2C_870E_3E0B210D37F3__INCLUDED_)
