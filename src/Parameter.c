#include "Parameter.h"

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