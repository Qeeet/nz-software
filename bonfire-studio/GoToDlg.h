#if !defined(AFX_GOTODLG_H__C550F55A_277C_4BC5_81EE_631EED7DCB93__INCLUDED_)
#define AFX_GOTODLG_H__C550F55A_277C_4BC5_81EE_631EED7DCB93__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GoToDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGoToDlg dialog

class CGoToDlg : public CDialog
{
// Construction
public:
	CGoToDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGoToDlg)
	enum { IDD = IDD_GOTO };
	CEdit	m_pGoToValueCtrl;
	CString	m_strGoToValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGoToDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGoToDlg)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GOTODLG_H__C550F55A_277C_4BC5_81EE_631EED7DCB93__INCLUDED_)
