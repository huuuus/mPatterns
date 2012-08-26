#include "NodeMngr.h"
#include "IMouseHandler.h"
#include "hasPickableNode.h"
#include "AppState.h"

#define _SCREEN_SIZE getWindowWidth(),getWindowHeight()
#define _SCREEN_CENTER getWindowWidth()/2,getWindowHeight()/2
#define _SCREEN_CENTER_VEC2F Vec2f(getWindowWidth()/2,getWindowHeight()/2)

namespace mPatterns {
    
    class GraphicAppState : public AppState, public hasPickableNode {
    public:    
        virtual void init();
        void postUpdate();
        void preDraw();
        void postDraw(bool drawScene=true);
        virtual void mouseDown( MouseEvent event );
        virtual void mouseUp( MouseEvent event );    
        virtual void mouseMove( MouseEvent event );
        virtual void mouseWheel( MouseEvent event ) {};                
        
    protected:
        params::InterfaceGl	mParams;
        Color mBgColor;
    };
    
};