namespace mPatterns {
	DECL_WEAK_SHARED_PTR_EX(Node, Vec2f)
    
    typedef vector<NodePtr>::const_iterator const_itNodeVec;
    typedef list<NodePtr>::const_iterator const_itNodeList;

    enum NODE_TYPE {
        NT_ROOT,
        NT_CIRCLE,
		NT_CIRCLES7,
        NT_AXIS
    };

	class Pickable {
		public:
			virtual bool pick(Vec2f pickPos) const = 0;
	};

    template<class CPos>
    class Node : public Pickable {
    public:
        Node(CPos p, NodeWeakPtr pParent) : mPos(p) , mpParent(pParent) 
        {
			if (pParent)
				pParent->addChild(NodePtr(this));
        };
        
        CPos mPos;
        NodeWeakPtr mpParent;
        list<NodePtr> mChilds;
        
        virtual enum NODE_TYPE getType() = 0;
        virtual void draw() const = 0;		
        
        void addChild(NodePtr pChild) {mChilds.push_back(pChild);};
        
        CPos mPosWorld;

		bool isInRadius(CPos pos, float radius) const {
			CPos diff = mPosWorld - pos;
			float l = diff.length();
			return l<radius;
		};

		virtual void spawnParameters() {};
    };
};
