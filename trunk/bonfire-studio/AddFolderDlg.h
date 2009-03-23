// AddFolderDlg.h : header file
//

#if !defined(AFX_ADDFOLDERDLG_H__1C793B7C_825C_4141_9D2A_73B16768340F__INCLUDED_)
#define AFX_ADDFOLDERDLG_H__1C793B7C_825C_4141_9D2A_73B16768340F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MRUCombo.h"

/////////////////////////////////////////////////////////////////////////////
// CAddFolderDlg dialog

class CAddFolderDlg : public CDialog
{
// Construction
public:
	CAddFolderDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAddFolderDlg)
	enum { IDD = IDD_PROJECT_ADDFOLDER };
	CMRUComboBox	m_pAddFiles;
	CString	m_strPath;
	BOOL	m_bAddSubFolders;
	CString	m_strAddFiles;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddFolderDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddFolderDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowse();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDFOLDERDLG_H__1C793B7C_825C_4141_9D2A_73B16768340F__INCLUDED_)
