#include "BaseCirclePCH.h"
#include "Node.h"
#include "Style.h"
#include "Axis.h"
#include "Circle.h"

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
	AxisWeakPtr a(new Axis(mPos, angle, this));
	a->setStyle(s);
	mAxises.push_back(a);
};

void Circle::setAxis(AxisWeakPtr pAxis) {
	mpAxis = pAxis;
}

void Circle::positionAlongAxis(float dist) {
	assert(mpAxis);
	mPos = mpAxis->mDir*dist;
}

