namespace mPatterns {
    DECL_SHARED_PTR(Axis)
    
    class Axis : public Node<Vec2f>, public hasStyle {
    public:
        Axis(Vec2f pos, float angle, NodeWeakPtr pParent);        
        virtual void draw() const;
        Vec2f mDir;
        virtual enum NODE_TYPE getType() {return NT_AXIS;};
    };
};