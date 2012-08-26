#if defined(__FOR_PCH) || defined(WIN32)

#include <assert.h>
#include <boost/shared_ptr.hpp>
#include <boost/format.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/condition_variable.hpp>

#include "cinder/app/AppBasic.h"

typedef unsigned char u38;
typedef char s8;

typedef unsigned int u32;
typedef int s32;

using namespace ci;
using namespace std;	
using boost::format;

#define DECL_SHARED_PTR(className)  class className;\
                                    typedef boost::shared_ptr<className> className##Ptr;\

#define DECL_WEAK_PTR(className)  class className;\
                                  typedef className* className##WeakPtr;

#define DECL_WEAK_SHARED_PTR_EX(className,extra)     template<class CName>\
							                         class className;\
							  	                     typedef boost::shared_ptr<className<extra> > className##Ptr;\
								 	                 typedef className<extra>* className##WeakPtr;

#define M_PIf (3.14159265358979323846f)

// -- to debug mem leaks
#ifdef _DEBUG
   #ifndef DBG_NEW
      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
      #define new DBG_NEW
   #endif
#endif  // _DEBUG

// -- log helper for win32
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <wchar.h>

#ifdef WIN32
	inline void __cdecl DEBUG_MSG(const wchar_t *format, ...)
	{
		wchar_t buf[4096], *p = buf;
	va_list args;
	int     n;

			va_start(args, format);
			n = _vsnwprintf(p, sizeof buf - 3, format, args); // buf-3 is room for CR/LF/NUL
			va_end(args);

			p += (n < 0) ? sizeof buf - 3 : n;

			while ( p > buf  &&  isspace(p[-1]) )
					*--p = '\0';

			*p++ = '\r';
			*p++ = '\n';
			*p   = '\0';

			OutputDebugString(buf);
	}
#else
	inline void DEBUG_MSG(const wchar_t *format, ...) {};
#endif 

#endif