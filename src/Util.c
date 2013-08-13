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
	unsigned long ulSize = 0;
	unsigned long ulDirLen = strlen(TEMPLATE_DIR) + strlen(p_cszName);
	zString chDir = (zString)malloc(sizeof(zString) * (ulDirLen + 1));
	strcpy(chDir, TEMPLATE_DIR);
	strcat(chDir, p_cszName);
	chDir[ulDirLen] = '\0';
	FILE* pFile = fopen(chDir, "r");
	if(pFile)
	{
		fseek (pFile , 0 , SEEK_END);
		ulSize = ftell (pFile);
		rewind (pFile);
		chBuff = (zString)malloc(sizeof(zString) * (ulSize + 1));
		fread(chBuff, 1, ulSize, pFile);
		chBuff[ulSize] = '\0';
		fclose(pFile);
	}
	free(chDir);
	return chBuff;
}

long seek(const zString p_cszSource, const zString p_cszPattern, const unsigned long p_culStart)
{
	if(p_cszSource == NULL)
		return -1;

	zString szResult = strstr(p_cszSource + p_culStart, p_cszPattern);
	if(szResult != NULL)
	{
		return strlen(p_cszSource) - strlen(szResult) + 1;
	}
	return -1;
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