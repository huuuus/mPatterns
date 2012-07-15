#include "cinder/rand.h"

namespace mPatterns {

    class Growthable {
    public:
        virtual float getRadius() = 0;
        virtual bool childAtAngle(float childRadius, float angleRad) = 0;
        virtual void randomize(int channelID, float strenght) = 0;        
    };

    DECL_SHARED_PTR(GrowthableCircle7);
    DECL_WEAK_PTR(GrowthableCircle7);
    class GrowthableCircle7 : public Growthable, public Circles7 {
    public:
        GrowthableCircle7(float radius, NodeWeakPtr pParent) : 
            Circles7(radius, pParent, false)
        {
            
        };
        
        virtual float getRadius() {
            vector<CircleWeakPtr>& pCircles = getCircles();
            return pCircles[0]->mRadius + pCircles[1]->mRadius;            
        };
        
        virtual bool childAtAngle(float childRadius, float angleRad) {            
            GrowthableCircle7WeakPtr pC7(new GrowthableCircle7(childRadius, this));
            //= NODE_MGR.createCircles7(childRadius, false, this);
            Vec2f pos = mPos;
            Vec2f dir(cos(angleRad), sin(angleRad));
            pC7->mPos = dir * (childRadius*2.f + getRadius());
            pC7->randomize(0,0);
            
            return true;
        };
        
        virtual void randomize(int channelID, float strenght) {
            for (int i=0;i<2;i++) {
                Color c;
                
                ColorAf &curC = mStyles[i]->mMainColor;
                Vec3f hsv;
                hsv = rgbToHSV( curC );
                
                float max = 1.f*0.55f;
                hsv.x += randFloat(-max, max);
                hsv.y += randFloat(-max, max);
                hsv.z = randFloat(0.25f, 0.45f);
                c.set(cinder::CM_HSV, hsv);
                
                curC = ColorAf(c.r, c.g, c.b, curC.a);
            }
        };
    };
    
    DECL_SHARED_PTR(GrowthState)
    class GrowthState : public GraphicAppState {
    public:
        virtual void init() {
            GraphicAppState::init();
            
            mpRootCrystal = new GrowthableCircle7(20.f, NODE_MGR.mpRoot);
            mpRootCrystal->mPos = Vec2f(getWindowWidth()/2,getWindowHeight()/2);
            //mStyles
            
            for (int i=0;i<48;i++) {
                mpRootCrystal->childAtAngle(Rand::randFloat(4,72), Rand::randFloat(0,2.f*M_PIf));
            }
        };
        
        virtual void deInit() {
            
        };
        
        virtual void update(float fDt) {
            GraphicAppState::postUpdate();
        };
        
        virtual void draw() {
            GraphicAppState::preDraw();
            gl::enableAdditiveBlending();
            GraphicAppState::postDraw();            
        };
        
        //virtual void mouseWheel( MouseEvent event );
        
        GrowthableCircle7WeakPtr mpRootCrystal;
    };
    
};