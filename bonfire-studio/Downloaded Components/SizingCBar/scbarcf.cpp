/////////////////////////////////////////////////////////////////////////
//
// CSizingControlBarCF          Version 2.44
// 
// Created: Dec 21, 1998        Last Modified: March 31, 2002
//
// See the official site at www.datamekanix.com for documentation and
// the latest news.
//
/////////////////////////////////////////////////////////////////////////
// Copyright (C) 1998-2002 by Cristi Posea. All rights reserved.
//
// This code is free for personal and commercial use, providing this 
// notice remains intact in the source files and all eventual changes are
// clearly marked with comments.
//
// You must obtain the author's consent before you can include this code
// in a software library.
//
// No warrantee of any kind, express or implied, is included with this
// software; use at your own risk, responsibility for damages (if any) to
// anyone resulting from the use of this software rests entirely with the
// user.
//
// Send bug reports, bug fixes, enhancements, requests, flames, etc. to
// cristi@datamekanix.com or post them at the message board at the site.
/////////////////////////////////////////////////////////////////////////

#include <stdafx.h>
#include "scbarcf.h"

/////////////////////////////////////////////////////////////////////////
// CSizingControlBarCF

IMPLEMENT_DYNAMIC(CSizingControlBarCF, baseCSizingControlBarCF);

int CALLBACK EnumFontFamProc(ENUMLOGFONT FAR *lpelf,
                             NEWTEXTMETRIC FAR *lpntm,
                             int FontType,
                             LPARAM lParam)
{
    UNUSED_ALWAYS(lpelf);
    UNUSED_ALWAYS(lpntm);
    UNUSED_ALWAYS(FontType);
    UNUSED_ALWAYS(lParam);

    return 0;
}
 
CSizingControlBarCF::CSizingControlBarCF()
{
    m_bActive = FALSE;

    CDC dc;
    dc.CreateCompatibleDC(NULL);

    m_sFontFace = (::EnumFontFamilies(dc.m_hDC,
        _T("Tahoma"), (FONTENUMPROC) EnumFontFamProc, 0) == 0) ?
        _T("Tahoma") : _T("Arial");

	m_cyGripper = 15;

    dc.DeleteDC();
}

BEGIN_MESSAGE_MAP(CSizingControlBarCF, baseCSizingControlBarCF)
    //{{AFX_MSG_MAP(CSizingControlBarCF)
    //}}AFX_MSG_MAP
    ON_MESSAGE(WM_SETTEXT, OnSetText)
END_MESSAGE_MAP()

void CSizingControlBarCF::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
    baseCSizingControlBarCF::OnUpdateCmdUI(pTarget, bDisableIfNoHndler);

    if (!HasGripper())
        return;

    BOOL bNeedPaint = FALSE;

    CWnd* pFocus = GetFocus();
    BOOL bActiveOld = m_bActive;

    m_bActive = (pFocus->GetSafeHwnd() && IsChild(pFocus));

    if (m_bActive != bActiveOld)
        bNeedPaint = TRUE;

    if (bNeedPaint)
        SendMessage(WM_NCPAINT);
}

// gradient defines (if not already defined)
#ifndef COLOR_GRADIENTACTIVECAPTION
#define COLOR_GRADIENTACTIVECAPTION     27
#define COLOR_GRADIENTINACTIVECAPTION   28
#define SPI_GETGRADIENTCAPTIONS         0x1008
#endif

void CSizingControlBarCF::NcPaintGripper(CDC* pDC, CRect rcClient)
{
    if (!HasGripper())
        return;

    // compute the caption rectangle
    BOOL bHorz = IsHorzDocked();
    CRect rcGrip = rcClient;
    CRect rcBtn = m_biHide.GetRect();
    if (bHorz)
    {   // right side gripper
        rcGrip.left -= m_cyGripper + 2;
        rcGrip.right = rcGrip.left + m_cyGripper - 1;
        rcGrip.top = rcBtn.bottom + 3;
    }
    else
    {   // gripper at top
        rcGrip.top -= m_cyGripper;
        rcGrip.bottom = rcGrip.top + m_cyGripper - 2;
        rcGrip.right = rcBtn.left - 3;
    }
    rcGrip.InflateRect(bHorz ? 1 : 0, bHorz ? 0 : 1);

    // draw the caption background
    //CBrush br;
    COLORREF clrCptn = m_bActive ?
        ::GetSysColor(COLOR_ACTIVECAPTION) :
        ::GetSysColor(COLOR_INACTIVECAPTION);

    // query gradient info (usually TRUE for Win98/Win2k)
    BOOL bGradient = FALSE;
    ::SystemParametersInfo(SPI_GETGRADIENTCAPTIONS, 0, &bGradient, 0);
    
    /*if (!bGradient)
        pDC->FillSolidRect(&rcGrip, clrCptn); // solid color
    else
    {
        // gradient from left to right or from bottom to top
        // get second gradient color (the right end)
        COLORREF clrCptnRight = m_bActive ?
            ::GetSysColor(COLOR_GRADIENTACTIVECAPTION) :
            ::GetSysColor(COLOR_GRADIENTINACTIVECAPTION);

        // this will make 2^6 = 64 fountain steps
        int nShift = 6;
        int nSteps = 1 << nShift;

        for (int i = 0; i < nSteps; i++)
        {
            // do a little alpha blending
            int nR = (GetRValue(clrCptn) * (nSteps - i) +
                      GetRValue(clrCptnRight) * i) >> nShift;
            int nG = (GetGValue(clrCptn) * (nSteps - i) +
                      GetGValue(clrCptnRight) * i) >> nShift;
            int nB = (GetBValue(clrCptn) * (nSteps - i) +
                      GetBValue(clrCptnRight) * i) >> nShift;

            COLORREF cr = RGB(nR, nG, nB);

            // then paint with the resulting color
            CRect r2 = rcGrip;
            if (bHorz)
            {
                r2.bottom = rcGrip.bottom - 
                    ((i * rcGrip.Height()) >> nShift);
                r2.top = rcGrip.bottom - 
                    (((i + 1) * rcGrip.Height()) >> nShift);
                if (r2.Height() > 0)
                    pDC->FillSolidRect(r2, cr);
            }
            else
            {
                r2.left = rcGrip.left + 
                    ((i * rcGrip.Width()) >> nShift);
                r2.right = rcGrip.left + 
                    (((i + 1) * rcGrip.Width()) >> nShift);
                if (r2.Width() > 0)
                    pDC->FillSolidRect(r2, cr);
            }
        }
    }*/

    // draw the caption text - first select a font
    CFont font;
    int ppi = pDC->GetDeviceCaps(LOGPIXELSX);
    int pointsize = MulDiv(85, 96, ppi); // 8.5 points at 96 ppi

    CString sTitle;
    GetWindowText(sTitle);
	int nSizeText = 6;

    LOGFONT lf;
    BOOL bFont = font.CreatePointFont(pointsize, m_sFontFace);
    if (bFont)
    {
        // get the text color
        COLORREF clrCptnText = COLOR_BTNTEXT;/* = m_bActive ?
            ::GetSysColor(COLOR_CAPTIONTEXT) :
            ::GetSysColor(COLOR_INACTIVECAPTIONTEXT);*/

        int nOldBkMode = pDC->SetBkMode(TRANSPARENT);
        COLORREF clrOldText = pDC->SetTextColor(clrCptnText);

        font.GetLogFont(&lf);
        font.DeleteObject();
		lf.lfWeight = FW_BOLD;
        if (bHorz)
        {
            // rotate text 90 degrees CCW if horizontally docked
            lf.lfEscapement = 900;
        }
		font.CreateFontIndirect(&lf);
        
        CFont* pOldFont = pDC->SelectObject(&font);
		CSize sizeO = pDC->GetTextExtent("O");

		// set text size to draw gripper right after text
		nSizeText += pDC->GetTextExtent(sTitle).cx;

        CPoint ptOrg = bHorz ?
            CPoint(rcGrip.left + m_cyGripper / 2 - sizeO.cy / 2, rcGrip.bottom - 3) :
            CPoint(rcGrip.left + 3, rcGrip.top + m_cyGripper / 2 - sizeO.cy / 2 - 1);

        pDC->ExtTextOut(ptOrg.x, ptOrg.y,
            ETO_CLIPPED, rcGrip, sTitle, NULL);

        pDC->SelectObject(pOldFont);
        pDC->SetBkMode(nOldBkMode);
        pDC->SetTextColor(clrOldText);
    }

	// draw gripper
	CRect rcGripLine = rcGrip;
	if (bHorz)
	{
		rcGripLine.bottom -= nSizeText;
		rcGripLine.left += rcGrip.Width() / 2 - 3;
		rcGripLine.right = rcGripLine.left + 3;

		if (rcGripLine.bottom > rcGripLine.top)
		{
			pDC->Draw3dRect(rcGripLine,
				GetSysColor(COLOR_BTNHIGHLIGHT),
				GetSysColor(COLOR_BTNSHADOW));

			rcGripLine.OffsetRect(4,0);

			pDC->Draw3dRect(rcGripLine,
				GetSysColor(COLOR_BTNHIGHLIGHT),
				GetSysColor(COLOR_BTNSHADOW));
		}
	}
	else
	{
		rcGripLine.left += nSizeText;
		rcGripLine.top += rcGrip.Height() / 2 - 3;
		rcGripLine.bottom = rcGripLine.top + 3;

		if (rcGripLine.left < rcGripLine.right)
		{
			pDC->Draw3dRect(rcGripLine,
				GetSysColor(COLOR_BTNHIGHLIGHT),
				GetSysColor(COLOR_BTNSHADOW));

			rcGripLine.OffsetRect(0,4);

			pDC->Draw3dRect(rcGripLine,
				GetSysColor(COLOR_BTNHIGHLIGHT),
				GetSysColor(COLOR_BTNSHADOW));
		}
	}

    // draw the button
    m_biHide.Paint(pDC);
}

LRESULT CSizingControlBarCF::OnSetText(WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = baseCSizingControlBarCF::OnSetText(wParam, lParam);

    SendMessage(WM_NCPAINT);

    return lResult;
}
