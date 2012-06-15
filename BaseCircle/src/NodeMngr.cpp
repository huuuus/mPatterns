#include "BaseCirclePCH.h"

#include "Node.h"
#include "Style.h"
#include "Axis.h"
#include "Circle.h"
#include "RootNode.h"
#include "NodeMngr.h"

using namespace mPatterns;

RootNodePtr NodeMngr::mpRoot(new RootNode());

CirclePtr NodeMngr::createCircle(Vec2f pos, float r, NodeWeakPtr pParent, PrimitiveStyle* s)
{
	CirclePtr pCircle(new Circle(pos,r,pParent));
	pCircle->setStyle(s);
	pParent->addChild(pCircle);
	return pCircle;
}

void NodeMngr::updateNodes(NodePtr pNode, Vec2f curPos) {
	curPos += pNode->mPos;
	pNode->mPosWorld = curPos;	
	for (const_itNodeList it = pNode->mChilds.begin(); it != pNode->mChilds.end(); ++it)
	{
		updateNodes(*it, curPos);
	}
}

void NodeMngr::draw(NodePtr pNode) {
	pNode->draw();
	for (const_itNodeList it = pNode->mChilds.begin(); it != pNode->mChilds.end(); ++it)
	{
		draw(*it);
	}
}

CirclePtr Circle::spawnCircleOnAxis(unsigned int axis, float distInRadiusUnits, float radius, PrimitiveStyle* s) 
{
	assert(axis<mAxises.size());		
	Vec2f dir = mAxises[axis]->mDir;
	CirclePtr pC = NODE_MGR.createCircle( dir * distInRadiusUnits * mRadius, radius, this, s);
	pC->setAxis(mAxises[axis]);
	pC->setStyle(s ? s : getStyle());
    
	return pC;
}