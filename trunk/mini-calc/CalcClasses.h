#pragma once

#pragma warning(disable:4786)

#include <algorithm>
#include <functional>
#include <cctype>
#include <limits>
#include <complex>

#include <map>
#include <string>
#include <vector>
#include <stack>
#include <ctype.h>
#include <math.h>

namespace Calc
{

// classes in this header
struct Token;

class Value;
struct Return;
class Symbol;
class SymbolList;
class Expression;
class ExpressionList;
class Eval;

// definitions
#ifndef decimal_t
typedef long double decimal_t;
#endif

#ifndef complex_t
typedef std::complex<decimal_t> complex_t;
#endif

#define STATE_START				0
#define TOKEN_UNKNOWN			0

// also used as DFA states
#define TOKEN_NUMBER			1
#define STATE_NUMBER_EXPONENT	2
//#define STATE_NUMBER_BINARY	18
//#define STATE_NUMBER_HEX		19
#define TOKEN_COMMA				3
#define TOKEN_OPEN_PAREN		4
#define TOKEN_CLOSE_PAREN		5
#define TOKEN_OPEN_BRACKET		6
#define TOKEN_CLOSE_BRACKET		7
#define TOKEN_OPEN_CURLY		8
#define TOKEN_CLOSE_CURLY		9
#define TOKEN_LESS_SIGN			10
#define TOKEN_GREATER_SIGN		11
#define TOKEN_STRING			12
#define TOKEN_LITERAL_STRING	13
#define STATE_LITERAL_END		14
#define STATE_LITERAL_ESCAPE	15
#define TOKEN_OPERATOR			16
#define TOKEN_SIGN				17

struct Token
{
	int type;
	std::string data;

	Token(int _type = TOKEN_UNKNOWN, std::string _data = "")
	{
		type = _type;
		data = _data;
	}
};

// Value types
#define VALUE_DECIMAL		1
#define VALUE_COMPLEX		2
#define VALUE_STRING		3
#define VALUE_LIST			4

class Value
{
private:
	int m_nType;
	decimal_t m_decValue;
	std::string* m_pStrValue;
	complex_t* m_pCplxValue;
	ExpressionList* m_pListValue;

public:
	Value(decimal_t decValue = 0.0);
	Value(const std::string& strValue);
	Value(const complex_t& cplxValue);
	Value(const ExpressionList& listValue);
	Value(const Value& src);
	virtual ~Value();
	Value& operator=(const Value& src);

	int GetType() const { return m_nType; }
	decimal_t GetDecimal() const { return m_decValue; }
	const std::string& GetString() const { return *m_pStrValue; }
	std::string& GetString(){ return *m_pStrValue; }
	const complex_t& GetComplex() const { return *m_pCplxValue; }
	complex_t& GetComplex(){ return *m_pCplxValue; }
	const ExpressionList& GetList() const { return *m_pListValue; }
	ExpressionList& GetList(){ return *m_pListValue; }

	std::string ToString(bool bQuote = true) const;

	void SetType(int nType);
	void SetDecimal(decimal_t decValue);
	void SetString(const std::string& strValue);
	void SetComplex(const complex_t& cplxValue);
	void SetList(const ExpressionList& listValue);

	void TryComplexToReal();

	void Negate();

	static std::string GetValueTypeString(int nValueType);
};

#define CALC_DELETE_SUCCESS	2
#define CALC_SUCCESS		1
#define CALC_NOSTATUS		0	// no status
#define CALC_ERR_SYNTAX		-1	// syntax error
#define CALC_ERR_PAREN		-2	// parentheses count wasn't back to 0
#define CALC_ERR_INCOMPLETE	-3	// FSA not in a final state or no input given
#define CALC_ERR_SYMBOL		-4	// undefined variable/function
#define CALC_ERR_DATA_TYPE	-5	// invalid data type
#define CALC_ERR_INPUT		-6	// invalid function input
#define CALC_ERR_FUNCTION	-7	// not enough/too many parameters for a function
#define CALC_ERR_DOMAIN		-8	// domain error

#define CALCSUCCESS(_val)		((_val).status > 0)

struct Return
{
	int status;				// status
	Value value;			// the solved value if CALC_SUCCESS
	int nErrPos;			// the character index (0-based) that caused CALC_ERR_SYNTAX
	std::string badSymbol;	// the var/function when CALC_ERR_SYMBOL and CALC_ERR_FUNCTION
	int nParam;				// the amount of parameters needed when CALC_ERR_FUNCTION
							// also, the first invalid data type used when CALC_ERR_DATA_TYPE
	int nParam2;			// second invalid data type used when CALC_ERR_DATA_TYPE

	Return(int _status = CALC_NOSTATUS, const Value* _pValue = NULL,
		int _nErrPos = -1, std::string _badSymbol = "", int _nParam = 0, int _nParam2 = 0)
	{
		status = _status;
		nErrPos = _nErrPos;
		badSymbol = _badSymbol;
		nParam = _nParam;
		nParam2 = _nParam2;

		if (_pValue)
			value = *_pValue;
	}

	Return& operator=(const Return& src)
	{
		// optimized
		status = src.status;
		value = src.value;
		nErrPos = src.nErrPos;
		if (status == CALC_ERR_SYMBOL || status == CALC_ERR_FUNCTION)
			badSymbol = src.badSymbol;
		nParam = src.nParam;
		nParam2 = src.nParam2;

		return *this;
	}
};

#define SYMBOL_VARIABLE	1
#define SYMBOL_FUNC		2
#define SYMBOL_INT_FUNC	3

class Symbol;
class Expression;
class Eval;

typedef void (*INTFUNC)(Eval&, Return&, Value*, int);

typedef std::map<std::string,Value> FUNCPARAMMAP;

class Symbol
{
private:
	int m_nType;
	std::string m_id;
	Value m_value;
	INTFUNC m_fnIntCallback;
	std::string m_strFuncDefinition;
	Expression* m_pFuncExpression;
	std::vector<std::string> m_vFuncParams;

public:
	// constructors/destructors
	Symbol(int nType = SYMBOL_VARIABLE, std::string id = "",
		const Value* pValue = NULL, INTFUNC fnIntCallback = NULL,
		std::string strFuncDefinition = "");
	Symbol(const Symbol& src);
	virtual ~Symbol();

	// operators
	Symbol& operator=(const Symbol& src);

	// functions
	int GetType() const { return m_nType; }
	const std::string& GetID() const { return m_id; }
	const Value& GetValue() const { return m_value; }
	Value& GetValue(){ return m_value; }
	INTFUNC GetIntFuncCallback() const { return m_fnIntCallback; }
	const std::string& GetFuncDefinition() const { return m_strFuncDefinition; }
	Expression* GetFuncExpression() const { return m_pFuncExpression; }
	const std::vector<std::string>* GetFuncParamList() const { return &m_vFuncParams; }
	std::vector<std::string>* GetFuncParamList(){ return &m_vFuncParams; }

	// operations/methods
	void SetType(int nType){ m_nType = nType; }
	void SetID(std::string id){ m_id = id; }
	void SetValue(const Value& value){ m_value = value; }
	void SetIntFuncCallback(INTFUNC func){ m_fnIntCallback = func; }
	bool SetFuncDefinition(std::string strFuncDefinition);
	bool SetFuncExpression(Expression* pExpr, bool bCopy = true);
};

typedef std::map<std::string,Symbol*> SYMBOLMAP;

class SymbolList
{
private:
	SYMBOLMAP m_map;

public:
	// constructors/destructors
	SymbolList();
	SymbolList(const SymbolList& src);
	virtual ~SymbolList();

	// operators
	SymbolList& operator=(const SymbolList& src);

	// functions
	SYMBOLMAP::const_iterator GetBeginIterator() const { return m_map.begin(); }
	SYMBOLMAP::iterator GetBeginIterator(){ return m_map.begin(); }
	SYMBOLMAP::const_iterator GetEndIterator() const { return m_map.end(); }
	SYMBOLMAP::iterator GetEndIterator(){ return m_map.end(); }
	size_t GetCount() const { return m_map.size(); }
	Symbol* Find(const std::string& strID);

	// methods
	void Set(Symbol* pSymbol, bool bCopy = true);
	void Remove(const std::string& strID, bool bDelete = true, Symbol** pDestPtr = NULL);
	void Remove(Symbol* pSymbol, bool bDelete = true);
	void Clear(); // deletes symbols
	void AddDefaultSymbols();
};

#define EXPR_GROUP		1
#define EXPR_VALUE		2
#define EXPR_VARIABLE	3
#define EXPR_FUNCTION	4

#define NO_OP		';'

class Expression
{
private:
	Eval* m_pEval;
	int m_nType;
	char m_cOperator;
	ExpressionList* m_pChildList; // group and function
	Value m_value; // value
	std::string m_string; // variable and function
	bool m_bNegative; // whether or not the expression is negative

public:
	// constructors/destructors
	Expression(Eval* pEval, int nType = EXPR_VALUE, char cOperator = NO_OP,
		const Value* pValue = NULL, std::string strValue = "", bool bNegative = false);
	Expression(const Expression& src);
	virtual ~Expression();

	// operators
	Expression& operator=(const Expression& src);

	// functions
	Eval* GetEval() const { return m_pEval; }
	int GetType() const { return m_nType; }
	char GetOperator() const { return m_cOperator; }
	const Value& GetValue() const { return m_value; }
	Value& GetValue(){ return m_value; }
	const std::string& GetString() const { return m_string; }
	ExpressionList* GetChildList() const { return m_pChildList; }
	bool IsNegative() const { return m_bNegative; }
	const Expression* FindFirstSymbolReference(std::string id) const;
	Expression* FindFirstSymbolReference(std::string id);

	// operations/methods
	void SetEval(Eval* pEval);
	void SetType(int nType){ m_nType = nType; }
	void SetOperator(char cOperator){ m_cOperator = cOperator; }
	void SetValue(const Value& value, bool bTypeValue = true);
	void SetString(std::string strValue){ m_string = strValue; }
	void SetNegative(bool bNegative){ m_bNegative = bNegative; }

	// must return std::string and not const std::string& because it returns local variable
	std::string GetExpressionString(bool bSpaces = true, bool bOuterParentheses = false, bool bQuoteStrings = true) const;
	void Evaluate(Return& retval, SymbolList* pSymbols, FUNCPARAMMAP* pFuncParams,
		bool bCanNegate = true);

	static int GetOperatorPriority(char cOperator);
	static void PerformOperation(Return& ret, const Value& left, char cOperator, const Value& right,
		Eval* pEval = NULL, SymbolList* pSymbols = NULL, FUNCPARAMMAP* pFuncParams = NULL, bool bCanNegate = true);

protected:
	static void _PerformBasicOperation(Return& ret, const Value& left, char cOperator, const Value& right);
};

class ExpressionList
{
private:
	std::vector<Expression*> m_vec;

public:
	// constructors/destructors
	ExpressionList();
	ExpressionList(const ExpressionList& src);
	virtual ~ExpressionList();

	// operators
	ExpressionList& operator=(const ExpressionList& src);

	// functions
	size_t GetCount() const { return m_vec.size(); }
	Expression* Get(size_t nIndex) const { return m_vec[nIndex]; }
	Expression* GetBack() const { return m_vec.back(); }

	// methods
	void Insert(size_t nPos, Expression* pExpr, bool bCopy = true);
	void Add(Expression* pExpr, bool bCopy = true);
	void Remove(size_t nIndex, bool bDelete = true, Expression** pDestPtr = NULL);
	void Clear(); // deletes expressions
};

class Eval
{
private:
	Expression* m_pEqualExpression;
	Expression* m_pRootExpression;
	SymbolList* m_pSymbolList;
	Return m_ret;
	decimal_t m_decTrigFactor; // should be 1.0 or Pi / 180.0

public:
	// constructors/destructors
	Eval(std::string strExpression = "", decimal_t decTrigFactor = 1.0, bool bAddDefaultSymbols = true);
	Eval(const Eval& src);
	virtual ~Eval();

	// operators
	Eval& operator=(const Eval& src);

	// functions
	Expression* GetEqualExpression() const { return m_pEqualExpression; }
	Expression* GetRootExpression() const { return m_pRootExpression; }
	SymbolList* GetSymbolList() const { return m_pSymbolList; }
	const Return& GetLastReturn() const { return m_ret; }
	decimal_t GetTrigFactor() const { return m_decTrigFactor; }

	// operations/methods
	void SetTrigFactor(decimal_t decTrigFactor){ m_decTrigFactor = decTrigFactor; }
	void AddDefaultSymbols(){ m_pSymbolList->AddDefaultSymbols(); }
	
	void QuickEval(Return& retval, std::string strExpression);

	void GenerateTree(std::string strExpression);
	void Evaluate();

private:
	void _GenTree(Return& retval, const char* str, Expression** pDestRootExpr,
		Expression** pDestEqualExpr = NULL);
	void _EvaluateTree(Return& retval, Expression* pRootExpr);
};

/*struct list_t
{
protected:
	std::vector<Expression*> m_list;

public:
	list_t(){ }
	list_t(const list_t& src){ *this = src; }

	list_t& operator=(const list_t& src)
	{
		clear();
		for (size_t i = 0; i < src.size(); i++)
			add(src[i],true);
		return *this;
	}

	const Expression* operator[](size_t n) const { return m_list[n]; }
	Expression* operator[](size_t n){ return m_list[n]; }

	size_t size() const { return m_list.size(); }
	void remove(size_t n){ delete m_list[n]; m_list.erase(m_list.begin() + n); }
	void clear()
	{
		for (size_t i = 0; i < m_list.size(); i++)
			delete m_list[i];
		m_list.clear();
	}

	void add(Expression* item, bool bCopy = false){ m_list.push_back(bCopy ? item : new Expression(*item)); }
	void add(const Expression* item, bool bCopy = false){ m_list.push_back(const_cast<Expression*>(bCopy ? item : new Expression(*item))); }
	void insert(size_t n, Expression* item, bool bCopy = false){ m_list.insert(m_list.begin() + n,bCopy ? item : new Expression(*item)); }
	void insert(size_t n, const Expression* item, bool bCopy = false){ m_list.insert(m_list.begin() + n,const_cast<Expression*>(bCopy ? item : new Expression(*item))); }
};*/

namespace DefaultSymbols
{
	#define _NZMDEFSYMB_FUNC(_f)	void _f(Eval&, Return&, Value*, int);

	// TODO: hyperbolic functions, summation, gcf, lcm, etc.
	_NZMDEFSYMB_FUNC(Floor)
	_NZMDEFSYMB_FUNC(Ceil)
	_NZMDEFSYMB_FUNC(Factorial)
	_NZMDEFSYMB_FUNC(Permutation)
	_NZMDEFSYMB_FUNC(Combination)
	_NZMDEFSYMB_FUNC(Compare)
	_NZMDEFSYMB_FUNC(SquareRoot)
	_NZMDEFSYMB_FUNC(Root)
	_NZMDEFSYMB_FUNC(Sine)
	_NZMDEFSYMB_FUNC(Arcsine)
	_NZMDEFSYMB_FUNC(Cosecant)
	_NZMDEFSYMB_FUNC(Cosine)
	_NZMDEFSYMB_FUNC(Arccosine)
	_NZMDEFSYMB_FUNC(Secant)
	_NZMDEFSYMB_FUNC(Tangent)
	_NZMDEFSYMB_FUNC(Arctangent)
	_NZMDEFSYMB_FUNC(Cotangent)
	_NZMDEFSYMB_FUNC(LogNatural)
	_NZMDEFSYMB_FUNC(LogTen)
	_NZMDEFSYMB_FUNC(AbsoluteValue)
	_NZMDEFSYMB_FUNC(Average)
	_NZMDEFSYMB_FUNC(Minimum)
	_NZMDEFSYMB_FUNC(Maximum)

	_NZMDEFSYMB_FUNC(Uppercase)
	_NZMDEFSYMB_FUNC(Lowercase)
	_NZMDEFSYMB_FUNC(Substring)

	const decimal_t
		Pi = 3.14159265358979323846,
		E = 2.71828182845;

	const complex_t I = complex_t(0,1);
};

struct FindReplace
{
	std::string fd, rep;
};

std::string& exprstr(std::string& s); // replaces \ with \\ and " with \"
std::string& rtrim(std::string& s);
std::string& ltrim(std::string& s);
bool iswhole(decimal_t n);
void cplxstr(const complex_t& cplx, std::string& str);
bool equalThresh(decimal_t a, decimal_t b);

};