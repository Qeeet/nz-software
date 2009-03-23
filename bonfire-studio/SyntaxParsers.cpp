#include "stdafx.h"
#include "SyntaxParsers.h"
#include "Bonfire.h"
#include "SourceView.h"

extern CBonfireApp theApp;

CMapStringToPtr g_Parsers;

#define COOKIE_COMMENT			0x0001
#define COOKIE_EXT_COMMENT		0x0002
#define COOKIE_ELEMENT			0x0004
#define COOKIE_EL_NAME			0x0008
#define COOKIE_EL_ATTRIB		0x0010
#define COOKIE_STRING			0x0020
#define COOKIE_UNQ_STRING		COOKIE_STRING
#define COOKIE_SNGQ_STRING		0x0040
#define COOKIE_DBLQ_STRING		0x0080
#define COOKIE_PREPROCESSOR		0x0100
#define COOKIE_CHAR				0x0200
#define COOKIE_ASP				0x0400

// Initialize the mapping of file extensions to parse functions
BOOL InitParsers()
{
	return TRUE;
}

// return the function needed to parse a line of code for syuntax highlighting
ParserFuncPtr LookupParser(LPCTSTR ext)
{
	CString strExtList;

	size_t nExt = theApp.m_opOptions.views.vAssociations.size();
	
	// see if the extension is in the list
	while (nExt-- > 0) // 0 is the default file extension
	{
		strExtList = theApp.m_opOptions.views.vAssociations[nExt].strExtensions;
		strExtList.Remove(' ');

		if (IsStringPresent(strExtList,ext))
		{
			switch (theApp.m_opOptions.views.vAssociations[nExt].nHighlightType)
			{
				case SYNHIGHLIGHT_NONE: return ParseDefault;
				case SYNHIGHLIGHT_XML: return ParseXML;
				case SYNHIGHLIGHT_CPP: return ParseCPP;
				case SYNHIGHLIGHT_ASP: return ParseASP;
				case SYNHIGHLIGHT_CSS: return ParseCSS;
				case SYNHIGHLIGHT_SQL: return ParseSQL;
				default: return ParseDefault;
			}

			break;
		}
	}

	return ParseDefault;

	// lookup the function in the map of file extensions
	/*
	ParserFuncPtr func;
	if (!ext || !g_Parsers.Lookup(ext, (void*&)func))
		return ParseDefault;

	return func;*/
}

#define DEFINE_BLOCK(pos,colorindex)	\
	ASSERT((pos) >= 0 && (pos) <= nLength);\
	if (pBuf != NULL)\
	{\
		if (nActualItems == 0 || pBuf[nActualItems - 1].m_nCharPos <= (pos)){\
		pBuf[nActualItems].m_nCharPos = (pos);\
		pBuf[nActualItems].m_nColorIndex = (colorindex);\
		nActualItems++;}\
	}

// default parse function - no syntax coloring
DWORD ParseDefault(CSourceView* pView, DWORD dwCookie, int nLineIndex, TEXTBLOCK *pBuf, int &nActualItems)
{
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// xml
////////////////////////////////////////////////////////////////////////////////////////////////

DWORD ParseXML(CSourceView* pView, DWORD dwCookie, int nLineIndex, TEXTBLOCK *pBuf, int &nActualItems)
{
	//return 0;
	int nLength			= pView->GetLineLength(nLineIndex);
	LPCTSTR pszChars    = pView->GetLineChars(nLineIndex);
	BOOL bRedefineBlock = TRUE;

	char ch =0;
	for (int I = 0; ; I++)
	{
		ch = pszChars[I];
		DWORD comment = dwCookie & COOKIE_COMMENT;
		DWORD element = dwCookie & COOKIE_ELEMENT;
		DWORD el_name = dwCookie & COOKIE_EL_NAME;
		DWORD attrib = dwCookie & COOKIE_EL_ATTRIB;
		DWORD snstring = dwCookie & COOKIE_SNGQ_STRING;
		DWORD dbstring = dwCookie & COOKIE_DBLQ_STRING;
		DWORD uqstring = dwCookie & COOKIE_UNQ_STRING;

		if (bRedefineBlock)
		{
			if (dwCookie & COOKIE_COMMENT)
			{	DEFINE_BLOCK(I,COLORINDEX_COMMENT);							}
			else if (dwCookie & (COOKIE_DBLQ_STRING | COOKIE_SNGQ_STRING 
					| COOKIE_UNQ_STRING))
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
				dwCookie &= ~COOKIE_ELEMENT & ~COOKIE_EL_NAME & ~COOKIE_EL_ATTRIB
					& ~COOKIE_UNQ_STRING;
				DEFINE_BLOCK(I, COLORINDEX_EL_TAG);
				bRedefineBlock = TRUE;
			}
			if (pszChars[I] == '"')
			{
				DEFINE_BLOCK(I, COLORINDEX_STRING);
				dwCookie |= COOKIE_DBLQ_STRING;
				dwCookie &= ~COOKIE_EL_NAME;
				dwCookie &= ~COOKIE_UNQ_STRING;
				continue;
			}
			if (pszChars[I] == '\'')
			{
				DEFINE_BLOCK(I, COLORINDEX_STRING);
				dwCookie |= COOKIE_SNGQ_STRING;
				dwCookie &= ~COOKIE_EL_NAME;
				dwCookie &= ~COOKIE_UNQ_STRING;
				continue;
			}
			if (pszChars[I] == '=' && dwCookie & COOKIE_EL_ATTRIB)
			{
				dwCookie |= COOKIE_UNQ_STRING;
				dwCookie &= ~COOKIE_EL_NAME;
				bRedefineBlock = TRUE;
				continue;
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
				dwCookie &= ~COOKIE_EL_ATTRIB;
				dwCookie &= ~COOKIE_UNQ_STRING;
			}
			else if (dwCookie & COOKIE_EL_NAME)
			{
				DEFINE_BLOCK(I, COLORINDEX_EL_NAME);
				dwCookie |= COOKIE_EL_NAME;
			}
			else if (!(dwCookie & COOKIE_UNQ_STRING))
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

////////////////////////////////////////////////////////////////////////////////////////////////
// asp vb
////////////////////////////////////////////////////////////////////////////////////////////////

DWORD ParseASP(CSourceView* pView, DWORD dwCookie, int nLineIndex, TEXTBLOCK *pBuf, int &nActualItems)
{
	return ParseXML(pView, dwCookie, nLineIndex, pBuf, nActualItems);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// asp vb
////////////////////////////////////////////////////////////////////////////////////////////////

// element - everything inside { }, el_name - BODY, .MYCLASS, etc., attrib - font-family, etc., string - the value

// note: COOKIE_STRING in ParseCSS means the value of the attribute, so there can be both
// COOKIE_SNGQ_STRING and DBLQ_STRING at any point if a value is quoted

DWORD ParseCSS(CSourceView* pView, DWORD dwCookie, int nLineIndex, TEXTBLOCK *pBuf, int &nActualItems)
{
	int nLength			= pView->GetLineLength(nLineIndex);
	LPCTSTR pszChars    = pView->GetLineChars(nLineIndex);
	BOOL bRedefineBlock = TRUE;

	char ch =0;
	for (int I = 0; ; I++)
	{
		ch = pszChars[I];
		if (bRedefineBlock)
		{
			if (dwCookie & COOKIE_COMMENT)
			{	DEFINE_BLOCK(I,COLORINDEX_COMMENT);							}
			else if (dwCookie & (COOKIE_DBLQ_STRING | COOKIE_SNGQ_STRING 
					| COOKIE_STRING))
			{	DEFINE_BLOCK(I,COLORINDEX_STRING);							}
			else if (dwCookie & COOKIE_EL_ATTRIB)
			{	DEFINE_BLOCK(I,COLORINDEX_EL_ATTRIB);						}
			else if (dwCookie & COOKIE_ELEMENT)
			{	DEFINE_BLOCK(I,COLORINDEX_NORMALTEXT);						}
			else
			{	DEFINE_BLOCK(I,COLORINDEX_EL_NAME);							}

			bRedefineBlock = FALSE;
		}

		if (I == nLength)
			break;

		//	comment /*....*/
		if (dwCookie & COOKIE_COMMENT)
		{
			if (strncmp(pszChars + I - 1, "*/", 2) == 0)
			{
				if (!(nLength > 2 && strncmp(pszChars + I - 2, "/*", 2) == 0))
				{
					dwCookie &= ~COOKIE_COMMENT;
					bRedefineBlock = TRUE;
				}
			}
			continue;
		}

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

		//	{ , }
		if (dwCookie & COOKIE_ELEMENT)
		{
			if (pszChars[I] == '}')
			{
				dwCookie &= ~COOKIE_ELEMENT & ~COOKIE_EL_ATTRIB
					& ~COOKIE_STRING;
				DEFINE_BLOCK(I, COLORINDEX_NORMALTEXT);
				bRedefineBlock = TRUE;
			}
			if (pszChars[I] == '"')
			{
				DEFINE_BLOCK(I, COLORINDEX_STRING);
				dwCookie |= COOKIE_DBLQ_STRING;
				continue;
			}
			if (pszChars[I] == '\'')
			{
				DEFINE_BLOCK(I, COLORINDEX_STRING);
				dwCookie |= COOKIE_SNGQ_STRING;
				continue;
			}
			if (pszChars[I] == ':' && (dwCookie & COOKIE_EL_ATTRIB))
			{
				DEFINE_BLOCK(I, COLORINDEX_NORMALTEXT);
				dwCookie |= COOKIE_STRING;
				dwCookie &= ~COOKIE_EL_ATTRIB;
				bRedefineBlock = TRUE;
				continue;
			}
			if (pszChars[I] == ';' && !(dwCookie & COOKIE_EL_ATTRIB))
			{
				DEFINE_BLOCK(I, COLORINDEX_NORMALTEXT);
				dwCookie |= COOKIE_EL_ATTRIB;
				dwCookie &= ~COOKIE_STRING;
				bRedefineBlock = TRUE;
				continue;
			}
			//continue;
		}
		if (strncmp(pszChars + I - 1, "/*", 2) == 0)
		{
			DEFINE_BLOCK(I - 1, COLORINDEX_COMMENT);
			dwCookie |= COOKIE_COMMENT;
			bRedefineBlock = TRUE;
			continue;
		}

		if (pszChars[I] == '{')
		{
			dwCookie &= ~COOKIE_EL_NAME;
			DEFINE_BLOCK(I, COLORINDEX_NORMALTEXT);
			dwCookie |= COOKIE_ELEMENT;
			bRedefineBlock = TRUE;
			continue;
		}
		if (dwCookie & COOKIE_ELEMENT)
		{
			if (dwCookie & COOKIE_EL_ATTRIB)
			{
				DEFINE_BLOCK(I, COLORINDEX_EL_ATTRIB);
				dwCookie |= COOKIE_EL_ATTRIB;
			}
			else if (!(dwCookie & COOKIE_STRING))
			{
				DEFINE_BLOCK(I, COLORINDEX_EL_ATTRIB);
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

////////////////////////////////////////////////////////////////////////////////////////////////
// c++
////////////////////////////////////////////////////////////////////////////////////////////////

//	C++ keywords (MSVC5.0 + POET5.0)
static LPTSTR s_apszCppKeywordList[] =
{
	_T("__asm"),
	_T("enum"),
	_T("__multiple_inheritance"),
	_T("template"),
	_T("auto"),
	_T("__except"),
	_T("__single_inheritance"),
	_T("this"),
	_T("__based"),
	_T("explicit"),
	_T("__virtual_inheritance"),
	_T("thread"),
	_T("bool"),
	_T("extern"),
	_T("mutable"),
	_T("throw"),
	_T("break"),
	_T("false"),
	_T("naked"),
	_T("true"),
	_T("case"),
	_T("__fastcall"),
	_T("namespace"),
	_T("try"),
	_T("catch"),
	_T("__finally"),
	_T("new"),
	_T("__try"),
	_T("__cdecl"),
	_T("float"),
	_T("operator"),
	_T("typedef"),
	_T("char"),
	_T("for"),
	_T("private"),
	_T("typeid"),
	_T("class"),
	_T("friend"),
	_T("protected"),
	_T("typename"),
	_T("const"),
	_T("goto"),
	_T("public"),
	_T("union"),
	_T("const_cast"),
	_T("if"),
	_T("register"),
	_T("unsigned"),
	_T("continue"),
	_T("inline"),
	_T("reinterpret_cast"),
	_T("using"),
	_T("__declspec"),
	_T("__inline"),
	_T("return"),
	_T("uuid"),
	_T("default"),
	_T("int"),
	_T("short"),
	_T("__uuidof"),
	_T("delete"),
	_T("__int8"),
	_T("signed"),
	_T("virtual"),
	_T("dllexport"),
	_T("__int16"),
	_T("sizeof"),
	_T("void"),
	_T("dllimport"),
	_T("__int32"),
	_T("static"),
	_T("volatile"),
	_T("do"),
	_T("__int64"),
	_T("static_cast"),
	_T("wmain"),
	_T("double"),
	_T("__leave"),
	_T("__stdcall"),
	_T("while"),
	_T("dynamic_cast"),
	_T("long"),
	_T("struct"),
	_T("xalloc"),
	_T("else"),
	_T("main"),
	_T("switch"),
	_T("interface"),
	//	Added by a.s.
	_T("persistent"),
	_T("_persistent"),
	_T("transient"),
	_T("depend"),
	_T("ondemand"),
	_T("transient"),
	_T("cset"),
	_T("useindex"),
	_T("indexdef"),
	NULL
};

static BOOL IsCppKeyword(LPCTSTR pszChars, int nLength)
{
	for (int L = 0; s_apszCppKeywordList[L] != NULL; L ++)
	{
		if (strncmp(s_apszCppKeywordList[L], pszChars, nLength) == 0
				&& s_apszCppKeywordList[L][nLength] == 0)
			return TRUE;
	}
	return FALSE;
}

static BOOL IsCppNumber(LPCTSTR pszChars, int nLength)
{
	if (nLength > 2 && pszChars[0] == '0' && pszChars[1] == 'x')
	{
		for (int I = 2; I < nLength; I++)
		{
			if (isdigit(pszChars[I]) || (pszChars[I] >= 'A' && pszChars[I] <= 'F') ||
										(pszChars[I] >= 'a' && pszChars[I] <= 'f'))
				continue;
			return FALSE;
		}
		return TRUE;
	}
	if (! isdigit(pszChars[0]))
		return FALSE;
	for (int I = 1; I < nLength; I++)
	{
		if (! isdigit(pszChars[I]) && pszChars[I] != '+' &&
			pszChars[I] != '-' && pszChars[I] != '.' && pszChars[I] != 'e' &&
			pszChars[I] != 'E')
			return FALSE;
	}
	return TRUE;
}

DWORD ParseCPP(CSourceView* pView, DWORD dwCookie, int nLineIndex, TEXTBLOCK *pBuf, int &nActualItems)
{
	int nLength = pView->GetLineLength(nLineIndex);
	if (nLength <= 0)
		return dwCookie & COOKIE_EXT_COMMENT;

	LPCTSTR pszChars    = pView->GetLineChars(nLineIndex);
	BOOL bFirstChar     = (dwCookie & ~COOKIE_EXT_COMMENT) == 0;
	BOOL bRedefineBlock = TRUE;
	BOOL bDecIndex  = FALSE;
	int nIdentBegin = -1;
	for (int I = 0; ; I++)
	{
		if (bRedefineBlock)
		{
			int nPos = I;
			if (bDecIndex)
				nPos--;
			if (dwCookie & (COOKIE_COMMENT | COOKIE_EXT_COMMENT))
			{
				DEFINE_BLOCK(nPos, COLORINDEX_COMMENT);
			}
			else
			if (dwCookie & (COOKIE_CHAR | COOKIE_STRING))
			{
				DEFINE_BLOCK(nPos, COLORINDEX_STRING);
			}
			else
			if (dwCookie & COOKIE_PREPROCESSOR)
			{
				DEFINE_BLOCK(nPos, COLORINDEX_PREPROCESSOR);
			}
			else
			{
				DEFINE_BLOCK(nPos, COLORINDEX_NORMALTEXT);
			}
			bRedefineBlock = FALSE;
			bDecIndex      = FALSE;
		}

		if (I == nLength)
			break;

		if (dwCookie & COOKIE_COMMENT)
		{
			DEFINE_BLOCK(I, COLORINDEX_COMMENT);
			dwCookie |= COOKIE_COMMENT;
			break;
		}

		//	String constant "...."
		if (dwCookie & COOKIE_STRING)
		{
			if (pszChars[I] == '"' && (I == 0 || pszChars[I - 1] != '\\'))
			{
				dwCookie &= ~COOKIE_STRING;
				bRedefineBlock = TRUE;
			}
			continue;
		}

		//	Char constant '..'
		if (dwCookie & COOKIE_CHAR)
		{
			if (pszChars[I] == '\'' && (I == 0 || pszChars[I - 1] != '\\'))
			{
				dwCookie &= ~COOKIE_CHAR;
				bRedefineBlock = TRUE;
			}
			continue;
		}

		//	Extended comment /*....*/
		if (dwCookie & COOKIE_EXT_COMMENT)
		{
			if (I > 0 && pszChars[I] == '/' && pszChars[I - 1] == '*')
			{
				dwCookie &= ~COOKIE_EXT_COMMENT;
				bRedefineBlock = TRUE;
			}
			continue;
		}

		if (I > 0 && pszChars[I] == '/' && pszChars[I - 1] == '/')
		{
			DEFINE_BLOCK(I - 1, COLORINDEX_COMMENT);
			dwCookie |= COOKIE_COMMENT;
			break;
		}

		//	Preprocessor directive #....
		if (dwCookie & COOKIE_PREPROCESSOR)
		{
			if (I > 0 && pszChars[I] == '*' && pszChars[I - 1] == '/')
			{
				DEFINE_BLOCK(I - 1, COLORINDEX_COMMENT);
				dwCookie |= COOKIE_EXT_COMMENT;
			}
			continue;
		}

		//	Normal text
		if (pszChars[I] == '"')
		{
			DEFINE_BLOCK(I, COLORINDEX_STRING);
			dwCookie |= COOKIE_STRING;
			continue;
		}
		if (pszChars[I] == '\'')
		{
			DEFINE_BLOCK(I, COLORINDEX_STRING);
			dwCookie |= COOKIE_CHAR;
			continue;
		}
		if (I > 0 && pszChars[I] == '*' && pszChars[I - 1] == '/')
		{
			DEFINE_BLOCK(I - 1, COLORINDEX_COMMENT);
			dwCookie |= COOKIE_EXT_COMMENT;
			continue;
		}

		if (bFirstChar)
		{
			if (pszChars[I] == '#')
			{
				DEFINE_BLOCK(I, COLORINDEX_PREPROCESSOR);
				dwCookie |= COOKIE_PREPROCESSOR;
				continue;
			}
			if (! isspace(pszChars[I]))
				bFirstChar = FALSE;
		}

		if (pBuf == NULL)
			continue;	//	We don't need to extract keywords,
						//	for faster parsing skip the rest of loop

		if (isalnum(pszChars[I]) || pszChars[I] == '_' || pszChars[I] == '.')
		{
			if (nIdentBegin == -1)
				nIdentBegin = I;
		}
		else
		{
			if (nIdentBegin >= 0)
			{
				if (IsCppKeyword(pszChars + nIdentBegin, I - nIdentBegin))
				{
					DEFINE_BLOCK(nIdentBegin, COLORINDEX_KEYWORD);
				}
				else
				if (IsCppNumber(pszChars + nIdentBegin, I - nIdentBegin))
				{
					DEFINE_BLOCK(nIdentBegin, COLORINDEX_NUMBER);
				}
				bRedefineBlock = TRUE;
				bDecIndex = TRUE;
				nIdentBegin = -1;
			}
		}
	}

	if (nIdentBegin >= 0)
	{
		if (IsCppKeyword(pszChars + nIdentBegin, I - nIdentBegin))
		{
			DEFINE_BLOCK(nIdentBegin, COLORINDEX_KEYWORD);
		}
		else
		if (IsCppNumber(pszChars + nIdentBegin, I - nIdentBegin))
		{
			DEFINE_BLOCK(nIdentBegin, COLORINDEX_NUMBER);
		}
	}

	if (pszChars[nLength - 1] != '\\')
		dwCookie &= COOKIE_EXT_COMMENT;
	return dwCookie;
}

//SQL keywords
static LPTSTR s_apszSqlKeywordList[] =
{
	_T("ALL"),
	_T("ALTER"),
	_T("AND"),
	_T("ANY"),
	_T("BACKUP"),
	_T("BEGIN"),
	_T("END"),
	_T("BETWEEN"),
	_T("BIGINT"),
	_T("BINARY"),
	_T("BIT"),
	_T("BREAK"),
	_T("BULK"),
	_T("BY"),
	_T("CHAR"),
	_T("CHECKPOINT"),
	_T("CLOSE"),
	_T("COLLATE"),
	_T("COMMIT"),
	_T("CONTAINS"),
	_T("CONTAINSTABLE"),
	_T("CONTINUE"),
	_T("CREATE"),
	_T("CURSOR"),
	_T("DATABASE"),
	_T("DATETIME"),
	_T("DBCC"),
	_T("DEALLOCATE"),
	_T("DECIMAL"),
	_T("DECLARE"),
	_T("DEFAULT"),
	_T("DELETE"),
	_T("DENY"),
	_T("DISTRIBUTED"),
	_T("DROP"),
	_T("DUMP"),
	_T("ELSE"),
	_T("EXECUTE"),
	_T("EXEC"),
	_T("EXISTS"),
	_T("FETCH"),
	_T("FILELISTONLY"),
	_T("FLOAT"),
	_T("FREETEXT"),
	_T("FREETEXTTABLE"),
	_T("FROM"),
	_T("GO"),
	_T("GOTO"),
	_T("GRANT"),
	_T("GROUP"),
	_T("GROUPING"),
	_T("HAVING"),
	_T("HEADERONLY"),
	_T("IDENTITY"),
	_T("IF"),
	_T("IMAGE"),
	_T("IN"),
	_T("INDEX"),
	_T("INSERT"),
	_T("INT"),
	_T("INTO"),
	_T("IS"),
	_T("KILL"),
	_T("LABELONLY"),
	_T("LIKE"),
	_T("LOAD"),
	_T("MAX"),
	_T("MIN"),
	_T("MONEY"),
	_T("NCHAR"),
	_T("NOT"),
	_T("NOCOUNT"),
	_T("NTEXT"),
	_T("NUMERIC"),
	_T("NVARCHAR"),
	_T("OPEN"),
	_T("OPENDATASOURCE"),
	_T("OPENQUERY"),
	_T("OPENROWSET"),
	_T("OPENXML"),
	_T("OR"),
	_T("ORDER"),
	_T("PRINT"),
	_T("PROCEDURE"),
	_T("RAISERROR"),
	_T("READTEXT"),
	_T("REAL"),
	_T("RECONFIGURE"),
	_T("RESTORE"),
	_T("RETURN"),
	_T("REVOKE"),
	_T("ROLLBACK"),
	_T("RULE"),
	_T("SAVE"),
	_T("SCHEMA"),
	_T("SELECT"),
	_T("SET"),
	_T("SETUSER"),
	_T("SHUTDOWN"),
	_T("SMALLDATETIME"),
	_T("SMALLINT"),
	_T("SMALLMONEY"),
	_T("SOME"),
	_T("SQL_VARIANT"),
	_T("STATISTICS"),
	_T("TABLE"),
	_T("TEXT"),
	_T("TIMESTAMP"),
	_T("TINYINT"),
	_T("TRUNCATE"),
	_T("TRANSACTION"),
	_T("TRIGGER"),
	_T("UNION"),
	_T("UNIQUEIDENTIFIER"),
	_T("UPDATE"),
	_T("UPDATETEXT"),
	_T("USE"),
	_T("VARBINARY"),
	_T("VARCHAR"),
	_T("VERIFYONLY"),
	_T("VIEW"),
	_T("WAITFOR"),
	_T("WHERE"),
	_T("WHILE"),
	_T("WORK"),
	_T("WRITETEXT"),
	NULL
};

//SQL functions
static LPTSTR s_apszSqlFunctionList[] =
{
	_T("@@CONNECTIONS"),	_T("@@connections"),
	_T("@@CPU_BUSY"),		_T("@@cpu_busy"),
	_T("@@CURSOR_ROWS"),	_T("@@cursor_rows"),
	_T("@@DATEFIRST"),		_T("@@datefirst"),
	_T("@@DBTS"),			_T("@@dbts"),
	_T("@@ERROR"),			_T("@@error"),
	_T("@@FETCH_STATUS"),	_T("@@fetch_status"),
	_T("@@IDENTITY"),		_T("@@identity"),
	_T("@@IDLE"),			_T("@@idle"),
	_T("@@IO_BUSY"),		_T("@@io_busy"),
	_T("@@LANGID"),			_T("@@langid"),
	_T("@@LANGUAGE"),		_T("@@language"),
	_T("@@LOCK_TIMEOUT"),	_T("@@lock_timeout"),
	_T("@@MAX_CONNECTIONS"),_T("@@max_connections"),
	_T("@@MAX_PRECISION"),	_T("@@max_precision"),
	_T("@@NESTLEVEL"),		_T("@@nestlevel"),
	_T("@@OPTIONS"),		_T("@@options"),
	_T("@@PACK_RECEIVED"),	_T("@@pack_received"),
	_T("@@PACK_SENT"),		_T("@@pack_sent"),
	_T("@@PACKET_ERRORS"),	_T("@@packet_errors"),
	_T("@@PROCID"),			_T("@@procid"),
	_T("@@REMSERVER"),		_T("@@remserver"),
	_T("@@ROWCOUNT"),		_T("@@rowcount"),
	_T("@@SERVERNAME"),		_T("@@servername"),
	_T("@@SERVICENAME"),	_T("@@servicename"),
	_T("@@SPID"),			_T("@@spid"),
	_T("@@TEXTSIZE"),		_T("@@textsize"),
	_T("@@TIMETICKS"),		_T("@@timeticks"),
	_T("@@TOTAL_ERRORS"),	_T("@@total_errors"),
	_T("@@TOTAL_READ"),		_T("@@total_read"),
	_T("@@TOTAL_WRITE"),	_T("@@total_write"),
	_T("@@TRANCOUNT"),		_T("@@trancount"),
	_T("@@VERSION"),		_T("@@version"),
	_T("ABS"),
	_T("ACOS"),
	_T("APP_NAME"),
	_T("ASCII"),
	_T("ASIN"),
	_T("ATAN"),
	_T("ATN2"),
	_T("AVG"),
	_T("BINARY_CHECKSUM"),
	_T("CASE"),
	_T("CAST"),
	_T("CONVERT"),
	_T("CEILING"),
	_T("CHARINDEX"),
	_T("CHECKSUM"),
	_T("CHECKSUM_AGG"),
	_T("COALESCE"),
	_T("COLLATIONPROPERTY"),
	_T("COL_LENGTH"),
	_T("COL_NAME"),
	_T("COLUMNPROPERTY"),
	_T("COS"),
	_T("COT"),
	_T("COUNT"),
	_T("COUNT_BIG"),
	_T("CURRENT_TIMESTAMP"),
	_T("CURRENT_USER"),
	_T("CURSOR_STATUS"),
	_T("DATABASEPROPERTY"),
	_T("DATABASEPROPERTYEX"),
	_T("DATALENGTH"),
	_T("DATEADD"),
	_T("DATEDIFF"),
	_T("DATENAME"),
	_T("DATEPART"),
	_T("DAY"),
	_T("DB_ID"),
	_T("DB_NAME"),
	_T("DEGREES"),
	_T("DIFFERENCE"),
	_T("EXP"),
	_T("FILE_ID"),
	_T("FILE_NAME"),
	_T("FILEGROUP_ID"),
	_T("FILEGROUP_NAME"),
	_T("FILEGROUPPROPERTY"),
	_T("FILEPROPERTY"),
	_T("FLOOR"),
	_T("FORMATMESSAGE"),
	_T("FULLTEXTCATALOGPROPERTY"),
	_T("FULLTEXTSERVICEPROPERTY"),
	_T("GETANSINULL"),
	_T("GETDATE"),
	_T("GETUTCDATE"),
	_T("HAS_DBACCESS"),
	_T("HOST_ID"),
	_T("HOST_NAME"),
	_T("IDENT_CURRENT"),
	_T("IDENT_INCR"),
	_T("IDENT_SEED"),
	_T("INDEXKEY_PROPERTY"),
	_T("INDEXPROPERTY"),
	_T("INDEX_COL"),
	_T("IS_MEMBER"),
	_T("IS_SRVROLEMEMBER"),
	_T("ISDATE"),
	_T("ISNULL"),
	_T("ISNUMERIC"),
	_T("LEFT"),
	_T("LEN"),
	_T("LOG"),
	_T("LOG10"),
	_T("LOWER"),
	_T("LTRIM"),
	_T("MONTH"),
	_T("NEWID"),
	_T("NULLIF"),
	_T("OBJECT_ID"),
	_T("OBJECT_NAME"),
	_T("OBJECTPROPERTY"),
	_T("PARSENAME"),
	_T("PATINDEX"),
	_T("PERMISSIONS"),
	_T("PI"),
	_T("POWER"),
	_T("QUOTENAME"),
	_T("RADIANS"),
	_T("RAND"),
	_T("REPLACE"),
	_T("REPLICATE"),
	_T("RIGHT"),
	_T("ROUND"),
	_T("ROWCOUNT_BIG"),
	_T("RTRIM"),
	_T("SCOPE_IDENTITY"),
	_T("SERVERPROPERTY"),
	_T("SESSION_USER"),
	_T("SESSIONPROPERTY"),
	_T("SIGN"),
	_T("SIN"),
	_T("SOUNDEX"),
	_T("SPACE"),
	_T("SQL_VARIANT_PROPERTY"),
	_T("SQUARE"),
	_T("SQRT"),
	_T("STATS_DATE"),
	_T("STDEV"),
	_T("STDEVP"),
	_T("STR"),
	_T("STUFF"),
	_T("SUBSTRING"),
	_T("SUM"),
	_T("SUSER_ID"),
	_T("SUSER_NAME"),
	_T("SUSER_SID"),
	_T("SUSER_SNAME"),
	_T("SYSTEM"),
	_T("SYSTEM_USER"),
	_T("TAN"),
	_T("TEXTPTR"),
	_T("TEXTVALID"),
	_T("TRIGGER_NESTLEVEL"),
	_T("TYPEPROPERTY"),
	_T("UNICODE"),
	_T("UPPER"),
	_T("USER"),
	_T("USER_ID"),
	_T("USER_NAME"),
	_T("VAR"),
	_T("VARP"),
	_T("YEAR"),
	NULL
};

static BOOL IsSqlKeyword(LPCTSTR pszChars, int nLength)
{
	BOOL retVal = FALSE;
	char* temp	= (char*)malloc(nLength);
	temp		= strncpy(temp, pszChars, nLength);
	temp		= _strupr(temp);
	
	for (int L = 0; s_apszSqlKeywordList[L] != NULL; L ++)
	{
		if (strncmp(s_apszSqlKeywordList[L], temp, nLength) == 0
				&& s_apszSqlKeywordList[L][nLength] == 0)
		{
			retVal = TRUE;
			break;
		}
	}
	free(temp);
	return retVal;
}

static BOOL IsSqlFunction(LPCTSTR pszChars, int nLength)
{
	for (int L = 0; s_apszSqlFunctionList[L] != NULL; L ++)
	{
		if (strncmp(s_apszSqlFunctionList[L], pszChars, nLength) == 0
				&& s_apszSqlFunctionList[L][nLength] == 0)
			return TRUE;
	}
	return FALSE;
}

DWORD ParseSQL(CSourceView* pView, DWORD dwCookie, int nLineIndex, TEXTBLOCK *pBuf, int &nActualItems)
{
	int nLength = pView->GetLineLength(nLineIndex);
	if (nLength <= 0)
		return dwCookie & COOKIE_EXT_COMMENT;

	LPCTSTR pszChars    = pView->GetLineChars(nLineIndex);
	BOOL bFirstChar     = (dwCookie & ~COOKIE_EXT_COMMENT) == 0;
	BOOL bRedefineBlock = TRUE;
	BOOL bDecIndex  = FALSE;
	int nIdentBegin = -1;
	for (int I = 0; ; I++)
	{
		if (bRedefineBlock)
		{
			int nPos = I;
			if (bDecIndex)
				nPos--;
			
			if (dwCookie & (COOKIE_COMMENT | COOKIE_EXT_COMMENT))
			{
				DEFINE_BLOCK(nPos, COLORINDEX_COMMENT);
			}
			else if (dwCookie & COOKIE_STRING)
			{
				DEFINE_BLOCK(nPos, COLORINDEX_STRING);
			}
			else
			{
				DEFINE_BLOCK(nPos, COLORINDEX_NORMALTEXT);
			}
			bRedefineBlock = FALSE;
			bDecIndex      = FALSE;
		}

		if (I == nLength)
			break;

		if (dwCookie & COOKIE_COMMENT)
		{
			DEFINE_BLOCK(I, COLORINDEX_COMMENT);
			dwCookie |= COOKIE_COMMENT;
			break;
		}

		//	String constant "...."
		if (dwCookie & COOKIE_STRING)
		{
			if (pszChars[I] == '\'' && (I == 0 || pszChars[I - 1] != '\\'))
			{
				dwCookie &= ~COOKIE_STRING;
				bRedefineBlock = TRUE;
			}
			continue;
		}

		//	Extended comment /*....*/
		if (dwCookie & COOKIE_EXT_COMMENT)
		{
			if (I > 0 && pszChars[I] == '/' && pszChars[I - 1] == '*')
			{
				dwCookie &= ~COOKIE_EXT_COMMENT;
				bRedefineBlock = TRUE;
			}
			continue;
		}

		if (I > 0 && pszChars[I] == '-' && pszChars[I - 1] == '-')
		{
			DEFINE_BLOCK(I - 1, COLORINDEX_COMMENT);
			dwCookie |= COOKIE_COMMENT;
			break;
		}

		//	Normal text
		if (pszChars[I] == '\'')
		{
			DEFINE_BLOCK(I, COLORINDEX_STRING);
			dwCookie |= COOKIE_CHAR;
			continue;
		}
		if (I > 0 && pszChars[I] == '*' && pszChars[I - 1] == '/')
		{
			DEFINE_BLOCK(I - 1, COLORINDEX_COMMENT);
			dwCookie |= COOKIE_EXT_COMMENT;
			continue;
		}

		if (bFirstChar && ! isspace(pszChars[I]))
			bFirstChar = FALSE;

		if (pBuf == NULL)
			continue;	//	We don't need to extract keywords,
						//	for faster parsing skip the rest of loop

		if (isalnum(pszChars[I]) || pszChars[I] == '@' || pszChars[I] == '_')
		{
			if (nIdentBegin == -1)
				nIdentBegin = I;
		}
		else
		{
			if (nIdentBegin >= 0)
			{
				if (IsSqlKeyword(pszChars + nIdentBegin, I - nIdentBegin))
				{
					DEFINE_BLOCK(nIdentBegin, COLORINDEX_KEYWORD);
				}
				else if (IsSqlFunction(pszChars + nIdentBegin, I - nIdentBegin))
				{
					DEFINE_BLOCK(nIdentBegin, COOKIE_PREPROCESSOR);
				}
				bRedefineBlock = TRUE;
				bDecIndex = TRUE;
				nIdentBegin = -1;
			}
		}
	}

	if (nIdentBegin >= 0)
	{
		if (IsSqlKeyword(pszChars + nIdentBegin, I - nIdentBegin))
		{
			DEFINE_BLOCK(nIdentBegin, COLORINDEX_KEYWORD);
		}
		else if (IsSqlFunction(pszChars + nIdentBegin, I - nIdentBegin))
		{
			DEFINE_BLOCK(nIdentBegin, COOKIE_PREPROCESSOR);
		}
	}

	if (dwCookie & ~COOKIE_EXT_COMMENT)
	{
		dwCookie = 0;
		bRedefineBlock = TRUE;
	}

	return dwCookie;
}