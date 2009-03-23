#include "CalcClasses.h"

using namespace Calc;

////////////////////////////////////////////////////////////////////////////////
// common functions

std::string& Calc::exprstr(std::string& s)
{
	std::string rep;
	size_t temp, temp2;
	size_t pos = 0;
	while (1)
	{
		// find closest double-quote or backslash
		temp = s.find("\"",pos);
		temp2 = s.find("\\",pos);
		if (temp < temp2)
		{
			pos = temp;
			rep = "\\\"";
		}
		else
		{
			pos = temp2;
			rep = "\\\\";
		}

        if (pos == std::string::npos)
			break;

		s.replace(pos,1,rep);
		pos += rep.size();
	}
	return s;
}

std::string& Calc::rtrim(std::string& s)
{
	s.erase(std::find_if(s.rbegin(),
		s.rend(),std::not1(std::ptr_fun(isspace))).base(),s.end());
	return s;
}

std::string& Calc::ltrim(std::string& s)
{
	s.erase(s.begin(),
		std::find_if(s.begin(),s.end(),std::not1(std::ptr_fun(isspace))));
	return s;
}

bool Calc::iswhole(decimal_t n)
{
	return bool(floor(n) == n);
}

void Calc::cplxstr(const complex_t& cplx, std::string& str)
{
	char str2[256];
	if (cplx.imag() == 0)
		sprintf(str2,"%g",cplx.real());
	else if (cplx.real() == 0)
	{
		if (cplx.imag() == 1.0) strcpy(str2,"i");
		else if (cplx.imag() == -1.0) strcpy(str2,"-i");
		else sprintf(str2,"%g*i",cplx.imag());
	}
	else
	{
		sprintf(str2,"%g",cplx.real());
		int n = (int)strlen(str2);
		if (cplx.imag() == 1.0) strcpy(str2 + n,"+i");
		else if (cplx.imag() == -1.0) strcpy(str2 + n,"-i");
		else if (cplx.imag() < 0)sprintf(str2 + n,"-%g*i",fabs(cplx.imag()));
		else sprintf(str2 + n,"+%g*i",cplx.imag());
	}

	str = str2;
}

bool Calc::equalThresh(decimal_t a, decimal_t b)
{
	return (fabs(a - b) < 0.0000001);
}

////////////////////////////////////////////////////////////////////////////////
// Value

Value::Value(decimal_t decValue)
{
	m_nType = VALUE_DECIMAL;
	m_decValue = decValue;
	m_pStrValue = NULL;
	m_pCplxValue = NULL;
	m_pListValue = NULL;
}

Value::Value(const std::string& strValue)
{
	m_nType = VALUE_STRING;
	m_decValue = 0.0;
	m_pStrValue = new std::string(strValue);
	m_pCplxValue = NULL;
	m_pListValue = NULL;
}

Value::Value(const complex_t& cplxValue)
{
	m_nType = VALUE_COMPLEX;
	m_decValue = 0.0;
	m_pStrValue = NULL;
	m_pCplxValue = new complex_t(cplxValue);
	m_pListValue = NULL;
	TryComplexToReal();
}

Value::Value(const ExpressionList& listValue)
{
	m_nType = VALUE_LIST;
	m_decValue = 0.0;
	m_pStrValue = NULL;
	m_pCplxValue = NULL;
	m_pListValue = new ExpressionList(listValue);
}

Value::Value(const Value& src)
{
	*this = src;
}

Value::~Value()
{
	SetType(VALUE_DECIMAL); // just deletes all pointers
}

Value& Value::operator=(const Value& src)
{
	SetType(src.GetType());

	if (m_nType == VALUE_DECIMAL)
		m_decValue = src.GetDecimal();
	else if (m_nType == VALUE_STRING)
		*m_pStrValue = src.GetString();
	else if (m_nType == VALUE_COMPLEX)
		*m_pCplxValue = src.GetComplex();
	else if (m_nType == VALUE_LIST)
		*m_pListValue = src.GetList();

	return *this;
}

std::string Value::ToString(bool bQuote) const
{
	char str[4096];

	if (GetType() == VALUE_DECIMAL)
	{
		if (_isnan(GetDecimal()))
			strcpy(str,"[Not a real number]");
		else if (!_finite(GetDecimal()))
			strcpy(str,"[Out of range]");
		else
			sprintf(str,"%g",GetDecimal());
	}
	else if (GetType() == VALUE_COMPLEX)
	{
		std::string tmp;
		cplxstr(GetComplex(),tmp);
		return tmp;
	}
	else if (GetType() == VALUE_LIST)
	{
		std::string tmp = "[";
		for (size_t i = 0; i < GetList().GetCount(); i++)
		{
			if (i != 0) tmp += ", ";
			tmp += GetList().Get(i)->GetExpressionString(true,false,bQuote);
		}
		tmp += "]";
		return tmp;
	}
	else if (GetType() == VALUE_STRING)
	{
		if (bQuote)
			return "\"" + GetString() + "\"";
		else
			return GetString();
	}

	return str;
}

void Value::SetType(int nType)
{
	if (m_nType == nType)
		return;

	// setting to a different type than current one

	m_decValue = 0.0;

	if (m_nType == VALUE_STRING)
		delete m_pStrValue;
	else if (m_nType == VALUE_COMPLEX)
		delete m_pCplxValue;
	else if (m_nType == VALUE_LIST)
		delete m_pListValue;

	if (nType == VALUE_STRING)
		m_pStrValue = new std::string;
	else if (nType == VALUE_COMPLEX)
		m_pCplxValue = new complex_t;
	else if (nType == VALUE_LIST)
		m_pListValue = new ExpressionList;

	m_nType = nType;
}

void Value::SetDecimal(decimal_t decValue)
{
	SetType(VALUE_DECIMAL);
	m_decValue = decValue;
}

void Value::SetString(const std::string& strValue)
{
	SetType(VALUE_STRING);
	*m_pStrValue = strValue;
}

void Value::SetComplex(const complex_t& cplxValue)
{
	SetType(VALUE_COMPLEX);
	*m_pCplxValue = cplxValue;
}

void Value::SetList(const ExpressionList& listValue)
{
	SetType(VALUE_LIST);
	*m_pListValue = listValue;
}

void Value::TryComplexToReal()
{
	if (m_nType == VALUE_COMPLEX && m_pCplxValue->imag() == 0)
	{
		decimal_t a = m_pCplxValue->real();
		SetType(VALUE_DECIMAL);
		m_decValue = a;
	}
}

void Value::Negate()
{
	if (m_nType == VALUE_DECIMAL)
		m_decValue = -m_decValue;
	else if (m_nType == VALUE_STRING)
		m_pStrValue; // TODO: negate a string here
	else if (m_nType == VALUE_COMPLEX)
		*m_pCplxValue *= -1;
	else if (m_nType == VALUE_LIST)
	{
		// negate all values in list
		for (size_t i = 0; i < m_pListValue->GetCount(); i++)
			(*m_pListValue).Get(i)->SetNegative(!(*m_pListValue).Get(i)->IsNegative());
	}
}

std::string Value::GetValueTypeString(int nValueType)
{
	switch (nValueType)
	{
		case VALUE_DECIMAL: return "Decimal";
		case VALUE_STRING:	return "String";
		case VALUE_COMPLEX:	return "Complex";
		case VALUE_LIST:	return "List";
	}

	return "Unknown";
}

////////////////////////////////////////////////////////////////////////////////
// Symbol

Symbol::Symbol(int nType, std::string id, const Value* pValue,
			   INTFUNC fnIntCallback, std::string strFuncDefinition)
{
	m_nType = nType;
	m_id = id;
	if (pValue)
		m_value = *pValue;
	m_fnIntCallback = fnIntCallback;
	m_pFuncExpression = NULL;

	SetFuncDefinition(strFuncDefinition);
}

Symbol::Symbol(const Symbol& src)
{
	m_pFuncExpression = NULL;
	*this = src;
}

Symbol::~Symbol()
{
	delete m_pFuncExpression;
}

Symbol& Symbol::operator=(const Symbol& src)
{
	m_nType = src.GetType();
	m_id = src.GetID();
	m_value = src.GetValue();
	m_fnIntCallback = src.GetIntFuncCallback();
	SetFuncDefinition(src.GetFuncDefinition());

	m_vFuncParams.clear();
	for (int i = 0; i < (int)src.GetFuncParamList()->size(); i++)
		m_vFuncParams.push_back(src.GetFuncParamList()->at(i));

	return *this;
}

bool Symbol::SetFuncDefinition(std::string strFuncDefinition)
{
	if (strFuncDefinition == "")
		return false;

	Eval eval;
	eval.GenerateTree(strFuncDefinition);
	if (!CALCSUCCESS(eval.GetLastReturn()))
		return false;

	delete m_pFuncExpression;
	m_pFuncExpression = new Expression(*eval.GetRootExpression());
	m_pFuncExpression->SetEval(NULL); // the real Eval will be set when using this symbol
	m_strFuncDefinition = strFuncDefinition;

	return true;
}

bool Symbol::SetFuncExpression(Expression* pExpr, bool bCopy)
{
	if (!pExpr)
		return false;

	delete m_pFuncExpression;
	m_pFuncExpression = bCopy ? new Expression(*pExpr) : pExpr;
	m_strFuncDefinition = pExpr->GetExpressionString();

	return true;
}

////////////////////////////////////////////////////////////////////////////////
// SymbolList

SymbolList::SymbolList()
{
}

SymbolList::SymbolList(const SymbolList& src)
{
	*this = src;
}

SymbolList::~SymbolList()
{
	Clear();
}

SymbolList& SymbolList::operator=(const SymbolList& src)
{
	Clear();

	SYMBOLMAP::const_iterator it = src.GetBeginIterator();
	while (it != src.GetEndIterator())
	{
		Set((*it).second,true);
		it++;
	}

	return *this;
}

Symbol* SymbolList::Find(const std::string& strID)
{
	SYMBOLMAP::const_iterator it = m_map.find(strID);
	if (it != m_map.end())
		return (*it).second;

	return NULL;
}

void SymbolList::Set(Symbol* pSymbol, bool bCopy)
{
	if (!pSymbol)
		return;

	Remove(pSymbol->GetID());
	m_map.insert(SYMBOLMAP::value_type(pSymbol->GetID(),
		bCopy ? new Symbol(*pSymbol) : pSymbol));
}

void SymbolList::Remove(const std::string& strID, bool bDelete, Symbol** pDestPtr)
{
	Symbol* pSymbol = Find(strID);
	if (pSymbol)
	{
		Remove(pSymbol,bDelete);
		if (!bDelete && pDestPtr)
			*pDestPtr = pSymbol;
	}
}

void SymbolList::Remove(Symbol* pSymbol, bool bDelete)
{
	if (pSymbol)
	{
		m_map.erase(pSymbol->GetID());
		if (bDelete)
			delete pSymbol;
	}
}

// deletes symbols
void SymbolList::Clear()
{
	SYMBOLMAP::iterator it = m_map.begin();
	while (it != m_map.end())
	{
		delete (*it).second;
		it++;
	}

	m_map.clear();
}

void SymbolList::AddDefaultSymbols()
{
	#define _NZM_CONSTANT(_n,_v) \
		Set(new Symbol(SYMBOL_VARIABLE,_n,_v),false);

	#define _NZM_INT_FUNCTION(_n,_f) \
		Set(new Symbol(SYMBOL_INT_FUNC,_n,NULL,_f),false);

	_NZM_CONSTANT("pi",			&Value(DefaultSymbols::Pi))
	_NZM_CONSTANT("e",			&Value(DefaultSymbols::E))
	_NZM_CONSTANT("i",			&Value(DefaultSymbols::I))
	
	_NZM_INT_FUNCTION("floor",	DefaultSymbols::Floor)
	_NZM_INT_FUNCTION("ceil",	DefaultSymbols::Ceil)
	_NZM_INT_FUNCTION("fact",	DefaultSymbols::Factorial)
	_NZM_INT_FUNCTION("perm",	DefaultSymbols::Permutation)
	_NZM_INT_FUNCTION("comb",	DefaultSymbols::Combination)
	_NZM_INT_FUNCTION("cmp",	DefaultSymbols::Compare)
	_NZM_INT_FUNCTION("sqrt",	DefaultSymbols::SquareRoot)
	_NZM_INT_FUNCTION("root",	DefaultSymbols::Root)
	_NZM_INT_FUNCTION("sin",	DefaultSymbols::Sine)
	_NZM_INT_FUNCTION("asin",	DefaultSymbols::Arcsine)
	_NZM_INT_FUNCTION("csc",	DefaultSymbols::Cosecant)
	_NZM_INT_FUNCTION("cos",	DefaultSymbols::Cosine)
	_NZM_INT_FUNCTION("acos",	DefaultSymbols::Arccosine)
	_NZM_INT_FUNCTION("sec",	DefaultSymbols::Secant)
	_NZM_INT_FUNCTION("tan",	DefaultSymbols::Tangent)
	_NZM_INT_FUNCTION("atan",	DefaultSymbols::Arctangent)
	_NZM_INT_FUNCTION("cot",	DefaultSymbols::Cotangent)
	_NZM_INT_FUNCTION("ln",		DefaultSymbols::LogNatural)
	_NZM_INT_FUNCTION("log",	DefaultSymbols::LogTen)
	_NZM_INT_FUNCTION("abs",	DefaultSymbols::AbsoluteValue)
	_NZM_INT_FUNCTION("avg",	DefaultSymbols::Average)
	_NZM_INT_FUNCTION("min",	DefaultSymbols::Minimum)
	_NZM_INT_FUNCTION("max",	DefaultSymbols::Maximum)

	_NZM_INT_FUNCTION("upcs",	DefaultSymbols::Uppercase)
	_NZM_INT_FUNCTION("lwcs",	DefaultSymbols::Lowercase)
	_NZM_INT_FUNCTION("substr",	DefaultSymbols::Substring)

	#undef _NZM_CONSTANT
	#undef _NZM_INT_FUNCTION
}

////////////////////////////////////////////////////////////////////////////////
// Expression

Expression::Expression(Eval* pEval, int nType, char cOperator, const Value* pObjValue,
					   std::string strValue, bool bNegative)
{
	m_pEval = pEval;
	m_nType = nType;
	m_cOperator = cOperator;
	m_pChildList = new ExpressionList();
	if (pObjValue)
		m_value = *pObjValue;
	m_string = strValue;
	m_bNegative = bNegative;
}

Expression::Expression(const Expression& src)
{
	m_pChildList = new ExpressionList();
	*this = src;
}

Expression::~Expression()
{
	delete m_pChildList;
}

Expression& Expression::operator=(const Expression& src)
{
	m_pEval = src.GetEval();
	m_nType = src.GetType();
	m_cOperator = src.GetOperator();
	m_value = src.GetValue();
	m_string = src.GetString();
	m_bNegative = src.IsNegative();

	*m_pChildList = *src.GetChildList();

	return *this;
}

const Expression* Expression::FindFirstSymbolReference(std::string id) const
{
	if ((GetType() == EXPR_VARIABLE || GetType() == EXPR_FUNCTION) &&
		m_string == id)
		return this;

	Expression* pRet = NULL;
	size_t n = GetChildList()->GetCount(), i = 0;
	while (i < n && !pRet)
		pRet = GetChildList()->Get(i++)->FindFirstSymbolReference(id);

	return pRet;
}

Expression* Expression::FindFirstSymbolReference(std::string id)
{
	return const_cast<Expression*>(((const Expression*)this)->FindFirstSymbolReference(id));
}

void Expression::SetEval(Eval* pEval)
{
	m_pEval = pEval;

	size_t n = GetChildList()->GetCount();
	while (n--)
		GetChildList()->Get(n)->SetEval(pEval);
}

void Expression::SetValue(const Value& value, bool bTypeValue)
{
	if (bTypeValue)
		SetType(EXPR_VALUE);

	m_value = value;
}

////////////////////////////////////////////////////////////////////////////////
// ExpressionList

ExpressionList::ExpressionList()
{
}

ExpressionList::ExpressionList(const ExpressionList& src)
{
	*this = src;
}

ExpressionList::~ExpressionList()
{
	Clear();
}

ExpressionList& ExpressionList::operator=(const ExpressionList& src)
{
	Clear();

	size_t nCount = src.GetCount();
	for (size_t i = 0; i < nCount; i++)
		Add(src.Get(i));

	return *this;
}

void ExpressionList::Insert(size_t nPos, Expression* pExpr, bool bCopy)
{
	if (!pExpr)
		return;

	m_vec.insert(m_vec.begin() + nPos,bCopy ? new Expression(*pExpr) : pExpr);
}

void ExpressionList::Add(Expression* pExpr, bool bCopy)
{
	m_vec.push_back(bCopy ? new Expression(*pExpr) : pExpr);
}

void ExpressionList::Remove(size_t nIndex, bool bDelete, Expression** pDestPtr)
{
	if (bDelete)
		delete m_vec[nIndex];
	else if (pDestPtr)
		*pDestPtr = m_vec[nIndex];

	m_vec.erase(m_vec.begin() + nIndex);
}

// deletes expressions
void ExpressionList::Clear()
{
	size_t nCount = GetCount();
	for (size_t i = 0; i < nCount; i++)
		delete m_vec[i];

	m_vec.clear();
}

////////////////////////////////////////////////////////////////////////////////
// Eval

Eval::Eval(std::string strExpression, decimal_t decTrigFactor, bool bAddDefaultSymbols)
{
	m_pEqualExpression = NULL;
	m_pRootExpression = NULL; // must set to NULL for "delete m_pRootExpression"
	m_pSymbolList = new SymbolList();
	if (bAddDefaultSymbols)
		AddDefaultSymbols();

	if (strExpression != "")
	{
		GenerateTree(strExpression);
		if (CALCSUCCESS(GetLastReturn()))
			Eval();
	}

	m_decTrigFactor = decTrigFactor;
}

Eval::Eval(const Eval& src)
{
	m_pEqualExpression = NULL;
	m_pRootExpression = NULL;
	m_pSymbolList = NULL;
	*this = src;
}

Eval::~Eval()
{
	delete m_pEqualExpression;
	delete m_pRootExpression;
	delete m_pSymbolList;
}

Eval& Eval::operator=(const Eval& src)
{
	delete m_pEqualExpression;
	delete m_pRootExpression;

	if (src.GetRootExpression())
	{
		m_pRootExpression = new Expression(*src.GetRootExpression());
		m_pRootExpression->SetEval(this);
	}

	if (src.GetEqualExpression())
	{
		m_pEqualExpression = new Expression(*src.GetEqualExpression());
		m_pEqualExpression->SetEval(this);
	}

	*m_pSymbolList = *src.GetSymbolList();
	m_decTrigFactor = src.GetTrigFactor();
	
	return *this;
}

// NOTE: only works with expressions/equations without '='
void Eval::QuickEval(Return& retval, std::string strExpression)
{
	Expression* pRootExpr;

	_GenTree(retval,strExpression.c_str(),&pRootExpr); // sets pRootExpr... must delete later
	if (CALCSUCCESS(retval))
		_EvaluateTree(retval,pRootExpr);

	delete pRootExpr;
}

void Eval::GenerateTree(std::string strExpression)
{
	delete m_pEqualExpression;
	delete m_pRootExpression;

	_GenTree(m_ret,strExpression.c_str(),&m_pRootExpression,
		&m_pEqualExpression);
}

void Eval::Evaluate()
{
	m_ret.status = CALC_NOSTATUS;
	if (m_pRootExpression)
		_EvaluateTree(m_ret,m_pRootExpression);
}

std::string Expression::GetExpressionString(bool bSpaces, bool bOuterParentheses, bool bQuoteStrings) const
{
	std::string ret;

	switch (m_nType)
	{
		case EXPR_VALUE:
			ret = GetValue().ToString(bQuoteStrings);
			break;			

		case EXPR_VARIABLE:
			ret = GetString();
			break;

		case EXPR_FUNCTION:
		{
			ret = GetString();
			ret += '(';

			size_t nNum = GetChildList()->GetCount();
			for (size_t i = 0; i < nNum; i++)
			{
				ret += GetChildList()->Get(i)->GetExpressionString(bSpaces,false);
				ret += (i == nNum - 1) ? ')' : ',';
			}

			break;
		}

		case EXPR_GROUP:
		{
			if (bOuterParentheses) ret = '(';

			size_t nNum = GetChildList()->GetCount();
			for (size_t i = 0; i < nNum; i++)
			{
				if (i > 0)
				{
					if (bSpaces) ret += ' ';
					ret += GetChildList()->Get(i)->GetOperator();
					if (bSpaces) ret += ' ';
				}

				ret += GetChildList()->Get(i)->GetExpressionString(bSpaces,true);
			}

			if (bOuterParentheses) ret += ')';

			break;
		}
	}

	if (IsNegative())
		ret = '-' + ret;

	return ret;
}

////////////////////////////////////////////////////////////////////////////////
//
//	Original algorithm implementations © 2002, Roman Nurik
//
//	Algorithms used:
//	 • Deterministic finite state automata (DFA/FSA)
//	 • Token-based expression tree generation
//	 • Recursive mathematical expression tree evaluation
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// DFAs and Legend:

// 0: SYNTAX ERROR
// 1: TOKEN (param)
// 2: TOKEN (param), VAL Char
// 3: VAL += Char
// 4: no spaces: VAL += Char; spaces: SYNTAX ERROR
// 5: STATE (param)
// 6: STATE (param), VAL += Char
// 7: no spaces: STATE (param), VAL += Char; spaces: SYNTAX ERROR
// 8: complete = expression

/*struct { int action; bool complete; int param; }
g_EqTokenDFA[10][12] = // everything before = sign must follow this DFA
					   // NOTE: LiteralString means a function parameter in this DFA
{
	//	Start						Number						Number.Exponent			Comma						Open Paren					Close Paren
	//	String						LiteralString				LiteralString.End		LiteralString.Escape		Operator					Sign

	{	0,0,0,						0,0,0,						0,0,0,					0,0,0,						0,0,0,						0,0,0,					// 0-9, .
		0,0,0,						0,0,0,						0,0,0,					0,0,0,						0,0,0,						0,0,0					},

	{	0,0,0,						0,0,0,						0,0,0,					0,0,0,						0,0,0,						0,0,0,					// ,
		0,0,0,						1,1,TOKEN_COMMA,			0,0,0,					0,0,0,						0,0,0,						0,0,0					},

	{	0,0,0,						0,0,0,						0,0,0,					0,0,0,						0,0,0,						0,0,0,					// (
		1,0,TOKEN_OPEN_PAREN,		0,0,0,						0,0,0,					0,0,0,						0,0,0,						0,0,0					},

	{	0,0,0,						0,0,0,						0,0,0,					0,0,0,						0,0,0,						0,0,0,					// )
		0,0,0,						1,1,TOKEN_CLOSE_PAREN,		0,0,0,					0,0,0,						0,0,0,						0,0,0					},

	{	2,0,TOKEN_STRING,			0,0,0,						0,0,0,					2,1,TOKEN_LITERAL_STRING,	2,1,TOKEN_LITERAL_STRING,	0,0,0,					// e, E
		4,0,0,						4,0,0,						0,0,0,					0,0,0,						0,0,0,						0,0,0					},

	{	2,0,TOKEN_STRING,			0,0,0,						0,0,0,					2,1,TOKEN_LITERAL_STRING,	2,1,TOKEN_LITERAL_STRING,	0,0,0,					// alnum
		4,0,0,						4,0,0,						0,0,0,					0,0,0,						0,0,0,						0,0,0					},

	{	0,0,0,						0,0,0,						0,0,0,					0,0,0,						0,0,0,						8,1,0,					// =
		8,1,0,						0,0,0,						0,0,0,					0,0,0,						0,0,0,						0,0,0					},

	{	0,0,0,						0,0,0,						0,0,0,					0,0,0,						0,0,0,						0,0,0,					// operator
		0,0,0,						0,0,0,						0,0,0,					0,0,0,						0,0,0,						0,0,0					},

	{	0,0,0,						0,0,0,						0,0,0,					0,0,0,						0,0,0,						0,0,0,					// "
		0,0,0,						0,0,0,						0,0,0,					0,0,0,						0,0,0,						0,0,0					},

	{	0,0,0,						0,0,0,						0,0,0,					0,0,0,						0,0,0,						0,0,0,					// '\\'
		0,0,0,						0,0,0,						0,0,0,					0,0,0,						0,0,0,						0,0,0					},
};*/

// 0: SYNTAX ERROR
// 1: TOKEN (param)
// 2: TOKEN (param), VAL Char
// 3: VAL += Char
// 4: no spaces: VAL += Char; spaces: SYNTAX ERROR
// 5: STATE (param)
// 6: STATE (param), VAL += Char
// 7: no spaces: STATE (param), VAL += Char; spaces: SYNTAX ERROR
// 8: complete = expression

struct { int action; bool complete; int param; }
g_TokenDFA[17][18] =
{
	//  the order in which these tokens are is based on their TOKEN_... value

	//	Start						Number						Number.Exponent				Comma						Open Paren					Close Paren
	//	Open Bracket				Close Bracket				Open Curly Brace			Close Curly Brace			Less Than Sign				Greater Than Sign
	//	String						LiteralString				LiteralString.End			LiteralString.Escape		Operator					Sign

	{	2,0,TOKEN_NUMBER,			4,0,0,						7,0,TOKEN_NUMBER,			2,1,TOKEN_NUMBER,			2,1,TOKEN_NUMBER,			0,0,0,						// 0-9, .
		2,1,TOKEN_NUMBER,			0,0,0,						2,1,TOKEN_NUMBER,			0,0,0,						2,1,TOKEN_NUMBER,			0,0,0,
		4,0,0,						3,0,0,						0,0,0,						0,0,0,						2,1,TOKEN_NUMBER,			2,1,TOKEN_NUMBER			},

	{	0,0,0,						1,1,TOKEN_COMMA,			0,0,0,						0,0,0,						0,0,0,						1,1,TOKEN_COMMA,			// ,
		0,0,0,						1,1,TOKEN_COMMA,			0,0,0,						1,1,TOKEN_COMMA,			0,0,0,						1,1,TOKEN_COMMA,
		1,1,TOKEN_COMMA,			3,0,0,						1,1,TOKEN_COMMA,			0,0,0,						0,0,0,						0,0,0						},

	{	1,0,TOKEN_OPEN_PAREN,		0,0,0,						0,0,0,						1,1,TOKEN_OPEN_PAREN,		1,1,TOKEN_OPEN_PAREN,		0,0,0,						// (
		1,1,TOKEN_OPEN_PAREN,		0,0,0,						0,0,0,						0,0,0,						0,0,0,						0,0,0,
		1,0,TOKEN_OPEN_PAREN,		3,0,0,						0,0,0,						0,0,0,						1,1,TOKEN_OPEN_PAREN,		2,1,TOKEN_OPEN_PAREN		},

	{	0,0,0,						1,1,TOKEN_CLOSE_PAREN,		0,0,0,						0,0,0,						1,1,TOKEN_CLOSE_PAREN,		1,1,TOKEN_CLOSE_PAREN,		// )
		0,0,0,						1,1,TOKEN_CLOSE_PAREN,		0,0,0,						1,1,TOKEN_CLOSE_PAREN,		0,0,0,						1,1,TOKEN_CLOSE_PAREN,
		1,1,TOKEN_CLOSE_PAREN,		3,0,0,						1,1,TOKEN_CLOSE_PAREN,		0,0,0,						0,0,0,						0,0,0						},

	{	1,0,TOKEN_OPEN_BRACKET,		0,0,0,						0,0,0,						1,1,TOKEN_OPEN_BRACKET,		1,1,TOKEN_OPEN_BRACKET,		0,0,0,						// [
		1,1,TOKEN_OPEN_BRACKET,		0,0,0,						0,0,0,						0,0,0,						0,0,0,						0,0,0,
		1,0,TOKEN_OPEN_BRACKET,		3,0,0,						0,0,0,						0,0,0,						1,1,TOKEN_OPEN_BRACKET,		2,1,TOKEN_OPEN_BRACKET		},

	{	0,0,0,						1,1,TOKEN_CLOSE_BRACKET,	0,0,0,						0,0,0,						0,0,0,						1,1,TOKEN_CLOSE_BRACKET,	// ]
		1,1,TOKEN_CLOSE_BRACKET,	1,1,TOKEN_CLOSE_BRACKET,	0,0,0,						1,1,TOKEN_CLOSE_BRACKET,	0,0,0,						1,1,TOKEN_CLOSE_BRACKET,
		1,1,TOKEN_CLOSE_BRACKET,	3,0,0,						1,1,TOKEN_CLOSE_BRACKET,	0,0,0,						0,0,0,						0,0,0						},

	{	1,0,TOKEN_OPEN_CURLY,		0,0,0,						0,0,0,						1,1,TOKEN_OPEN_CURLY,		1,1,TOKEN_OPEN_CURLY,		0,0,0,						// {
		1,1,TOKEN_OPEN_CURLY,		0,0,0,						0,0,0,						0,0,0,						0,0,0,						0,0,0,
		1,0,TOKEN_OPEN_CURLY,		3,0,0,						0,0,0,						0,0,0,						1,1,TOKEN_OPEN_CURLY,		2,1,TOKEN_OPEN_CURLY		},

	{	0,0,0,						1,1,TOKEN_CLOSE_CURLY,		0,0,0,						0,0,0,						0,0,0,						1,1,TOKEN_CLOSE_CURLY,		// }
		0,0,0,						1,1,TOKEN_CLOSE_CURLY,		1,1,TOKEN_CLOSE_CURLY,		1,1,TOKEN_CLOSE_CURLY,		0,0,0,						1,1,TOKEN_CLOSE_CURLY,
		1,1,TOKEN_CLOSE_CURLY,		3,0,0,						1,1,TOKEN_CLOSE_CURLY,		0,0,0,						0,0,0,						0,0,0						},

	{	1,0,TOKEN_LESS_SIGN,		0,0,0,						0,0,0,						1,1,TOKEN_LESS_SIGN,		1,1,TOKEN_LESS_SIGN,		0,0,0,						// <
		1,1,TOKEN_LESS_SIGN,		0,0,0,						0,0,0,						0,0,0,						0,0,0,						0,0,0,
		1,0,TOKEN_LESS_SIGN,		3,0,0,						0,0,0,						0,0,0,						1,1,TOKEN_LESS_SIGN,		2,1,TOKEN_LESS_SIGN			},

	{	0,0,0,						1,1,TOKEN_GREATER_SIGN,		0,0,0,						0,0,0,						0,0,0,						1,1,TOKEN_GREATER_SIGN,		// >
		0,0,0,						1,1,TOKEN_GREATER_SIGN,		0,0,0,						1,1,TOKEN_GREATER_SIGN,		1,1,TOKEN_GREATER_SIGN,		1,1,TOKEN_GREATER_SIGN,
		1,0,TOKEN_GREATER_SIGN,		3,0,0,						1,1,TOKEN_GREATER_SIGN,		0,0,0,						0,0,0,						0,0,0						},

	{	2,0,TOKEN_STRING,			7,0,STATE_NUMBER_EXPONENT,	0,0,0,						2,1,TOKEN_STRING,			2,1,TOKEN_STRING,			0,0,0,						// e, E
		2,1,TOKEN_STRING,			0,0,0,						2,1,TOKEN_STRING,			0,0,0,						2,1,TOKEN_STRING,			0,0,0,
		4,0,0,						3,0,0,						0,0,0,						0,0,0,						2,1,TOKEN_STRING,			2,1,TOKEN_STRING			},

	{	2,0,TOKEN_STRING,			0,0,0,						0,0,0,						2,1,TOKEN_STRING,			2,1,TOKEN_STRING,			0,0,0,						// alnum
		2,1,TOKEN_STRING,			0,0,0,						2,1,TOKEN_STRING,			0,0,0,						2,1,TOKEN_STRING,			0,0,0,
		4,0,0,						3,0,0,						0,0,0,						0,0,0,						2,1,TOKEN_STRING,			2,1,TOKEN_STRING			},

	{	2,0,TOKEN_SIGN,				2,1,TOKEN_OPERATOR,			7,0,TOKEN_NUMBER,			2,1,TOKEN_SIGN,				2,1,TOKEN_SIGN,				2,1,TOKEN_OPERATOR,			// +,-
		2,1,TOKEN_SIGN,				2,1,TOKEN_OPERATOR,			2,1,TOKEN_SIGN,				2,1,TOKEN_OPERATOR,			2,1,TOKEN_SIGN,				2,1,TOKEN_OPERATOR,
		2,1,TOKEN_OPERATOR,			3,0,0,						2,1,TOKEN_OPERATOR,			0,0,0,						2,1,TOKEN_SIGN,				0,0,0						},

	{	0,0,0,						0,0,0,						0,0,0,						0,0,0,						0,0,0,						8,1,0,						// =
		0,0,0,						0,0,0,						0,0,0,						0,0,0,						0,0,0,						0,0,0,
		8,1,0,						3,0,0,						0,0,0,						0,0,0,						0,0,0,						0,0,0						},

	{	0,0,0,						2,1,TOKEN_OPERATOR,			0,0,0,						0,0,0,						0,0,0,						2,1,TOKEN_OPERATOR,			// operator
		0,0,0,						2,1,TOKEN_OPERATOR,			0,0,0,						2,1,TOKEN_OPERATOR,			0,0,0,						2,1,TOKEN_OPERATOR,
		2,1,TOKEN_OPERATOR,			3,0,0,						2,1,TOKEN_OPERATOR,			0,0,0,						0,0,0,						0,0,0						},

	{	1,0,TOKEN_LITERAL_STRING,	0,0,0,						0,0,0,						1,1,TOKEN_LITERAL_STRING,	1,1,TOKEN_LITERAL_STRING,	0,0,0,						// "
		1,1,TOKEN_LITERAL_STRING,	0,0,0,						1,1,TOKEN_LITERAL_STRING,	0,0,0,						1,1,TOKEN_LITERAL_STRING,	0,0,0,
		0,0,0,						5,0,STATE_LITERAL_END,		0,0,0,						6,0,TOKEN_LITERAL_STRING,	1,1,TOKEN_LITERAL_STRING,	0,0,0						},

	{	0,0,0,						0,0,0,						0,0,0,						0,0,0,						0,0,0,						0,0,0,						// '\\'
		0,0,0,						0,0,0,						0,0,0,						0,0,0,						0,0,0,						0,0,0,
		0,0,0,						5,0,STATE_LITERAL_ESCAPE,	0,0,0,						6,0,TOKEN_LITERAL_STRING,	0,0,0,						0,0,0						},
};

void Eval::_GenTree(Return& retval, const char* str, Expression** pDestRootExpr,
					Expression** pDestEqualExpr)
// *pDestRootExpr (and optionally) *pDestEqualExpr should point to valid pointer variables
// pDestRootExpr and pDestEqualExpr will be filled in by this function.. must be deleted later
// pDestEqualExpr is optional
{
	if (pDestEqualExpr)
		*pDestEqualExpr = NULL;

	// NOTE: if '=' operator is encountered, the existing expression will be transferred
	// to pEqualExpr and pRootExpr will be re-newed

	// set up default
	Expression* pRootExpr = new Expression(this,EXPR_GROUP,NO_OP,NULL,""); // the temporary root expression
	*pDestRootExpr = NULL;

	std::vector<Token> vTokenBuffer; // the token list/buffer that gets completed
	std::stack<Expression*> stackExpr; // stack of group expressions

	// expression used when completing tokens... this gets pushed as a child of the
	// topmost expression on the stack when an expression is completed
	Expression exprCur(this);
	bool bCurExprComplete = false; // whether or not exprCur is a complete expression

	// when the expression is pushed as a child, whether or not to push it on the
	// expression stack (for functions, groups, lists)
	bool bStepIntoExpr = false;

	// whether or not a space has been encountered since the last token completion
	bool bSpace = false;

	stackExpr.push(pRootExpr);

	int nState = STATE_START; // current DFA state

	char c = 0;

	int nAction = 0;
	int nCharType = -1;
	bool bComplete = false;
	int nParam = -1;

	// loop while the current character isn't the null-terminator
	for (const char* strPos = str; /* ended at the end of loop code */ ; strPos++)
	{
		c = *strPos;

		// NOTE: this loop will be run until c == 0. when c == 0,
		// the only thing that will be done is the "complete" procedure
		// everything else will be skipped, then the loop will exit

		if (c == 0)
		{
			// reached the end, check if in a final state
			// if not in a final state (number, string, close parentheses), syntax error
			if (nState != TOKEN_NUMBER && nState != TOKEN_STRING &&
				nState != STATE_LITERAL_END && nState != TOKEN_CLOSE_PAREN &&
				nState != TOKEN_CLOSE_BRACKET && nState != TOKEN_CLOSE_CURLY &&
				nState != TOKEN_GREATER_SIGN)
			{
				retval.status = CALC_ERR_SYNTAX;
				retval.nErrPos = (int)(strPos - str) - 1;
				return;
			}

			strPos--; // all syntax errors will be directed to the last character
		}

		if (c != 0)
		{
			if (isspace(c))
			{
				if (nState != STATE_START)
				{
					// a space has been encountered, add the character to the token data
					// if there is a token
					bSpace = true;
					if (vTokenBuffer.size() &&
						nState != STATE_LITERAL_END) // special case for literal strings
						vTokenBuffer.back().data += c;
				}

				continue; // go to next character
			}

			// get the type of character
			nCharType = -1; // start out with invalid char type, then see if it matches
			if (isdigit(c) || (nState != TOKEN_STRING && c == '.'))
				nCharType = 0;
			else if (c == ',')
				nCharType = 1;
			else if (c == '(')
				nCharType = 2;
			else if (c == ')')
				nCharType = 3;
			else if (c == '[')
				nCharType = 4;
			else if (c == ']')
				nCharType = 5;
			else if (c == '{')
				nCharType = 6;
			else if (c == '}')
				nCharType = 7;
			else if (c == '<')
				nCharType = 8;
			else if (c == '>')
				nCharType = 9;
			else if (c == 'e' || c == 'E')
				nCharType = 10;
			else if (isalpha(c) || c == '_')
				nCharType = 11;
			else if (c == '+' || c == '-')
				nCharType = 12;
			else if (c == '=')
				nCharType = 13;
			else if (Expression::GetOperatorPriority(c) != 0)
				nCharType = 14;
			else if (c == '"')
				nCharType = 15;
			else if (c == '\\')
				nCharType = 16;

			if (nCharType < 0) // error, no transition to character
			{
				// special case for literal strings
				if (nState == TOKEN_LITERAL_STRING)
				{
					nAction = 3; // VAL += Char
					bComplete = false;
					nParam = 0;
				}
				else
				{
					retval.status = CALC_ERR_SYNTAX;
					retval.nErrPos = (int)(strPos - str);
					return;
				}
			}
			else
			{
				nAction = g_TokenDFA[nCharType][nState].action;
				bComplete = g_TokenDFA[nCharType][nState].complete;
				nParam = g_TokenDFA[nCharType][nState].param;
			}
		}

		// complete token(s)
		if (bComplete || c == 0)
		{
			if (vTokenBuffer.size() < 1) // this should never happen
			{
				retval.status = CALC_NOSTATUS; // unknown error
				return;
			}

			// analyze token list and generate the expression tree
			switch (vTokenBuffer[0].type)
			{
				case TOKEN_NUMBER:
					// all numbers (after all digits and modifiers have been added to the
					// number token's data)
					rtrim(vTokenBuffer[0].data);
					exprCur.SetValue(
						Value(atof(vTokenBuffer[0].data.c_str())),true);
					bCurExprComplete = true;
					break;

				case TOKEN_COMMA:
					// pop the top expression (make sure there are at least 3
					// expressions on the stack: root, function, parameter)
					if (stackExpr.size() >= 3)
					{
						stackExpr.pop(); // pop the parameter or list item expression

						// make sure we're on a function now, if not, then there's
						// a syntax error with a misplaced comma
						if (stackExpr.top()->GetType() == EXPR_FUNCTION)
						{
							// push a new group (the parameter expression)
							stackExpr.top()->GetChildList()->Add(new
								Expression(this,EXPR_GROUP,NO_OP,NULL,""),false);

							// push the new parameter onto the stack
							stackExpr.push(stackExpr.top()->GetChildList()->GetBack());
						}
						else if (stackExpr.top()->GetType() == EXPR_VALUE && stackExpr.top()->GetValue().GetType() == VALUE_LIST)
						{
							// push a new group (the parameter expression)
							stackExpr.top()->GetValue().GetList().Add(new
								Expression(this,EXPR_GROUP,NO_OP,NULL,""),false);

							// push the new parameter onto the stack
							stackExpr.push(stackExpr.top()->GetValue().GetList().GetBack());
						}
						else
						{
							retval.status = CALC_ERR_SYNTAX;
							retval.nErrPos = (int)(strPos - str);
							return;
						}
					}
					else
					{
						retval.status = CALC_ERR_SYNTAX;
						retval.nErrPos = (int)(strPos - str);
						return;
					}

					break;

				case TOKEN_OPEN_PAREN:
					exprCur.SetType(EXPR_GROUP);
					bStepIntoExpr = true;
					bCurExprComplete = true;
					break;

				case TOKEN_CLOSE_PAREN:
					// first, check if this is a syntax error by making sure
					// there are at least 2 expressions on the stack (root, the expr being
					// closed by this close parentheses)
					if (stackExpr.size() < 2 || (stackExpr.top()->GetType() != EXPR_GROUP && stackExpr.top()->GetType() != EXPR_FUNCTION))
					{
						retval.status = CALC_ERR_PAREN;
						retval.nErrPos = (int)(strPos - str);
						return;
					}

					// pop the expression from the stack
					stackExpr.pop();

					// now check if in a function expression (if the expr just popped was
					// a parameter of it)... if yes, then pop the function too
					if (stackExpr.top()->GetType() == EXPR_FUNCTION)
						stackExpr.pop();

					break;

				case TOKEN_OPEN_BRACKET:
					exprCur.SetType(EXPR_VALUE);
					exprCur.GetValue().SetType(VALUE_LIST);
					bStepIntoExpr = true;
					bCurExprComplete = true;
					break;

				case TOKEN_CLOSE_BRACKET:
				{
					// first, check if this is a syntax error by making sure
					// there are at least 2 expressions on the stack (root, the expr being
					// closed by this close bracket)
					if (stackExpr.size() < 2)
					{
						retval.status = CALC_ERR_SYNTAX;
						retval.nErrPos = (int)(strPos - str);
						return;
					}

					// pop the expression from the stack
					stackExpr.pop();
					stackExpr.pop();

					// now check if in a element access (if the expr just popped was
					// a parameter of it)... if yes, then pop the function too
					//if (stackExpr.top()->GetType() == EXPR_ELEMENT)
					//	stackExpr.pop();

					break;
				}

				case TOKEN_STRING:
					// determine whether the expression is a variable or a function
					// by checking whether or not an open parentheses is in the
					// token buffer (that's why String->'(' doesn't complete the
					// token buffer
					rtrim(vTokenBuffer[0].data);
					if (vTokenBuffer.size() > 1 && vTokenBuffer[1].type == TOKEN_OPEN_PAREN)
					{
						// function
						exprCur.SetString(vTokenBuffer[0].data);
						exprCur.SetType(EXPR_FUNCTION);
						bStepIntoExpr = true;
						bCurExprComplete = true;
					}
					else
					{
						// variable
						exprCur.SetType(EXPR_VARIABLE);
						exprCur.SetString(vTokenBuffer[0].data);
						bCurExprComplete = true;
					}
					break;

				case TOKEN_LITERAL_STRING:
					exprCur.SetValue(Value(vTokenBuffer[0].data),true);
					bCurExprComplete = true;
					break;

				case TOKEN_OPERATOR:
					bCurExprComplete = false;
					exprCur.SetOperator(vTokenBuffer.front().data[0]);
					break;

				case TOKEN_SIGN:
					bCurExprComplete = false;
					exprCur.SetNegative(vTokenBuffer.front().data[0] == '-');
					break;
			}

			// clear the token buffer
			vTokenBuffer.clear();

			if (bCurExprComplete)
			{
				// if the top of the expression stack is a function,
				// then add a group expression (parameter) before
				// adding the current expression
				if (stackExpr.top()->GetType() == EXPR_FUNCTION)
				{
					// push a new parameter onto the function expression
					stackExpr.top()->GetChildList()->Add(
						new Expression(this,EXPR_GROUP,NO_OP,NULL,""),false);

					stackExpr.push(stackExpr.top()->GetChildList()->GetBack());
				}
				else if (stackExpr.top()->GetType() == EXPR_VALUE && stackExpr.top()->GetValue().GetType() == VALUE_LIST)
				{
					// push a new parameter onto the function expression
					stackExpr.top()->GetValue().GetList().Add(
						new Expression(this,EXPR_GROUP,NO_OP,NULL,""),false);

					stackExpr.push(stackExpr.top()->GetValue().GetList().GetBack());
				}

				// push the current expression as a child
				stackExpr.top()->GetChildList()->Add(&exprCur);

				// clear the current expression
				exprCur = Expression(this,EXPR_VALUE,NO_OP,NULL,"");

				// if stepping into the expression, push the
				// current expression (retrieve from current parent) to the stack
				if (bStepIntoExpr)
				{
					//if (stackExpr.top()->GetType() == EXPR_GROUP)
						stackExpr.push(stackExpr.top()->GetChildList()->GetBack());
					//else if (stackExpr.top()->GetType() == EXPR_VALUE && stackExpr.top()->GetValue().GetType() == VALUE_LIST)
					//	stackExpr.push(stackExpr.top()->GetValue().GetList().GetBack());
				}

				bCurExprComplete = false;
				bStepIntoExpr = false;
			}

			bSpace = false; // clear the hasSpace flag
		}

		if (c != 0)
		{
			// DFA state transitions (see above for actions/legend)
			switch (nAction)
			{
				// 0: SYNTAX ERROR
				case 0:
				{
					retval.status = CALC_ERR_SYNTAX;
					retval.nErrPos = (int)(strPos - str);
					return;
				}

				// 1: TOKEN (param)
				case 1: vTokenBuffer.push_back(Token(nParam)); nState = nParam; break;

				// 2: TOKEN (param), VAL Char
				case 2: vTokenBuffer.push_back(Token(nParam,std::string(1,c))); nState = nParam; break;

				// 3: VAL += Char
				case 3: vTokenBuffer.back().data += c; break;

				// 4: no spaces: VAL += Char; spaces: SYNTAX ERROR
				case 4:
					if (bSpace)
					{
						retval.status = CALC_ERR_SYNTAX;
						retval.nErrPos = (int)(strPos - str);
						return;
					}
					else
						vTokenBuffer.back().data += c;
					break;

				// 5: STATE (param)
				case 5: nState = nParam; break;

				// 6: STATE (param), VAL += Char
				case 6: nState = nParam; vTokenBuffer.back().data += c; break;

				// 7: no spaces: STATE (param), VAL += Char; spaces: SYNTAX ERROR
				case 7:
					if (bSpace)
					{
						retval.status = CALC_ERR_SYNTAX;
						retval.nErrPos = (int)(strPos - str);
						return;
					}
					else
					{
						nState = nParam;
						vTokenBuffer.back().data += c;
					}
					break;

				// 8: complete = expression
				case 8:
					if (pDestEqualExpr && *pDestEqualExpr) // only if there was no equal expression already, else syntax error
					{
						retval.status = CALC_ERR_SYNTAX;
						retval.nErrPos = (int)(strPos - str);
						return;
					}
					else
					{
						nState = STATE_START;
						stackExpr.pop(); // pop expression
						*pDestEqualExpr = pRootExpr; // the pRootExpr was actually equals expression
						pRootExpr = new Expression(this,EXPR_GROUP,NO_OP,NULL,""); // create the new expression.. now it's root
						stackExpr.push(pRootExpr);
					}
					break;
			}
		}
		else
			break; // break out of loop if the current character is NULL terminator
	}

	// the only expression left in the stack should be the root expression
	if (stackExpr.size() > 1)
	{
		retval.status = CALC_ERR_PAREN;
		retval.nErrPos = (int)(strPos - str);
		return;
	}

	*pDestRootExpr = pRootExpr;

	retval.status = CALC_SUCCESS;
}

void Eval::_EvaluateTree(Return& retval, Expression* pRootExpr)
{
	pRootExpr->Evaluate(retval,m_pSymbolList,NULL);
}

void Expression::Evaluate(Return& retval, SymbolList* pSymbols,
						  FUNCPARAMMAP* pFuncParams, bool bCanNegate)
{
	retval.status = CALC_NOSTATUS;

	if (!pSymbols)
		return;

	switch (m_nType)
	{
		case EXPR_VALUE:
		{
			retval.status = CALC_SUCCESS;
			if (GetValue().GetType() == VALUE_LIST)
			{
				// for lists, evaluate each expression in the list
				// and return the list with the final evaluated expressions
				Value v;
				v.SetType(VALUE_LIST);

				for (size_t i = 0; i < GetValue().GetList().GetCount(); i++)
				{
					GetValue().GetList().Get(i)->Evaluate(retval,pSymbols,pFuncParams,bCanNegate);
					if (!CALCSUCCESS(retval))
						return;

					v.GetList().Add(new Expression(m_pEval,EXPR_VALUE,NO_OP,&retval.value),false);
				}

				retval.value = v;
			}
			else
				retval.value = GetValue();
			break;
		}

		case EXPR_VARIABLE:
		{
			// check in the function parameter map first, if it exists
			FUNCPARAMMAP::iterator it;
			if (pFuncParams)
				it = pFuncParams->find(GetString());

			if (pFuncParams && it != pFuncParams->end())
			{
				retval.status = CALC_SUCCESS;
				retval.value = (*it).second;
			}
			else
			{
				// now check global variables
				Symbol* pSym = NULL;
				if (pSymbols)
					pSym = pSymbols->Find(GetString());

				if (pSymbols && pSym && pSym->GetType() == SYMBOL_VARIABLE)
				{
					retval.status = CALC_SUCCESS;
					retval.value = pSym->GetValue();
				}
				else
				{
					// unknown symbol
					retval.status = CALC_ERR_SYMBOL;
					retval.badSymbol = GetString();
				}
			}
			break;
		}

		case EXPR_FUNCTION:
		{
            Symbol* pSym = NULL;
			if (pSymbols)
				pSym = pSymbols->Find(GetString());

			if (pSymbols && pSym &&
				(pSym->GetType() == SYMBOL_FUNC ||
				 pSym->GetType() == SYMBOL_INT_FUNC))
			{
				// if the function type is an internal function, make sure
				// the number of parameters passed in is exact
				if (pSym->GetType() == SYMBOL_FUNC &&
					pSym->GetFuncParamList()->size() != GetChildList()->GetCount())
				{
					retval.status = CALC_ERR_FUNCTION;
					retval.nParam = (int)pSym->GetFuncParamList()->size();
					break;
				}

				// first solve all parameter expressions
				Value* arrParams = new Value[GetChildList()->GetCount()];
				for (int i = 0; i < (int)GetChildList()->GetCount(); i++)
				{
					GetChildList()->Get(i)->Evaluate(retval,pSymbols,pFuncParams);
					if (CALCSUCCESS(retval))
						arrParams[i] = retval.value;
					else
					{
						// error, return the error caused in the parameter
						delete [] arrParams;
						return;
					}
				}

				// run either expression or internal function, depending on symbol type
				if (pSym->GetType() == SYMBOL_FUNC)
				{
					// create a parameter symbol map
					FUNCPARAMMAP mapParams;

					// by this point, the given parameter count should be confirmed
					// to equal the expected parameter count of the internal function
					size_t nChildCount = GetChildList()->GetCount();
					for (int i = 0; i < (int)nChildCount; i++)
					{
						// add each given parameter to the parameter map
						mapParams.insert(FUNCPARAMMAP::value_type(
							pSym->GetFuncParamList()->at(i),
							arrParams[i]));
					}

					// evaluate the internal function expression with the given
					// parameter map and standard set of symbols

					// first, set the Eval for the symbol
					pSym->GetFuncExpression()->SetEval(m_pEval);

					if (pSym->GetFuncExpression())
						pSym->GetFuncExpression()->Evaluate(retval,
						pSymbols,&mapParams);

					pSym->GetFuncExpression()->SetEval(NULL); // reset Eval
				}
				else
				{
					// run internal function callback, using its return value as this
					// expression's return value
					if (m_pEval)
						pSym->GetIntFuncCallback()
							(*m_pEval,retval,arrParams,(int)GetChildList()->GetCount());

					if (retval.status == CALC_ERR_FUNCTION)
						retval.badSymbol = GetString();
				}

				delete [] arrParams;
			}
			else
			{
				// unknown symbol
				retval.status = CALC_ERR_SYMBOL;
				retval.badSymbol = GetString();
			}
			break;
		}

////////////////////////////////////////////////////////////////////////////////
// below is the main implementation of the algorithm for evaluating a
// multi-term expression (using the stack algorithm and conforming to "PEMDAS")
		case EXPR_GROUP:
		{
			retval.status = CALC_SUCCESS;

			if (GetChildList()->GetCount() == 0)
			{
				retval.status = CALC_ERR_INCOMPLETE;
				break;
			}
			else if (GetChildList()->GetCount() == 1)
			{
				GetChildList()->Get(0)->Evaluate(retval,pSymbols,pFuncParams);
				break;
			}

			std::stack<Expression> stackExpr;

			// solve the first expression in the group and add the resulting value
			// to the stack if succeeded
			GetChildList()->Get(0)->Evaluate(retval,pSymbols,pFuncParams,false);
			if (CALCSUCCESS(retval))
				stackExpr.push(
					Expression(m_pEval,EXPR_VALUE,GetChildList()->Get(0)->GetOperator(),
					&retval.value,"",GetChildList()->Get(0)->IsNegative()));
			else
				break; // exit, returning the error in retval

			Expression expr1(m_pEval), expr2(m_pEval), exprMerged(m_pEval);

			int i = 1;
			while (1)
			{
				// add all expressions with greater operator ranks than the
				// topmost expression to the stack
				while (i < (int)GetChildList()->GetCount() &&
					(GetOperatorPriority(GetChildList()->Get(i)->GetOperator()) >
					 GetOperatorPriority(stackExpr.top().GetOperator())))
				{
					// before pushing an expression onto the stack, evaluate it
					// and create a new expression as EXPR_VALUE
					GetChildList()->Get(i)->Evaluate(retval,pSymbols,pFuncParams,false);
					if (CALCSUCCESS(retval))
						stackExpr.push(
							Expression(m_pEval,EXPR_VALUE,GetChildList()->Get(i)->GetOperator(),
							&retval.value,"",GetChildList()->Get(i)->IsNegative()));
					else
					{
						// inside two structures, "break" won't work here, so return
						return;
					}
					i++;
				}

				// get and pop two expressions off the stack
				expr2 = stackExpr.top();
				stackExpr.pop();
				expr1 = stackExpr.top();
				stackExpr.pop();

				exprMerged.SetOperator(expr1.GetOperator());

				if (expr2.GetOperator() == '^')
				{
					// exponents cannot use negative to negative operation,
					// must first calculate the non-negative operation result
					// and then set the result to negative

					// if the first expression is negative, then
					// set the merged expression as negative
					// (to avoid -2^2 type errors)
					exprMerged.SetNegative(expr1.IsNegative());

					// calculate the decimal (excluding the first expression's sign)
					if (expr2.IsNegative())
						expr2.GetValue().Negate();

					Return ret; ret.status = CALC_SUCCESS;
					PerformOperation(ret,expr1.GetValue(),expr2.GetOperator(),expr2.GetValue(),
						m_pEval,pSymbols,pFuncParams,bCanNegate);
					if (!CALCSUCCESS(ret))
					{
						retval = ret;
						return;
					}					

					exprMerged.SetValue(ret.value);
				}
				else
				{
					// all operators except exponents can perform negative expression
					// to negative expression binary operations
					exprMerged.SetNegative(false);

					if (expr1.IsNegative())
						expr1.GetValue().Negate();
					if (expr2.IsNegative())
						expr2.GetValue().Negate();


					Return ret; ret.status = CALC_SUCCESS;
					PerformOperation(ret,expr1.GetValue(),expr2.GetOperator(),expr2.GetValue(),
						m_pEval,pSymbols,pFuncParams,bCanNegate);
					if (!CALCSUCCESS(ret))
					{
						retval = ret;
						return;
					}					

					exprMerged.SetValue(ret.value);
				}

				stackExpr.push(exprMerged);

				// if the expression iterator is done then exit, unless
				// there is something left in the stack, then operate
				// on the remaining items
				if (stackExpr.size() <= (size_t)1 && i >= (int)GetChildList()->GetCount())
					break;
			}

			// top of stack is last value (might be IsNegative())
			if (stackExpr.top().IsNegative())
				stackExpr.top().GetValue().Negate();

			retval.value = stackExpr.top().GetValue();
			break;
		}
////////////////////////////////////////////////////////////////////////////////
	}

	if (IsNegative() && bCanNegate)
		retval.value.Negate();
}

int Expression::GetOperatorPriority(char cOperator)
{
	switch (cOperator)
	{
		case '^':
			return 3;

		case '*':
		case '/':
		case '%':
			return 2;

		case '+':
		case '-':
			return 1;

		default:
			return 0; // this includes NO_OP
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
// 3D Operation Matrix:  [operation][left data type][right data type]
// The value at [op][l][r] determines result of [l] op [r]:
//   Note: basic operation means forwarding straight to C++ (must convert to decimal or complex)
//   Note: positioning in the array depends on the VALUE_... constant

enum OpMatrixCodes
{
	OP_ERROR = 0,			// DATA TYPE ERROR
	OP_BASIC = 1,			// Basic operation
	OP_O_EACH = 2,			// Operate on single operand and all elements of other operand
	OP_B_EACH = 3,			// Operate on all elements of both operands IF SAME SIZE, otherwise DOMAIN ERROR

	OP_SPECIAL = 15,		// Special handling (if not handled, return DATA TYPE ERROR)
};

// 6 operations, 4 data types
OpMatrixCodes g_3DOperationMatrix[6][4][4] =
{
	//		Decimal		Complex		String		List

	// Exponent ..  ^
	{
		{	OP_BASIC,	OP_BASIC,	OP_ERROR,	OP_O_EACH	,},		// Decimal
		{	OP_BASIC,	OP_BASIC,	OP_ERROR,	OP_O_EACH	,},		// Complex
		{	OP_ERROR,	OP_ERROR,	OP_ERROR,	OP_ERROR	,},		// String
		{	OP_O_EACH,	OP_O_EACH,	OP_ERROR,	OP_B_EACH	,},		// List
	},

	// Multiplication ..  *
	{
		{	OP_BASIC,	OP_BASIC,	OP_SPECIAL,	OP_O_EACH	,},		// Decimal
		{	OP_BASIC,	OP_BASIC,	OP_ERROR,	OP_O_EACH	,},		// Complex
		{	OP_SPECIAL,	OP_ERROR,	OP_ERROR,	OP_ERROR	,},		// String
		{	OP_O_EACH,	OP_O_EACH,	OP_ERROR,	OP_B_EACH	,},		// List
	},

	// Division ..  /
	{
		{	OP_BASIC,	OP_BASIC,	OP_ERROR,	OP_O_EACH	,},		// Decimal
		{	OP_BASIC,	OP_BASIC,	OP_ERROR,	OP_O_EACH	,},		// Complex
		{	OP_ERROR,	OP_ERROR,	OP_ERROR,	OP_ERROR	,},		// String
		{	OP_O_EACH,	OP_O_EACH,	OP_ERROR,	OP_B_EACH	,},		// List
	},

	// Modulo ..  %
	{
		{	OP_BASIC,	OP_ERROR,	OP_ERROR,	OP_O_EACH	,},		// Decimal
		{	OP_ERROR,	OP_ERROR,	OP_ERROR,	OP_ERROR	,},		// Complex
		{	OP_ERROR,	OP_ERROR,	OP_ERROR,	OP_ERROR	,},		// String
		{	OP_O_EACH,	OP_ERROR,	OP_ERROR,	OP_B_EACH	,},		// List
	},

	// Addition ..  +
	{
		{	OP_BASIC,	OP_BASIC,	OP_SPECIAL,	OP_O_EACH	,},		// Decimal
		{	OP_BASIC,	OP_BASIC,	OP_SPECIAL,	OP_O_EACH	,},		// Complex
		{	OP_SPECIAL,	OP_SPECIAL,	OP_SPECIAL,	OP_O_EACH	,},		// String
		{	OP_O_EACH,	OP_O_EACH,	OP_O_EACH,	OP_B_EACH	,},		// List
	},

	// Subtraction ..  -
	{
		{	OP_BASIC,	OP_BASIC,	OP_ERROR,	OP_O_EACH	,},		// Decimal
		{	OP_BASIC,	OP_BASIC,	OP_ERROR,	OP_O_EACH	,},		// Complex
		{	OP_SPECIAL,	OP_ERROR,	OP_SPECIAL,	OP_ERROR	,},		// String
		{	OP_O_EACH,	OP_O_EACH,	OP_ERROR,	OP_B_EACH	,},		// List
	},
};

void Expression::PerformOperation(Return& ret, const Value& left, char cOperator, const Value& right,
								  Eval* pEval, SymbolList* pSymbols, FUNCPARAMMAP* pFuncParams, bool bCanNegate)
{
	// find the operation number
	int nOpNumber = -1;
	switch (cOperator)
	{
	case '^': nOpNumber = 0; break;
	case '*': nOpNumber = 1; break;
	case '/': nOpNumber = 2; break;
	case '%': nOpNumber = 3; break;
	case '+': nOpNumber = 4; break;
	case '-': nOpNumber = 5; break;
	}

	if (nOpNumber < 0)
	{
		ret.status = CALC_ERR_DOMAIN;
		return;
	}

	OpMatrixCodes nOpAction = g_3DOperationMatrix[nOpNumber][left.GetType() - 1][right.GetType() - 1];

	ret.status = CALC_SUCCESS;

	// now, perform the operation based on some help from the action matrix
	switch (nOpAction)
	{
		case OP_ERROR:
			ret.status = CALC_ERR_DATA_TYPE;
			ret.nParam = left.GetType();
			ret.nParam2 = right.GetType();
			return;

		case OP_BASIC:
			_PerformBasicOperation(ret,left,cOperator,right);
			break;

		case OP_O_EACH:
		{
			// check which operand is multi-element and which isn't (e.g. list is multi-element)
			Value vRet;									// the final return value
			ExpressionList* pRetItems = NULL;			// item list of the return value
			Value vSingleValue;							// value of the single item operand
			const ExpressionList* pMultiItems = NULL;	// item list of the multiple item operand
			bool bLeftIsSingle = false;					// tells if the single item operand is the left one

			if (left.GetType() == VALUE_LIST)
			{
				vRet.SetType(VALUE_LIST);
				pRetItems = &vRet.GetList();
				vSingleValue = right;
				pMultiItems = &left.GetList();
				bLeftIsSingle = false;
			}
			else if (right.GetType() == VALUE_LIST)
			{
				vRet.SetType(VALUE_LIST);
				pRetItems = &vRet.GetList();
				vSingleValue = left;
				pMultiItems = &right.GetList();
				bLeftIsSingle = true;
			}

			for (size_t i = 0; i < pMultiItems->GetCount(); i++)
			{
				pMultiItems->Get(i)->Evaluate(ret,pSymbols,pFuncParams,bCanNegate);
				if (!CALCSUCCESS(ret))
					return;

				Return ret3; // don't pass same object as both a parameter and return into PerformOperation
				PerformOperation(ret3,bLeftIsSingle ? left : ret.value,cOperator,bLeftIsSingle ? ret.value : right,
					pEval,pSymbols,pFuncParams,bCanNegate);
				if (!CALCSUCCESS(ret3))
					return;

				pRetItems->Add(new Expression(pEval,EXPR_VALUE,NO_OP,&ret3.value),false);
			}

			ret.value = vRet;
			break;
		}

		case OP_B_EACH:
		{
			Value vRet;									// the final return value
			ExpressionList* pRetItems = NULL;			// item list of the return value
			const ExpressionList* pLeftItems = NULL;	// item list of the left operand
			const ExpressionList* pRightItems = NULL;	// item list of the right operand

			if (left.GetType() == VALUE_LIST && right.GetType() == VALUE_LIST)
			{
				vRet.SetType(VALUE_LIST);
				pRetItems = &vRet.GetList();
				pLeftItems = &left.GetList();
				pRightItems = &right.GetList();

				if (left.GetList().GetCount() != right.GetList().GetCount())
				{
					ret.status = CALC_ERR_DOMAIN;
					return;
				}
			}
			else
			{
				ret.status = CALC_ERR_DATA_TYPE;
				ret.nParam = left.GetType();
				ret.nParam2 = left.GetType();
				return;
			}

			for (size_t i = 0; i < pLeftItems->GetCount(); i++)
			{
				Return ret2;

				pLeftItems->Get(i)->Evaluate(ret,pSymbols,pFuncParams,bCanNegate);
				if (!CALCSUCCESS(ret))
					return;

				pRightItems->Get(i)->Evaluate(ret2,pSymbols,pFuncParams,bCanNegate);
				if (!CALCSUCCESS(ret2))
				{
					ret = ret2; // forward the error properly
					return;
				}

				Return ret3; // don't pass same object as both a parameter and return into PerformOperation
				PerformOperation(ret3,ret.value,cOperator,ret2.value,pEval,pSymbols,pFuncParams,bCanNegate);
				if (!CALCSUCCESS(ret))
					return;

				pRetItems->Add(new Expression(pEval,EXPR_VALUE,NO_OP,&ret3.value),false);
			}

			ret.value = vRet;
			break;
		}

		case OP_SPECIAL:
			switch (cOperator)
			{
				case '*':
					if (left.GetType() == VALUE_DECIMAL && right.GetType() == VALUE_STRING)
					{
						std::string str = "";
						for (int i = 0; i < (int)left.GetDecimal(); i++)
							str += right.GetString();

						ret.value = Value(str);
					}
					else if (left.GetType() == VALUE_STRING && right.GetType() == VALUE_DECIMAL)
					{
						std::string str = "";
						for (int i = 0; i < (int)right.GetDecimal(); i++)
							str += left.GetString();

						ret.value = Value(str);
					}
					break;

				case '+':
					if (left.GetType() == VALUE_STRING || right.GetType() == VALUE_STRING)
						ret.value = Value(left.ToString(false) + right.ToString(false));
					break;

				case '-':
					if (left.GetType() == VALUE_STRING)
					{
						std::string str = left.GetString();

						if (right.GetType() == VALUE_DECIMAL)
						{
							// chop off a certain number of characters from the end
							str.erase(str.end() - (int)std::min((int)str.length(),(int)right.GetDecimal()),str.end());
						}
						else if (right.GetType() == VALUE_STRING)
						{
							// delete all occurences of characters in second string
							size_t pos = 0;
							while (1)
							{
								pos = str.find_first_of(right.GetString(),pos);
								if (pos == std::string::npos)
									break;

								str.erase(str.begin() + pos);
							}
						}

						ret.value = Value(str);
					}
					break;
			}
			break;
	}
	
	return;
}

void Expression::_PerformBasicOperation(Return& ret, const Value& left, char cOperator, const Value& right)
{
	// HACK: this is probably inefficient to convert to complex first, but it saves lots of coding
	// oh well... we're not looking for efficiency just yet
	ret.status = CALC_SUCCESS;

	if (cOperator == '%')
	{
		if (left.GetType() == VALUE_DECIMAL && right.GetType() == VALUE_DECIMAL)
			ret.value = Value(fmod(left.GetDecimal(),right.GetDecimal()));
		else
		{
			ret.status = CALC_ERR_DATA_TYPE;
			ret.nParam = left.GetType();
			ret.nParam2 = right.GetType();
		}
	}
	else
	{
		complex_t a, b;

		if (left.GetType() == VALUE_DECIMAL)
			a = complex_t(left.GetDecimal(),0);
		else
			a = left.GetComplex();

		if (right.GetType() == VALUE_DECIMAL)
			b = complex_t(right.GetDecimal(),0);
		else
			b = right.GetComplex();

		switch (cOperator)
		{
			case '^':
				if (a.imag() == 0 && a.real() == 0 && b.imag() == 0 && b.real() == 0)
					ret.status = CALC_ERR_DOMAIN;
				else
					ret.value = pow(a,b);
				break;

			case '*': ret.value = Value(a * b); break;
			case '/': ret.value = Value(a / b); break;
			case '+': ret.value = Value(a + b); break;
			case '-': ret.value = Value(a - b); break;
		}

		// HACK: for some things, values get really close to 0, so make them 0
		if (CALCSUCCESS(ret))
		{
			if (ret.value.GetType() == VALUE_COMPLEX)
			{
				if (equalThresh(ret.value.GetComplex().imag(),0.0))
					ret.value.GetComplex().imag(0.0);
				if (equalThresh(ret.value.GetComplex().real(),0.0))
					ret.value.GetComplex().real(0.0);
			}
			else if (ret.value.GetType() == VALUE_DECIMAL)
			{
				if (equalThresh(ret.value.GetDecimal(),0.0))
					ret.value.SetDecimal(0.0);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// default symbols

void DefaultSymbols::Floor(Eval&v,Return&r,Value*p,int c)
{if(c!=1){r.status=CALC_ERR_FUNCTION;r.nParam=1;return;}
 if(p[0].GetType()!=VALUE_DECIMAL){r.status=CALC_ERR_DATA_TYPE;r.nParam=p[0].GetType();return;}
 r.value=Value(floor(p[0].GetDecimal()));r.status=CALC_SUCCESS;}

void DefaultSymbols::Ceil(Eval&v,Return&r,Value*p,int c)
{if(c!=1){r.status=CALC_ERR_FUNCTION;r.nParam=1;return;}
 if(p[0].GetType()!=VALUE_DECIMAL){r.status=CALC_ERR_DATA_TYPE;r.nParam=p[0].GetType();return;}
 r.value=Value(ceil(p[0].GetDecimal()));r.status=CALC_SUCCESS;}

unsigned int fact(unsigned int n)
{
	if (n == 0) return 1;
	unsigned int j = n;
	while (--n > 0)
		j *= n;
	return j;
}

// TODO: complex factorial
void DefaultSymbols::Factorial(Eval&v,Return&r,Value*p,int c)
{if(c!=1){r.status=CALC_ERR_FUNCTION;r.nParam=1;return;}
 if(p[0].GetType()!=VALUE_DECIMAL){r.status=CALC_ERR_DATA_TYPE;r.nParam=p[0].GetType();return;}
 if(!iswhole(p[0].GetDecimal())||p[0].GetDecimal()<0){r.status=CALC_ERR_INPUT;return;}
 r.value=Value(fact((int)p[0].GetDecimal()));r.status=CALC_SUCCESS;}

unsigned int perm(unsigned int n, unsigned int r)
{
	if (r == 0) return 1;
	unsigned int j = n;
	unsigned int k = n - r;
	while (--n > k)
		j *= n;
	return j;
}

// TODO: complex permutation
void DefaultSymbols::Permutation(Eval&v,Return&r,Value*p,int c)
{if(c!=2){r.status=CALC_ERR_FUNCTION;r.nParam=2;return;}
 if(p[0].GetType()!=VALUE_DECIMAL){r.status=CALC_ERR_DATA_TYPE;r.nParam=p[0].GetType();return;}
 if(p[1].GetType()!=VALUE_DECIMAL){r.status=CALC_ERR_DATA_TYPE;r.nParam=p[1].GetType();return;}
 if(!iswhole(p[0].GetDecimal())||p[0].GetDecimal()<0){r.status=CALC_ERR_INPUT;return;}
 if(!iswhole(p[1].GetDecimal())||p[1].GetDecimal()<0){r.status=CALC_ERR_INPUT;return;}
 if(p[1].GetDecimal()>p[0].GetDecimal()){r.status=CALC_ERR_INPUT;return;}
 r.value=Value(perm((int)p[0].GetDecimal(),(int)p[1].GetDecimal()));
 r.status=CALC_SUCCESS;}

// TODO: complex combination
void DefaultSymbols::Combination(Eval&v,Return&r,Value*p,int c)
{if(c!=2){r.status=CALC_ERR_FUNCTION;r.nParam=2;return;}
 if(p[0].GetType()!=VALUE_DECIMAL){r.status=CALC_ERR_DATA_TYPE;r.nParam=p[0].GetType();return;}
 if(p[1].GetType()!=VALUE_DECIMAL){r.status=CALC_ERR_DATA_TYPE;r.nParam=p[1].GetType();return;}
 if(!iswhole(p[0].GetDecimal())||p[0].GetDecimal()<0){r.status=CALC_ERR_INPUT;return;}
 if(!iswhole(p[1].GetDecimal())||p[1].GetDecimal()<0){r.status=CALC_ERR_INPUT;return;}
 if(p[1].GetDecimal()>p[0].GetDecimal()){r.status=CALC_ERR_INPUT;return;}
 r.value=Value(perm((int)p[0].GetDecimal(),(int)p[1].GetDecimal())/fact((int)p[1].GetDecimal()));
 r.status=CALC_SUCCESS;}

void DefaultSymbols::Compare(Eval&v,Return&r,Value*p,int c)
{if(c!=2){r.status=CALC_ERR_FUNCTION;r.nParam=2;return;}
 if(p[0].GetType()==VALUE_DECIMAL&&p[1].GetType()==VALUE_DECIMAL){
 if(p[0].GetDecimal()>p[1].GetDecimal())r.value=1.0;
 else if(p[0].GetDecimal()<p[1].GetDecimal())r.value=-1.0;
 else r.value=0.0;}
 else if(p[0].GetType()==VALUE_STRING&&p[1].GetType()==VALUE_STRING)
 r.value=(decimal_t)p[0].GetString().compare(p[1].GetString());
 /*else if(p[0].GetType()==VALUE_COMPLEX&&p[1].GetType()==VALUE_COMPLEX){
 if(p[0].GetComplex()>p[1].GetComplex())r.value=1.0;
 else if(p[0].GetComplex()<p[1].GetComplex())r.value=-1.0;
 else r.value=0.0;}
 else if(p[0].GetType()==VALUE_STRING&&p[1].GetType()==VALUE_STRING)*/
 else{r.status=CALC_ERR_DATA_TYPE;r.nParam=p[1].GetType();return;}
 r.status=CALC_SUCCESS;}

void DefaultSymbols::SquareRoot(Eval&v,Return&r,Value*p,int c)
{if(c!=1){r.status=CALC_ERR_FUNCTION;r.nParam=1;return;}
 if(p[0].GetType()==VALUE_DECIMAL){r.value=Value(sqrt(p[0].GetDecimal()));r.status=CALC_SUCCESS;}
 else if(p[0].GetType()==VALUE_COMPLEX){r.value=Value(sqrt(p[0].GetComplex()));r.status=CALC_SUCCESS;}
 else{r.status=CALC_ERR_DATA_TYPE;r.nParam=p[0].GetType();return;}}

// TODO: complex roots
void DefaultSymbols::Root(Eval&v,Return&r,Value*p,int c)
{if(c!=2){r.status=CALC_ERR_FUNCTION;r.nParam=2;return;}
 /*if((p[0].GetType()==VALUE_DECIMAL||p[0].GetType()==VALUE_COMPLEX)&&
 (p[1].GetType()==VALUE_DECIMAL||p[1].GetType()==VALUE_COMPLEX))
 {r.value=Value(*/
 if(p[0].GetType()!=VALUE_DECIMAL){r.status=CALC_ERR_DATA_TYPE;r.nParam=p[0].GetType();return;}
 if(p[1].GetType()!=VALUE_DECIMAL){r.status=CALC_ERR_DATA_TYPE;r.nParam=p[1].GetType();return;}
 if(p[0].GetDecimal()==0.0){r.status=CALC_ERR_INPUT;return;}
 r.value=Value(pow((decimal_t)p[0].GetDecimal(),(decimal_t)1.0/p[1].GetDecimal()));r.status=CALC_SUCCESS;}

// TODO: sinh, cosh, tanh, arcsinh, arccosh, arctanh, complex trigonometry
decimal_t QSine(Eval&v,decimal_t p)
{if(v.GetTrigFactor()!=1.0){
 while(p>=360.0)p-=360.0;while(p<0.0)p+=360.0;
 if(p==0.0)return 0.0;else if(p==90.0)return 1.0;
 else if(p==180.0)return 0.0;else if(p==270.0)return -1.0;}
 return sin(p*v.GetTrigFactor());}

void DefaultSymbols::Sine(Eval&v,Return&r,Value*p,int c)
{if(c!=1){r.status=CALC_ERR_FUNCTION;r.nParam=1;return;}
 if(p[0].GetType()!=VALUE_DECIMAL){r.status=CALC_ERR_DATA_TYPE;r.nParam=p[0].GetType();return;}
 r.value=Value(QSine(v,p[0].GetDecimal()));r.status=CALC_SUCCESS;}

void DefaultSymbols::Arcsine(Eval&v,Return&r,Value*p,int c)
{if(c!=1){r.status=CALC_ERR_FUNCTION;r.nParam=1;return;}
 if(p[0].GetType()!=VALUE_DECIMAL){r.status=CALC_ERR_DATA_TYPE;r.nParam=p[0].GetType();return;}
 r.value=Value(asin(p[0].GetDecimal())/v.GetTrigFactor());r.status=CALC_SUCCESS;}

void DefaultSymbols::Cosecant(Eval&v,Return&r,Value*p,int c)
{if(c!=1){r.status=CALC_ERR_FUNCTION;r.nParam=1;return;}
 if(p[0].GetType()!=VALUE_DECIMAL){r.status=CALC_ERR_DATA_TYPE;r.nParam=p[0].GetType();return;}
 r.value=Value(1/QSine(v,p[0].GetDecimal()));r.status=CALC_SUCCESS;}

decimal_t QCosine(Eval&v,decimal_t p)
{if(v.GetTrigFactor()!=1.0){
 while(p>=360.0)p-=360.0;while(p<0.0)p+=360.0;
 if(p==0.0)return 1.0;else if(p==90.0)return 0.0;
 else if(p==180.0)return -1.0;else if(p==270.0)return 0.0;}
 return cos(p*v.GetTrigFactor());}

void DefaultSymbols::Cosine(Eval&v,Return&r,Value*p,int c)
{if(c!=1){r.status=CALC_ERR_FUNCTION;r.nParam=1;return;}
 if(p[0].GetType()!=VALUE_DECIMAL){r.status=CALC_ERR_DATA_TYPE;r.nParam=p[0].GetType();return;}
 r.value=Value(QCosine(v,p[0].GetDecimal()));r.status=CALC_SUCCESS;}

void DefaultSymbols::Arccosine(Eval&v,Return&r,Value*p,int c)
{if(c!=1){r.status=CALC_ERR_FUNCTION;r.nParam=1;return;}
 if(p[0].GetType()!=VALUE_DECIMAL){r.status=CALC_ERR_DATA_TYPE;r.nParam=p[0].GetType();return;}
 r.value=Value(acos(p[0].GetDecimal())/v.GetTrigFactor());r.status=CALC_SUCCESS;}

void DefaultSymbols::Secant(Eval&v,Return&r,Value*p,int c)
{if(c!=1){r.status=CALC_ERR_FUNCTION;r.nParam=1;return;}
 if(p[0].GetType()!=VALUE_DECIMAL){r.status=CALC_ERR_DATA_TYPE;r.nParam=p[0].GetType();return;}
 r.value=Value(1/QCosine(v,p[0].GetDecimal()));r.status=CALC_SUCCESS;}

decimal_t QTangent(Eval&v,decimal_t p)
{if(v.GetTrigFactor()!=1.0){
 while(p>=360.0)p-=360.0;while(p<0.0)p+=360.0;
 if(p==0.0||p==180.0)return 0.0;else if(p==90.0||p==270.0 &&
 std::numeric_limits<decimal_t>::has_quiet_NaN)
 return std::numeric_limits<decimal_t>::quiet_NaN();}
 return tan(p*v.GetTrigFactor());}

void DefaultSymbols::Tangent(Eval&v,Return&r,Value*p,int c)
{if(c!=1){r.status=CALC_ERR_FUNCTION;r.nParam=1;return;}
 r.value=Value(QTangent(v,p[0].GetDecimal()));r.status=CALC_SUCCESS;}

void DefaultSymbols::Arctangent(Eval&v,Return&r,Value*p,int c)
{if(c!=1){r.status=CALC_ERR_FUNCTION;r.nParam=1;return;}
 if(p[0].GetType()!=VALUE_DECIMAL){r.status=CALC_ERR_DATA_TYPE;r.nParam=p[0].GetType();return;}
 r.value=Value(atan(p[0].GetDecimal())/v.GetTrigFactor());r.status=CALC_SUCCESS;}

decimal_t QCotangent(Eval&v,decimal_t p)
{if(v.GetTrigFactor()!=1.0){
 while(p>=360.0)p-=360.0;while(p<0.0)p+=360.0;
 if(p==90.0||p==270.0)return 0.0;else if(p==0.0||p==180.0 &&
 std::numeric_limits<decimal_t>::has_quiet_NaN)
 return std::numeric_limits<decimal_t>::quiet_NaN();}
 return 1/tan(p*v.GetTrigFactor());}

void DefaultSymbols::Cotangent(Eval&v,Return&r,Value*p,int c)
{if(c!=1){r.status=CALC_ERR_FUNCTION;r.nParam=1;return;}
 if(p[0].GetType()!=VALUE_DECIMAL){r.status=CALC_ERR_DATA_TYPE;r.nParam=p[0].GetType();return;}
 r.value=Value(QCotangent(v,p[0].GetDecimal()));r.status=CALC_SUCCESS;}

// TODO: complex natural log, base ten log
void DefaultSymbols::LogNatural(Eval&v,Return&r,Value*p,int c)
{if(c!=1){r.status=CALC_ERR_FUNCTION;r.nParam=1;return;}
 if(p[0].GetType()!=VALUE_DECIMAL){r.status=CALC_ERR_DATA_TYPE;r.nParam=p[0].GetType();return;}
 r.value=Value(log(p[0].GetDecimal()));r.status=CALC_SUCCESS;}

void DefaultSymbols::LogTen(Eval&v,Return&r,Value*p,int c)
{if(c!=1){r.status=CALC_ERR_FUNCTION;r.nParam=1;return;}
 if(p[0].GetType()!=VALUE_DECIMAL){r.status=CALC_ERR_DATA_TYPE;r.nParam=p[0].GetType();return;}
 r.value=Value(log10(p[0].GetDecimal()));r.status=CALC_SUCCESS;}

void DefaultSymbols::AbsoluteValue(Eval&v,Return&r,Value*p,int c)
{if(c!=1){r.status=CALC_ERR_FUNCTION;r.nParam=1;return;}
 if(p[0].GetType()==VALUE_DECIMAL){r.value=Value(fabs(p[0].GetDecimal()));r.status=CALC_SUCCESS;}
 else if(p[0].GetType()==VALUE_COMPLEX){r.value=Value(abs(p[0].GetComplex()));r.status=CALC_SUCCESS;}
 else{r.status=CALC_ERR_DATA_TYPE;r.nParam=p[0].GetType();return;}}

// TODO: complex average
void DefaultSymbols::Average(Eval&v,Return&r,Value*p,int c)
{if(c<2){r.status=CALC_ERR_FUNCTION;r.nParam=-2;return;}
 decimal_t result=0;
 for(int i=0;i<c;i++){
 if(p[i].GetType()!=VALUE_DECIMAL){r.status=CALC_ERR_DATA_TYPE;r.nParam=p[i].GetType();return;}
 result+=p[i].GetDecimal();}
 r.value=Value(result/c);r.status=CALC_SUCCESS;}

void DefaultSymbols::Minimum(Eval&v,Return&r,Value*p,int c)
{if(c<2){r.status=CALC_ERR_FUNCTION;r.nParam=-2;return;}
 if(p[0].GetType()==VALUE_DECIMAL){decimal_t result=p[0].GetDecimal();
 for(int i=1;i<c;i++){
 if(p[i].GetType()!=VALUE_DECIMAL){r.status=CALC_ERR_DATA_TYPE;r.nParam=p[i].GetType();return;}
 if(p[i].GetDecimal()<result)result=p[i].GetDecimal();
 r.value=Value(result);}}
 else if(p[0].GetType()==VALUE_STRING){std::string result=p[0].GetString();
 for(int i=1;i<c;i++){
 if(p[i].GetType()!=VALUE_STRING){r.status=CALC_ERR_DATA_TYPE;r.nParam=p[i].GetType();return;}
 if(p[i].GetString()<result)result=p[i].GetString();
 r.value=Value(result);}}
 /*else if(p[0].GetType()==VALUE_COMPLEX){complex_t result=p[0].GetComplex();
 for(int i=1;i<c;i++){
 if(p[i].GetType()!=VALUE_COMPLEX){r.status=CALC_ERR_DATA_TYPE;r.nParam=p[i].GetType();return;}
 if(p[i].GetComplex()<result)result=p[i].GetComplex();
 r.value=Value(result);}}*/
 else{r.status=CALC_ERR_DATA_TYPE;r.nParam=p[0].GetType();return;}
 r.status=CALC_SUCCESS;}

void DefaultSymbols::Maximum(Eval&v,Return&r,Value*p,int c)
{if(c<2){r.status=CALC_ERR_FUNCTION;r.nParam=-2;return;}
 if(p[0].GetType()==VALUE_DECIMAL){decimal_t result=p[0].GetDecimal();
 for(int i=1;i<c;i++){
 if(p[i].GetType()!=VALUE_DECIMAL){r.status=CALC_ERR_DATA_TYPE;r.nParam=p[i].GetType();return;}
 if(p[i].GetDecimal()>result)result=p[i].GetDecimal();
 r.value=Value(result);}}
 else if(p[0].GetType()==VALUE_STRING){std::string result=p[0].GetString();
 for(int i=1;i<c;i++){
 if(p[i].GetType()!=VALUE_STRING){r.status=CALC_ERR_DATA_TYPE;r.nParam=p[i].GetType();return;}
 if(p[i].GetString()>result)result=p[i].GetString();
 r.value=Value(result);}}
 /*else if(p[0].GetType()==VALUE_COMPLEX){complex_t result=p[0].GetComplex();
 for(int i=1;i<c;i++){
 if(p[i].GetType()!=VALUE_COMPLEX){r.status=CALC_ERR_DATA_TYPE;r.nParam=p[i].GetType();return;}
 if(p[i].GetComplex()>result)result=p[i].GetComplex();
 r.value=Value(result);}}*/
 else{r.status=CALC_ERR_DATA_TYPE;r.nParam=p[0].GetType();return;}
 r.status=CALC_SUCCESS;}

void DefaultSymbols::Uppercase(Eval&v,Return&r,Value*p,int c)
{if(c!=1){r.status=CALC_ERR_FUNCTION;r.nParam=1;return;}
 if(p[0].GetType()!=VALUE_STRING){r.status=CALC_ERR_DATA_TYPE;r.nParam=p[0].GetType();return;}
 r.value=Value(p[0].GetString());std::transform(r.value.GetString().begin(),
 r.value.GetString().end(),r.value.GetString().begin(),std::ptr_fun<int,int>(std::toupper));
 r.status=CALC_SUCCESS;}

void DefaultSymbols::Lowercase(Eval&v,Return&r,Value*p,int c)
{if(c!=1){r.status=CALC_ERR_FUNCTION;r.nParam=1;return;}
 if(p[0].GetType()!=VALUE_STRING){r.status=CALC_ERR_DATA_TYPE;r.nParam=p[0].GetType();return;}
 r.value=Value(p[0].GetString());std::transform(r.value.GetString().begin(),
 r.value.GetString().end(),r.value.GetString().begin(),std::ptr_fun<int,int>(std::tolower));
 r.status=CALC_SUCCESS;}

void DefaultSymbols::Substring(Eval&v,Return&r,Value*p,int c)
{if(c!=3){r.status=CALC_ERR_FUNCTION;r.nParam=3;return;}
 if(p[0].GetType()!=VALUE_STRING){r.status=CALC_ERR_DATA_TYPE;r.nParam=p[0].GetType();return;}
 if(p[1].GetType()!=VALUE_DECIMAL){r.status=CALC_ERR_DATA_TYPE;r.nParam=p[1].GetType();return;}
 if(p[2].GetType()!=VALUE_DECIMAL){r.status=CALC_ERR_DATA_TYPE;r.nParam=p[2].GetType();return;}
 r.value=Value(p[0].GetString().substr(std::max(0,(int)p[1].GetDecimal()),
 std::min((int)p[0].GetString().length(),(int)p[2].GetDecimal())));
 r.status=CALC_SUCCESS;}