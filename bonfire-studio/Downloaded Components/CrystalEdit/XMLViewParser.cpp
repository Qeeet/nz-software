#include "stdafx.h"
#include "..\..\SourceView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DEFINE_BLOCK(pos,colorindex)	\
	ASSERT((pos) >= 0 && (pos) <= nLength);\
	if (pBuf != NULL)\
	{\
		if (nActualItems == 0 || pBuf[nActualItems - 1].m_nCharPos <= (pos)){\
		pBuf[nActualItems].m_nCharPos = (pos);\
		pBuf[nActualItems].m_nColorIndex = (colorindex);\
		nActualItems++;}\
	}

#define COOKIE_COMMENT			0x0001
#define COOKIE_ELEMENT			0x0002
#define COOKIE_EL_NAME			0x0004
#define COOKIE_EL_ATTRIB		0x0008
#define COOKIE_SNGQ_STRING		0x0010
#define COOKIE_DBLQ_STRING		0x0020

DWORD CSourceView::ParseLine(DWORD dwCookie, int nLineIndex, TEXTBLOCK *pBuf, int &nActualItems)
{
	int nLength = GetLineLength(nLineIndex);

	LPCTSTR pszChars    = GetLineChars(nLineIndex);
	BOOL bRedefineBlock = TRUE;

	for (int I = 0; ; I++)
	{
		if (bRedefineBlock)
		{
			if (dwCookie & COOKIE_COMMENT)
			{	DEFINE_BLOCK(I,COLORINDEX_COMMENT);							}
			else if (dwCookie & (COOKIE_DBLQ_STRING | COOKIE_SNGQ_STRING))
			{	DEFINE_BLOCK(I,COLORINDEX_STRING);							}
			else if (dwCookie & COOKIE_EL_NAME)
			{	DEFINE_BLOCK(I,COLORINDEX_EL_NAME);							}
			else if (dwCookie & COOKIE_EL_ATTRIB)
			{	DEFINE_BLOCK(I,COLORINDEX_EL_ATTRIB);						}
			else if (dwCookie & COOKIE_ELEMENT)
			{	DEFINE_BLOCK(I,COLORINDEX_EL_TAG);							}
			else
			{	DEFINE_BLOCK(I,COLORINDEX_NORMALTEXT);						}

			bRedefineBlock = FALSE;
		}

		if (I == nLength)
			break;

		//	String constant "...."
		if (dwCookie & COOKIE_DBLQ_STRING)
		{
			if (pszChars[I] == '"')
			{
				dwCookie &= ~COOKIE_DBLQ_STRING;
				bRedefineBlock = TRUE;
			}
			continue;
		}

		//	String constant '....'
		if (dwCookie & COOKIE_SNGQ_STRING)
		{
			if (pszChars[I] == '\'')
			{
				dwCookie &= ~COOKIE_SNGQ_STRING;
				bRedefineBlock = TRUE;
			}
			continue;
		}

		//	comment <!--....-->
		if (dwCookie & COOKIE_COMMENT)
		{
			if (I > 1 && strncmp(pszChars + I - 2, "-->", 3) == 0)
			{
				BOOL bOkay = TRUE;
				if (nLength > 4 && strncmp(pszChars + I - 4, "<!--", 4) == 0)bOkay = FALSE;
				if (nLength > 5 && strncmp(pszChars + I - 5, "<!--", 4) == 0)bOkay = FALSE;

				if (bOkay)
				{
					dwCookie &= ~COOKIE_COMMENT;
					bRedefineBlock = TRUE;
				}
			}
			continue;
		}

		//	< , >
		if (dwCookie & COOKIE_ELEMENT)
		{
			if (pszChars[I] == '>')
			{
				dwCookie &= ~COOKIE_ELEMENT & ~COOKIE_EL_NAME & ~COOKIE_EL_ATTRIB;
				DEFINE_BLOCK(I, COLORINDEX_EL_TAG);
				bRedefineBlock = TRUE;
			}
			//continue;
		}
		if (I > 2 && strncmp(pszChars + I - 4, "<!--", 4) == 0)
		{
			DEFINE_BLOCK(I - 3, COLORINDEX_COMMENT);
			//dwCookie &= ~COOKIE_ELEMENT & ~COOKIE_EL_NAME & ~COOKIE_EL_ATTRIB;
			dwCookie = COOKIE_COMMENT;
			bRedefineBlock = TRUE;
			continue;
		}
		if (pszChars[I] == '"')
		{
			DEFINE_BLOCK(I, COLORINDEX_STRING);
			dwCookie |= COOKIE_DBLQ_STRING;
			dwCookie &= ~COOKIE_EL_NAME;
			continue;
		}
		if (pszChars[I] == '\'')
		{
			DEFINE_BLOCK(I, COLORINDEX_STRING);
			dwCookie |= COOKIE_SNGQ_STRING;
			dwCookie &= ~COOKIE_EL_NAME;
			continue;
		}
		if (pszChars[I] == '<')
		{
			if (nLength >= 4 && strncmp(pszChars + I, "<!--", 4) == 0)
			{
				DEFINE_BLOCK(I, COLORINDEX_COMMENT);
				dwCookie |= COOKIE_COMMENT;
			}
			else
			{
				DEFINE_BLOCK(I, COLORINDEX_EL_TAG);
				dwCookie |= COOKIE_ELEMENT | COOKIE_EL_NAME;
			}
			continue;
		}
		if (dwCookie & COOKIE_ELEMENT)
		{
			if (pszChars[I] == '/' || pszChars[I] == '?')
			{
				DEFINE_BLOCK(I, COLORINDEX_EL_TAG);
				continue;
			}
			else if (isspace(pszChars[I]))
			{
				dwCookie &= ~COOKIE_EL_NAME;
			}
			else if (dwCookie & COOKIE_EL_NAME)
			{
				DEFINE_BLOCK(I, COLORINDEX_EL_NAME);
				dwCookie |= COOKIE_EL_NAME;
			}
			else
			{
				DEFINE_BLOCK(I, COLORINDEX_EL_ATTRIB);
				dwCookie &= ~COOKIE_EL_NAME;
				dwCookie |= COOKIE_EL_ATTRIB;
			}
			continue;
		}

		if (pBuf == NULL)
			continue;
	}

	//dwCookie &= COOKIE_ELEMENT | COOKIE_COMMENT | COOKIE_STRING;
					// items that can be on more than one line
	return dwCookie;
}