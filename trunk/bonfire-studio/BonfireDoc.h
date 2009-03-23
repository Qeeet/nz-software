// BonfireDoc.h : interface of the CBonfireDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_BONFIREDOC_H__2D4E25B3_AB78_4346_B8CC_FCE1A5A0A3D5__INCLUDED_)
#define AFX_BONFIREDOC_H__2D4E25B3_AB78_4346_B8CC_FCE1A5A0A3D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define XML_VIEW_COUNT		2

#include "StdAfx.h"
#include "Downloaded Components\CrystalEdit\CCrystalTextBuffer.h"
#include "SyntaxParsers.h"
#include "XNode.h"
#include "BonfireView.h"

class CBonfireDoc : public CDocument
{
protected: // create from serialization only
	CBonfireDoc();
	DECLARE_DYNCREATE(CBonfireDoc)

// Attributes
public:
	class CXMLTextBuffer : public CCrystalTextBuffer
	{
	private:
		CBonfireDoc *m_pOwnerDoc;
	public:
		CXMLTextBuffer(CBonfireDoc *pDoc) { m_pOwnerDoc = pDoc; };
		virtual void SetModified(BOOL bModified = TRUE)
		{
			// set the buffer, doc, and file tree node to be modified
			CCrystalTextBuffer::SetModified(bModified);
			m_pOwnerDoc->SetModifiedFlag(bModified);
			CXFile* pFile = m_pOwnerDoc->m_pXFile;
			if (pFile)
				pFile->m_bSaved = !bModified;

			m_pOwnerDoc->UpdateDocumentTitle();

			// set this flag for other views not to refresh if not needed
			if (bModified)
			{
				m_pOwnerDoc->SetViewRefreshFlag();
			}
		}
		void GetAllText(char* strBuffer)
		{
			char* strCurLine = strBuffer;

			int nLineLength;
			
			for (int i = 0; i < GetLineCount(); i++)
			{
				nLineLength = GetLineLength(i);
				memcpy(strCurLine,GetLineChars(i),nLineLength);
				strCurLine += nLineLength;

				// add CR-LF (windows)
				if (GetCRLFMode() == CRLF_STYLE_DOS)
				{
					memcpy(strCurLine,"\r\n",2);
					strCurLine += 2;
				}
				else if (GetCRLFMode() == CRLF_STYLE_UNIX)
				{
					strCurLine[0] = '\n';
					strCurLine ++;
				}
				else if (GetCRLFMode() == CRLF_STYLE_MAC)
				{
					strCurLine[0] = '\r';
					strCurLine ++;
				}
			}

			strCurLine[0] = 0;
		}
		int GetAllTextLength()
		{
			int nLineExtra, nTextLength = 0;

			if (GetCRLFMode() == CRLF_STYLE_DOS || GetCRLFMode() == CRLF_STYLE_MAC)
				nLineExtra = 2;
			else nLineExtra = 1;

			for (int i = 0; i < GetLineCount(); i++)
				nTextLength += GetLineLength(i) + nLineExtra;

			return nTextLength;
		}
		void SetReadOnly(BOOL bReadOnly = TRUE)
		{
			CCrystalTextBuffer::SetReadOnly(bReadOnly);

			m_pOwnerDoc->UpdateDocumentTitle();
			if (m_pOwnerDoc->m_pXFile)
				m_pOwnerDoc->m_pXFile->m_bReadOnly = (bReadOnly != 0);
		}
	};

	CXMLTextBuffer m_xTextBuffer;
	
	CMapStringToPtr* m_pViews;
	
	// pointer to the CXFile object in the project tree
	CXFile*	m_pXFile;

	//syntax highlighting code
	ParserFuncPtr	m_fnParse;

protected:
	DWORD m_hFileWatch;

// Operations
public:
	void OnFileReload();
	void AddToFileWatch();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBonfireDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void DeleteContents();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE);
	virtual void OnCloseDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	void RemoveFileWatch();
	BOOL ExistsSourceView(LPCTSTR lpszPathName);
	void SwitchToFirstView();
	BOOL SwitchToView(CString strTab);
	void SetViewRefreshFlag();
	void AddViewToMap(CBonfireView* pNewView);
	void UpdateTabs(LPCTSTR lpszPathName = NULL);
	void UpdateDocumentTitle(LPCTSTR lpszPathName = NULL);
	virtual BOOL DoSave(LPCTSTR lpszPathName, BOOL bReplace = TRUE);
	virtual BOOL DoFileSave();

	OutputMessage* CreateXMLErrorMessage(IXMLDOMParseError* pXError, bool bShowLine);
	virtual ~CBonfireDoc();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CBonfireDoc)
	afx_msg void OnUpdateEditReadOnly(CCmdUI* pCmdUI);
	afx_msg void OnGotoLine();
	afx_msg void OnEditReadOnly();
	afx_msg void OnToolsValidateFile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BONFIREDOC_H__2D4E25B3_AB78_4346_B8CC_FCE1A5A0A3D5__INCLUDED_)
