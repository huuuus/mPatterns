#include "Node.h"
#include "Style.h"
#include "Axis.h"
#include "Circle.h"
#include "RootNode.h"
#include "Circles7.h"

namespace mPatterns {
    class NodeMngr {
    public:
        NodeMngr() {};
		~NodeMngr() {delete mpRoot;};
        
        static NodeMngr& instance() {static NodeMngr inst; return inst;};
        static CircleWeakPtr createCircle(Vec2f pos, float r, NodeWeakPtr pParent = mpRoot, PrimitiveStyle* s=0);
		Circles7WeakPtr createCircles7(float radius, bool need7Styles, NodeWeakPtr pParent=mpRoot);
        
        static RootNodeWeakPtr mpRoot;
        
        static void draw(NodeWeakPtr pNode=mpRoot);
        static void updateNodes(NodeWeakPtr pNode = mpRoot, Vec2f curPos = Vec2f(0,0));
		static NodeWeakPtr picking(bool affectSelectables, Vec2f pickPos, NodeWeakPtr pNode = mpRoot);
    };
};

#define NODE_MGR NodeMngr::instance()