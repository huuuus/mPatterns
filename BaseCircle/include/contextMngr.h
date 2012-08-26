#include "cinder/Thread.h"
#include <boost/thread/mutex.hpp>

class contextMngr {
public:

#ifdef WIN32
	static HGLRC	_WIN32_DEFAULT_CONTEXT;
	static HDC		_WIN32_DEFAULT_DC;
#endif

    static void init() {
#ifdef WIN32
		_WIN32_DEFAULT_CONTEXT = wglGetCurrentContext();
		_WIN32_DEFAULT_DC = wglGetCurrentDC();
#endif        
    }
    
    static void deInit() {
        glMutex.unlock();
    }

    static void lockMain() {
		glMutex.lock();

#ifdef WIN32
		BOOL res = wglMakeCurrent(_WIN32_DEFAULT_DC, _WIN32_DEFAULT_CONTEXT);
		int err = GetLastError();
		if (!res && err) {
			assert(0);
		}
#else
		App::get()->getRenderer()->makeCurrentContext();
#endif		
    }
    
    static void unlockMain() {        
#ifdef WIN32
		BOOL res = wglMakeCurrent(NULL, NULL);
		int err = GetLastError();
		if (!res && err) {
			assert(0);
		}
#endif
		glMutex.unlock();
    }

private:
	static mutex glMutex;
};