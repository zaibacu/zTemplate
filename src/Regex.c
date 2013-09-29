#include "Regex.h"
#include "Util.h"

//Eval functions
bool alphanumeric_state(struct RegexState* p_pCaller, char p_chArg)
{
	return isalnum(p_chArg) || strchr("_-.>", p_chArg) != NULL;
}

bool digit_state(struct RegexState* p_pCaller, char p_chArg)
{
	return isdigit(p_chArg);
}

bool whitespace_state(struct RegexState* p_pCaller, char p_chArg)
{
	return isspace(p_chArg);
}

bool dot_state(struct RegexState* p_pCaller, char p_chArg)
{
	return isspace(p_chArg) || isdigit(p_chArg) || isalnum(p_chArg) || strchr(".,?!|@#$%^&*()-+", p_chArg) != NULL;
}

bool constant_state(struct RegexState* p_pCaller, char p_chArg)
{
	DEBUG(3, "Matching %c to constant %c\n", p_chArg, p_pCaller->m_chExpr);
	return p_chArg == p_pCaller->m_chExpr;
}

bool class_state(struct RegexState* p_pCaller, char p_chArg)
{
	const unsigned long culSize = strlen(p_pCaller->m_szExpr);
	unsigned long ulIndex = 0;
	for(ulIndex = 0; ulIndex < culSize; ulIndex++)
	{
		if(p_pCaller->m_szExpr[ulIndex] == p_chArg)
			return true;
	}

	return false;
}

bool regex_match(struct RegexState* p_pState, char p_chArg)
{
	return p_pState->f(p_pState, p_chArg);
}

struct RegexState* compile_regex(const zString p_cszPattern)
{
	if(p_cszPattern == NULL)
		return NULL;
	
	struct RegexState* pRoot = (struct RegexState*)malloc(sizeof(struct RegexState));
	pRoot->m_eRegexType = eRoot;
	struct RegexState* pPrev = pRoot;
	const unsigned long culLength = strlen(p_cszPattern);
	unsigned long ulIndex = 0;
	bool bClass = false;
	unsigned long ulClassStart = 0;
	for(ulIndex = 0; ulIndex < culLength; ulIndex++)
	{
		bool bRepeat = p_cszPattern[ulIndex + 1] == '+' ? true : false || p_cszPattern[ulIndex + 1] == '*' ? true : false;
		bool bCanSkip = p_cszPattern[ulIndex + 1] == '*' ? true : false || p_cszPattern[ulIndex + 1] == '?' ? true : false;

		if(bClass && p_cszPattern[ulIndex] != ']')
		{
			//These characters goes inside a class
			continue;
		}

		//Search for alpha numeric character
		if(p_cszPattern[ulIndex] == 'w' && p_cszPattern[ulIndex - 1] == '\\')
		{
			struct RegexState* pRegex = (struct RegexState*)malloc(sizeof(struct RegexState));
			pRegex->m_eRegexType = eAlpha;
			pRegex->m_bRepeat = bRepeat;
			pRegex->m_bCanSkip = bCanSkip;
			pRegex->f = &alphanumeric_state;
			pRegex->m_pNext = NULL;
			pPrev->m_pNext = pRegex;
			pPrev = pRegex;
		}
		//Search for digit
		else if(p_cszPattern[ulIndex] == 'd' && p_cszPattern[ulIndex - 1] == '\\')
		{
			struct RegexState* pRegex = (struct RegexState*)malloc(sizeof(struct RegexState));
			pRegex->m_eRegexType = eNumeric;
			pRegex->m_bRepeat = bRepeat;
			pRegex->m_bCanSkip = bCanSkip;
			pRegex->f = &digit_state;
			pRegex->m_pNext = NULL;
			pPrev->m_pNext = pRegex;
			pPrev = pRegex;
		}
		//Search for whitespace
		else if(p_cszPattern[ulIndex] == 's' && p_cszPattern[ulIndex - 1] == '\\')
		{
			struct RegexState* pRegex = (struct RegexState*)malloc(sizeof(struct RegexState));
			pRegex->m_eRegexType = eWhiteSpace;
			pRegex->m_bRepeat = bRepeat;
			pRegex->m_bCanSkip = bCanSkip;
			pRegex->f = &whitespace_state;
			pRegex->m_pNext = NULL;
			pPrev->m_pNext = pRegex;
			pPrev = pRegex;
		}
		//Any character expect end line
		else if(p_cszPattern[ulIndex] == '.' && p_cszPattern[ulIndex - 1] == '\\')
		{
			struct RegexState* pRegex = (struct RegexState*)malloc(sizeof(struct RegexState));
			pRegex->m_eRegexType = eDot;
			pRegex->m_bRepeat = false;
			pRegex->m_bCanSkip = false;
			pRegex->f = &dot_state;
			pRegex->m_pNext = NULL;
			pPrev->m_pNext = pRegex;
			pPrev = pRegex;
		}
		else if(p_cszPattern[ulIndex] == '+' || p_cszPattern[ulIndex] == '*' || p_cszPattern[ulIndex] == '?' || p_cszPattern[ulIndex] == '\\')
		{
			//Just skip these command characters
		}
		else if(p_cszPattern[ulIndex] == '(' || p_cszPattern[ulIndex] == ')')
		{
			//Grouping symbols
		}
		else if(bClass == false && p_cszPattern[ulIndex] == '[')
		{
			bClass = true;
			ulClassStart = ulIndex + 1;
		}
		else if(bClass == true && p_cszPattern[ulIndex] == ']')
		{
			bClass = false;
			//We need to form a class
			struct RegexState* pRegex = (struct RegexState*)malloc(sizeof(struct RegexState));
			pRegex->m_eRegexType = eClass;
			pRegex->m_bRepeat = bRepeat;
			pRegex->m_bCanSkip = bCanSkip;
			pRegex->f = &class_state;

			const unsigned long culLength = ulIndex - 1 - ulClassStart;
			pRegex->m_szExpr = (zString)malloc(sizeof(zString) * (culLength + 1));
			strncpy(pRegex->m_szExpr, p_cszPattern + ulClassStart, culLength);
			pRegex->m_szExpr[culLength] = '\0';

			pRegex->m_pNext = NULL;
			pPrev->m_pNext = pRegex;
			pPrev = pRegex;
		}
		else //It is constant
		{
			struct RegexState* pRegex = (struct RegexState*)malloc(sizeof(struct RegexState));
			pRegex->m_eRegexType = eConstant;
			pRegex->m_bRepeat = false;
			pRegex->m_bCanSkip = false;
			pRegex->f = &constant_state;
			pRegex->m_chExpr = p_cszPattern[ulIndex];
			pRegex->m_pNext = NULL;
			pPrev->m_pNext = pRegex;
			pPrev = pRegex;
		}
	}
	return (struct RegexState*)pRoot->m_pNext; //Ignore first element in list, because it is just a place holder
}

zString regex_search(struct RegexState* p_pRegex, const zString p_cszText, unsigned long* p_pulLastIndex, const bool p_cbStrict)
{
	if(p_pRegex == NULL)
		return NULL;

	struct RegexState* pRegex = (struct RegexState*)malloc(sizeof(struct RegexState));
	*pRegex = *p_pRegex;
	const unsigned long culLength = strlen(p_cszText);
	unsigned long ulIndex = 0, ulStart, ulEnd, ulMatched = 0;
	bool bStarted = false;
	for(ulIndex = *p_pulLastIndex; ulIndex < culLength; ulIndex++)
	{
		bool bMatch = regex_match(pRegex, p_cszText[ulIndex]);
		DEBUG(2, "State: %d Char:'%c' Matched? %d\n", pRegex->m_eRegexType, p_cszText[ulIndex], bMatch);
		ulMatched += bMatch;
		if(!bMatch && !bStarted && !p_cbStrict && !pRegex->m_bCanSkip) //Just ignore at this point
			continue;

		if(bMatch && !bStarted) //Pattern started
		{
			bStarted = true;
			ulStart = ulIndex;
		}

		if(!bMatch)
		{
			if(pRegex && ((!bMatch && ulMatched > 0) || (!bMatch && ulMatched == 0 && pRegex->m_bCanSkip)))
			{
				pRegex = (struct RegexState*)pRegex->m_pNext;
				if(pRegex)
					ulIndex--;
			}
			else
			{
				break;
			}
			ulMatched = 0;
		}
		else if(bMatch && !pRegex->m_bRepeat)
		{
			pRegex = (struct RegexState*)pRegex->m_pNext;
			ulMatched = 0;
		}
		if(pRegex == NULL || ulIndex == (culLength - 1)) //We end when there is no more regex left
		{
			ulEnd = ulIndex;
			*p_pulLastIndex = ulEnd;
			const unsigned long ulLength = ulEnd - ulStart + bMatch /* bMatch tells us if last character was successfully matched*/;
			zString szResult = (zString)malloc(sizeof(zString) * (ulLength + 1));
			memcpy(szResult, p_cszText + ulStart, ulLength);
			szResult[ulLength] = '\0';
			return szResult;
		}
	}
	return NULL;
}

zString regex(const zString p_cszPattern, const zString p_cszText)
{
	struct RegexState* pRegex = compile_regex(p_cszPattern);
	unsigned long ulIndex = 0;
	return regex_search(pRegex, p_cszText, &ulIndex, false);
}

bool regex_test(struct RegexState* p_pRegex, const zString p_cszText)
{
	unsigned long ulIndex = 0;
	return regex_search(p_pRegex, p_cszText, &ulIndex, true) != NULL;
}

bool regex_iter(struct RegexIter* p_pIter)
{
	unsigned long ulIndex = 0;
	p_pIter->m_szResult = regex_search(p_pIter->m_pRegex, p_pIter->m_cszText, &ulIndex, false);
	if(p_pIter->m_szResult == NULL)
		return false;

	p_pIter->m_cszText = p_pIter->m_cszText + ulIndex;
	return true;
}