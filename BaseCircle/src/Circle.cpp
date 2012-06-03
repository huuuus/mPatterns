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
	gl::color( mColor );
	glLineWidth( CIRCLE_DEFAULT_STROKE );
		gl::drawStrokedCircle( mPosWorld, mRadius );
	glLineWidth( 1.0f );

	std::vector<Axis*>::const_iterator it;
	for (it = mAxises.begin(); it != mAxises.end(); ++it) {
		(*it)->draw();
	}
};

void Circle::addAxis(float angle) {
	mAxises.push_back(new Axis(mPos, angle, this));
};

CirclePtr CircleMngr::spawnChildOnCircleAxis(CirclePtr pCircle, unsigned int axis, float distInRadiusUnits, float radius) {
	assert(axis<pCircle->mAxises.size());		
	Vec2f dir = pCircle->mAxises[axis]->mDir;
	CirclePtr pC = CIRCLE_MGR.createCircle(	/*pCircle->mPos +*/ dir * distInRadiusUnits * pCircle->mRadius, 
											pCircle->mColor, radius,
											pCircle);
	pC->setAxis(pCircle->mAxises[axis]);
	return pC;
}

void Circle::setAxis(const Axis* pAxis) {
	mpAxis = pAxis;
}

void Circle::positionAlongAxis(float dist) {
	assert(mpAxis);
	mPos = mpAxis->mDir*dist;
}

CirclePtr CircleMngr::createCircle(Vec2f pos, Color c, float r, NodePtr pParent) 
{
	CirclePtr pCircle(new Circle(pos,c,r,pParent.get()));
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