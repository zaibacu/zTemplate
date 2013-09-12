#pragma once

struct Value
{
	/*
		type 1 - string
		type 2 - bool
		type 3 - number
		type 4 - list
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

struct NumberValue
{
	long m_lValue;
};

struct ListValue
{
	struct Value* m_pVal;
	struct ListValue* m_pNext;
};

struct Param
{
	zString m_szKey;
	struct Value* m_pVal;
	struct Param* m_pNext;
};

struct Value* search_parameter(struct Param* p_pParameters, const zString p_cszName);
bool* search_parameter_bool(struct Param* p_pParameters, const zString p_cszName);
long* search_parameter_number(struct Param* p_pParameters, const zString p_cszName);
zString search_parameter_str(struct Param* p_pParameters, const zString p_cszName);