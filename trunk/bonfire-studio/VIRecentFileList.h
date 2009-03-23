//////////////////////////////////////////////////////////////////////
// VIRecentFileList.h
// Visual Interface
// by Todd C. Gleason
// Begun:  02-19-1998

#if !defined(AFX_VIRECENTFILELIST_H__90D1D1DC_C7EF_4A89_AC5B_B0E19C826551__INCLUDED_)
#define AFX_VIRECENTFILELIST_H__90D1D1DC_C7EF_4A89_AC5B_B0E19C826551__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/* This class is a subclass of CRecentFileList which overrides the
* UpdateMenu() member function.
* This class may be freely used, distributed, and modified.
* The four-line header and this comment must be redistributed
* unmodified with any modified version of this class.
* In addition, whenever a newly modified form of this class is
* redistributed, the author must email tcg@visint.com with the
* changes.
*
* This file, and the implementation file, are
* Copyright (c) 1998 Visual Interface.
*/
#include <afxadv.h>  // for CRecentFileList

class VIRecentFileList : public CRecentFileList {
public:
	//ID_FILE_MRU_PROJECT1
    VIRecentFileList(UINT nStart, LPCTSTR lpszSection,
        LPCTSTR lpszEntryFormat, int nSize,
        int nMaxDispLen = AFX_ABBREV_FILENAME_LEN)
        : CRecentFileList(nStart, lpszSection, lpszEntryFormat, nSize,
        nMaxDispLen) {}
    virtual void UpdateMenu(CCmdUI* pCmdUI);
	//virtual ~VIRecentFileList();
};  // class VIRecentFileList

#endif // !defined(AFX_VIRECENTFILELIST_H__90D1D1DC_C7EF_4A89_AC5B_B0E19C826551__INCLUDED_)