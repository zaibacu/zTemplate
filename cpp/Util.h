#pragma once
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

namespace Util
{
	

	// trim from start
	static inline std::string& TrimLeft(std::string& p_rszStr) {
	        p_rszStr.erase(p_rszStr.begin(), std::find_if(p_rszStr.begin(), p_rszStr.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	        return p_rszStr;
	}

	// trim from end
	static inline std::string& TrimRight(std::string& p_rszStr) {
	        p_rszStr.erase(std::find_if(p_rszStr.rbegin(), p_rszStr.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), p_rszStr.end());
	        return p_rszStr;
	}

	// trim from both ends
	static inline std::string& Trim(std::string& p_rszStr) {
	        return TrimLeft(TrimRight(p_rszStr));
	}
}