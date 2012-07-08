#include "BaseCirclePCH.h"
#include "cinder/gl/gl.h"
#include "NodeMngr.h"
#include "hasPickableNode.h"

using namespace mPatterns;

void hasPickableNode::mouseUp( MouseEvent event )
{
    mCurPickedNode = 0;    
}

void hasPickableNode::mouseMove( MouseEvent event )
{
    mCurHighlightedNode = NODE_MGR.picking(false, event.getPos(), NODE_MGR.mpRoot);
}

void hasPickableNode::mouseDown( MouseEvent event )
{
    mCurPickedNode = NODE_MGR.picking(true, event.getPos(), NODE_MGR.mpRoot);    
}