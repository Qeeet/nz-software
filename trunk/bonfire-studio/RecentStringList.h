// RecentStringList.h: interface for the CRecentStringList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECENTSTRINGLIST_H__5C0FF998_7865_4476_87A4_5B670427EAA5__INCLUDED_)
#define AFX_RECENTSTRINGLIST_H__5C0FF998_7865_4476_87A4_5B670427EAA5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRecentStringList : public CRecentFileList  
{
public:
	CRecentStringList(UINT nStart, LPCTSTR lpszSection, LPCTSTR lpszEntryFormat, int nSize, int nMaxDispLen = 50);
	~CRecentStringList();

	virtual void Add(LPCTSTR lpszPathName);
};

#endif // !defined(AFX_RECENTSTRINGLIST_H__5C0FF998_7865_4476_87A4_5B670427EAA5__INCLUDED_)
