#include "BaseCirclePCH.h"
#include "cinder/params/Params.h"
#include "cinder/gl/gl.h"
#include "GraphicAppState.h"
#include "TwoCircles7State.h"

using namespace mPatterns;
using namespace std;

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
    
protected:
    AppStateWeakPtr getSt(string n) {
        return mStates[n].get();
    };
 private:
    map<string, AppStatePtr> mStates;
    AppStateWeakPtr mpCurSt;
};

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
