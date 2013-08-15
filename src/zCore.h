#pragma once

struct Value
{
	/*
		type 1 - string
		type 2 - bool
	*/
	unsigned int m_uiType;
	void* m_pValue;
};

struct StringValue
{
	const zString m_szValue;
};

struct BoolValue
{
	bool m_bValue;
};

struct Param
{
	zString m_szKey;
	struct Value m_Val;
	struct Param* m_pNext;
};



struct Block
{
	//Header
	unsigned long m_ulHeaderStart;
	unsigned long m_ulHeaderEnd;
	//Footer
	unsigned long m_ulFooterStart;
	unsigned long m_ulFooterEnd;
	//Content
	unsigned long m_ulStart;
	unsigned long m_ulEnd;
	
	zString m_szBlock;
};

//Prototypes
zString render(const zString p_cszTemplate, struct Param* p_pParameters);
