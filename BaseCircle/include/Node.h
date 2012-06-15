namespace mPatterns {
	using namespace ci;
	using namespace std;

    DECL_SHARED_PTR_EX(Node, Vec2f)
    
    typedef vector<NodePtr>::const_iterator const_itNodeVec;
    typedef list<NodePtr>::const_iterator const_itNodeList;

    enum NODE_TYPE {
        NT_ROOT,
        NT_CIRCLE,
        NT_AXIS
    };

    template<class CPos>
    class Node {
    public:
        Node(CPos p, NodeWeakPtr pParent) : mPos(p) , mpParent(pParent) 
        {
        };
        
        CPos mPos;
        NodeWeakPtr mpParent;
        list<NodePtr> mChilds;
        
        virtual enum NODE_TYPE getType() = 0;
        virtual void draw() const = 0;
        
        void addChild(NodePtr pChild) {mChilds.push_back(pChild);};
        
        CPos mPosWorld;
    };
};
