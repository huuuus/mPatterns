#include "baseCirclePCH.h"
#include "cinder/params/Params.h"
#include "cinder/gl/gl.h"
#include "graphicAppState.h"
#include "colorAnimator.h"
#include "contextMngr.h"

using namespace mPatterns;
using namespace params;
using namespace app;
using namespace std;

void GraphicAppState::init() {
    mParams = params::InterfaceGl( "GraphicAppState", Vec2i( 128, 128 ) );
    mBgColor.set(cinder::CM_RGB, Vec3f(0.f, 0.f, 0.f));
    mParams.addParam( "bg", &mBgColor, "" );
    mParams.setOptions("", (format("position='%d %d'") % 
                            (getWindowWidth()/96) % 
                            (getWindowHeight()/16)).str());
	contextMngr::init();	
}

void GraphicAppState::mouseUp( MouseEvent event )
{
    hasPickableNode::mouseUp(event);    
}

void GraphicAppState::mouseDown( MouseEvent event )
{
    hasPickableNode::mouseDown(event);
}

void GraphicAppState::mouseMove( MouseEvent event )
{
    hasPickableNode::mouseMove(event);
}

void GraphicAppState::preDraw() {
    gl::clear( Color( mBgColor ) );
}

void GraphicAppState::postDraw(bool drawScene) {
    if (drawScene)
        NODE_MGR.draw();
    params::InterfaceGl::draw();
}

void GraphicAppState::postUpdate() {
    NODE_MGR.updateNodes();
	IColorAnimator::tickAll();
}
