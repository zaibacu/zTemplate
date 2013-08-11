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