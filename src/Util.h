#pragma once

zString trim(const zString p_cszText);
zString read_file(const zString p_cszName);
long seek(const zString p_cszSource, const zString p_cszPattern, const unsigned long p_culStart);
void str_insert(zString p_szSource, const zString p_cszInclude, const unsigned long p_culStart, const unsigned long p_culEnd);