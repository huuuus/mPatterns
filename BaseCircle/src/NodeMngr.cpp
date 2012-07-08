#include "BaseCirclePCH.h"
#include "NodeMngr.h"

using namespace mPatterns;

RootNodeWeakPtr NodeMngr::mpRoot(new RootNode());

CircleWeakPtr NodeMngr::createCircle(Vec2f pos, float r, NodeWeakPtr pParent, PrimitiveStyle* s)
{
	CircleWeakPtr pCircle = new Circle(pos,r,pParent);
	pCircle->setStyle(s);
	return pCircle;
}

Circles7WeakPtr NodeMngr::createCircles7(float radius, bool need7Styles) {
	Circles7WeakPtr pC(new Circles7(radius, mpRoot, need7Styles));
	return pC;
}

void NodeMngr::updateNodes(NodeWeakPtr pNode, Vec2f curPos) {
	curPos += pNode->mPos;
	pNode->mPosWorld = curPos;	
	for (const_itNodeList it = pNode->mChilds.begin(); it != pNode->mChilds.end(); ++it)
	{
		updateNodes(it->get(), curPos);
	}
}

void NodeMngr::draw(NodeWeakPtr pNode) {
	pNode->draw();
	for (const_itNodeList it = pNode->mChilds.begin(); it != pNode->mChilds.end(); ++it)
	{
		draw(it->get());
	}
}

CircleWeakPtr Circle::spawnCircleOnAxis(unsigned int axis, float distInRadiusUnits, float radius, PrimitiveStyle* s) 
{
	assert(axis<mAxises.size());		
	Vec2f dir = mAxises[axis]->mDir;
	CircleWeakPtr pC = NODE_MGR.createCircle( dir * distInRadiusUnits * mRadius, radius, this, s);
	pC->setAxis(mAxises[axis]);
	pC->setStyle(s ? s : getStyle());    
	return pC;
}


NodeWeakPtr NodeMngr::picking(bool affectSelectables, Vec2f pickPos, NodeWeakPtr pNode) {
	if (pNode->pick(pickPos))
    {
        if (affectSelectables)
            pNode->notifySelected();
		return pNode;
    }
	for (const_itNodeList it = pNode->mChilds.begin(); it != pNode->mChilds.end(); ++it) {
		NodeWeakPtr res = picking(affectSelectables, pickPos, it->get());
		if (res)
			return res;
	}
	return 0;
}

