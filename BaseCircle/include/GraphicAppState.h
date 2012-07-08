#include "NodeMngr.h"
#include "IMouseHandler.h"
#include "hasPickableNode.h"
#include "AppState.h"

namespace mPatterns {
    
    class GraphicAppState : public AppState, public hasPickableNode {
    public:    
        virtual void init();
        void postUpdate();
        void preDraw();
        void postDraw();
        virtual void mouseDown( MouseEvent event );
        virtual void mouseUp( MouseEvent event );    
        virtual void mouseMove( MouseEvent event );
        virtual void mouseWheel( MouseEvent event ) {};
        
    protected:
        params::InterfaceGl	mParams;
        Color mBgColor;
    };
    
};