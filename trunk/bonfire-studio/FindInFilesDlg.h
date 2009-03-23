#if !defined(AFX_FINDINFILESDLG_H__BD5485C5_6683_49F5_BABA_0F145CD27BF1__INCLUDED_)
#define AFX_FINDINFILESDLG_H__BD5485C5_6683_49F5_BABA_0F145CD27BF1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FindInFilesDlg.h : header file
//

#include "MRUCombo.h"

/////////////////////////////////////////////////////////////////////////////
// CFindInFilesDlg dialog

enum
{
	FIF_FIND_MATCH_CASE		= 0x0001,
	FIF_FIND_WHOLE_WORD		= 0x0002,
};

class CFindInFilesDlg : public CDialog
{
// Construction
public:
	CFindInFilesDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFindInFilesDlg)
	enum { IDD = IDD_FINDINFILES };
	CMRUComboBox	m_pFileTypes;
	CMRUComboBox	m_pSearchPath;
	CMRUComboBox	m_pSearchText;
	BOOL	m_bWholeWord;
	BOOL	m_bMatchCase;
	BOOL	m_bSearchSubfolders;
	CString	m_strSearchText;
	CString	m_strSearchPath;
	CString	m_strFileTypes;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindInFilesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFindInFilesDlg)
	afx_msg void OnBrowse();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDINFILESDLG_H__BD5485C5_6683_49F5_BABA_0F145CD27BF1__INCLUDED_)
