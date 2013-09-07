#pragma once

zString trim(const zString p_cszText);
zString read_file(const zString p_cszName);
long seek(const zString p_cszSource, const zString p_cszPattern, const unsigned long p_culStart);
long seek_back(const zString p_cszSource, const zString p_cszPattern, const unsigned long p_culStart);
void str_insert(zString p_szSource, const zString p_cszInclude, const unsigned long p_culStart, const unsigned long p_culEnd);
bool str_remove(zString p_szSource, const unsigned long p_culStart, const unsigned long p_culEnd);
bool str_remove_tag(zString p_szSource, const zString p_cszRemove);

long get_or_default(const long p_clValue, const long p_clDefault);


//Regex stuff
enum ERegexType
{
	eRoot = 0,
	eConstant = 1,
	eAlpha = 2,
	eNumeric = 3,
	eWhiteSpace = 4,
	eDot = 5,
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
};

zString regex(const zString p_cszPattern, const zString p_cszText);