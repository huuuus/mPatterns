#include "stdafx.h"
#include "circle.h"
#include "cinder/params/Params.h"

using namespace mPatterns;

class BaseCircleApp : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();

	void		prepareSettings( Settings *settings );	

	params::InterfaceGl	mParams;	

	StylePtr mStyleCircles;
	StylePtr mStyleAxises;

	float mDistFromCenter;
	Color mBgColor;	
};

//---------------------------------------------------------------------------
void BaseCircleApp::prepareSettings( Settings *settings ) 
{
	settings->setWindowSize( 128 + 512, 512 );
	settings->setWindowPos( 32, 96 );
	settings->setFullScreen( false );
	settings->setResizable( true );
	settings->setFrameRate( 60.0f );

	mParams = params::InterfaceGl( "MCircles", Vec2i( 0, 0 ) );
//	mParams.addSeparator();

	mBgColor.set(cinder::CM_RGB, Vec3f(0.f, 0.f, 0.f));
	mParams.addParam( "bg", &mBgColor, "" );

	mDistFromCenter = 64.f;
	mParams.addParam( "mDistFromCenter", &mDistFromCenter, "min=-100.0 max=100.0 step=1.0 keyIncr=d keyDecr=D" );
}

std::vector<CirclePtr> CirclesPositive;
std::vector<CirclePtr> CirclesNegative;

void BaseCircleApp::setup()
{
	// - styles
	mStyleCircles.reset(new PrimitiveStyle);
	mStyleCircles->mMainColor = ColorA8u(255,0,0,113);
	PrimitiveStyle *sC = mStyleCircles.get();
	mParams.addParam( "Circles", &sC->mMainColor, "" );

	mStyleAxises.reset(new PrimitiveStyle);
	mStyleAxises->mMainColor = ColorA8u(174,119,94,24);
	PrimitiveStyle *sA = mStyleAxises.get();
	mParams.addParam( "Axises", &sA->mMainColor, "" );
	
	// - create main six circles
	Vec2f pos = getWindowCenter();
	pos.x = 128 + 256;

	CirclePtr pC = CIRCLE_MGR.createCircle(pos, Color::white(), 64.f, CIRCLE_MGR.mpRoot, sC);

	float axisAngles[3] = {M_PI/2.f, M_PI/6.f, M_PI - M_PI/6.f};
	const float D_CENTER = 1.f;
	for (int i=0;i<3;i++) 
	{
		pC->addAxis(axisAngles[i], sA); // 0
		CirclesPositive.push_back(CIRCLE_MGR.spawnChildOnCircleAxis(pC, i, D_CENTER, pC->mRadius, sC));
		CirclesNegative.push_back(CIRCLE_MGR.spawnChildOnCircleAxis(pC, i, -D_CENTER, pC->mRadius, sC));
	}

	// - create sub circles / sub axises
	for (int i=0;i<2;i++) {
		std::vector<CirclePtr> cur = (i==0) ? CirclesNegative: CirclesPositive;
		std::vector<CirclePtr>::iterator it;
		for (it=cur.begin();it!=cur.end();++it) 
		{
			CirclePtr pCircle = *it; //CirclesPositive.begin();
			for (int i=0;i<6*2;i++) {		
				pCircle->addAxis(i*M_PI/6.f, sA);
				//CIRCLE_MGR.spawnChildOnCircleAxis(pCircle, i, D_CENTER, Rand::randFloat(pC->mRadius/2.f, pC->mRadius/4.f));
			}
		}
	}
}

void BaseCircleApp::mouseDown( MouseEvent event )
{
}

void BaseCircleApp::update()
{
	float off = mDistFromCenter; //32.f + (sin(getElapsedSeconds()*1.75f)*0.5f+0.5f) * 128.f;

	std::vector<CirclePtr>::iterator it;
	for (it=CirclesPositive.begin();it!=CirclesPositive.end();++it) {
		(*it)->positionAlongAxis(off);
	}
	for (it=CirclesNegative.begin();it!=CirclesNegative.end();++it) {
		(*it)->positionAlongAxis(-off);
	}

	CIRCLE_MGR.updateNodes();
}

void BaseCircleApp::draw()
{
	// clear out the window with black
	gl::clear( Color( mBgColor ) );	
	gl::enableAdditiveBlending();
	CIRCLE_MGR.draw();
	params::InterfaceGl::draw();
}

CINDER_APP_BASIC( BaseCircleApp, RendererGl )
