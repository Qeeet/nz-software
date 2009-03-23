#if !defined(AFX_OPENFROMURLDLG_H__8EAC7831_6AA5_43A3_AF95_A10DE1A122F4__INCLUDED_)
#define AFX_OPENFROMURLDLG_H__8EAC7831_6AA5_43A3_AF95_A10DE1A122F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OpenFromURLDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COpenFromURLDlg dialog

class COpenFromURLDlg : public CDialog
{
// Construction
public:
	COpenFromURLDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COpenFromURLDlg)
	enum { IDD = IDD_OPEN_FROM_URL };
	CString	m_strURL;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COpenFromURLDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COpenFromURLDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPENFROMURLDLG_H__8EAC7831_6AA5_43A3_AF95_A10DE1A122F4__INCLUDED_)
