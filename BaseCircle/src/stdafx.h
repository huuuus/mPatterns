#if defined(__FOR_PCH) || defined(WIN32)

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/rand.h"
#include "cinder/Utilities.h"
#include <assert.h>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace boost;

#define DECL_SHARED_PTR(className)  class className;\
                                    typedef boost::shared_ptr<className> className##Ptr;\
                                    typedef className* className##WeakPtr;

#define DECL_SHARED_PTR_EX(className,extra)     template<class CName>\
                                                class className;\
                                                typedef boost::shared_ptr<className<extra> > className##Ptr;\
                                                typedef className<extra>* className##WeakPtr;

#endif