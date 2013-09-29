#pragma once
struct Param;
struct BlockMemory;
//Prototypes
zString render(const zString p_cszTemplate, struct Param* p_pParameters);
zString render_text(zString p_szText, struct Param* p_pParameters);
zString interpret(zString p_szSource, struct Param* p_pParameters, struct BlockMemory* p_pBM);
