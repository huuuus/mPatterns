#include "stdafx.h"
#include "circle.h"

class BaseCircleApp : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();

	void		prepareSettings( Settings *settings );	
};

//---------------------------------------------------------------------------
void BaseCircleApp::prepareSettings( Settings *settings ) 
{
	settings->setWindowSize( 512, 512 );
	settings->setWindowPos( 32, 96 );
	settings->setFullScreen( false );
	settings->setResizable( true );
	settings->setFrameRate( 60.0f );
}

std::vector<CirclePtr> CirclesPositive;
std::vector<CirclePtr> CirclesNegative;

void BaseCircleApp::setup()
{
	Vec2f pos = getWindowCenter();

	CirclePtr pC = CIRCLE_MGR.createCircle(pos, Color::white(), 64.f);

	const float D_CENTER = 1.f;

	pC->addAxis(M_PI/2.f); // 0
	CirclesPositive.push_back(CIRCLE_MGR.spawnChildOnCircleAxis(pC, 0, D_CENTER, pC->mRadius));
	CirclesNegative.push_back(CIRCLE_MGR.spawnChildOnCircleAxis(pC, 0, -D_CENTER, pC->mRadius));

	pC->addAxis(M_PI/6.f/*-M_PI/3.f*/); // 1
	CirclesPositive.push_back(CIRCLE_MGR.spawnChildOnCircleAxis(pC, 1, D_CENTER, pC->mRadius));
	CirclesNegative.push_back(CIRCLE_MGR.spawnChildOnCircleAxis(pC, 1, -D_CENTER, pC->mRadius));

	pC->addAxis(M_PI - M_PI/6.f); // 2
	CirclesPositive.push_back(CIRCLE_MGR.spawnChildOnCircleAxis(pC, 2, D_CENTER, pC->mRadius));
	CirclesNegative.push_back(CIRCLE_MGR.spawnChildOnCircleAxis(pC, 2, -D_CENTER, pC->mRadius));

	//positionAlongAxis
	std::vector<CirclePtr>::iterator it;
	for (it=CirclesPositive.begin();it!=CirclesPositive.end();++it) 
	{
		CirclePtr pCircle = *it; //CirclesPositive.begin();
		for (int i=0;i<6*2;i++) {		
			pCircle->addAxis(i*M_PI/6.f);
			CIRCLE_MGR.spawnChildOnCircleAxis(pCircle, i, D_CENTER, Rand::randFloat(pC->mRadius/2.f, pC->mRadius/4.f));
		}
	}
}

void BaseCircleApp::mouseDown( MouseEvent event )
{
}

void BaseCircleApp::update()
{
	float off = 32.f + (sin(getElapsedSeconds()*1.75f)*0.5f+0.5f) * 128.f;

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
	gl::clear( Color( 0.5f, 0.5f, 0.5f ) );
	
	CIRCLE_MGR.draw();
}

CINDER_APP_BASIC( BaseCircleApp, RendererGl )
