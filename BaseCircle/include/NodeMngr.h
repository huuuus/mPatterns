namespace mPatterns {
    class NodeMngr {
    public:
        NodeMngr() {};
        
        static NodeMngr& instance() {static NodeMngr inst; return inst;};
        static CirclePtr createCircle(Vec2f pos, float r, NodeWeakPtr pParent = mpRoot.get(), PrimitiveStyle* s=0);
        
        static RootNodePtr mpRoot;
        
        static void draw(NodePtr pNode=mpRoot);
        static void updateNodes(NodePtr pNode = mpRoot, Vec2f curPos = Vec2f(0,0));	
    };
};

#define NODE_MGR NodeMngr::instance()