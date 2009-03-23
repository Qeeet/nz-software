#include "stdafx.h"
#include "MiniCalc.h"

int floor(int n, int step)
{
	int j = n % step;
	if (j == 0)
		return n;

	return (n >= 0) ? (n - j) : (n - (step + j));
}

int ceil(int n, int step)
{
	int j = n % step;
	if (j == 0)
		return n;

	return (n < 0) ? (n - j) : (n - j + step);
}

std::string GetFilePath(const std::string& s)
{
	size_t n = s.rfind('\\');
	if (n != std::string::npos)
		return s.substr(0,n + 1); // include '\\' in result
	else
		return s;
}

BOOL InitSymbols()
{
	// init trigonometric function factors
	if (g_arrProgData[PROGDATA_DEGREES])
	{	g_eval.SetTrigFactor(DefaultSymbols::Pi / 180.0); }
	else
	{	g_eval.SetTrigFactor(1.0); }

	g_eval.GetSymbolList()->Set(new Symbol(SYMBOL_VARIABLE,"ans",NULL),false);
	g_eval.GetSymbolList()->Set(new Symbol(SYMBOL_INT_FUNC,"clear",NULL,ClearStatement),false);
	g_eval.GetSymbolList()->Set(new Symbol(SYMBOL_INT_FUNC,"print",NULL,PrintStatement),false);
	g_eval.GetSymbolList()->Set(new Symbol(SYMBOL_INT_FUNC,"delete",NULL,DeleteStatement),false);

	return TRUE;
}

void ProcessInput(LPCSTR strText)
{
	char str[256];
	size_t nLength = strlen(strText);

	if (!nLength)
		return;

	std::vector<Token> tokens;
	int nEnd = -1;

	g_eval.GenerateTree(strText);
	if (!CALCSUCCESS(g_eval.GetLastReturn()))
	{
		AddOutput(strText,FALSE,OUTSTYLE_DEFAULT);

		AddReturnOutput(strText,g_eval.GetLastReturn(),FALSE);
		return;
	}

	Expression* pEqualExpr = g_eval.GetEqualExpression();
	if (pEqualExpr)
	{
		AddOutput(strText,FALSE,OUTSTYLE_DEFAULT);

		if (pEqualExpr->GetChildList()->GetCount() == 0)
		{
			AddOutput("ERROR: Internal error.",FALSE,OUTSTYLE_ERROR);
			return;
		}
        
		int nType = pEqualExpr->GetChildList()->Get(0)->GetType();
		if (pEqualExpr->GetChildList()->GetCount() != 1)
		{
			AddOutput("ERROR: Statement error.",FALSE,OUTSTYLE_ERROR);
			return;
		}

		if (nType == EXPR_FUNCTION)
		{
			// defining a function
			Expression* pFn = pEqualExpr->GetChildList()->Get(0);
			const char* strName = pFn->GetString().c_str();

			Symbol* nf = new Symbol(SYMBOL_FUNC,strName);
			nf->SetFuncExpression(g_eval.GetRootExpression(),true);

			// check for possible recursion
			if (nf->GetFuncExpression()->FindFirstSymbolReference(strName))
			{
				AddOutput("ERROR: Circular function reference.",FALSE,OUTSTYLE_ERROR);
				return;
			}

			int nParams = (int)pFn->GetChildList()->GetCount();
			for (int i = 0; i < nParams; i ++)
			{
				Expression* pC = pFn->GetChildList()->Get(i); // group
				if (pC->GetChildList()->GetCount() != 1 ||
					pC->GetChildList()->Get(0)->GetType() != EXPR_VARIABLE)
				{
					AddOutput("ERROR: Statement error.",FALSE,OUTSTYLE_ERROR);
					return;
				}

				pC = pC->GetChildList()->Get(0); // literal string value (parameter name)
				nf->GetFuncParamList()->push_back(pC->GetString());
			}

			g_eval.GetSymbolList()->Set(nf,false);

			sprintf(str,"Function '%s' defined.",strName);
			AddOutput(str,TRUE,OUTSTYLE_SYSMESSAGE);
		}
		else if (nType == EXPR_VARIABLE)
		{
			g_eval.Evaluate();
			if (!CALCSUCCESS(g_eval.GetLastReturn()))
			{
				AddReturnOutput(strText,g_eval.GetLastReturn(),FALSE);
				return;
			}

			// defining a variable
			const char* cstr = pEqualExpr->GetChildList()->Get(0)->GetString().c_str();
			g_eval.GetSymbolList()->Set(
				new Symbol(SYMBOL_VARIABLE,cstr,&g_eval.GetLastReturn().value),false);

			sprintf(str,"Variable '%s' defined.",cstr);
			AddOutput(str,TRUE,OUTSTYLE_SYSMESSAGE);
		}
		else
		{
			AddOutput("ERROR: Statement error.",FALSE,OUTSTYLE_ERROR);
			return;
		}
	}
	else
	{
		g_eval.Evaluate();
		if (g_bHoldOutput)
		{
			g_bHoldOutput = FALSE; // cancel flag immediately after it is used
		}
		else
		{
			AddOutput(strText,FALSE,OUTSTYLE_DEFAULT);
			AddReturnOutput(strText,g_eval.GetLastReturn(),TRUE);
		}
	}
}

void AddReturnOutput(LPCSTR strSrc, const Return& ret, BOOL bSetAnsVar)
{
	char str[4096];

	if (CALCSUCCESS(ret))
	{
		std::string temp = ret.value.ToString();
		if (ret.status == CALC_DELETE_SUCCESS)
			AddOutput(("Symbol " + temp + " deleted.").c_str(),FALSE,OUTSTYLE_SYSMESSAGE);
		else
			AddOutput(temp.c_str(),FALSE,OUTSTYLE_RESULT);

		// str is the string version of the result
		if (g_arrProgData[PROGDATA_AUTO_COPY] && g_wndMain)
		{
			if (OpenClipboard(g_wndMain))
			{
				EmptyClipboard();

				HGLOBAL hClip = GlobalAlloc(GMEM_MOVEABLE,(int)temp.length() + 1);
				if (hClip)
				{
					char* pClip = (char*)GlobalLock(hClip);
					if (pClip)
						strcpy(pClip,temp.c_str());
					GlobalUnlock(hClip);
				}

				SetClipboardData(CF_TEXT,hClip);

				CloseClipboard();
			}
		}

		if (bSetAnsVar)
		{
			g_eval.GetSymbolList()->Set(new Symbol(SYMBOL_VARIABLE,"ans",&ret.value),false);
		}
	}
	else if (ret.status == CALC_NOSTATUS)
		AddOutput("ERROR: Internal error.",FALSE,OUTSTYLE_ERROR);
	else if (ret.status == CALC_ERR_SYNTAX)
	{
		sprintf(str,"%s %d '%c')","Syntax Error (Index",
			ret.nErrPos,strSrc[ret.nErrPos]);
		AddOutput(str,FALSE,OUTSTYLE_ERROR);
	}
	else if (ret.status == CALC_ERR_PAREN)
		AddOutput("Syntax Error: Parentheses don't match.",FALSE,OUTSTYLE_ERROR);
	else if (ret.status == CALC_ERR_INCOMPLETE)
		AddOutput("Syntax Error: Expression is incomplete.",FALSE,OUTSTYLE_ERROR);
	else if (ret.status == CALC_ERR_SYMBOL)
	{
		sprintf(str,"%s '%s'","Semantic Error: Undefined symbol",ret.badSymbol.c_str());
		AddOutput(str,FALSE,OUTSTYLE_ERROR);
	}
	else if (ret.status == CALC_ERR_INPUT)
		AddOutput("Semantic Error: Invalid function input.",FALSE,OUTSTYLE_ERROR);
	else if (ret.status == CALC_ERR_DATA_TYPE)
	{
		std::string dt1 = Calc::Value::GetValueTypeString(ret.nParam);
		std::string dt2 = ", " + Calc::Value::GetValueTypeString(ret.nParam2);

		sprintf(str,"%s (%s%s)","Data Type Error",dt1.c_str(),(ret.nParam2 == 0) ? "" : dt2.c_str());
		AddOutput(str,FALSE,OUTSTYLE_ERROR);
	}
	else if (ret.status == CALC_ERR_FUNCTION)
	{
		if (ret.nParam >= 0)
			sprintf(str,"Semantic Error: Function '%s' requires exactly %d parameter(s).",
				ret.badSymbol.c_str(),ret.nParam);
		else
			sprintf(str,"Semantic Error: Function '%s' requires at least %d parameter(s).",
				ret.badSymbol.c_str(),abs(ret.nParam));
		AddOutput(str,FALSE,OUTSTYLE_ERROR);
	}
	else if (ret.status == CALC_ERR_DOMAIN)
		AddOutput("Domain Error",FALSE,OUTSTYLE_ERROR);
}

void ClearStatement(Eval& v, Return& r, Value* p, int c)
{
	if (c != 0){ r.status = CALC_ERR_FUNCTION; r.nParam = 0; return; }

	if (g_wndOutput)
	{
		SetWindowText(g_wndOutput,NULL);
		SetWindowText(g_wndInput,NULL);
	}

	g_bHoldOutput = TRUE;
	r.status = CALC_SUCCESS;
	r.value = 0.0;
}

void DeleteStatement(Eval& v, Return& r, Value* p, int c)
{
	if (c != 1){ r.status = CALC_ERR_FUNCTION; r.nParam = 1; return; }
	if (p[0].GetType() != VALUE_STRING){ r.status = CALC_ERR_DATA_TYPE;
		r.nParam = p[0].GetType(); return; }

	// delete variable
	Symbol* pSymbol = g_eval.GetSymbolList()->Find(p[0].GetString());
	if (!pSymbol)
	{
		r.status = CALC_ERR_SYMBOL;
		r.badSymbol = p[0].GetString();
		return;
	}

	if (pSymbol->GetType() == SYMBOL_INT_FUNC || pSymbol->GetID() == "i" || pSymbol->GetID() == "e" ||
		pSymbol->GetID() == "pi")
	{
		r.status = CALC_ERR_INPUT;
		return;
	}

	g_eval.GetSymbolList()->Remove(pSymbol);

	//g_bHoldOutput = TRUE;
	r.status = CALC_DELETE_SUCCESS;
	r.value = p[0].GetString();
}

void PrintStatement(Eval& v, Return& r, Value* p, int c)
{
	if (c != 1){ r.status = CALC_ERR_FUNCTION; r.nParam = 1; return; }
	if (p[0].GetType() != VALUE_STRING){ r.status = CALC_ERR_DATA_TYPE;
		r.nParam = p[0].GetType(); return; }

	AddOutput(p[0].GetString().c_str(),TRUE,OUTSTYLE_UNDERLINE);

	g_bHoldOutput = TRUE;
	r.status = CALC_SUCCESS;
	r.value = 0.0;
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR lpCmdLine, int)
{
	{
		// get version info
		char strFile[MAX_PATH];
		GetModuleFileName(NULL,strFile,MAX_PATH);

		DWORD dwInfoSize = 0;
		if (dwInfoSize = GetFileVersionInfoSize(strFile,&dwInfoSize))
		{
			void* pInfo = malloc(dwInfoSize);

			if (GetFileVersionInfo(strFile,0,dwInfoSize,pInfo))
			{
				UINT nFFSize;
				VS_FIXEDFILEINFO* pVSFFI;
				if (VerQueryValue(pInfo,"\\",(LPVOID*)&pVSFFI,&nFFSize))
				{
					char strTemp[10];
					struct
					{
						UINT minor : 16;
						UINT major : 16;
					} version;

					struct
					{
						UINT temp : 16;
						UINT build : 16;
					} version2;

					memcpy(&version,&pVSFFI->dwProductVersionMS,
						sizeof(pVSFFI->dwProductVersionMS));
					memcpy(&version2,&pVSFFI->dwProductVersionLS,
						sizeof(pVSFFI->dwProductVersionLS));

					sprintf(strTemp,"%u.%u",version.major,version.minor);
					g_strShortVersion = strTemp;
					g_strLongVersion = strTemp;
					sprintf(strTemp,".%u",version2.build);
					g_strLongVersion += strTemp;
				}
			}

			free(pInfo);
		}
	}

	ZeroMemory(g_arrProgData,sizeof(g_arrProgData));
	ZeroMemory(g_arrLoadedLibs,sizeof(g_arrLoadedLibs));
	ZeroMemory(&g_wpOrig,sizeof(g_wpOrig));
	g_hInstance = hInstance;

	g_imlSymbolIcons = ImageList_LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_SYMBOL_ICONS),
		16,0,RGB(255,0,255));

	if (!LoadSettings())
		return FALSE;

	g_eval.AddDefaultSymbols();

	if (!InitSymbols())
		return FALSE;

	g_bHoldOutput = FALSE;

	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(icex);
	icex.dwICC = ICC_LISTVIEW_CLASSES;
	if (!InitCommonControlsEx(&icex))
		return FALSE;

	if (!RegisterClasses())
		return FALSE;

	if (!InitInstance())
		return FALSE;

	HACCEL hAccelTable = LoadAccelerators(hInstance,MAKEINTRESOURCE(IDC_MINICALC));

	MSG msg;
	while (GetMessage(&msg,NULL,0,0))
	{
		if (!TranslateAccelerator(g_wndMain,hAccelTable,&msg) &&
			!IsDialogMessage(g_wndMain,&msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	// SaveSettings occurs before window is destroyed
	ExitInstance();

	return (int)msg.wParam;
}

BOOL LoadSettings()
{
	HKEY hKey = NULL;
	if (RegOpenKeyEx(HKEY_CURRENT_USER,"Software\\NZ Software\\MiniCalc",0,
		GENERIC_READ,&hKey) != ERROR_SUCCESS)
		return TRUE; // don't stop program from loading
	
	BOOL bFlag; DWORD dwSize = sizeof(bFlag);
	if (RegQueryValueEx(hKey,"AlwaysOnTop",NULL,NULL,(BYTE*)&bFlag,&dwSize) == ERROR_SUCCESS)
		g_arrProgData[PROGDATA_ALWAYS_ON_TOP] = bFlag;
	
	dwSize = sizeof(bFlag);
	if (RegQueryValueEx(hKey,"Translucent",NULL,NULL,(BYTE*)&bFlag,&dwSize) == ERROR_SUCCESS)
		g_arrProgData[PROGDATA_TRANSLUCENT] = bFlag;
	
	dwSize = sizeof(bFlag);
	if (RegQueryValueEx(hKey,"AutoCopy",NULL,NULL,(BYTE*)&bFlag,&dwSize) == ERROR_SUCCESS)
		g_arrProgData[PROGDATA_AUTO_COPY] = bFlag;
	
	dwSize = sizeof(bFlag);
	if (RegQueryValueEx(hKey,"Degrees",NULL,NULL,(BYTE*)&bFlag,&dwSize) == ERROR_SUCCESS)
		g_arrProgData[PROGDATA_DEGREES] = bFlag;

	dwSize = sizeof(g_wpOrig);
	if (RegQueryValueEx(hKey,"WindowPlacement",NULL,NULL,(BYTE*)&g_wpOrig,&dwSize) != ERROR_SUCCESS)
		ZeroMemory(&g_wpOrig,sizeof(g_wpOrig));

	dwSize = sizeof(bFlag);
	if (RegQueryValueEx(hKey,"SaveSymbols",NULL,NULL,(BYTE*)&bFlag,&dwSize) == ERROR_SUCCESS)
	{
		g_arrProgData[PROGDATA_SAVE_SYMBOLS] = bFlag;

		// open the default saved symbols file "symbols.dat"
		char str[MAX_PATH * 2];
		GetModuleFileName(NULL,str,sizeof(str));
		std::string strFile = GetFilePath(str) + "symbols.dat";
		HANDLE hFile = CreateFile(strFile.c_str(),GENERIC_READ,FILE_SHARE_READ,NULL,
			OPEN_EXISTING,0,NULL);

		if (hFile != INVALID_HANDLE_VALUE)
		{
			DWORD dwSize2;
			DWORD nVal, nVal2;
			while (1)
			{
				// read symbol id's length
				if (!ReadFile(hFile,&nVal,sizeof(nVal),&dwSize2,NULL) ||
					dwSize2 != sizeof(nVal))
					break;

				if (!ReadFile(hFile,str,nVal,&dwSize2,NULL) ||
					dwSize2 != nVal)
					break;

				str[nVal] = 0; // null-terminate

				if (!ReadFile(hFile,&nVal,sizeof(nVal),&dwSize2,NULL) ||
					dwSize2 != sizeof(nVal))
					break;

				Symbol* temp = new Symbol(nVal,str);

				if (nVal == SYMBOL_VARIABLE)
				{
					if (!ReadFile(hFile,&nVal,sizeof(nVal),&dwSize2,NULL) ||
						dwSize2 != sizeof(nVal))
						break;

					if (nVal == 0) // if it's 0, a decimal value follows
					{
						if (!ReadFile(hFile,str,sizeof(decimal_t),&dwSize2,NULL) ||
							dwSize2 != sizeof(decimal_t))
							break;

						temp->SetValue(Value(*(decimal_t*)str));
					}
					else if (nVal == -1) // if it's -1, a complex value follows
					{
						if (!ReadFile(hFile,str,sizeof(decimal_t),&dwSize2,NULL) ||
							dwSize2 != sizeof(decimal_t))
							break;
						decimal_t tmp;
						if (!ReadFile(hFile,&tmp,sizeof(decimal_t),&dwSize2,NULL) ||
							dwSize2 != sizeof(decimal_t))
							break;

						temp->SetValue(Value(complex_t(*(decimal_t*)str,tmp)));
					}
					else // if not 0 or -1, the string data follows, length is (nVal - 1), so blank strings are 1
					{
						if (!ReadFile(hFile,str,nVal - 1,&dwSize2,NULL) ||
							dwSize2 != nVal - 1)
							break;

						str[nVal - 1] = 0; // null-terminate

						temp->SetValue(Value(str));
					}
				}
				else if (nVal == SYMBOL_FUNC)
				{
					// read the number of parameters
					if (!ReadFile(hFile,&nVal,sizeof(nVal),&dwSize2,NULL) ||
						dwSize2 != sizeof(nVal))
						break;

					for (DWORD i = 0; i < nVal; i++)
					{
						// read the function parameter string length
						if (!ReadFile(hFile,&nVal2,sizeof(nVal2),&dwSize2,NULL) ||
							dwSize2 != sizeof(nVal2))
							break;

						if (!ReadFile(hFile,str,nVal2,&dwSize2,NULL) || dwSize2 != nVal2)
							break;

						str[nVal2] = 0; // null-terminate

						temp->GetFuncParamList()->push_back(str);
					}

					// read the expression string length
					if (!ReadFile(hFile,&nVal,sizeof(nVal),&dwSize2,NULL) ||
						dwSize2 != sizeof(nVal))
						break;

					if (!ReadFile(hFile,str,nVal,&dwSize2,NULL) ||
						dwSize2 != nVal)
						break;

					str[nVal] = 0; // null-terminate

					if (!temp->SetFuncDefinition(str))
						temp->SetFuncExpression(new Expression(&g_eval,EXPR_GROUP),false);
				}

				g_eval.GetSymbolList()->Set(temp,false);
			}

			CloseHandle(hFile);
		}
	}

	RegCloseKey(hKey);
	return TRUE;
}

void SaveSettings()
{
	HKEY hKey = NULL;
	if (RegCreateKeyEx(HKEY_CURRENT_USER,"Software\\NZ Software\\MiniCalc",0,NULL,0,
		GENERIC_WRITE,NULL,&hKey,NULL) != ERROR_SUCCESS)
		return;
	
	RegSetValueEx(hKey,"AlwaysOnTop",0,REG_DWORD,(BYTE*)&g_arrProgData[PROGDATA_ALWAYS_ON_TOP],sizeof(BOOL));
	RegSetValueEx(hKey,"Translucent",0,REG_DWORD,(BYTE*)&g_arrProgData[PROGDATA_TRANSLUCENT],sizeof(BOOL));
	RegSetValueEx(hKey,"AutoCopy",0,REG_DWORD,(BYTE*)&g_arrProgData[PROGDATA_AUTO_COPY],sizeof(BOOL));
	RegSetValueEx(hKey,"Degrees",0,REG_DWORD,(BYTE*)&g_arrProgData[PROGDATA_DEGREES],sizeof(BOOL));
	RegSetValueEx(hKey,"SaveSymbols",0,REG_DWORD,(BYTE*)&g_arrProgData[PROGDATA_SAVE_SYMBOLS],sizeof(BOOL));

	g_wpOrig.length = sizeof(WINDOWPLACEMENT);
	if (GetWindowPlacement(g_wndMain,&g_wpOrig))
		RegSetValueEx(hKey,"WindowPlacement",0,REG_BINARY,(BYTE*)&g_wpOrig,sizeof(g_wpOrig));

	// write to the default saved symbols file "symbols.dat"
	char str[MAX_PATH * 2];
	GetModuleFileName(NULL,str,sizeof(str));
	std::string strFile = GetFilePath(str) + "symbols.dat";

	if (g_arrProgData[PROGDATA_SAVE_SYMBOLS])
	{
		HANDLE hFile = CreateFile(strFile.c_str(),GENERIC_WRITE,FILE_SHARE_READ,NULL,
			CREATE_ALWAYS,0,NULL);

		if (hFile != INVALID_HANDLE_VALUE)
		{
			DWORD dwSize2;
			DWORD nVal, nVal2;

			SYMBOLMAP::iterator it = g_eval.GetSymbolList()->GetBeginIterator();
			while (it != g_eval.GetSymbolList()->GetEndIterator())
			{
				Symbol* pSymbol = (*it).second;

				if (pSymbol->GetType() == SYMBOL_INT_FUNC || pSymbol->GetID() == "i" || pSymbol->GetID() == "e" ||
					pSymbol->GetID() == "pi")
				{
					it++;
					continue;
				}

				// write symbol id's length
				nVal = (DWORD)pSymbol->GetID().length();
				if (!WriteFile(hFile,&nVal,sizeof(nVal),&dwSize2,NULL) ||
					dwSize2 != sizeof(nVal))
					break;

				if (!WriteFile(hFile,pSymbol->GetID().c_str(),nVal,&dwSize2,NULL) ||
					dwSize2 != nVal)
					break;

				nVal = pSymbol->GetType();
				if (!WriteFile(hFile,&nVal,sizeof(nVal),&dwSize2,NULL) ||
					dwSize2 != sizeof(nVal))
					break;

				if (pSymbol->GetType() == SYMBOL_VARIABLE)
				{
					if (pSymbol->GetValue().GetType() == VALUE_DECIMAL) // decimal
					{
						nVal = 0; // decimal
						if (!WriteFile(hFile,&nVal,sizeof(nVal),&dwSize2,NULL) ||
							dwSize2 != sizeof(nVal))
							break;

						decimal_t decTemp = pSymbol->GetValue().GetDecimal();
						if (!WriteFile(hFile,&decTemp,
							sizeof(decimal_t),&dwSize2,NULL) || dwSize2 != sizeof(decimal_t))
							break;
					}
					else if (pSymbol->GetValue().GetType() == VALUE_COMPLEX)
					{
						nVal = -1; // complex
						if (!WriteFile(hFile,&nVal,sizeof(nVal),&dwSize2,NULL) ||
							dwSize2 != sizeof(nVal))
							break;

						decimal_t decTemp = pSymbol->GetValue().GetComplex().real();
						if (!WriteFile(hFile,&decTemp,
							sizeof(decimal_t),&dwSize2,NULL) || dwSize2 != sizeof(decimal_t))
							break;

						decTemp = pSymbol->GetValue().GetComplex().imag();
						if (!WriteFile(hFile,&decTemp,
							sizeof(decimal_t),&dwSize2,NULL) || dwSize2 != sizeof(decimal_t))
							break;
					}
					else if (pSymbol->GetValue().GetType() == VALUE_STRING)
					{
						nVal = (DWORD)pSymbol->GetValue().GetString().length() + 1;
						if (!WriteFile(hFile,&nVal,sizeof(nVal),&dwSize2,NULL) ||
							dwSize2 != sizeof(nVal))
							break;

						if (!WriteFile(hFile,pSymbol->GetValue().GetString().c_str(),
							nVal - 1,&dwSize2,NULL) || dwSize2 != nVal - 1)
							break;
					}
				}
				else if (pSymbol->GetType() == SYMBOL_FUNC)
				{
					// write the number of parameters
					nVal = (DWORD)pSymbol->GetFuncParamList()->size();
					if (!WriteFile(hFile,&nVal,sizeof(nVal),&dwSize2,NULL) ||
						dwSize2 != sizeof(nVal))
						break;

					for (DWORD i = 0; i < nVal; i++)
					{
						// write the function parameter string length
						std::string& s2 = (*pSymbol->GetFuncParamList())[i];
						nVal2 = (DWORD)s2.length();
						if (!WriteFile(hFile,&nVal2,sizeof(nVal2),&dwSize2,NULL) ||
							dwSize2 != sizeof(nVal2))
							break;

						if (!WriteFile(hFile,s2.c_str(),
							nVal2,&dwSize2,NULL) || dwSize2 != nVal2)
							break;
					}

					// write the expression string length
					std::string temp = pSymbol->GetFuncExpression()->GetExpressionString(false);
					nVal = (DWORD)temp.length();
					if (!WriteFile(hFile,&nVal,sizeof(nVal),&dwSize2,NULL) ||
						dwSize2 != sizeof(nVal))
						break;

					if (!WriteFile(hFile,temp.c_str(),
						nVal,&dwSize2,NULL) || dwSize2 != nVal)
						break;
				}

				it++;
			}

			CloseHandle(hFile);
		}
	}
	else
		DeleteFile(strFile.c_str());

	RegCloseKey(hKey);
	return;
}

BOOL RegisterClasses()
{
	WNDCLASSEX wcex; ZeroMemory(&wcex,sizeof(wcex));
	wcex.style = CS_DBLCLKS;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.lpfnWndProc = (WNDPROC)WndProc;
	wcex.hInstance = g_hInstance;
	wcex.hIcon = LoadIcon(g_hInstance,MAKEINTRESOURCE(IDI_MINICALC));
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_MINICALC);
	wcex.lpszClassName = _T("MiniCalc");
	wcex.hIconSm = (HICON)LoadImage(wcex.hInstance,MAKEINTRESOURCE(IDI_MINICALC),
		IMAGE_ICON,GetSystemMetrics(SM_CXSMICON),GetSystemMetrics(SM_CYSMICON),
		LR_SHARED);

	return RegisterClassEx(&wcex);
}

BOOL InitInstance()
{
	OleInitialize(NULL);

	TCHAR strTitle[256];
	strcpy(strTitle,"MiniCalc ");
	strcat(strTitle,g_strShortVersion.c_str());

	// load windows
	g_wndMain = CreateWindowEx(0,
		_T("MiniCalc"),strTitle,
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		0,0,0,0,NULL,NULL,g_hInstance,NULL);

	if (!g_wndMain)
		return FALSE;

	g_arrLoadedLibs[0] = LoadLibrary(_T("riched20.dll"));

	g_wndInput = CreateWindowEx(0,"EDIT",NULL,
		WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP | ES_AUTOHSCROLL | ES_NOHIDESEL,
		0,0,0,0,g_wndMain,(HMENU)IDC_EDIT_INPUT,g_hInstance,NULL);

	g_wndOutput = CreateWindowEx(0,RICHEDIT_CLASS,NULL,
		WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP | ES_MULTILINE | ES_READONLY |
		ES_AUTOVSCROLL | ES_NOHIDESEL | WS_VSCROLL,
		0,0,0,0,g_wndMain,(HMENU)IDC_EDIT_OUTPUT,g_hInstance,NULL);

	if (!g_wndInput || !g_wndOutput)
		return FALSE;

	// sub-class input edit window
	g_oldInputProc = SetWindowLongPtr(g_wndInput,GWL_WNDPROC,(LONG_PTR)InputSubProc);

	// initialize all interface objects dependent on system colors
	SendMessage(g_wndMain,WM_SYSCOLORCHANGE,0,0);

	// initialize edit controls (fonts, etc)
	CHARFORMAT chf; chf.cbSize = sizeof(chf);
	chf.dwMask = CFM_FACE | CFM_SIZE | CFM_BOLD | CFM_CHARSET | CFM_FACE | CFM_ITALIC |
        CFM_OFFSET | CFM_PROTECTED | CFM_SIZE | CFM_STRIKEOUT | CFM_UNDERLINE | CFM_COLOR;
	chf.crTextColor = g_clrTxOutput;

	_tcscpy(chf.szFaceName,_T("Courier New")); // Lucida Console, Courier, _M9
	chf.yHeight = 9 * 1440 / 72;
	chf.bCharSet = DEFAULT_CHARSET;
	chf.bPitchAndFamily = DEFAULT_PITCH;
	chf.dwEffects = 0;
	chf.yOffset = 0;

	SendMessage(g_wndOutput,EM_SETCHARFORMAT,SCF_ALL,(LPARAM)&chf);

	HDC tempdc = GetDC(g_wndMain);
	g_fntInput = CreateFont(-MulDiv(9,GetDeviceCaps(tempdc,LOGPIXELSY),72),0,0,0,
		FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,	
		DEFAULT_QUALITY,DEFAULT_PITCH | FF_DONTCARE,chf.szFaceName);
	SendMessage(g_wndInput,WM_SETFONT,(WPARAM)g_fntInput,FALSE);
	ReleaseDC(g_wndMain,tempdc);

	SendMessage(g_wndOutput,EM_SETEVENTMASK,0,0);

	SendMessage(g_wndInput,EM_SETMARGINS,EC_LEFTMARGIN | EC_RIGHTMARGIN,MAKELPARAM(4,4));
	SendMessage(g_wndOutput,EM_SETMARGINS,EC_LEFTMARGIN | EC_RIGHTMARGIN,MAKELPARAM(4,4));

	SetFocus(g_wndInput);

	// show windows
	SetWindowPos(g_wndMain,g_arrProgData[PROGDATA_ALWAYS_ON_TOP] ? HWND_TOPMOST : HWND_NOTOPMOST,
		(GetSystemMetrics(SM_CXSCREEN) - 250) / 2,
		(GetSystemMetrics(SM_CYSCREEN) - 200) / 2,
		250,200,0);

	SendMessage(g_wndMain,WM_SIZE,0,0); // reposition edit windows

	// take out Translucent option if no win2k+
	OSVERSIONINFO osvi; osvi.dwOSVersionInfoSize = sizeof(osvi);
	if (GetVersionEx(&osvi) && osvi.dwMajorVersion < 5)
		RemoveMenu(GetSubMenu(GetMenu(g_wndMain),0),ID_TRANSLUCENT,MF_BYCOMMAND);

	UpdateInterface();

	if (g_wpOrig.length)
		SetWindowPlacement(g_wndMain,&g_wpOrig);

	ShowWindow(g_wndMain,SW_SHOW);
	UpdateWindow(g_wndMain);

	return TRUE;
}

void ExitInstance()
{
	// un-subclass input edit window
	SetWindowLongPtr(g_wndInput,GWL_WNDPROC,g_oldInputProc);

	// clean up
	DeleteObject(g_fntInput);
	DeleteObject(g_brushBkInput);

	if (g_imlSymbolIcons)
		ImageList_Destroy(g_imlSymbolIcons);

	// free all loaded libraries
	int n = sizeof(g_arrLoadedLibs) / sizeof(HMODULE);
	while (n--)
		if (g_arrLoadedLibs[n])
			FreeLibrary(g_arrLoadedLibs[n]);

	OleUninitialize();
}

void UpdateInterface()
{
	if (g_wndMain)
	{
		CheckMenuItem(GetSubMenu(GetMenu(g_wndMain),0),ID_TRANSLUCENT,g_arrProgData[PROGDATA_TRANSLUCENT] ? MF_CHECKED : MF_UNCHECKED);
		CheckMenuItem(GetSubMenu(GetMenu(g_wndMain),0),ID_AUTO_COPY,g_arrProgData[PROGDATA_AUTO_COPY] ? MF_CHECKED : MF_UNCHECKED);
		CheckMenuItem(GetSubMenu(GetMenu(g_wndMain),0),ID_ALWAYS_ON_TOP,g_arrProgData[PROGDATA_ALWAYS_ON_TOP] ? MF_CHECKED : MF_UNCHECKED);

		// update PROGDATA_GRIP_CX
		g_arrProgData[PROGDATA_GRIP_CX] = GetSystemMetrics(SM_CXHSCROLL) + BORDER;

		// update position, always on top
		SetWindowPos(g_wndMain,g_arrProgData[PROGDATA_ALWAYS_ON_TOP] ? HWND_TOPMOST : HWND_NOTOPMOST,
			0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);

		// update layering
		OSVERSIONINFO osvi; osvi.dwOSVersionInfoSize = sizeof(osvi);
		if (GetVersionEx(&osvi) && osvi.dwMajorVersion >= 5)
		{
			LONG ex = GetWindowLong(g_wndMain,GWL_EXSTYLE);

			if (g_arrProgData[PROGDATA_TRANSLUCENT])
			{
				SetWindowLong(g_wndMain,GWL_EXSTYLE,ex | WS_EX_LAYERED);

				// run SetLayeredWindowAttributes
				if (!g_arrLoadedLibs[1])
					g_arrLoadedLibs[1] = LoadLibrary("user32.dll");

				if (g_arrLoadedLibs[1])
				{
					SETLAYEREDWINDOWATTRIBUTESFN pFn = 
						(SETLAYEREDWINDOWATTRIBUTESFN)GetProcAddress(g_arrLoadedLibs[1], 
						"SetLayeredWindowAttributes");

					if (pFn)
						pFn(g_wndMain,0,(255 * 50) / 100,LWA_ALPHA);
				}
			}
			else
				SetWindowLong(g_wndMain,GWL_EXSTYLE,ex & ~WS_EX_LAYERED);
		}

		RedrawWindow(g_wndMain,NULL,NULL,RDW_INVALIDATE | RDW_FRAME | RDW_NOCHILDREN);
	}
}

void DrawMainWindow(HWND hWnd, HDC hdc, RECT& rc)
{
	FillRect(hdc,&rc,GetSysColorBrush(COLOR_BTNFACE));

	HPEN pen = CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNSHADOW));
	HGDIOBJ oldpen = SelectObject(hdc,pen);

	// draw border
	MoveToEx(hdc,BORDER,0,NULL);
	LineTo(hdc,BORDER,rc.bottom - BORDER - 1);
	LineTo(hdc,rc.right - g_arrProgData[PROGDATA_GRIP_CX] - 1,rc.bottom - BORDER - 1);
	LineTo(hdc,rc.right - g_arrProgData[PROGDATA_GRIP_CX] - 1,rc.bottom - INPUT_CY - BORDER - 1);
	LineTo(hdc,rc.right - BORDER - 1,rc.bottom - INPUT_CY - BORDER - 1);
	LineTo(hdc,rc.right - BORDER - 1,0);
	LineTo(hdc,BORDER,0);

	DeleteObject(SelectObject(hdc,oldpen));

	RECT rc2 = {rc.right - g_arrProgData[PROGDATA_GRIP_CX],rc.bottom - INPUT_CY - BORDER,rc.right,rc.bottom};
	DrawFrameControl(hdc,&rc2,DFC_SCROLL,DFCS_SCROLLSIZEGRIP);
}

void AddOutput(LPCSTR strText, BOOL bClearInput, UINT nStyle)
{
	if (!g_wndOutput)
		return;

	CHARFORMAT chf; ZeroMemory(&chf,sizeof(chf)); chf.cbSize = sizeof(chf);
	chf.dwMask = CFM_BOLD | CFM_ITALIC | CFM_STRIKEOUT | CFM_UNDERLINE | CFM_COLOR;
	chf.crTextColor = g_clrTxOutput;

	PARAFORMAT2 pf; ZeroMemory(&pf,sizeof(pf)); pf.cbSize = sizeof(pf);
	pf.dwMask = PFM_ALIGNMENT | PFM_NUMBERING | PFM_OFFSET | PFM_RIGHTINDENT |
		PFM_STARTINDENT | PFM_LINESPACING | PFM_SPACEAFTER;
	pf.dyLineSpacing = 27;
	pf.bLineSpacingRule = 5;

	switch (nStyle)
	{
		case OUTSTYLE_DEFAULT:
			pf.wAlignment = PFA_LEFT;
			break;

		case OUTSTYLE_RESULT:
			pf.wAlignment = PFA_RIGHT;
			chf.dwEffects = CFE_BOLD;
			break;

		case OUTSTYLE_SYSMESSAGE:
			pf.wAlignment = PFA_RIGHT;
			chf.dwEffects = CFE_ITALIC;
			break;

		case OUTSTYLE_ERROR:
			pf.wAlignment = PFA_RIGHT;
			chf.dwEffects = CFE_BOLD;// | CFE_UNDERLINE;
			chf.crTextColor = g_clrTxOutputError;
			break;

		case OUTSTYLE_UNDERLINE:
			pf.wAlignment = PFA_LEFT;
			chf.dwEffects = CFE_UNDERLINE;
			break;
	}

	int n = GetWindowTextLength(g_wndOutput);
	SendMessage(g_wndOutput,EM_SETSEL,n,n);
	SendMessage(g_wndOutput,EM_SETCHARFORMAT,SCF_SELECTION,(LPARAM)&chf);
	SendMessage(g_wndOutput,EM_SETPARAFORMAT,SCF_SELECTION,(LPARAM)&pf);
	SendMessage(g_wndOutput,EM_REPLACESEL,TRUE,(LPARAM)strText);
	SendMessage(g_wndOutput,EM_REPLACESEL,TRUE,(LPARAM)"\r");

	if (bClearInput)
		SetWindowText(g_wndInput,NULL);
}

COLORREF BlendColors(COLORREF clr1, COLORREF clr2, UINT percentage)
{
	return RGB(
		GetRValue(clr1) + (GetRValue(clr2) - GetRValue(clr1)) * percentage / 100,
		GetGValue(clr1) + (GetGValue(clr2) - GetGValue(clr1)) * percentage / 100,
		GetBValue(clr1) + (GetBValue(clr2) - GetBValue(clr1)) * percentage / 100);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case WM_LBUTTONDOWN:
		case WM_MOUSEMOVE:
		{
			RECT rc; GetClientRect(hWnd,&rc);
			rc.right -= rc.left; rc.bottom -= rc.top;
			int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
			if (x >= rc.right - g_arrProgData[PROGDATA_GRIP_CX] &&
				y >= rc.bottom - INPUT_CY - BORDER)
			{
				if (message == WM_MOUSEMOVE)
					SetCursor(LoadCursor(NULL,IDC_SIZENWSE));
				else
				{
					ReleaseCapture();
					SendMessage(hWnd,WM_NCLBUTTONDOWN,HTBOTTOMRIGHT,0);
				}
			}
			else
			{
				SetCursor(LoadCursor(NULL,IDC_ARROW));
			}
			break;
		}

		case WM_SETTINGCHANGE:
			UpdateInterface();
			break;

		case WM_DESTROY:
			SaveSettings();
			PostQuitMessage(0);
			break;

		case WM_ACTIVATE:
			SetFocus(g_wndInput);
			break;

		case WM_SIZE:
		{
			RECT rc; GetClientRect(hWnd,&rc);
			rc.right -= rc.left; rc.bottom -= rc.top;

			SetWindowPos(g_wndOutput,NULL,BORDER + 1,1,
				rc.right - BORDER * 2 - 2,rc.bottom - INPUT_CY - BORDER - 2,
				SWP_NOOWNERZORDER);
			SetWindowPos(g_wndInput,NULL,BORDER + 1,rc.bottom - INPUT_CY - BORDER - 1,
				rc.right - g_arrProgData[PROGDATA_GRIP_CX] - BORDER - 2,
				INPUT_CY,SWP_NOOWNERZORDER);

			UpdateInterface();
			break;
		}

		case WM_NEXTDLGCTL:
			if (lParam)
				SetFocus((HWND)wParam);
			else
				SetFocus(GetNextDlgTabItem(hWnd,GetFocus(),(BOOL)wParam));
			break;

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case IDOK:
				{
					char str[500]; str[0] = 0;
					GetWindowText(g_wndInput,str,500);

					if (strcmp(str,"") != 0)
					{
						// if the input command was selected from recent, delete that recent entry
						if (g_nSelPrevCommand >= 0)
							g_prevCommands.erase(g_prevCommands.end() - g_nSelPrevCommand - 1);

						g_prevCommands.push_back(str);
						g_nSelPrevCommand = -1; // reset selected recent command

						// if there are more than the maximum number of commands, delete oldest one (first)
						if (g_prevCommands.size() > MAX_PREVCOMMANDS)
							g_prevCommands.erase(g_prevCommands.begin());

						ProcessInput(str);

						// don't clear if holding control
						if (!(GetAsyncKeyState(VK_CONTROL) & 0x8000))
							SetWindowText(g_wndInput,NULL);
						else
							// if holding control, recent input is latest one
							g_nSelPrevCommand = 0;
					}
					break;
				}

				case IDC_EDIT_INPUT:
					if (HIWORD(wParam) == EN_CHANGE)
						g_nSelPrevCommand = -1;
					break;

				case ID_ALWAYS_ON_TOP:
					g_arrProgData[PROGDATA_ALWAYS_ON_TOP] = !g_arrProgData[PROGDATA_ALWAYS_ON_TOP];
					UpdateInterface();
					break;

				case ID_TRANSLUCENT:
					g_arrProgData[PROGDATA_TRANSLUCENT] = !g_arrProgData[PROGDATA_TRANSLUCENT];
					UpdateInterface();
					break;

				case ID_AUTO_COPY:
					g_arrProgData[PROGDATA_AUTO_COPY] = !g_arrProgData[PROGDATA_AUTO_COPY];
					UpdateInterface();
					break;

				case ID_CLEAR_RESULTS:
					if (g_wndOutput)
					{
						SetWindowText(g_wndOutput,NULL);
						SetWindowText(g_wndInput,NULL);
					}
					break;

				case ID_OPTIONS:
					DialogBox(g_hInstance,MAKEINTRESOURCE(IDD_OPTIONS),hWnd,(DLGPROC)OptionsDlgProc);
					break;

				case ID_VIEW_SYMBOL_TABLE:
					DialogBox(g_hInstance,MAKEINTRESOURCE(IDD_SYMBOL_TABLE),hWnd,(DLGPROC)SymbolTableDlgProc);
					break;

				case ID_HELP_CONTENTS:
				{
					char strFile[MAX_PATH]; GetModuleFileName(NULL,strFile,MAX_PATH);
					char* pSlash = strrchr(strFile,'\\');
					strcpy(pSlash ? pSlash + 1 : strFile,"minicalc.chm::/welcome.html");
					HtmlHelp(NULL,strFile,HH_DISPLAY_TOC,0);
					break;
				}

				case IDM_ABOUT:
					DialogBox(g_hInstance,MAKEINTRESOURCE(IDD_ABOUTBOX),hWnd,(DLGPROC)About);
					break;

				case IDM_EXIT:
					DestroyWindow(hWnd);
					break;

				default:
					return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		}

		case WM_PAINT:
		{
			RECT rc; GetClientRect(hWnd,&rc);
			rc.right -= rc.left; rc.bottom -= rc.top;

			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd,&ps);
			HDC dbdc = CreateCompatibleDC(hdc);
			HGDIOBJ oldbmp = SelectObject(dbdc,
				CreateCompatibleBitmap(hdc,rc.right,rc.bottom));
			DrawMainWindow(hWnd,dbdc,rc);
			BitBlt(hdc,0,0,rc.right,rc.bottom,dbdc,0,0,SRCCOPY);
			DeleteObject(SelectObject(dbdc,oldbmp));
			DeleteDC(dbdc);
			EndPaint(hWnd,&ps);
			break;
		}

		case WM_SYSCOLORCHANGE:
		{
			if (g_brushBkInput)
				DeleteObject(g_brushBkInput);

			// create input color based on system color
			/*COLORREF clr = RGB(95,110,140);
			g_brushBkInput = CreateSolidBrush(clr);
			g_clrBkOutput = RGB(55,70,105);
			g_clrTxInput = RGB(230,230,240);
			g_clrTxOutput = RGB(230,230,240);
			g_clrTxOutputError = RGB(230,120,80);*/

			g_brushBkInput = CreateSolidBrush(BlendColors(GetSysColor(COLOR_BTNFACE),GetSysColor(COLOR_WINDOW),50));
			g_clrBkOutput = GetSysColor(COLOR_WINDOW);
			g_clrTxInput = GetSysColor(COLOR_WINDOWTEXT);
			g_clrTxOutput = GetSysColor(COLOR_WINDOWTEXT);
			g_clrTxOutputError = RGB(255,0,0);


			SendMessage(g_wndOutput,EM_SETBKGNDCOLOR,0,(LPARAM)g_clrBkOutput);
			break;
		}

		case WM_CTLCOLOREDIT:
			if (lParam == (LPARAM)g_wndInput)
			{
				SetTextColor((HDC)wParam,g_clrTxInput);
				SetBkMode((HDC)wParam,TRANSPARENT);
				return (LRESULT)g_brushBkInput;
			}
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

LRESULT CALLBACK InputSubProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_KEYDOWN:
			if (wParam == VK_UP || wParam == VK_DOWN)
			{
				// don't select recent command if there are none or pressing 'down' while
				// user hasn't started selecting recent commands
				if (!g_prevCommands.size() || (wParam == VK_DOWN && g_nSelPrevCommand == -1))
					break;

				int nSize = (int)g_prevCommands.size();
				if (wParam == VK_UP)
					g_nSelPrevCommand = min(g_nSelPrevCommand + 1,nSize - 1);
				else
					g_nSelPrevCommand = max(g_nSelPrevCommand - 1,0);

				int nTemp = g_nSelPrevCommand;
				SetWindowText(g_wndInput,g_prevCommands[nSize - g_nSelPrevCommand - 1].c_str());
				g_nSelPrevCommand = nTemp; // modified in EN_CHANGE
				SendMessage(g_wndInput,EM_SETSEL,0,
					(LPARAM)g_prevCommands[nSize - g_nSelPrevCommand - 1].length());
			}
			else
				break;
			return 0;
	}

	return CallWindowProc((WNDPROC)g_oldInputProc,hWnd,message,wParam,lParam);
}

LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
			{
				TCHAR strTemp[256];
                strcpy(strTemp,"Version ");
				strcat(strTemp,g_strLongVersion.c_str());
				SetDlgItemText(hDlg,IDC_VERSION,strTemp);
			}
			return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg,LOWORD(wParam));
				return TRUE;
			}
			break;
	}

	return FALSE;
}

LRESULT CALLBACK OptionsDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
		{
			CheckDlgButton(hDlg,g_arrProgData[PROGDATA_DEGREES] ?
				IDC_DEGREES : IDC_RADIANS,BST_CHECKED);
			CheckDlgButton(hDlg,IDC_SAVE_SYMBOLS,
				g_arrProgData[PROGDATA_SAVE_SYMBOLS] ? BST_CHECKED : BST_UNCHECKED);
			return TRUE;
		}

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK)
			{
				g_arrProgData[PROGDATA_DEGREES] = IsDlgButtonChecked(hDlg,IDC_DEGREES);
				g_arrProgData[PROGDATA_SAVE_SYMBOLS] = IsDlgButtonChecked(hDlg,IDC_SAVE_SYMBOLS);

				if (g_arrProgData[PROGDATA_DEGREES])
				{	g_eval.SetTrigFactor(DefaultSymbols::Pi / 180.0); }
				else
				{	g_eval.SetTrigFactor(1.0); }

				EndDialog(hDlg,LOWORD(wParam));
				return TRUE;
			}
			else if (LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg,LOWORD(wParam));
				return TRUE;
			}
			break;
	}

	return FALSE;
}

struct SymbolListSortData
{
	HWND wnd;
	int nColumn, nMethod;
};

int CALLBACK SymbolListSortProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	SymbolListSortData* data = (SymbolListSortData*)lParamSort;

	char str[500], str2[500];
	ListView_GetItemText(data->wnd,lParam1,data->nColumn,str,sizeof(str));
	ListView_GetItemText(data->wnd,lParam2,data->nColumn,str2,sizeof(str2));

	int n;
	if (data->nColumn == 1)
	{
		if (str[0] == 'V') n = -(int)(str2[0] != 'V'); // if second isn't V, always -1
		else if (str[0] == 'I') n = (int)(str2[0] != 'I'); // if second isn't I, always 1
		else n = (str2[0] == 'F') ? 0 : ((str2[0] == 'V') ? 1 : -1);
	}
	else
		n = stricmp(str,str2); // simple string comparison

	if (data->nMethod) n = -n; // inverse sorting if nMethod is 1 (descending)
	return n;
}

void UpdateSymbolTableItem(HWND wndSymbols, int nItem, Symbol* pSymbolData)
{
	LVITEM lvi; ZeroMemory(&lvi,sizeof(lvi));
	lvi.mask = LVIF_IMAGE | LVIF_TEXT;
	lvi.iItem = nItem;
	lvi.pszText = (LPSTR)pSymbolData->GetID().c_str();
	lvi.iImage = pSymbolData->GetType() - 1;
	ListView_SetItem(wndSymbols,&lvi);

	lvi.iImage = -1;
	lvi.iSubItem = 1;

	std::string temp;
	switch (pSymbolData->GetType())
	{
		case SYMBOL_FUNC:
			lvi.pszText = "Function";
			ListView_SetItem(wndSymbols,&lvi);
			lvi.iSubItem = 2;
			temp = pSymbolData->GetFuncExpression()->GetExpressionString();
			lvi.pszText = (LPTSTR)temp.c_str();
			break;

		case SYMBOL_INT_FUNC:
			lvi.pszText = "Internal Function";
			ListView_SetItem(wndSymbols,&lvi);
			lvi.iSubItem = 2;
			lvi.pszText = "[Internal Code]";
			break;

		case SYMBOL_VARIABLE:
			lvi.pszText = "Variable";
			ListView_SetItem(wndSymbols,&lvi);
			lvi.iSubItem = 2;
			temp = pSymbolData->GetValue().ToString();
			lvi.pszText = (LPTSTR)temp.c_str();
			break;
	}

	ListView_SetItem(wndSymbols,&lvi);
}

LRESULT CALLBACK SymbolTableDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int s_nSortColumn = 1;
	static int s_nSortMethod = 0;

	switch (message)
	{
		case WM_NOTIFY:
			if (wParam == IDC_SYMBOLS)
			{
				NMLISTVIEW* pLV = (NMLISTVIEW*)lParam;
				switch (((NMHDR*)lParam)->code)
				{
					case LVN_COLUMNCLICK:
						if (pLV->iSubItem == s_nSortColumn)
							s_nSortMethod = !s_nSortMethod;
						else
							s_nSortMethod = 0;

						s_nSortColumn = pLV->iSubItem;

						SymbolListSortData data;
						data.wnd = pLV->hdr.hwndFrom;
						data.nColumn = s_nSortColumn;
						data.nMethod = s_nSortMethod;
						ListView_SortItemsEx(pLV->hdr.hwndFrom,SymbolListSortProc,(LPARAM)&data);
						break;

					case LVN_ITEMCHANGING:
					{
						// if de-selecting, reset controls
						if (!(pLV->uNewState & LVIS_SELECTED))
						{
							EnableWindow(GetDlgItem(hDlg,IDC_SYMBOL_ID),TRUE);
							EnableWindow(GetDlgItem(hDlg,IDC_SYMBOL_TYPE),TRUE);
							EnableWindow(GetDlgItem(hDlg,IDC_SYMBOL_VALUE),TRUE);
							EnableWindow(GetDlgItem(hDlg,IDC_SYMBOL_PARAMS),FALSE);
							EnableWindow(GetDlgItem(hDlg,IDC_ADD_UPDATE_SYMBOL),TRUE);
							EnableWindow(GetDlgItem(hDlg,IDC_DELETE_SYMBOL),FALSE);

							SetDlgItemText(hDlg,IDC_SYMBOL_ID,NULL);
							SendDlgItemMessage(hDlg,IDC_SYMBOL_TYPE,CB_SETCURSEL,0,0);
							SetDlgItemText(hDlg,IDC_SYMBOL_VALUE,NULL);
							SetDlgItemText(hDlg,IDC_SYMBOL_PARAMS,NULL);
							SetDlgItemText(hDlg,IDC_ADD_UPDATE_SYMBOL,"&Add");
						}
						else
						{
							LVITEM lvi;
							lvi.mask = LVIF_PARAM;
							lvi.iItem = pLV->iItem;
							lvi.iSubItem = 0;
							if (ListView_GetItem(pLV->hdr.hwndFrom,&lvi))
							{
								Symbol* pSymbol = (Symbol*)lvi.lParam;
								
								BOOL bEnable = BOOL(pSymbol->GetType() != SYMBOL_INT_FUNC);
								if (pSymbol->GetID() == "i" || pSymbol->GetID() == "pi" ||
									pSymbol->GetID() == "e")
									bEnable = FALSE;
								EnableWindow(GetDlgItem(hDlg,IDC_SYMBOL_ID),bEnable);
								EnableWindow(GetDlgItem(hDlg,IDC_SYMBOL_TYPE),bEnable);
								EnableWindow(GetDlgItem(hDlg,IDC_SYMBOL_VALUE),bEnable);
								EnableWindow(GetDlgItem(hDlg,IDC_ADD_UPDATE_SYMBOL),bEnable);
								EnableWindow(GetDlgItem(hDlg,IDC_DELETE_SYMBOL),bEnable);

								SetDlgItemText(hDlg,IDC_SYMBOL_ID,pSymbol->GetID().c_str());

								int nType;
								if (pSymbol->GetType() == SYMBOL_VARIABLE)
								{
									if (pSymbol->GetValue().GetType() == VALUE_STRING)
										nType = 1;
									else if (pSymbol->GetValue().GetType() == VALUE_DECIMAL ||
										pSymbol->GetValue().GetType() == VALUE_COMPLEX)
										nType = 0;
									EnableWindow(GetDlgItem(hDlg,IDC_SYMBOL_PARAMS),FALSE);
								}
								else if (bEnable)
								{
									nType = 2;
									EnableWindow(GetDlgItem(hDlg,IDC_SYMBOL_PARAMS),TRUE);

									std::string str;
									size_t nParams = pSymbol->GetFuncParamList()->size();
									for (size_t i = 0; i < nParams; i++)
									{
										str += (*pSymbol->GetFuncParamList())[i];
										if (i < nParams - 1) str += ", ";
									}

									SetDlgItemText(hDlg,IDC_SYMBOL_PARAMS,str.c_str());
								}

								SendDlgItemMessage(hDlg,IDC_SYMBOL_TYPE,CB_SETCURSEL,
									(WPARAM)(bEnable ? nType : -1),0);

								if (bEnable)
								{
									char str[2000]; str[0] = 0;
									if (pSymbol->GetType() == SYMBOL_VARIABLE)
										strcpy(str,pSymbol->GetValue().ToString(false).c_str());
									else if (pSymbol->GetType() == SYMBOL_FUNC)
										strcpy(str,pSymbol->GetFuncExpression()->GetExpressionString().c_str());
										
									SetDlgItemText(hDlg,IDC_SYMBOL_VALUE,str);
								}

								SetDlgItemText(hDlg,IDC_ADD_UPDATE_SYMBOL,"&Update");
							}
						}
						break;
					}

					case LVN_KEYDOWN:
					{
						if (((NMLVKEYDOWN*)pLV)->wVKey == VK_DELETE)
							SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_DELETE_SYMBOL,0),0);
						break;
					}
				}
				return TRUE;
			}
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:
				case IDCANCEL:
					EndDialog(hDlg,LOWORD(wParam));
					break;

				case IDC_ADD_UPDATE_SYMBOL:
				{
					Symbol* pSymbol = NULL;

					HWND wndSymbols = GetDlgItem(hDlg,IDC_SYMBOLS);
					LVITEM lvi;
					lvi.mask = LVIF_PARAM;
					lvi.iItem = ListView_GetNextItem(wndSymbols,-1,LVNI_SELECTED);
					lvi.iSubItem = 0;

					BOOL bAdding = (lvi.iItem < 0);

					if (bAdding)
						pSymbol = new Symbol;
					else
					{
						if (!ListView_GetItem(wndSymbols,&lvi))
							break;
						
						pSymbol = (Symbol*)lvi.lParam;
						if (pSymbol->GetType() == SYMBOL_INT_FUNC)
							break;
					}

					char strTemp[2000];
					GetDlgItemText(hDlg,IDC_SYMBOL_ID,strTemp,sizeof(strTemp));
					{
						std::string str2 = strTemp;
						rtrim(str2);
						if (str2.length() == 0) // must have 
						{
							MessageBox(hDlg,_T("Please enter a valid symbol ID."),
								_T("MiniCalc"),MB_ICONEXCLAMATION);
							if (bAdding)
								delete pSymbol; // clean up
							break;
						}
					}
					
					if (strcmp(strTemp,pSymbol->GetID().c_str()) != 0 || bAdding)
						// adds symbol if ID changed or adding (not updating)
					{
						// symbol ID changed
						if (g_eval.GetSymbolList()->Find(strTemp))
						{
							// already exists
							MessageBox(hDlg,_T("The specified symbol ID already exists."),
								_T("MiniCalc"),MB_ICONEXCLAMATION);
							if (bAdding)
								delete pSymbol; // clean up
							break; // don't do anymore updating
						}
						else
						{
							// rename, refresh in map
							g_eval.GetSymbolList()->Remove(pSymbol->GetID(),false);
							pSymbol->SetID(strTemp);
							g_eval.GetSymbolList()->Set(pSymbol,false);
						}
					}

					GetDlgItemText(hDlg,IDC_SYMBOL_VALUE,strTemp,sizeof(strTemp));
					int nSelType = (int)SendDlgItemMessage(hDlg,IDC_SYMBOL_TYPE,CB_GETCURSEL,0,0);

					// TODO: solve the expression using an Eval and set it to result
					if (nSelType == 0) // numerical variable
					{
						pSymbol->SetType(SYMBOL_VARIABLE);

						Return ret;
						g_eval.QuickEval(ret,strTemp);

						// set function definition
						if (!CALCSUCCESS(ret))
						{
							MessageBox(hDlg,_T("The specified value is invalid."),
								_T("MiniCalc"),MB_ICONEXCLAMATION);
							if (bAdding)
							{
								g_eval.GetSymbolList()->Remove(pSymbol->GetID(),false);
								delete pSymbol; // clean up
							}
							break;
						}

						pSymbol->SetValue(ret.value);
					}
					else if (nSelType == 1) // string variable
					{
						pSymbol->SetType(SYMBOL_VARIABLE);
						pSymbol->SetValue(Value(strTemp));
					}
					else if (nSelType == 2) // function
					{
						pSymbol->SetType(SYMBOL_FUNC);

						// set function definition
						if (!pSymbol->SetFuncDefinition(strTemp))
						{
							MessageBox(hDlg,_T("The expression string is not valid."),
								_T("MiniCalc"),MB_ICONEXCLAMATION);
							if (bAdding)
							{
								g_eval.GetSymbolList()->Remove(pSymbol->GetID(),false);
								delete pSymbol; // clean up
							}
							break;
						}

						// set parameters
						pSymbol->GetFuncParamList()->clear();

						GetDlgItemText(hDlg,IDC_SYMBOL_PARAMS,strTemp,sizeof(strTemp));
						char seps[] = { ' ', ';', ',' };
						char* strT = strtok(strTemp,seps);
						while (strT)
						{
							pSymbol->GetFuncParamList()->push_back(strT);
							strT = strtok(NULL,seps);
						}
					}

					if (lvi.iItem < 0) // adding
					{
						// add
						ZeroMemory(&lvi,sizeof(lvi));
						lvi.mask = LVIF_PARAM;
						lvi.lParam = (LPARAM)pSymbol;
						lvi.iItem = ListView_InsertItem(wndSymbols,&lvi);
					}
					
					UpdateSymbolTableItem(wndSymbols,lvi.iItem,pSymbol);

					// re-select
					ListView_SetItemState(wndSymbols,lvi.iItem,LVIS_SELECTED,LVIS_SELECTED);

					// re-sort
					SymbolListSortData data;
					data.wnd = wndSymbols;
					data.nColumn = s_nSortColumn;
					data.nMethod = s_nSortMethod;
					ListView_SortItemsEx(wndSymbols,SymbolListSortProc,(LPARAM)&data);
					break;
				}

				case IDC_DELETE_SYMBOL:
				{
					HWND wndSymbols = GetDlgItem(hDlg,IDC_SYMBOLS);
					LVITEM lvi;
					lvi.mask = LVIF_PARAM;
					lvi.iItem = ListView_GetNextItem(wndSymbols,-1,LVNI_SELECTED);
					lvi.iSubItem = 0;
					if (lvi.iItem >= 0 && ListView_GetItem(wndSymbols,&lvi))
					{
						Symbol* pSymbol = (Symbol*)lvi.lParam;
						if (pSymbol->GetType() != SYMBOL_INT_FUNC)
						{
							std::string temp = pSymbol->GetID();
							g_eval.GetSymbolList()->Remove(temp);
							ListView_DeleteItem(wndSymbols,lvi.iItem);
						}
					}
					break;
				}

				case IDC_SYMBOL_TYPE:
					if (HIWORD(wParam) == LBN_SELCHANGE)
					{
						EnableWindow(GetDlgItem(hDlg,IDC_SYMBOL_PARAMS),
							BOOL(SendDlgItemMessage(
							hDlg,IDC_SYMBOL_TYPE,CB_GETCURSEL,0,0) == 2));

					}
					break;

				default:
					return FALSE;
			}
			return TRUE;

		case WM_INITDIALOG:
		{
			HWND wndSymbols = GetDlgItem(hDlg,IDC_SYMBOLS);

			// set up symbols view and add in existing symbols
			if (wndSymbols)
			{
				//SetWindowLong(wndSymbols,GWL_STYLE,
				//	GetWindowLong(wndSymbols,GWL_STYLE) | LVS_REPORT | LVS_SHAREIMAGELISTS);
				ListView_SetImageList(wndSymbols,g_imlSymbolIcons,LVSIL_SMALL);

				LVCOLUMN lvc; ZeroMemory(&lvc,sizeof(lvc)); lvc.mask = LVCF_TEXT | LVCF_WIDTH;
				lvc.cx = 65; lvc.pszText = "Name";
				ListView_InsertColumn(wndSymbols,0,&lvc);
				lvc.cx = 85; lvc.pszText = "Type";
				ListView_InsertColumn(wndSymbols,1,&lvc);
				lvc.cx = 175; lvc.pszText = "Value";
				ListView_InsertColumn(wndSymbols,2,&lvc);

				SYMBOLMAP::iterator it = g_eval.GetSymbolList()->GetBeginIterator();
				while (it != g_eval.GetSymbolList()->GetEndIterator())
				{
					Symbol* pSymbol = (*it).second;

					LVITEM lvi; ZeroMemory(&lvi,sizeof(lvi));
					lvi.mask = LVIF_PARAM;
					lvi.lParam = (LPARAM)pSymbol;
					int nItem = ListView_InsertItem(wndSymbols,&lvi);

					if (nItem >= 0)
						UpdateSymbolTableItem(wndSymbols,nItem,pSymbol);

					it++;
				}

				// default sort to 'Type'
				SymbolListSortData data;
				data.wnd = wndSymbols;
				data.nColumn = 1;
				data.nMethod = 0;
				ListView_SortItemsEx(wndSymbols,SymbolListSortProc,(LPARAM)&data);
			}

			HWND wndSymbolType = GetDlgItem(hDlg,IDC_SYMBOL_TYPE);
			SendMessage(wndSymbolType,CB_ADDSTRING,0,(LPARAM)"Numerical Variable");
			SendMessage(wndSymbolType,CB_ADDSTRING,0,(LPARAM)"String Variable");
			SendMessage(wndSymbolType,CB_ADDSTRING,0,(LPARAM)"Function");
			SendMessage(wndSymbolType,CB_SETCURSEL,0,0);
			return TRUE;
		}
	}

	return FALSE;
}