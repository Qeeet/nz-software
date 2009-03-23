#if !defined(AFX_OPTIONSDLG_H__921665D3_1DBA_4B23_A260_3CCB21E78854__INCLUDED_)
#define AFX_OPTIONSDLG_H__921665D3_1DBA_4B23_A260_3CCB21E78854__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsDlg.h : header file
//

#include <afxtempl.h>
#include "resource.h"

#define WM_DOAPPLY		WM_USER + 63

/////////////////////////////////////////////////////////////////////////////
// PropPageData

struct PropPageData
{
	CString strCaption;
	UINT nDlgID;
	CDialog* pDialog;

	PropPageData(CString caption = "", UINT id = 0, CDialog* dlg = NULL)
	{
		strCaption = caption;
		nDlgID = id;
		pDialog = dlg;
	}
};

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg dialog

class COptionsDlg : public CDialog
{
// Construction
public:
	COptionsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COptionsDlg();

	CArray<PropPageData,PropPageData&> m_arrPages;
	int m_nSelItem;

	void SelectTab(int nIndex);
	BOOL DoApply();

// Dialog Data
	//{{AFX_DATA(COptionsDlg)
	enum { IDD = IDD_OPTIONS };
	CTabCtrl	m_wndTabs;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangeTabs(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnApply();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSDLG_H__921665D3_1DBA_4B23_A260_3CCB21E78854__INCLUDED_)
