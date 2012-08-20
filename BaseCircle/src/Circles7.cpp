#include "BaseCirclePCH.h"
#include "NodeMngr.h"
#include "cinder/params/Params.h"

using namespace mPatterns;
using namespace std;
using namespace app;
using namespace cinder::params;

const float axisAngles[3] = {M_PIf/2.f, M_PIf/6.f, M_PIf - M_PIf/6.f};
const float D_CENTER = 1.f;

Circles7::Circles7(float radius, NodeWeakPtr pParent, bool need7Styles) : 
	Node<Vec2f>(Vec2f(0,0), pParent)
{	
	for (int i=0;i<(need7Styles ? 7 : 2);i++) {
		// circle styles
		PrimitiveStyle *s = new PrimitiveStyle;
		s->mMainColor = (i==0) ? ColorA8u(255,0,0,113) : ColorA8u(255,0,255,113);
		s->mStroke = math<int>::clamp(radius/8+2,1,1000);
        
		mStyles.push_back(PrimitiveStylePtr(s));
		// axises styles
		s = new PrimitiveStyle;
		s->mMainColor = (i==0) ? ColorA8u(174,119,94,24) : ColorA8u(174,119,94,24);
		mAxisesStyles.push_back(PrimitiveStylePtr(s));
	}

	// main circle	
	mCircles.push_back(
		NODE_MGR.createCircle(Vec2f(0,0), radius, this, mStyles[0].get()));
	CircleWeakPtr pMain = mCircles[0];

	// add 3 axises to main circle, and 2 circles per axis
	for (int i=0;i<3;i++)
	{
		pMain->addAxis(axisAngles[i], mAxisesStyles[0].get());
		for (int sign=-1;sign<2;sign+=2)
			mCircles.push_back(
				pMain->spawnCircleOnAxis(i, sign * D_CENTER, pMain->mRadius, mStyles[1].get()));
	}
}

void Circles7::changeRadius(float r) {
    vector<CircleWeakPtr>::iterator it = mCircles.begin();
    (*it)->mRadius = r;
    it++;
    int cur = 0;
    for (;it!=mCircles.end();++it) {
        (*it)->mRadius = r;
        (*it)->positionAlongAxis(((cur&1) ? 1.f : -1.f)*r*D_CENTER);
        cur++;
    }
}

const int TOOLBOX_W = 200;
const int TOOLBOX_H = (int)(200*0.9);

void Circles7::spawnParameters() {	
	if (!mpParams.get()) {
		unsigned long id = (unsigned long)(this);
		string sId = (format("7CIRCLE_%d") % id).str();
		mpParams.reset(new InterfaceGl(sId, Vec2i(TOOLBOX_W,TOOLBOX_H)));

		mpParams->addParam( "CENTRAL", &mStyles[0]->mMainColor, "" );
		mpParams->addParam( "AXISES", &mAxisesStyles[0]->mMainColor, "" );
		mpParams->addParam( "SUB", &mStyles[1]->mMainColor, "" );
        //	pParams.addParam( "SUB_AXISES_" + sId, &mAxisesStyles[1]->mMainColor, "" );
	}
    mpParams->show();
}

void Circles7::draw() const {
    if (mpParams.get()) {
        float newX = mPosWorld.x-TOOLBOX_W/2;
        float newY = mPosWorld.y-TOOLBOX_H-mCircles[0]->mRadius*2.f-16.f;
        
        std::string prm = (format("position='%d %d'") % 
						  (math<int>::clamp(newX,0,getWindowWidth())) % 
						  (math<int>::clamp(newY,0,getWindowHeight()))).str();
        
        DEBUG_MSG(L"%S\n",prm.c_str());
        
		mpParams->setOptions("", std::string(prm.c_str()));
    }
}

void Circles7::onSelected()
{
    spawnParameters();   
}

void Circles7::onUnselected()
{
    if (mpParams)
        mpParams->hide();
}
