#ifndef __SYNTAXPARSERS_H_
#define __SYNTAXPARSERS_H_

#include "stdafx.h"

class CSourceView;
struct TEXTBLOCK;

typedef DWORD ParserFunc(CSourceView*, DWORD, int, TEXTBLOCK*, int&);
typedef ParserFunc *ParserFuncPtr;

ParserFunc ParseDefault;
ParserFunc ParseXML;
ParserFunc ParseCPP;
ParserFunc ParseASP;
ParserFunc ParseCSS;
ParserFunc ParseSQL;

BOOL InitParsers();
ParserFuncPtr LookupParser(LPCTSTR ext);

#endif