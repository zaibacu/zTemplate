#include "Block.h"
#include "Parameter.h"
#include "zCore.h"
#include "Util.h"
#include "Regex.h"

void clean_BM(struct BlockMemory* p_pBM)
{
	if(p_pBM == NULL)
		return; 

	p_pBM->m_lLine = 0;
	p_pBM->m_bEven = true;
	p_pBM->m_bOdd = false;
	p_pBM->m_bCondition = false;
	if(p_pBM->m_pLocal != NULL)
	{
		if(p_pBM->m_pLocal->m_pVal != NULL)
		{
			p_pBM->m_pLocal->m_pVal->m_pValue ? free(p_pBM->m_pLocal->m_pVal->m_pValue) : false;
			free(p_pBM->m_pLocal->m_pVal);
		}
		free(p_pBM->m_pLocal->m_szKey);
		free(p_pBM->m_pLocal);
	}
	p_pBM->m_pLocal = NULL;
}

void step_BM(struct BlockMemory* p_pBM)
{
	if(p_pBM == NULL)
		return; 

	p_pBM->m_lLine++;
	p_pBM->m_bEven = !p_pBM->m_bEven;
	p_pBM->m_bOdd = !p_pBM->m_bOdd;
	if(p_pBM->m_pLocal != NULL && p_pBM->m_pLocal->m_pVal != NULL && p_pBM->m_pLocal->m_pVal->m_uiType == 4)
	{
		if(p_pBM->m_pLocal->m_pVal->m_pValue != NULL)
		{
			p_pBM->m_pLocal->m_pVal->m_pValue = (void*)((struct ListValue*)p_pBM->m_pLocal->m_pVal->m_pValue)->m_pNext;
			p_pBM->m_bCondition = true;
		}
		else
		{
			p_pBM->m_bCondition = false;
		}
	}
	else
	{
		p_pBM->m_bCondition = false;
	}
}

struct Value* search_block_parameter(struct BlockMemory* p_pBM, const zString p_cszName)
{
	if(strcmp(p_cszName, "even") == 0)
	{
		p_pBM->m_pEvenValue = &(struct Value){ 2, (void*)&(struct BoolValue){p_pBM->m_bEven} };
		return p_pBM->m_pEvenValue;
	}
	else if(strcmp(p_cszName, "odd") == 0)
	{
		p_pBM->m_pOddValue = &(struct Value){ 2, (void*)&(struct BoolValue){p_pBM->m_bOdd} };
		return p_pBM->m_pOddValue;
	}
	else if(strcmp(p_cszName, "line") == 0)
	{
		p_pBM->m_pLineValue = &(struct Value){ 3, (void*)&(struct NumberValue){p_pBM->m_lLine} };
		return p_pBM->m_pLineValue;
	}
	else if(p_pBM->m_pLocal && p_pBM->m_pLocal->m_szKey && strcmp(p_cszName, p_pBM->m_pLocal->m_szKey) == 0)
	{
		if(p_pBM->m_pLocal->m_pVal->m_uiType == 4 && p_pBM->m_pLocal->m_pVal->m_pValue != NULL)
		{
			return (struct Value*)((struct ListValue*)p_pBM->m_pLocal->m_pVal->m_pValue)->m_pVal;
		}
		else
			return p_pBM->m_pLocal->m_pVal;
	}

	return NULL;
}

struct Block* seek_block(zString p_szSource, const unsigned long p_culStart, struct Param* p_pParameters)
{
	if(p_szSource == NULL || p_culStart >= strlen(p_szSource))
		return NULL;

	struct Block* pBlock = (struct Block*)malloc(sizeof(struct Block));
	long lExpStart = seek(p_szSource, BLOCK_ENTRY, p_culStart) + strlen(BLOCK_ENTRY);
	long lExpEnd = seek(p_szSource, BLOCK_EXIT, p_culStart);
	
	//If both - expression end and expression start found
	if(lExpStart >= 0 && lExpEnd >= 0)
	{

		unsigned long ulExpSize = lExpEnd - lExpStart;
		zString szExp = (zString)malloc(sizeof(zString) * (ulExpSize + 1));
		memcpy(szExp, p_szSource + lExpStart, ulExpSize);
		szExp[ulExpSize] = '\0';
		eval_block(pBlock, szExp, p_pParameters);

		pBlock->m_ulHeaderStart = lExpStart - strlen(BLOCK_ENTRY);	
		pBlock->m_ulHeaderEnd = lExpEnd + strlen(BLOCK_EXIT);
		
		long lBlockPos = seek(p_szSource, BLOCK_END, p_culStart);
		pBlock->m_ulEnd = lBlockPos;

		if(pBlock->m_ulEnd < 0) //End tag is missing
		{
			free(pBlock);
			return NULL;
		}
		pBlock->m_ulStart = pBlock->m_ulHeaderEnd;
		pBlock->m_ulFooterStart = lBlockPos;
		pBlock->m_ulFooterEnd = lBlockPos + strlen(BLOCK_END);
	}
	else
	{
		free(pBlock);
		return NULL;
	}

	const long unsigned culBlockSize = pBlock->m_ulEnd - pBlock->m_ulStart;
	pBlock->m_szBlock = (zString)malloc(sizeof(zString) * (culBlockSize + 1));
	memcpy(pBlock->m_szBlock, p_szSource + pBlock->m_ulStart, culBlockSize);
	pBlock->m_szBlock[culBlockSize] = '\0';
	DEBUG(3, "\nBlock: '%s'\n", pBlock->m_szBlock);
	return pBlock;
}

void eval_block(struct Block* p_pBlock, zString p_szExpression, struct Param* p_pParameters)
{
	static bool bRegexLoaded = false;
	static struct RegexState* reForeach;
	static struct RegexState* reLogical;
	static struct RegexState* reParam;
	if(!bRegexLoaded)
	{
		bRegexLoaded = true;
		reForeach = compile_regex("\\s*(foreach)\\s*($\\w+)\\s*(in)\\s*($\\w+)");
		reLogical = compile_regex("\\s*(if)\\s*($\\w+)");
		reParam = compile_regex("$\\w+");
	}

	if(strlen(p_szExpression) > 0)
	{
		zString pPtr = strtok(p_szExpression, ";"); //Handle multiple expressions
		while(pPtr != NULL)
		{
			zString szExp = trim(pPtr);
			//Possible for-each statement
			if(regex_test(reForeach, pPtr))
			{
				unsigned long ulPosition = 0;
				zString szFirst = regex_search(reParam, pPtr, &ulPosition, false);
				zString szSecond = regex_search(reParam, pPtr, &ulPosition, false);
				if(szFirst == NULL || szSecond == NULL)
				{
					printf("Impossible has happened... Fix damn bugs in regex :) \n");
					return;
				}
				p_pBlock->m_BM.m_pLocal = (struct Param*)calloc(1, sizeof(struct Param));
				p_pBlock->m_BM.m_pLocal->m_szKey = (zString)malloc(sizeof(zString) * (strlen(pull_param(szFirst)) + 1));
				strcpy(p_pBlock->m_BM.m_pLocal->m_szKey, pull_param(szFirst));
				struct Value* pParam = search_parameter(p_pParameters, pull_param(szSecond));
				if(pParam && pParam->m_uiType == 4)
				{
					p_pBlock->m_BM.m_pLocal->m_pVal = (struct Value*)malloc(sizeof(struct Value));
					p_pBlock->m_BM.m_pLocal->m_pVal->m_uiType = 4;
					p_pBlock->m_BM.m_pLocal->m_pVal->m_pValue = malloc(sizeof(struct ListValue));
					((struct ListValue*)p_pBlock->m_BM.m_pLocal->m_pVal->m_pValue)->m_pVal = ((struct ListValue*)pParam->m_pValue)->m_pVal;
					((struct ListValue*)p_pBlock->m_BM.m_pLocal->m_pVal->m_pValue)->m_pNext = ((struct ListValue*)pParam->m_pValue)->m_pNext;
					p_pBlock->m_BM.m_bCondition = true;
				}
					
				free(szFirst);
				free(szSecond);
			}
			//Possible variable check statement
			if(regex_test(reLogical, pPtr))
			{
				unsigned long ulPosition = 0;
				zString szParamName = regex_search(reParam, pPtr, &ulPosition, false);
				p_pBlock->m_BM.m_bCondition = *search_parameter_bool(p_pParameters, pull_param(szParamName));
				p_pBlock->m_BM.m_pLocal = NULL;
				free(szParamName);
			}

			free(szExp);
			pPtr = strtok(NULL, ";");
		}
	}

	free(p_szExpression);
}


zString render_block(zString p_szBuffer, struct Param* p_pParameters, struct BlockMemory* p_pBM)
{
	long lStart = 0;
	long lEnd = 0;
	long lTagSize = 0;
	zString chTempBuff = NULL;

	unsigned int uiStLen = strlen(START_TAG);
	unsigned int uiEnLen = strlen(END_TAG);
	unsigned int uiTagLen = uiStLen + uiEnLen;
	unsigned long ulResultLen = 0;
	while(true)
	{
		lStart = seek(p_szBuffer, START_TAG, lStart);
		if(lStart < 0)
			break;
		lEnd = seek(p_szBuffer, END_TAG, lStart);
		if(lEnd < 0)
			break;
		lTagSize = lEnd - (lStart + uiStLen);
		//Copy tag to buffer
		chTempBuff = (zString)malloc(sizeof(zString) * (lTagSize + 1));
		memcpy(chTempBuff, p_szBuffer + lStart + uiStLen, lTagSize);
		chTempBuff[lTagSize] = '\0';

		zString szResult = interpret(chTempBuff, p_pParameters, p_pBM);
		if(szResult != NULL)
		{
			//Insert result
			str_insert(p_szBuffer, szResult, lStart, lEnd + uiEnLen);
			free(szResult);
		}
		else
			str_insert(p_szBuffer, "", lStart, lEnd + uiEnLen);
		
		free(chTempBuff);

		lStart += ulResultLen - uiTagLen;
	}
	return p_szBuffer;
}

void handle_block(struct Block* p_pBlock, struct Param* p_pParameters, zString p_szBuffer)
{
	if(p_pBlock == NULL)
		return;

	zString szResult = (zString)calloc(sizeof(zString), 1024);
	while(p_pBlock->m_BM.m_bCondition)
	{
		zString szRenderBuff = (zString)malloc(sizeof(zString) * strlen(p_pBlock->m_szBlock));
		strcpy(szRenderBuff, p_pBlock->m_szBlock);
	 	strcat(szResult, render_block(szRenderBuff, p_pParameters, &p_pBlock->m_BM));
	 	step_BM(&p_pBlock->m_BM);
	 	free(szRenderBuff);
	}
	clean_BM(&p_pBlock->m_BM);
	str_insert(p_szBuffer, szResult, p_pBlock->m_ulHeaderStart, p_pBlock->m_ulFooterEnd);
	free(p_pBlock->m_szBlock);
	free(p_pBlock);
	free(szResult);
}