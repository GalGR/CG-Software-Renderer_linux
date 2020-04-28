#include <string>

#if defined(_WIN32)
typedef std::wstring string;
#elif defined(__linux__)
typedef std::string string;
#endif