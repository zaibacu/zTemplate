#pragma once
enum ERegexType
{
	eRoot = 0,
	eConstant = 1,
	eAlpha = 2,
	eNumeric = 3,
	eWhiteSpace = 4,
	eDot = 5,
	eClass = 6
};

struct RegexState
{
	enum ERegexType m_eRegexType;
	void* m_pState;
	void* m_pNext;
	bool m_bRepeat;
	bool m_bCanSkip;
	bool (*f)(struct RegexState*, char);
	char m_chExpr;
	zString m_szExpr;
};

struct RegexIter
{
	struct RegexState* m_pRegex;
	const zString m_cszText;
	zString m_szResult;
};

struct RegexState* compile_regex(const zString p_cszPattern);
zString regex(const zString p_cszPattern, const zString p_cszText);
zString regex_search(struct RegexState* p_pRegex, const zString p_cszText, unsigned long* p_pulLastIndex, const bool p_cbStrict);
bool regex_test(struct RegexState* p_pRegex, const zString p_cszText);
bool regex_iter(struct RegexIter* p_pIter);