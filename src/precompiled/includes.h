 #define _LINUX
#ifdef _LINUX
 #define EXPORT extern "C"
#endif
#ifdef _WINDOWS
 #define EXPORT extern "C" __declspec(dllexport)
#endif
#include <map>

typedef const char* zString;
typedef std::map<std::string, std::string> StrMap;
#include <exception> 

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

// trim from start
static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
}