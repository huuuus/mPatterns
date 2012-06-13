namespace mPatterns {
    DECL_SHARED_PTR(PrimitiveStyle)
    
    class hasStyle {
    public:
        hasStyle() {
            mStyle = 0;
        };
        
        PrimitiveStyle* mStyle;	
        
        PrimitiveStyle* getStyle() const {return mStyle;};
        void setStyle(PrimitiveStyle* s) {mStyle=s;};
    };
    
    class PrimitiveStyle {
    public:
        PrimitiveStyle() {
            mStroke = 1;
        };
        
        ColorAf mMainColor;
        ColorAf mSecondColor;
        
        float mStroke; //CIRCLE_DEFAULT_STROKE
    };

    /*enum FX_STYLE_FLAGS {
     GLOW_ADDITIVE,
     };
     */
};
