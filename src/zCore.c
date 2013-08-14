#include "zCore.h"
#include "Util.h"
void eval_block(struct Block* p_pBlock, zString p_szExpression);

zString search_parameter(struct Param* p_pParameters, const zString p_cszName)
{
	if(p_pParameters == NULL)
		return "";

	struct Param* pPtr = p_pParameters;
	while(pPtr != NULL)
	{
		if(pPtr->m_szKey != NULL && pPtr->m_Val.m_uiType == 1 && strcmp(pPtr->m_szKey, p_cszName) == 0)
		{
			//We want to return a copy instead of pointer to string
			const zString cszValue = ((struct StringValue*)pPtr->m_Val.m_pValue)->m_szValue;
			
			const unsigned long culSize = strlen(cszValue);
			zString szCopy = (zString)malloc(sizeof(zString) * (culSize + 1));
			memcpy(szCopy, cszValue, culSize);
			szCopy[culSize] = '\0';
			return szCopy;
		}
		pPtr = pPtr->m_pNext;
	}
	return NULL;
}

struct Block* seek_block(const zString p_cszSource, const unsigned long p_culStart)
{
	if(p_cszSource == NULL || p_culStart >= strlen(p_cszSource))
		return NULL;

	struct Block* pBlock = (struct Block*)malloc(sizeof(struct Block));
	unsigned long ulExpStart = seek(p_cszSource, BLOCK_ENTRY, p_culStart);
	unsigned long ulExpEnd = seek(p_cszSource, BLOCK_EXIT, p_culStart);
	unsigned long ulExpSize = ulExpEnd - ulExpStart;
	zString szExp = (zString)malloc(sizeof(zString) * (ulExpSize + 1));
	memcpy(szExp, p_cszSource + ulExpStart, ulExpSize);
	szExp[ulExpSize] = '\0';

	//If both - expression end and expression start found
	if(ulExpStart * ulExpEnd < 0)
	{
		pBlock->m_ulStart = ulExpEnd + sizeof(BLOCK_EXIT);
		pBlock->m_ulEnd = seek(p_cszSource, BLOCK_END, p_culStart);
		if(pBlock->m_ulEnd) //End tag is missing
		{
			free(pBlock);
			return NULL;
		}
	}
	//Otherwise we take whole file as single block
	else
	{
		pBlock->m_ulStart = 0;
		pBlock->m_ulEnd = strlen(p_cszSource);
	}

	const unsigned long culBlockSize = pBlock->m_ulEnd - pBlock->m_ulStart;

	pBlock->m_szBlock = (zString)malloc(sizeof(zString) * (culBlockSize + 1));
	memcpy(pBlock->m_szBlock, p_cszSource + pBlock->m_ulStart, culBlockSize);
	pBlock->m_szBlock[culBlockSize] = '\0';
	eval_block(pBlock, szExp);
	return pBlock;
}

void eval_block(struct Block* p_pBlock, zString p_szExpression)
{
	if(strlen(p_szExpression) > 0)
	{
		printf("Expression: %s\n", p_szExpression);
	}

	free(p_szExpression);
}

zString interpret(zString p_szSource, struct Param* p_pParameters)
{
	p_szSource = trim(p_szSource);
	//Guessing this is include
	{
		unsigned long ulStart = seek(p_szSource, "include", 0);
		if(ulStart != -1)
		{
			zString szFileName = trim(p_szSource + ulStart + 7);
			zString szResult = render(szFileName, p_pParameters);
			free(szFileName);
			free(p_szSource);
			return szResult;
		}
	}
	//Guessing this is variable
	{
		unsigned long ulStart = seek(p_szSource, "$", 0);
		if(ulStart != -1)
		{
			//Now search for key
			zString szResult = search_parameter(p_pParameters, p_szSource + ulStart);
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
		lTagSize = lEnd - (lStart + uiStLen + uiEnLen);
		//Copy tag to buffer
		chTempBuff = (zString)malloc(sizeof(char) * (lTagSize + 1));
		memcpy(chTempBuff, p_szBuffer + lStart + uiStLen, lTagSize);
		chTempBuff[lTagSize] = '\0';

		zString szResult = interpret(chTempBuff, p_pParameters);
		
		//Insert result
		str_insert(p_szBuffer, szResult, lStart - 1, lEnd + 1);

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
	while((pBlock = seek_block(szBuffer, ulBlockPos)) != NULL)
	{
		zString szResult = render_block(pBlock->m_szBlock, p_pParameters);
		str_insert(szBuffer, szResult, pBlock->m_ulStart, pBlock->m_ulEnd);
		ulBlockPos = pBlock->m_ulEnd;
		free(pBlock->m_szBlock);
		free(pBlock);
	}
	//printf("Result: %s\n", szBuffer);
	return szBuffer;
}