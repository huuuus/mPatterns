namespace mPatterns {
    DECL_SHARED_PTR(RootNode)
    
    class RootNode : public Node<Vec2f> {
    public:
        RootNode() : Node<Vec2f>(Vec2f(0,0), NULL) {};
        virtual void draw() const {};
        virtual enum NODE_TYPE getType() {return NT_ROOT;};
        virtual PrimitiveStyle* getStyle() const {return 0;};
    };
};