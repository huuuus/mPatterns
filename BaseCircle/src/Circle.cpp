#include "stdafx.h"
#include "Circle.h"

Circle::Circle(Vec2f pos, Color c, float r, NodeConstPtr pParent) :
	Node(pos, pParent)	
{
	mpAxis = 0;
	mPos = pos;
	mColor = c;
	mRadius = r;		
};

void Circle::draw() const {
	if (mStyle)
		gl::color( 1,0,0,0.1f/*mStyle->mMainColor*/ );
	else
		gl::color( Color(255,255,255) );

	glLineWidth( CIRCLE_DEFAULT_STROKE );
		gl::drawStrokedCircle( mPosWorld, mRadius );
	glLineWidth( 1.0f );
};

void Circle::addAxis(float angle, Style* s) {
	AxisPtr a(new Axis(mPos, angle, this));
	a->setStyle(s);
	mAxises.push_back(a.get());
	mChilds.push_back(a);
};

CirclePtr CircleMngr::spawnChildOnCircleAxis(CirclePtr pCircle, unsigned int axis, float distInRadiusUnits, float radius, Style* s) {
	assert(axis<pCircle->mAxises.size());		
	Vec2f dir = pCircle->mAxises[axis]->mDir;
	CirclePtr pC = CIRCLE_MGR.createCircle( dir * distInRadiusUnits * pCircle->mRadius, 
											pCircle->mColor, radius,
											pCircle,
											s);
	pC->setAxis(pCircle->mAxises[axis]);
	pC->setStyle(s ? s : pCircle->getStyle());

	return pC;
}

void Circle::setAxis(const Axis* pAxis) {
	mpAxis = pAxis;
}

void Circle::positionAlongAxis(float dist) {
	assert(mpAxis);
	mPos = mpAxis->mDir*dist;
}

CirclePtr CircleMngr::createCircle(Vec2f pos, Color c, float r, NodePtr pParent, Style* s) 
{
	CirclePtr pCircle(new Circle(pos,c,r,pParent.get()));
	pCircle->setStyle(s);
	pParent->addChild(pCircle);
	return pCircle;
};

RootPtr CircleMngr::mpRoot(new Root());

void CircleMngr::updateNodes(NodePtr pNode, Vec2f curPos) {
	curPos += pNode->mPos;
	pNode->mPosWorld = curPos;	
	for (const_itNodeList it = pNode->mChilds.begin(); it != pNode->mChilds.end(); ++it)
	{
		updateNodes(*it, curPos);
	}
}

void CircleMngr::draw(NodePtr pNode) {
	pNode->draw();
	for (const_itNodeList it = pNode->mChilds.begin(); it != pNode->mChilds.end(); ++it)
	{
		draw(*it);
	}
}