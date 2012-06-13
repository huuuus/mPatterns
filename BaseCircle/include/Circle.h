namespace mPatterns {
    DECL_SHARED_PTR(Circle)

    class Circle : public Node<Vec2f>, public hasStyle {
        public:
            Circle(Vec2f pos, float r, NodeWeakPtr pParent);
        
            float mRadius;
        
            virtual void draw() const;
            void addAxis(float angle, PrimitiveStyle* s=0);
            void setAxis(const Axis* pAxis);
        
            void positionAlongAxis(float dist);
		
            vector<Axis*> mAxises;
        
            const Axis* mpAxis;
        
            virtual enum NODE_TYPE getType() {return NT_CIRCLE;};
        
            CirclePtr spawnCircleOnAxis(unsigned int axis, float distInRadiusUnits, float radius, PrimitiveStyle* s);
    };
};
