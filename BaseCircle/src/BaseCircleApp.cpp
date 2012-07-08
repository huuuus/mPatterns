#include "BaseCirclePCH.h"

#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"
#include "cinder/rand.h"
#include "cinder/Timeline.h"

#include "NodeMngr.h"

using namespace mPatterns;
using namespace ci;
using namespace params;
using namespace app;
using namespace std;

DECL_SHARED_PTR(AppState)
DECL_WEAK_PTR(AppState)

class IMouseHandler {
public:
    virtual void mouseMove( MouseEvent event ) {};
	virtual void mouseDown( MouseEvent event ) {};
	virtual void mouseDrag( MouseEvent event ) {};
	virtual void mouseUp( MouseEvent event ) {};
	virtual void mouseWheel( MouseEvent event ) {};
};

class AppState : public IMouseHandler {
public:
    virtual void init() = 0;    
    virtual void deInit() = 0;    
    virtual void update(float fDt) = 0;
    virtual void draw() = 0;
};

class hasPickableNode {
protected:
    hasPickableNode() {
        mCurPickedNode = 0;
        mCurHighlightedNode = 0;
    };
	virtual void mouseDown( MouseEvent event );
	virtual void mouseUp( MouseEvent event );
    virtual void mouseMove( MouseEvent event );
    NodeWeakPtr mCurPickedNode;
    NodeWeakPtr mCurHighlightedNode;
};

void hasPickableNode::mouseUp( MouseEvent event )
{
    mCurPickedNode = 0;    
}

void hasPickableNode::mouseMove( MouseEvent event )
{
    mCurHighlightedNode = NODE_MGR.picking(false, event.getPos(), NODE_MGR.mpRoot);
}

void hasPickableNode::mouseDown( MouseEvent event )
{
    mCurPickedNode = NODE_MGR.picking(true, event.getPos(), NODE_MGR.mpRoot);    
}

class GraphicAppState : public AppState, public hasPickableNode {
public:    
    virtual void init();
    void postUpdate();
    void preDraw();
    void postDraw();
    virtual void mouseDown( MouseEvent event );
	virtual void mouseUp( MouseEvent event );    
    virtual void mouseMove( MouseEvent event );
	virtual void mouseWheel( MouseEvent event );
    
protected:
    params::InterfaceGl	mParams;
    Color mBgColor;
};

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

void GraphicAppState::mouseWheel( MouseEvent event )
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

DECL_SHARED_PTR(TwoCircles7State)
class TwoCircles7State : public GraphicAppState {
public:
    virtual void init();    
    virtual void deInit();    
    virtual void update(float fDt);
    virtual void draw();    
protected:
    Anim<Vec2f> mAnimatedPos;
    vector<Vec2f> mAnimDestinations; 
    Circles7WeakPtr mpC7_1,mpC7_2;
protected:
    void initAnimator();
};

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

class BaseCircleApp : public AppBasic {
  public:
	void setup();
	void update();
	void draw();

	void mouseMove( MouseEvent event );
	void mouseDown( MouseEvent event );
	void mouseDrag( MouseEvent event );
	void mouseUp( MouseEvent event );
	void mouseWheel( MouseEvent event );

	void prepareSettings( Settings *settings );	
    
	Vec2f pickPos;
	NodeWeakPtr mCurCircles7;
	NodeWeakPtr mSelectedCircles7;
protected:
    AppStateWeakPtr getSt(string n) {return mStates[n].get();};
 private:
    map<string, AppStatePtr> mStates;
    AppStateWeakPtr mpCurSt;
};

//---------------------------------------------------------------------------

void BaseCircleApp::mouseDown( MouseEvent event ) {
    if (mpCurSt) mpCurSt->mouseDown(event);
}

void BaseCircleApp::mouseDrag( MouseEvent event ) {
    if (mpCurSt) mpCurSt->mouseDrag(event);
}

void BaseCircleApp::mouseUp( MouseEvent event ) {
    if (mpCurSt) mpCurSt->mouseUp(event);
}

void	BaseCircleApp::mouseMove( MouseEvent event ) 
{
    if (mpCurSt) mpCurSt->mouseMove(event);
}

void	BaseCircleApp::mouseWheel( MouseEvent event ) {
     if (mpCurSt) mpCurSt->mouseWheel(event);
}

//---------------------------------------------------------------------------
void BaseCircleApp::prepareSettings( Settings *settings ) 
{
	settings->setWindowSize( 1024, 768 );
	settings->setWindowPos( 32, 96 );
	settings->setFullScreen( false );
	settings->setResizable( true );
	settings->setFrameRate( 60.0f );
}

void BaseCircleApp::setup()
{
    mStates["TWO_CIRCLES_7"] = TwoCircles7StatePtr(new TwoCircles7State());

    string initialState("TWO_CIRCLES_7");
    
    mpCurSt = getSt(initialState);
    mpCurSt->init();
}

void BaseCircleApp::update()
{
    float fDt = 1.f/60.f;
    if (mpCurSt)
        mpCurSt->update(fDt);
}

void BaseCircleApp::draw()
{
    if (mpCurSt)
        mpCurSt->draw();
}


CINDER_APP_BASIC( BaseCircleApp, RendererGl )
