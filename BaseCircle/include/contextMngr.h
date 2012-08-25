#include "cinder/Thread.h"
#include <boost/thread/mutex.hpp>

class contextMngr {
public:
    /*static void init() {
        
    }
    
    static void deInit() {
        
    }*/
    
    static void setMainAsCurrent() {
        App::get()->getRenderer()->makeCurrentContext();
    }
    
    static void lockMain() {
        glMutex.lock();
    }
    
    static void unlockMain() {
        glMutex.unlock();        
    }
    
protected:
    static mutex glMutex;
};