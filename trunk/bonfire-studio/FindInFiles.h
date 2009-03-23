// FindInFiles.h: interface for the CFindInFiles class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FINDINFILES_H__EFEE4E27_ABDE_4F0B_96D7_FBA9CA7ADB99__INCLUDED_)
#define AFX_FINDINFILES_H__EFEE4E27_ABDE_4F0B_96D7_FBA9CA7ADB99__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFindInFilesDlg;

class CFindInFiles  
{
private:
	DWORD m_dwLastSearchFlags;

public:
	CFindInFiles();
	virtual ~CFindInFiles();
	BOOL FindInFile(LPCTSTR pszFileName, LPCTSTR pszWhat);
	BOOL FindInFiles();
	BOOL SearchFolder(CString strFindWhat, CString strFolderPath, CString strFileFilter, BOOL bSubfolders);
};

#endif // !defined(AFX_FINDINFILES_H__EFEE4E27_ABDE_4F0B_96D7_FBA9CA7ADB99__INCLUDED_)
