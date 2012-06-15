#include "BaseCirclePCH.h"
#include "NodeMngr.h"
#include "cinder/params/Params.h"
#include <boost/format.hpp>

using namespace mPatterns;
using namespace std;
using namespace ci;
using namespace cinder::params;
using boost::format;

Circles7::Circles7(float radius, NodeWeakPtr pParent, bool need7Styles) : 
	Node(Vec2f(0,0), pParent),
	mpParams(0)
{	
	for (int i=0;i<(need7Styles ? 7 : 2);i++) {
		// circle styles
		PrimitiveStyle *s = new PrimitiveStyle;
		s->mMainColor = (i==0) ? ColorA8u(255,0,0,113) : ColorA8u(255,0,255,113);		
		mStyles.push_back(s);
		// axises styles
		s = new PrimitiveStyle;
		s->mMainColor = (i==0) ? ColorA8u(174,119,94,24) : ColorA8u(174,119,94,24);
		mAxisesStyles.push_back(s);
	}

	// main circle	
	mCircles.push_back(
		NODE_MGR.createCircle(Vec2f(0,0), radius, this, mStyles[0]));
	CircleWeakPtr pMain = mCircles[0];

	// add 3 axises to main circle, and 2 circles per axis
	const float axisAngles[3] = {M_PIf/2.f, M_PIf/6.f, M_PIf - M_PIf/6.f};
	const float D_CENTER = 1.f;
	for (int i=0;i<3;i++)
	{
		pMain->addAxis(axisAngles[i], mAxisesStyles[0]);
		for (int sign=-1;sign<2;sign+=2)
			mCircles.push_back(
				pMain->spawnCircleOnAxis(i, sign * D_CENTER, pMain->mRadius, mStyles[1]));
	}
}

const int TOOLBOX_W = 200;
const int TOOLBOX_H = (int)(200*0.9);

void Circles7::spawnParameters() {	
	if (!mpParams) {
		unsigned int id = (unsigned int)(this);
		string sId = (format("7CIRCLE_%d") % id).str();
		mpParams = new InterfaceGl(sId, Vec2i(TOOLBOX_W,TOOLBOX_H));	

		mpParams->addParam( "CENTRAL", &mStyles[0]->mMainColor, "" );
		mpParams->addParam( "AXISES", &mAxisesStyles[0]->mMainColor, "" );
		mpParams->addParam( "SUB", &mStyles[1]->mMainColor, "" );
	}		
//	pParams.addParam( "SUB_AXISES_" + sId, &mAxisesStyles[1]->mMainColor, "" );
}

void Circles7::draw() const {
	//TwDefine(" mybar position='200 40' "); // move bar to position (200, 40)
	mpParams->setOptions("",(format("position='%d %d'") % (mPosWorld.x-TOOLBOX_W/2) % (mPosWorld.y-TOOLBOX_H-mCircles[0]->mRadius*2.f-16.f)).str());
}