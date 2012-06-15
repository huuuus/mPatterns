#if defined(__FOR_PCH) || defined(WIN32)

#include <assert.h>
#include <boost/shared_ptr.hpp>

#include "cinder/app/AppBasic.h"

using namespace ci;
using namespace std;	

#define DECL_SHARED_PTR(className)  class className;\
                                    typedef boost::shared_ptr<className> className##Ptr;\

#define DECL_WEAK_PTR(className)  class className;\
                                  typedef className* className##WeakPtr;

#define DECL_WEAK_SHARED_PTR_EX(className,extra)     template<class CName>\
							                         class className;\
							  	                     typedef boost::shared_ptr<className<extra> > className##Ptr;\
								 	                 typedef className<extra>* className##WeakPtr;

#define M_PIf (3.14159265358979323846f)

#endif