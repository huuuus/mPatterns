#include "BaseCirclePCH.h"

#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"
#include "cinder/rand.h"

#include "NodeMngr.h"

using namespace mPatterns;
using namespace ci;
using namespace params;
using namespace app;

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

	void		prepareSettings( Settings *settings );	

	params::InterfaceGl	mParams;	
	Color mBgColor;

	NodeWeakPtr mCurCircles7;
	NodeWeakPtr mSelectedCircles7;

	Vec2f pickPos;
};

//---------------------------------------------------------------------------

void BaseCircleApp::mouseDown( MouseEvent event ) {
	if (mCurCircles7) {	
		mSelectedCircles7 = mCurCircles7;
		OutputDebugStringA("whoo\n");
		pickPos = event.getPos();
	}
}

void BaseCircleApp::mouseDrag( MouseEvent event ) {
	if (mSelectedCircles7) {
		mSelectedCircles7->mPos += event.getPos() - pickPos;
		pickPos = event.getPos();
	}
}

void BaseCircleApp::mouseUp( MouseEvent event ) {
	if (mSelectedCircles7) {
		mSelectedCircles7 = 0;
	}
}

void	BaseCircleApp::mouseMove( MouseEvent event ) 
{
	NodeWeakPtr pNode = NODE_MGR.picking(event.getPos(), NODE_MGR.mpRoot);
	mCurCircles7 = pNode; 
}

void	BaseCircleApp::mouseWheel( MouseEvent event ) {
	NodeWeakPtr pNode = mCurCircles7;
	if (pNode) {
		float wheelInc = event.getWheelIncrement();
		if (pNode->getType() == NT_CIRCLES7) {
			Circles7WeakPtr p = static_cast<Circles7WeakPtr>(pNode);
			for (int i=0;i<2;i++) {
				Color c;

				float H = randFloat(0.0f,1.f);
				float S = randFloat(0.5f,1.f);
				float V = randFloat(0.25f,1.f);
				c.set(cinder::CM_HSV, Vec3f(H,S,V));

				ColorAf &curC = p->mStyles[i]->mMainColor;
				curC = ColorAf(c.r, c.g, c.b, curC.a);
			}
		}
	}
}

//---------------------------------------------------------------------------
void BaseCircleApp::prepareSettings( Settings *settings ) 
{
	settings->setWindowSize( 1024, 768 );
	settings->setWindowPos( 32, 96 );
	settings->setFullScreen( false );
	settings->setResizable( true );
	settings->setFrameRate( 60.0f );

	mParams = params::InterfaceGl( "Global", Vec2i( 0, 0 ) );
	mBgColor.set(cinder::CM_RGB, Vec3f(0.f, 0.f, 0.f));
	mParams.addParam( "bg", &mBgColor, "" );
//	mDistFromCenter = 64.f;
//	mParams.addParam( "mDistFromCenter", &mDistFromCenter, "min=-100.0 max=100.0 step=1.0 keyIncr=d keyDecr=D" );
}

void BaseCircleApp::setup()
{
	mCurCircles7 = 0;

	Circles7WeakPtr pC7_1 = NODE_MGR.createCircles7(48.f, false);
	Vec2f pos = getWindowCenter();
	//pos.x = 128 /*+ 256*/;
	pC7_1->mPos = pos;

	Circles7WeakPtr pC7_2 = NODE_MGR.createCircles7(32.f, false);
	pos = getWindowCenter();
	pos.x += 256;
	pC7_2->mPos = pos;

	pC7_1->spawnParameters();
	pC7_2->spawnParameters();
}

void BaseCircleApp::update()
{
//	float off = mDistFromCenter; //32.f + (sin(getElapsedSeconds()*1.75f)*0.5f+0.5f) * 128.f;

/*	vector<CirclePtr>::iterator it;
	for (it=CirclesPositive.begin();it!=CirclesPositive.end();++it) {
		(*it)->positionAlongAxis(off);
	}
	for (it=CirclesNegative.begin();it!=CirclesNegative.end();++it) {
		(*it)->positionAlongAxis(-off);
	}*/

	NODE_MGR.updateNodes();
}

void BaseCircleApp::draw()
{
	// clear out the window with black
	gl::clear( Color( mBgColor ) );	
	gl::enableAdditiveBlending();
	NODE_MGR.draw();
	params::InterfaceGl::draw();
}

CINDER_APP_BASIC( BaseCircleApp, RendererGl )
