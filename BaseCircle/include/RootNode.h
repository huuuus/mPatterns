namespace mPatterns {
    DECL_WEAK_PTR(RootNode)
    
    class RootNode : public NodeNotSelectable {
    public:
        RootNode() : NodeNotSelectable(Vec2f(0,0), NULL) {};
        virtual void draw() const {};
        virtual enum NODE_TYPE getType() {return NT_ROOT;};
        virtual PrimitiveStyle* getStyle() const {return 0;};
		virtual bool pick(Vec2f pickPos) const {return false;};
    };
};
