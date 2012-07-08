#include "cinder/Timeline.h"

namespace mPatterns {
    
    DECL_SHARED_PTR(TwoCircles7State)
    class TwoCircles7State : public GraphicAppState {
    public:
        virtual void init();    
        virtual void deInit();    
        virtual void update(float fDt);
        virtual void draw();
        virtual void mouseWheel( MouseEvent event );
    protected:
        Anim<Vec2f> mAnimatedPos;
        vector<Vec2f> mAnimDestinations; 
        Circles7WeakPtr mpC7_1,mpC7_2;
    protected:
        void initAnimator();
    };
       
};