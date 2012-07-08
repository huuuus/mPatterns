using namespace app;

namespace mPatterns {

    class hasPickableNode {
    protected:
        hasPickableNode() {
            mCurPickedNode = 0;
            mCurHighlightedNode = 0;
        };
        virtual void mouseDown( MouseEvent event );
        virtual void mouseUp( MouseEvent event );
        virtual void mouseMove( MouseEvent event );
        NodeWeakPtr mCurPickedNode;
        NodeWeakPtr mCurHighlightedNode;
    };

};