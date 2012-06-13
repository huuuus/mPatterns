#include "stdafx.h"

#include "Node.h"
#include "Style.h"
#include "Axis.h"
#include "Circle.h"
#include "RootNode.h"
#include "NodeMngr.h"

using namespace mPatterns;

Axis::Axis(Vec2f pos, float angle, NodeWeakPtr pParent) : 
    Node(pos, pParent)
{
    mDir.set(cos(angle),-sin(angle));
}

void Axis::draw() const 
{
    if (getStyle())
        gl::color( getStyle()->mMainColor );
    else
        gl::color( Color(255,0,0) );
    
    glLineWidth( getStyle()->mStroke );
    const float LONG_DIST = 5000.f;
    gl::drawLine(	mpParent->mPosWorld - mDir*LONG_DIST, 
                 mpParent->mPosWorld + mDir*LONG_DIST);
    glLineWidth( 1.0f );
}
        
