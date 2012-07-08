using namespace ci::app;

namespace mPatterns {

    class IMouseHandler {
    public:
        virtual void mouseMove( MouseEvent event ) {};
        virtual void mouseDown( MouseEvent event ) {};
        virtual void mouseDrag( MouseEvent event ) {};
        virtual void mouseUp( MouseEvent event ) {};
        virtual void mouseWheel( MouseEvent event ) {};
    };

};
