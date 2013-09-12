#pragma once

struct Value;

struct BlockMemory
{
	long m_lCurValue;
	long m_lLine;
	bool m_bOdd;
	bool m_bEven;
	struct Param* m_pLocal;
	struct Value* m_pLineValue;
	struct Value* m_pOddValue;
	struct Value* m_pEvenValue;
	bool m_bCondition;
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

	struct BlockMemory m_BM;
};

zString render_block(zString p_szBuffer, struct Param* p_pParameters, struct BlockMemory* p_pBM);
struct Block* seek_block(zString p_szSource, const unsigned long p_culStart, struct Param* p_pParameters);
void handle_block(struct Block* p_pBlock, struct Param* p_pParameters, zString p_szBuffer);
struct Value* search_block_parameter(struct BlockMemory* p_pBM, const zString p_cszName);
void eval_block(struct Block* p_pBlock, zString p_szExpression, struct Param* p_pParameters);