#ifndef __NZDOCKBAR_H_
#define __NZDOCKBAR_H_

#if _MSC_VER > 1000
#pragma once
#endif

class CNZDockBar : public CDockBar
{
	DECLARE_DYNAMIC(CNZDockBar)

public:
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
};

#endif