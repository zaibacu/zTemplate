#include "zCore.h"
#include "Util.h"

zString search_parameter(struct Param* p_pParameters, const zString p_cszName)
{
	if(p_pParameters == NULL)
		return "";

	struct Param* pPtr = p_pParameters;
	while(pPtr != NULL)
	{
		if(pPtr->m_szKey != NULL && strcmp(pPtr->m_szKey, p_cszName) == 0)
			return pPtr->m_szVal;
		pPtr = pPtr->m_pNext;
	}
	return "";
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
			//printf("Include:%s\n", szFileName);
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

zString render(const zString p_cszTemplate, struct Param* p_pParameters)
{
	zString szBuffer = read_file(p_cszTemplate);
	long lStart = 0;
	long lEnd = 0;
	long lTagSize = 0;
	zString chTempBuff = NULL;

	unsigned int uiStLen = strlen(START_TAG);
	unsigned int uiEnLen = strlen(END_TAG);
	unsigned int uiTagLen = uiStLen + uiEnLen;
	unsigned long ulResultLen = 0;
	while(TRUE)
	{
		lStart = seek(szBuffer, START_TAG, lStart);
		if(lStart < 0)
			break;
		lEnd = seek(szBuffer, END_TAG, lStart);
		if(lEnd < 0)
			break;

		lTagSize = lEnd - (lStart + uiStLen + uiEnLen);
		//Copy tag to buffer
		chTempBuff = (zString)malloc(sizeof(char) * (lTagSize + 1));
		memcpy(chTempBuff, szBuffer + lStart + uiStLen, lTagSize);
		chTempBuff[lTagSize] = '\0';

		zString szResult = interpret(chTempBuff, p_pParameters);
		//Free memory
		free(chTempBuff);
		if(szResult == NULL)
			ulResultLen = 0;
		else
			ulResultLen = strlen(szResult);
		//----Copy our result
		//Allocate enough size
		long lBuffSize = strlen(szBuffer) + ulResultLen;
		chTempBuff = (zString)malloc(sizeof(zString) * (lBuffSize + 1));
		chTempBuff[lBuffSize] = '\0';
		//Move first part of our buffer to temp buffer
		memcpy(chTempBuff, szBuffer, lStart - 1);
		//Move our result
		memcpy(chTempBuff + lStart - 1, szResult, ulResultLen);
		//Move rest part of buffer
		memcpy(chTempBuff + lStart - 1 + ulResultLen, szBuffer + lEnd + 1, strlen(szBuffer) + lStart + 1);
		//Move everything to our main buffer
		memcpy(szBuffer, chTempBuff, strlen(chTempBuff) + 1);
		szBuffer[strlen(szBuffer)] = '\0';
		//Free memory
		free(chTempBuff);

		//printf("Buff:\n%s\n", szBuffer);
		lStart += ulResultLen - uiTagLen;
	}
	return szBuffer;
}