#pragma once


#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#endif


#define ArrayCount(x)		(sizeof(x) / sizeof(x[0]))

#define ENSURE(con, res)	if (!(con)){return res;}
#define ENSURET(con)	ENSURE(con, ;)
#define ENSUREf(con)	ENSURE(con, false)
#define ENSUREL(con)	ENSURE(con, NULL)
#define ENSUREO(con)	ENSURE(con, 0)

#define ENSUREK(con)		if (!(con)){break;}

#define DISABLE_CONSTRUCTER(class) class(){}




__inline void _DbgPoint()
{
#ifdef _DEBUG
#ifdef _WIN32
	__asm int 3;
#endif
#endif // _DEBUG
}



#ifdef _DEBUG
#ifdef _WIN32
#define AssertMsg(exps, comment) if (!(exps)){char msg[1024];sprintf(msg, "%s:%d", __FILE__, __LINE__);MessageBoxA(NULL, msg, comment, MB_ICONERROR);_DbgPoint();}
#else
#define AssertMsg(exps, comment) if (!(exps)){char msg[1024];sprintf(msg, "%s:%d", __FILE__, __LINE__);assert(msg);}
#endif // _WIN32
#else
#define AssertMsg(exps, comment) if (!(exps)){}
#endif // _DEBUG

#define Assert(exps) AssertMsg(exps, "Assert Happened")