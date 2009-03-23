////////////////////////////////////////////////////////////////////////////
//	File:		CEditReplaceDlg.cpp
//	Version:	1.0.0.0
//	Created:	29-Dec-1998
//
//	Author:		Stcherbatchenko Andrei
//	E-mail:		windfall@gmx.de
//
//	Implementation of the CEditReplaceDlg dialog, a part of Crystal Edit -
//	syntax coloring text editor.
//
//	You are free to use or modify this code to the following restrictions:
//	- Acknowledge me somewhere in your about box, simple "Parts of code by.."
//	will be enough. If you can't (or don't want to), contact me personally.
//	- LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CEditReplaceDlg.h"
#include "CCrystalEditView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditReplaceDlg dialog


CEditReplaceDlg::CEditReplaceDlg(CCrystalEditView *pBuddy) : CDialog(CEditReplaceDlg::IDD, NULL)
{
	ASSERT(pBuddy != NULL);
	m_pBuddy = pBuddy;
	//{{AFX_DATA_INIT(CEditReplaceDlg)
	m_bMatchCase = FALSE;
	m_bWholeWord = FALSE;
	m_sText = _T("");
	m_sReplaceText = _T("");
	m_nScope = -1;
	//}}AFX_DATA_INIT
	m_bEnableScopeSelection = TRUE;
}


void CEditReplaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditReplaceDlg)
	DDX_Check(pDX, IDC_EDIT_MATCH_CASE, m_bMatchCase);
	DDX_Check(pDX, IDC_EDIT_WHOLE_WORD, m_bWholeWord);
	DDX_Control(pDX, IDC_FINDREP_TEXT_EX, m_pSearchText);
	DDX_Control(pDX, IDC_EDIT_REPLACE_WITH_EX, m_pReplaceText);
	DDX_CBString(pDX, IDC_FINDREP_TEXT_EX, m_sText);
	DDX_CBString(pDX, IDC_EDIT_REPLACE_WITH_EX, m_sReplaceText);
	//DDX_Text(pDX, IDC_EDIT_TEXT, m_sText);
	//DDX_Text(pDX, IDC_EDIT_REPLACE_WITH, m_sReplaceText);
	DDX_Radio(pDX, IDC_EDIT_SCOPE_SELECTION, m_nScope);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditReplaceDlg, CDialog)
	//{{AFX_MSG_MAP(CEditReplaceDlg)
	ON_CBN_EDITUPDATE(IDC_FINDREP_TEXT_EX, OnChangeEditText)
	ON_CBN_EDITUPDATE(IDC_EDIT_REPLACE_WITH_EX, OnChangeEditText)
	//ON_EN_CHANGE(IDC_FINDREP_TEXT_EX, OnChangeEditText)
	ON_BN_CLICKED(IDC_EDIT_REPLACE, OnEditReplace)
	ON_BN_CLICKED(IDC_EDIT_REPLACE_ALL, OnEditReplaceAll)
	ON_BN_CLICKED(IDC_EDIT_SKIP, OnEditSkip)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditReplaceDlg message handlers

void CEditReplaceDlg::OnChangeEditText() 
{
	CString strFind, strReplace;
	GetDlgItem(IDC_FINDREP_TEXT_EX)->GetWindowText(strFind);
	GetDlgItem(IDC_EDIT_REPLACE_WITH_EX)->GetWindowText(strReplace);
	BOOL ok = (strFind != _T(""));// && strReplace != _T(""));
	
	// enable the buttons if something was entered into the choices
	GetDlgItem(IDC_EDIT_SKIP)->EnableWindow(ok);
	GetDlgItem(IDC_EDIT_REPLACE)->EnableWindow(ok);
	GetDlgItem(IDC_EDIT_REPLACE_ALL)->EnableWindow(ok);
}

void CEditReplaceDlg::OnCancel() 
{
	VERIFY(UpdateData());

	CDialog::OnCancel();
}

BOOL CEditReplaceDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// load the choices from registry
	m_pSearchText.SetMRURegKey( _T("Find\\Search String") );
	m_pSearchText.SetMRUValueFormat( _T("Search String #%d") );
	//m_pSearchText.SetAutoRefreshAfterAdd( TRUE );
	//m_pSearchText.SetAutoSaveAfterAdd( TRUE );
	m_pSearchText.SetMaxMRUSize(8);
	m_pSearchText.LoadMRU();
	m_pSearchText.RefreshCtrl();

	m_pReplaceText.SetMRURegKey( _T("Find\\Replace String") );
	m_pReplaceText.SetMRUValueFormat( _T("Replace String #%d") );
	//m_pReplaceText.SetAutoRefreshAfterAdd( TRUE );
	//m_pReplaceText.SetAutoSaveAfterAdd( TRUE );
	m_pReplaceText.SetMaxMRUSize(8);
	m_pReplaceText.LoadMRU();
	m_pReplaceText.RefreshCtrl();
	
	// disable/enable the buttons
	OnChangeEditText();

	GetDlgItem(IDC_EDIT_SCOPE_SELECTION)->EnableWindow(m_bEnableScopeSelection);
	m_bFound = FALSE;

	return TRUE;
}

BOOL CEditReplaceDlg::DoHighlightText()
{
	ASSERT(m_pBuddy != NULL);
	DWORD dwSearchFlags = 0;
	if (m_bMatchCase)
		dwSearchFlags |= FIND_MATCH_CASE;
	if (m_bWholeWord)
		dwSearchFlags |= FIND_WHOLE_WORD;

	BOOL bFound;
	if (m_nScope == 0)
	{
		//	Searching selection only
		bFound = m_pBuddy->FindTextInBlock(m_sText, m_ptFoundAt, m_ptBlockBegin, m_ptBlockEnd,
											dwSearchFlags, FALSE, &m_ptFoundAt);
	}
	else
	{
		//	Searching whole text
		bFound = m_pBuddy->FindText(m_sText, m_ptFoundAt, dwSearchFlags, FALSE, &m_ptFoundAt);
	}

	if (! bFound)
	{
		CString prompt;
		prompt.Format(IDS_EDIT_TEXT_NOT_FOUND, m_sText);
		AfxMessageBox(prompt);
		m_ptCurrentPos = m_nScope == 0 ? m_ptBlockBegin : CPoint(0, 0);
		return FALSE;
	}

	m_pBuddy->HighlightText(m_ptFoundAt, lstrlen(m_sText));
	return TRUE;
}

void CEditReplaceDlg::OnEditSkip() 
{
	if (! UpdateData())
		return;

	// save the choices in registry
	CString strSearchText;
	m_pSearchText.GetWindowText ( strSearchText );
	m_pSearchText.AddToMRU( strSearchText );
	m_pSearchText.SaveMRU();

	if (! m_bFound)
	{
		m_ptFoundAt = m_ptCurrentPos;
		m_bFound = DoHighlightText();
		return;
	}

	m_ptFoundAt.x += 1;
	m_bFound = DoHighlightText();
}

void CEditReplaceDlg::OnEditReplace() 
{
	if (! UpdateData())
		return;

	// save the choices in registry
	CString strReplaceText;
	m_pReplaceText.GetWindowText ( strReplaceText );
	m_pReplaceText.AddToMRU( strReplaceText );
	m_pReplaceText.SaveMRU();

	if (! m_bFound)
	{
		m_ptFoundAt = m_ptCurrentPos;
		m_bFound = DoHighlightText();
		return;
	}

	//	We have highlighted text
	VERIFY(m_pBuddy->ReplaceSelection(m_sReplaceText));

	//	Manually recalculate points
	if (m_bEnableScopeSelection)
	{
		if (m_ptBlockBegin.y == m_ptFoundAt.y && m_ptBlockBegin.x > m_ptFoundAt.x)
		{
			m_ptBlockBegin.x -= lstrlen(m_sText);
			m_ptBlockBegin.x += lstrlen(m_sReplaceText);
		}
		if (m_ptBlockEnd.y == m_ptFoundAt.y && m_ptBlockEnd.x > m_ptFoundAt.x)
		{
			m_ptBlockEnd.x -= lstrlen(m_sText);
			m_ptBlockEnd.x += lstrlen(m_sReplaceText);
		}
	}
	m_ptFoundAt.x += lstrlen(m_sReplaceText);
	m_bFound = DoHighlightText();
}

void CEditReplaceDlg::OnEditReplaceAll() 
{
	if (! UpdateData())
		return;

	// save the choices in registry
	CString strReplaceText;
	m_pReplaceText.GetWindowText ( strReplaceText );
	m_pReplaceText.AddToMRU( strReplaceText );
	m_pReplaceText.SaveMRU();

	if (! m_bFound)
	{
		m_ptFoundAt = m_ptCurrentPos;
		m_bFound = DoHighlightText();
	}

	while (m_bFound)
	{
		//	We have highlighted text
		VERIFY(m_pBuddy->ReplaceSelection(m_sReplaceText));

		//	Manually recalculate points
		if (m_bEnableScopeSelection)
		{
			if (m_ptBlockBegin.y == m_ptFoundAt.y && m_ptBlockBegin.x > m_ptFoundAt.x)
			{
				m_ptBlockBegin.x -= lstrlen(m_sText);
				m_ptBlockBegin.x += lstrlen(m_sReplaceText);
			}
			if (m_ptBlockEnd.y == m_ptFoundAt.y && m_ptBlockEnd.x > m_ptFoundAt.x)
			{
				m_ptBlockEnd.x -= lstrlen(m_sText);
				m_ptBlockEnd.x += lstrlen(m_sReplaceText);
			}
		}
		m_ptFoundAt.x += lstrlen(m_sReplaceText);
		m_bFound = DoHighlightText();
	}
}
