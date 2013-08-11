#pragma once

struct Param
{
	zString m_szKey;
	zString m_szVal;
	struct Param* m_pNext;
};

//Prototypes
zString render(const zString p_cszTemplate, struct Param* p_pParameters);
