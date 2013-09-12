#include "zCore.h"
#include "Util.h"
#include "Block.h"
#include "Parameter.h"

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
					sprintf(szResult, "%d", ((struct BoolValue*)pValue->m_pValue)->m_bValue);
				}
				else if(pValue->m_uiType == 3)
				{
					sprintf(szResult, "%ld", ((struct NumberValue*)pValue->m_pValue)->m_lValue);
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
						sprintf(szResult, "%ld", *pResult);
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