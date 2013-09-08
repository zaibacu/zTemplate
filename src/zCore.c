#include "zCore.h"
#include "Util.h"

//Block memory handling

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
//Other stuff
void eval_block(struct Block* p_pBlock, zString p_szExpression, struct Param* p_pParameters);

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
	else if(p_pBM->m_pLocal && strcmp(p_cszName, p_pBM->m_pLocal->m_szKey) == 0)
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

struct Value* search_parameter(struct Param* p_pParameters, const zString p_cszName)
{
	if(p_pParameters == NULL || p_cszName == NULL)
		return NULL;

	struct Param* pPtr = p_pParameters;
	while(pPtr != NULL)
	{
		if(pPtr->m_szKey != NULL && strcmp(pPtr->m_szKey, p_cszName) == 0)
		{
			return pPtr->m_pVal;
		}
		pPtr = pPtr->m_pNext;
	}
	return NULL;
}



//Returns pointer to value
bool* search_parameter_bool(struct Param* p_pParameters, const zString p_cszName)
{
	struct Value* pParameter = search_parameter(p_pParameters, p_cszName);
	if(pParameter != NULL && pParameter->m_uiType == 2)
	{
		return &((struct BoolValue*)pParameter->m_pValue)->m_bValue;
	}

	return NULL;
}

long* search_parameter_number(struct Param* p_pParameters, const zString p_cszName)
{
	struct Value* pParameter = search_parameter(p_pParameters, p_cszName);
	if(pParameter != NULL)
	{
		return &(((struct NumberValue*)pParameter->m_pValue)->m_lValue);
	}

	return NULL;
}

//Returns copy of value
zString search_parameter_str(struct Param* p_pParameters, const zString p_cszName)
{
	struct Value* pParameter = search_parameter(p_pParameters, p_cszName);
	if(pParameter != NULL && pParameter->m_uiType == 1 && pParameter->m_pValue != NULL)
	{
		//We want to return a copy instead of pointer to string
		const zString cszValue = ((struct StringValue*)pParameter->m_pValue)->m_szValue;
		if(cszValue == NULL)
			return NULL;

		const unsigned long culSize = strlen(cszValue);
		zString szCopy = (zString)malloc(sizeof(zString) * (culSize + 1));
		memcpy(szCopy, cszValue, culSize);
		szCopy[culSize] = '\0';
		return szCopy;
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
	//printf("\nBlock: '%s'\n", pBlock->m_szBlock);
	return pBlock;
}

void eval_block(struct Block* p_pBlock, zString p_szExpression, struct Param* p_pParameters)
{
	if(strlen(p_szExpression) > 0)
	{
		bool bEvaluated = false;
		zString pPtr = strtok(p_szExpression, ";"); //Handle multiple expressions
		while(pPtr != NULL)
		{
			zString szExp = trim(pPtr);
			bEvaluated = false;
			//Possible for-each statement
			if(!bEvaluated)
			{
				long lStart = seek(szExp, "foreach", 0);
				if(lStart != -1)
				{
					long lPosPtr = lStart + 7;
					zString szFirst = pull_param_name(szExp, &lPosPtr);
					zString szSecond = pull_param_name(szExp, &lPosPtr);
					bEvaluated = true;
					p_pBlock->m_BM.m_pLocal = (struct Param*)malloc(sizeof(struct Param));
					p_pBlock->m_BM.m_pLocal->m_szKey = (zString)malloc(sizeof(zString) * (strlen(szFirst) + 1));
					strcpy(p_pBlock->m_BM.m_pLocal->m_szKey, szFirst);
					struct Value* pParam = search_parameter(p_pParameters, szSecond);
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
			}
			//Possible variable check statement
			if(!bEvaluated)
			{
				long lStart = seek(szExp, "if", 0);
				if(lStart != -1)
				{
					bEvaluated = true;
					zString szParamName = pull_param_name(szExp, &lStart);
					p_pBlock->m_BM.m_bCondition = *search_parameter_bool(p_pParameters, szParamName);
					free(szParamName);
				}
			}

			free(szExp);
			pPtr = strtok(NULL, ";");
		}
	}

	free(p_szExpression);
}

zString interpret(zString p_szSource, struct Param* p_pParameters, struct BlockMemory* p_pBM)
{
	p_szSource = trim(p_szSource);
	//Guessing this is include
	{
		long lStart = seek(p_szSource, "include", 0);
		if(lStart != -1)
		{
			zString szFileName = trim(p_szSource + lStart + 7);
			zString szResult = render(szFileName, p_pParameters);
			free(szFileName);
			free(p_szSource);
			return szResult;
		}
	}
	//Guessing this is variable
	{
		long lStart = seek(p_szSource, "$", 0);
		if(lStart != -1)
		{
			zString szResult = (zString)malloc(sizeof(zString) * 10 /* Let's assume this is enough space... */);
			//Now search for key
			struct Value* pValue = p_pBM != NULL ? search_block_parameter(p_pBM, p_szSource + lStart + 1) : NULL;
			if(pValue != NULL)
			{
				if(pValue->m_uiType == 1)
				{
					//We want to return a copy instead of pointer to string
					const zString cszValue = ((struct StringValue*)pValue->m_pValue)->m_szValue;
					if(cszValue != NULL)
					{
						const unsigned long culSize = strlen(cszValue);
						free(szResult);
						szResult = (zString)malloc(sizeof(zString) * (culSize + 1));
						memcpy(szResult, cszValue, culSize);
						szResult[culSize] = '\0';
					}
				}
				else if(pValue->m_uiType == 2)
				{
					sprintf(szResult, "%d\0", ((struct BoolValue*)pValue->m_pValue)->m_bValue);
				}
				else if(pValue->m_uiType == 3)
				{
					sprintf(szResult, "%d\0", ((struct NumberValue*)pValue->m_pValue)->m_lValue);
				}
			}
			else
			{
				szResult = search_parameter_str(p_pParameters, p_szSource + lStart + 1);
				if(szResult == NULL)
				{
					long* pResult = search_parameter_number(p_pParameters, p_szSource + lStart + 1);
					if(pResult != NULL)
					{
						sprintf(szResult, "%d\0", *pResult);
					}
				}
			}

			
			free(p_szSource);
			return szResult;
		}
	}
	
	free(p_szSource);
	return "";
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
		
		//Insert result
		str_insert(p_szBuffer, szResult, lStart, lEnd + uiEnLen);

		//Free memory
		free(szResult);
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

zString render(const zString p_cszTemplate, struct Param* p_pParameters)
{
	zString szBuffer = read_file(p_cszTemplate);
	unsigned long ulBlockPos = 0;
	struct Block* pBlock = NULL;
	while((pBlock = seek_block(szBuffer, ulBlockPos, p_pParameters)) != NULL)
	{
		handle_block(pBlock, p_pParameters, szBuffer);
	}
	render_block(szBuffer, p_pParameters, NULL);
	return szBuffer;
}