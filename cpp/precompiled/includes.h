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
#include <memory>
#include <vector>