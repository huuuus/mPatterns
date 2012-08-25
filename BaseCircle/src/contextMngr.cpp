#include "BaseCirclePCH.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"
#include "GraphicAppState.h"
#include "contextMngr.h"

boost::mutex contextMngr::glMutex;