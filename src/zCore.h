#pragma once

struct Value
{
	/*
		type 1 - string
	*/
	unsigned int m_uiType;
	void* m_pValue;
};

struct StringValue
{
	const zString m_szValue;
};

struct Param
{
	zString m_szKey;
	struct Value m_Val;
	struct Param* m_pNext;
};



struct Block
{
	unsigned long m_ulStart;
	unsigned long m_ulEnd;
	bool (*exp)(void*);
	zString m_szBlock;
};

//Prototypes
zString render(const zString p_cszTemplate, struct Param* p_pParameters);
