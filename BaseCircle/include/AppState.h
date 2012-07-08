namespace mPatterns {
    
    DECL_SHARED_PTR(AppState)
    DECL_WEAK_PTR(AppState)
    class AppState : public IMouseHandler {
    public:
        virtual void init() = 0;    
        virtual void deInit() = 0;    
        virtual void update(float fDt) = 0;
        virtual void draw() = 0;
    };
    
};
