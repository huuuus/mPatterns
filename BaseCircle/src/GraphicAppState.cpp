#include "BaseCirclePCH.h"
#include "cinder/params/Params.h"
#include "cinder/gl/gl.h"
#include "GraphicAppState.h"

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

void GraphicAppState::postDraw() {
	NODE_MGR.draw();
    params::InterfaceGl::draw();
}

void GraphicAppState::postUpdate() {
    NODE_MGR.updateNodes();
}
