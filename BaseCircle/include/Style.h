namespace mPatterns {
	
	const float DEFAULT_STROKE = 30;
	const cinder::ColorAf DEFAULT_COLOR1 = ColorAf(1.0,0.0,1.0,1.0);
	const cinder::ColorAf DEFAULT_COLOR2 = ColorAf(0.5,0.5,0.5,1.0);

    DECL_WEAK_PTR(PrimitiveStyle)
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
            mStroke = DEFAULT_STROKE;
		    mMainColor = DEFAULT_COLOR1;
	        mSecondColor = DEFAULT_COLOR2;
        };
        
        cinder::ColorAf mMainColor;
        cinder::ColorAf mSecondColor;
        
        float mStroke;
    };
    
    //----------------------------------------------------------

};
