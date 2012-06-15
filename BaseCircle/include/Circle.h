namespace mPatterns {
    DECL_WEAK_PTR(Circle)

    class Circle : public Node<Vec2f>, public hasStyle {
        public:
            Circle(Vec2f pos, float r, NodeWeakPtr pParent);
        
            float mRadius;
        
            virtual void draw() const;
            void addAxis(float angle, PrimitiveStyle* s=0);
            void setAxis(AxisWeakPtr pAxis);
        
            void positionAlongAxis(float dist);
		
            vector<AxisWeakPtr> mAxises;
        
            AxisWeakPtr mpAxis;
        
            virtual enum NODE_TYPE getType() {return NT_CIRCLE;};
        
            CircleWeakPtr spawnCircleOnAxis(unsigned int axis, float distInRadiusUnits, float radius, PrimitiveStyle* s);

			virtual bool pick(Vec2f pickPos) const {return false;};
    };
};
