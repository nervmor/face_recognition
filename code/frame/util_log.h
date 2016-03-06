#pragma once

#include <stdio.h>
#include <string>
#include <stdarg.h>
#ifdef _WIN32
#include <Windows.h>
#endif

#define DISABLE_TAG	"DISABLE_TAG"

class util_log
{
public:
	static void log(const char* szTag, const char* szformat, ...)
	{
		std::string strTag = szTag;
		if (strTag == DISABLE_TAG)
		{
			return;
		}
		char buffer[50 * 4096];
		char* p = buffer;
		va_list args;
		setlocale(LC_CTYPE,"chs");
		va_start(args, szformat); 
#ifdef _WIN32
		p += _vsnprintf(buffer, sizeof(buffer) - 1, szformat, args);
#else
		p += vsnprintf(buffer, sizeof(buffer) - 1, szformat, args);
#endif // _WIN32
		*p = '\0';
		va_end(args);
		printf("[--%s--]%s\n", szTag, buffer);
	}
	static void logd(const char* szTag, const char* szformat, ...)
	{
		std::string strTag = szTag;
		if (strTag == DISABLE_TAG)
		{
			return;
		}
		char buffer[50 * 1024];
		char* p = buffer;
		va_list args;
		setlocale(LC_CTYPE,"chs");
		va_start(args, szformat); 
#ifdef _WIN32
		p += _vsnprintf(buffer, sizeof(buffer) - 1, szformat, args);
#else
		p += vsnprintf(buffer, sizeof(buffer) - 1, szformat, args);
#endif // _WIN32
		*p = '\0';
		va_end(args);
		char _buffer[4096];
		unsigned int thread_id = -1;
#ifdef _WIN32
		thread_id = GetCurrentThreadId();
#endif
		sprintf(_buffer, "{%d}[--%s--]%s\n", thread_id, szTag, buffer);
#ifdef _WIN32
		OutputDebugStringA(_buffer);
#else
		printf(_buffer);
#endif // _WIN32
	}

};