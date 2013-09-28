#include "Util.h"

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