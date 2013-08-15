#include "zCore.h"
#include "Util.h"



//Block memory handling
static struct BlockMemory
{
	bool* m_pbCondition;
} BM;

//Other stuff
void eval_block(struct Block* p_pBlock, zString p_szExpression, struct Param* p_pParameters);


struct Param* search_parameter(struct Param* p_pParameters, const zString p_cszName)
{
	if(p_pParameters == NULL || p_cszName == NULL)
		return NULL;

	struct Param* pPtr = p_pParameters;
	while(pPtr != NULL)
	{
		if(pPtr->m_szKey != NULL && strcmp(pPtr->m_szKey, p_cszName) == 0)
			return pPtr;
		pPtr = pPtr->m_pNext;
	}
	return NULL;
}

//Returns pointer to value
bool* search_parameter_bool(struct Param* p_pParameters, const zString p_cszName)
{
	struct Param* pParameter = search_parameter(p_pParameters, p_cszName);
	if(pParameter != NULL && pParameter->m_Val.m_uiType == 2)
	{
		return &((struct BoolValue*)pParameter->m_Val.m_pValue)->m_bValue;
	}

	return NULL;
}

//Returns copy of value
zString search_parameter_str(struct Param* p_pParameters, const zString p_cszName)
{
	struct Param* pParameter = search_parameter(p_pParameters, p_cszName);
	if(pParameter != NULL && pParameter->m_Val.m_uiType == 1)
	{
		//We want to return a copy instead of pointer to string
		const zString cszValue = ((struct StringValue*)pParameter->m_Val.m_pValue)->m_szValue;
		
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

		pBlock->m_ulHeaderStart = get_or_default(seek_back(p_szSource, "\n", lExpStart - strlen(BLOCK_ENTRY)), lExpStart - strlen(BLOCK_ENTRY));	
		pBlock->m_ulHeaderEnd = get_or_default(seek(p_szSource, "\n", lExpEnd + strlen(BLOCK_EXIT)) + 1, lExpEnd + strlen(BLOCK_EXIT));
		
		long lBlockPos = seek(p_szSource, BLOCK_END, p_culStart);
		pBlock->m_ulEnd = get_or_default(seek_back(p_szSource, "\n", lBlockPos), lBlockPos);

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
					bEvaluated = true;
				}
			}
			//Possible variable check statement
			if(!bEvaluated)
			{
				long lStart = seek(szExp, "if", 0);
				if(lStart != -1)
				{
					lStart = seek(szExp, "$", 0);
					bEvaluated = true;
					BM.m_pbCondition = search_parameter_bool(p_pParameters, szExp + lStart + 1);
				}
			}

			free(szExp);
			pPtr = strtok(NULL, ";");
		}
	}

	free(p_szExpression);
}

zString interpret(zString p_szSource, struct Param* p_pParameters)
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
			//Now search for key
			zString szResult = search_parameter_str(p_pParameters, p_szSource + lStart + 1);
			free(p_szSource);
			return szResult;
		}
	}
	
	free(p_szSource);
	return "";
}

zString render_block(zString p_szBuffer, struct Param* p_pParameters)
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

		zString szResult = interpret(chTempBuff, p_pParameters);
		
		//Insert result
		str_insert(p_szBuffer, szResult, lStart, lEnd + uiEnLen);

		//Free memory
		free(szResult);
		free(chTempBuff);

		lStart += ulResultLen - uiTagLen;
	}

	return p_szBuffer;
}

zString render(const zString p_cszTemplate, struct Param* p_pParameters)
{
	zString szBuffer = read_file(p_cszTemplate);
	unsigned long ulBlockPos = 0;
	struct Block* pBlock = NULL;
	while((pBlock = seek_block(szBuffer, ulBlockPos, p_pParameters)) != NULL)
	{
		zString szResult = NULL;
		if(BM.m_pbCondition && *BM.m_pbCondition)
		 	szResult = render_block(pBlock->m_szBlock, p_pParameters);
		str_insert(szBuffer, szResult, pBlock->m_ulHeaderStart, pBlock->m_ulFooterEnd);
		free(pBlock->m_szBlock);
		free(pBlock);
		//printf("Result: %s\n", szBuffer);
	}
	//Do last, full page render
	zString szResult = render_block(szBuffer, p_pParameters);
	str_insert(szBuffer, szResult, 0, strlen(szResult));
	printf("Result: %s\n", szBuffer);
	return szBuffer;
}