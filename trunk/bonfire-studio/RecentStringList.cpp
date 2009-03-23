// RecentStringList.cpp: implementation of the CRecentStringList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "bonfire.h"
#include "RecentStringList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//CRecentStringList::CRecentStringList()
//{
//
//}
CRecentStringList::CRecentStringList(UINT nStart, LPCTSTR lpszSection, LPCTSTR lpszEntryFormat, int nSize, int nMaxDispLen) 
				: CRecentFileList(nStart, lpszSection, lpszEntryFormat, nSize, nMaxDispLen)
{
}

CRecentStringList::~CRecentStringList()
{

}

void CRecentStringList::Add(LPCTSTR lpszPathName)
{
	ASSERT(m_arrNames != NULL);
	ASSERT(lpszPathName != NULL);
	ASSERT(AfxIsValidString(lpszPathName));

	// update the MRU list, if an existing MRU string matches 
	for (int iMRU = 0; iMRU < m_nSize-1; iMRU++)
	{
		if (!lstrcmpi(m_arrNames[iMRU], lpszPathName))
			break;      // iMRU will point to matching entry
	}
	// move MRU strings before this one down
	for (; iMRU > 0; iMRU--)
	{
		ASSERT(iMRU > 0);
		ASSERT(iMRU < m_nSize);
		m_arrNames[iMRU] = m_arrNames[iMRU-1];
	}
	// place this one at the beginning
	m_arrNames[0] = lpszPathName;
}