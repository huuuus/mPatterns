#include "stdafx.h"
#include "Node.h"
#include "Style.h"
#include "Axis.h"
#include "Circle.h"
#include "RootNode.h"
#include "NodeMngr.h"

using namespace mPatterns;

Circle::Circle(Vec2f pos, float r, NodeWeakPtr pParent) :
	Node(pos, pParent)	
{
	mpAxis = 0;
	mPos = pos;
	mRadius = r;
};

void Circle::draw() const {
	if (mStyle)
		gl::color( mStyle->mMainColor );
	else
		gl::color( ColorA(255,255,255,255) );

	glLineWidth( mStyle->mStroke );
		gl::drawStrokedCircle( mPosWorld, mRadius );
	glLineWidth( 1.0f );
};

void Circle::addAxis(float angle, PrimitiveStyle* s) {
	AxisPtr a(new Axis(mPos, angle, this));
	a->setStyle(s);
	mAxises.push_back(a.get());
	mChilds.push_back(a);
};

CirclePtr Circle::spawnCircleOnAxis(unsigned int axis, float distInRadiusUnits, float radius, PrimitiveStyle* s) 
{
	assert(axis<mAxises.size());		
	Vec2f dir = mAxises[axis]->mDir;
	CirclePtr pC = NODE_MGR.createCircle( dir * distInRadiusUnits * mRadius, radius, this, s);
	pC->setAxis(mAxises[axis]);
	pC->setStyle(s ? s : getStyle());
    
	return pC;
}

void Circle::setAxis(const Axis* pAxis) {
	mpAxis = pAxis;
}

void Circle::positionAlongAxis(float dist) {
	assert(mpAxis);
	mPos = mpAxis->mDir*dist;
}

