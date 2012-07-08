#include "BaseCirclePCH.h"

#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"
#include "cinder/rand.h"
#include "cinder/Timeline.h"

#include "GraphicAppState.h"
#include "TwoCircles7State.h"

using namespace mPatterns;
using namespace params;
using namespace app;
using namespace std;

void TwoCircles7State::init() {
    GraphicAppState::init();
    
    mpC7_1 = NODE_MGR.createCircles7(48.f, false);
	Vec2f pos = getWindowCenter();
	mpC7_1->mPos = pos;
    
	mpC7_2 = NODE_MGR.createCircles7(32.f, false);
	pos.x += 256;
	mpC7_2->mPos = pos;
    
    initAnimator();
}

void TwoCircles7State::initAnimator() {
    int mNumDestinations=8;
    mAnimDestinations.push_back(getWindowCenter());
    timeline().apply( &mAnimatedPos, (Vec2f)mAnimDestinations[0], 3.f, EaseInOutQuad() );
    for( int i=1; i<mNumDestinations; i++ ) {
        mAnimDestinations.push_back(Vec2f(Rand::randFloat(getWindowWidth()/2),getWindowHeight()/3+Rand::randFloat(getWindowHeight()/4)));
        timeline().appendTo( &mAnimatedPos, (Vec2f)mAnimDestinations[i], 1.f, EaseInOutQuad() );
    }
}

void TwoCircles7State::deInit() {
}

void TwoCircles7State::update(float fDt) {
    mpC7_1->mPos = mAnimatedPos;
    GraphicAppState::postUpdate();
}

void TwoCircles7State::draw() {
    GraphicAppState::preDraw();
    gl::enableAdditiveBlending();
    GraphicAppState::postDraw();
}

void TwoCircles7State::mouseWheel( MouseEvent event )
{
	NodeWeakPtr pNode = mCurHighlightedNode;
	if (!pNode)
        return;
    
    float wheelInc = event.getWheelIncrement();
    if (pNode->getType() == NT_CIRCLES7) {
        Circles7WeakPtr p = static_cast<Circles7WeakPtr>(pNode);
        for (int i=0;i<2;i++) {
            Color c;
            
            ColorAf &curC = p->mStyles[i]->mMainColor;
            Vec3f hsv;
            hsv = rgbToHSV( curC );
            
            float max = wheelInc*0.25f;
            hsv.x += randFloat(-max, max);
            hsv.y += randFloat(-max, max);
            hsv.z = randFloat(0.25f, 0.5f);
            c.set(cinder::CM_HSV, hsv);
            
            curC = ColorAf(c.r, c.g, c.b, curC.a);
        }
    }
}
