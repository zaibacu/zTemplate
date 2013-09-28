#include "Util.h"
#include <ctype.h>

zString trim(const zString p_cszText)
{
	const zString cpStart = p_cszText;
	const zString cpEnd = p_cszText + strlen(p_cszText) - 1;

	if(isspace(*cpStart))
		while(isspace(*++cpStart));
	if(isspace(*cpEnd))
		while(isspace(*--cpEnd));

	unsigned long ulStrLen = strlen(cpStart) - strlen(cpEnd) + 1;
	zString chBuff = (zString)malloc(sizeof(zString) * (ulStrLen + 1));
	memcpy(chBuff, cpStart, ulStrLen);
	chBuff[ulStrLen] = '\0';
	return chBuff;
}

zString read_file(const zString p_cszName)
{
	zString chBuff = NULL;
	unsigned long ulDirLen = strlen(TEMPLATE_DIR) + strlen(p_cszName);
	zString chDir = (zString)malloc(sizeof(zString) * (ulDirLen + 1));
	strcpy(chDir, TEMPLATE_DIR);
	strcat(chDir, p_cszName);
	chDir[ulDirLen] = '\0';
	FILE* pFile = fopen(chDir, "r");
	if(pFile)
	{
		//Get correct file size
		unsigned long ulSize = 0;
		fseek (pFile , 0 , SEEK_END);
		ulSize = ftell (pFile);
		rewind (pFile);
#ifdef _USE_FSEEK_IMPL
		chBuff = (zString)malloc(sizeof(zString) * (ulSize + 1));
		fread(chBuff, 1, ulSize, pFile);
#else
		#define BUFF_SIZE 80
		chBuff = (zString)calloc(ulSize + 1, sizeof(zString));
		zString chLine = (zString)malloc(sizeof(zString) * BUFF_SIZE);
		while(fgets(chLine, BUFF_SIZE, pFile) != NULL)
		{
			strcat(chBuff, chLine);
		} 
		free(chLine);
#endif
		chBuff[ulSize] = '\0';
		fclose(pFile);
	}
	free(chDir);
	return chBuff;
}

long seek(const zString p_cszSource, const zString p_cszPattern, const unsigned long p_culStart)
{
	if(p_cszSource == NULL || p_cszPattern == NULL)
		return -1;

	zString szResult = strstr(p_cszSource + p_culStart, p_cszPattern);
	if(szResult != NULL)
	{
		return strlen(p_cszSource) - strlen(szResult);
	}
	return -1;
}

long seek_back(const zString p_cszSource, const zString p_cszPattern, const unsigned long p_culStart)
{
	if(p_cszSource == NULL)
		return -1;

	long lLast_index = -1;
	long lOffset = 0;
	while(true)
	{
		lOffset = seek(p_cszSource, p_cszPattern, lOffset);
		if(lOffset > p_culStart || lOffset == -1)
			break;

		lLast_index = lOffset;
		lOffset += strlen(p_cszPattern);
	}
	
	return lLast_index;
}

void str_insert(zString p_szSource, const zString p_cszInclude, const unsigned long p_culStart, const unsigned long p_culEnd)
{
	if(p_szSource == NULL)
		return;

	unsigned long ulIncludeSize = 0;
	if(p_cszInclude != NULL)
		ulIncludeSize = strlen(p_cszInclude);

	const unsigned long culSize = strlen(p_szSource) - (p_culEnd - p_culStart) + ulIncludeSize;
	zString chBuff = NULL;
	chBuff = (zString)malloc(sizeof(zString) * (culSize + 1));
	chBuff[culSize] = '\0';
	memcpy(chBuff, p_szSource, p_culStart);
	if(p_cszInclude != NULL)
		memcpy(chBuff + p_culStart, p_cszInclude, ulIncludeSize);
	memcpy(chBuff + p_culStart + ulIncludeSize, p_szSource + p_culEnd, strlen(p_szSource) - p_culEnd);

	memcpy(p_szSource, chBuff, strlen(chBuff));
	p_szSource[strlen(chBuff)] = '\0';
	free(chBuff);
} 

bool str_remove(zString p_szSource, const unsigned long p_culStart, const unsigned long p_culEnd)
{
	str_insert(p_szSource, NULL, p_culStart, p_culEnd);
	return true;
}

bool str_remove_tag(zString p_szSource, const zString p_cszRemove)
{
	if(p_szSource == NULL || p_cszRemove == NULL)
		return false;

	long lStart = seek(p_szSource, p_cszRemove, 0);
	if(lStart < 0)
		return false;

	return str_remove(p_szSource, lStart, lStart + strlen(p_cszRemove));
}

long get_or_default(const long p_clValue, const long p_clDefault)
{
	if(p_clValue < 0)
		return p_clDefault;
	return p_clValue;
}

zString pull_param_name(const zString p_cszSource, long* p_pPos)
{
	long lStart = seek(p_cszSource, "$", *p_pPos);
	if(lStart != -1)
	{
		*p_pPos = lStart;
		long lEnd = seek(p_cszSource, " ", *p_pPos);
		if(lEnd != -1)
			*p_pPos = lEnd;
		else
			*p_pPos = strlen(p_cszSource);

		unsigned long ulSize = *p_pPos - lStart - 1;
		zString chBuff = (zString)malloc(sizeof(zString) * (ulSize + 1));
		memcpy(chBuff, p_cszSource + lStart + 1, ulSize);
		chBuff[ulSize] = '\0';
		return chBuff;
	}

	return NULL;
}

//Regex stuff

//Eval functions
bool alphanumeric_state(struct RegexState* p_pCaller, char p_chArg)
{
	return isalnum(p_chArg);
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
	return p_chArg != '.';
}

bool constant_state(struct RegexState* p_pCaller, char p_chArg)
{
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
		else if(p_cszPattern[ulIndex] == '.')
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
	return (struct RegexState*)pRoot->m_pNext; //Ignore first element in list, becouse it is just a place holder
}

zString regex_search(struct RegexState* p_pRegex, const zString p_cszText, unsigned long* p_pulLastIndex)
{
	if(p_pRegex == NULL)
		return NULL;

	struct RegexState* pRegex = (struct RegexState*)malloc(sizeof(struct RegexState));
	*pRegex = *p_pRegex;
	const unsigned long culLength = strlen(p_cszText);
	unsigned long ulIndex = 0, ulStart, ulEnd, ulMatched = 0;
	bool bStarted = false;
	for(ulIndex = 0; ulIndex < culLength; ulIndex++)
	{
		bool bMatch = regex_match(pRegex, p_cszText[ulIndex]);
		DEBUG(2, "State: %d Char:'%c' Matched? %d\n", pRegex->m_eRegexType, p_cszText[ulIndex], bMatch);
		ulMatched += bMatch;
		if(!bMatch && !bStarted) //Just ignore at this point
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
	return regex_search(pRegex, p_cszText, &ulIndex);
}

bool regex_test(struct RegexState* p_pRegex, const zString p_cszText)
{
	unsigned long ulIndex = 0;
	return regex_search(p_pRegex, p_cszText, &ulIndex) != NULL;
}

bool regex_iter(struct RegexIter* p_pIter)
{
	unsigned long ulIndex = 0;
	p_pIter->m_szResult = regex_search(p_pIter->m_pRegex, p_pIter->m_cszText, &ulIndex);
	if(p_pIter->m_szResult == NULL)
		return false;

	p_pIter->m_cszText = p_pIter->m_cszText + ulIndex;
	return true;
}