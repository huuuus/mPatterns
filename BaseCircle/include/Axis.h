namespace mPatterns {
    DECL_WEAK_PTR(Axis)
    
    class Axis : public NodeNotSelectable, public hasStyle {
    public:
        Axis(Vec2f pos, float angle, NodeWeakPtr pParent);

		virtual ~Axis() {
			DEBUG_MSG(L"delete a Axis %x\n",(long)(this));
		};

        virtual void draw() const;
        Vec2f mDir;
        virtual enum NODE_TYPE getType() {return NT_AXIS;};

		virtual bool pick(Vec2f pickPos) const {return false;};
    };
};