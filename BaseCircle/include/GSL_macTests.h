#include <stdio.h>

#include "cinder/rand.h"
#include "cinder/gl/texture.h"
#include "cinder/timer.h"
#include "cinder/gl/Fbo.h"

#include "contextMngr.h"
#include "fitness.h"
#include "timerScope.h"
#include "fboTools.h"

namespace mPatterns {
    DECL_SHARED_PTR(GSL_macTestState)
          
    class GSL_macTestState;
    GSL_macTestState* gSt;

    class GSL_macTestState : public GraphicAppState {
        
    public:
        Circles7* m_pCircle;
        gl::Fbo mFbo;
        gl::Fbo mReferenceFbo;
        unsigned char *mRefData_RGBA;
        bool mFirstTime;       

    public:
        virtual void init() {
            gSt = this;
            GraphicAppState::init();
            
            m_pCircle = new Circles7(20.f, NODE_MGR.mpRoot, false);
            m_pCircle->mPos = _SCREEN_CENTER_VEC2F;
            
            mFbo = createFbo(_SCREEN_SIZE, GL_RGBA8, 4);
            
            mFirstTime = true;
        };
               
        virtual void deInit() {		
        }
      
        void renderToFbo(gl::Fbo &fbo, float posX, float posY, ColorAf c1, ColorAf c2, float r) {
            
            gl::SaveFramebufferBinding bindingSaver;
            bindAndSetViewport(fbo);
            
            gl::clear(Color( 0.0, 0.0, 0.0 ));
            
            gl::enableAdditiveBlending();
                m_pCircle->mPos = Vec2f(posX, posY);
                m_pCircle->changeRadius(r);
            
                vector<PrimitiveStylePtr>& styles = m_pCircle->mStyles;
                for (int i=0;i<2;i++) {
                    ColorAf &c = (i==0) ? c1 : c2;
                    styles[i]->mMainColor = ColorAf(c.r,c.g,c.b,styles[i]->mMainColor.a);
                }

                NODE_MGR.updateNodes(); 
                NODE_MGR.draw();             
            gl::disableAlphaBlending();
		}
               
        float evaluate_fXY_C1_C2_R(Vec2f pos, ColorAf c1, ColorAf c2,float r) {
			int nbPixels = getWindowWidth()*getWindowHeight();
            static unsigned char *curSample = 0;
            if (!curSample)
                curSample = new unsigned char[nbPixels*4];
			float d = 0.f;

            {   TimerScope("renderToFbo");
                renderToFbo(mFbo, pos.x, pos.y, c1, c2, r); }
            
            {   TimerScope("getFboData_RGBA");                
                getFboData_RGBA(mFbo, curSample); }            
            
            {   TimerScope("rate");                
                u32* pRef = (u32*)mRefData_RGBA;
                u32* pSample = (u32*)curSample;
                for (int i=0;i<nbPixels;i++)
                    d += fastFitness_RGBA(pSample[i], pRef[i]); }
            
            return d;
        }

		ColorAf mC1,mC2;
		float mR;

        float evaluate_fXY(Vec2f pos) {
            return evaluate_fXY_C1_C2_R(pos, mC1, mC2, mR);
        }

        void initRefImage() {
            mReferenceFbo = createFbo(_SCREEN_SIZE, GL_RGBA8, 4);
            			
            vector<PrimitiveStylePtr>& styles = m_pCircle->mStyles;
            mC1 = styles[0]->mMainColor;
            mC2 = styles[1]->mMainColor;
            mR = m_pCircle->getCircles()[0]->mRadius;            
            renderToFbo(mReferenceFbo, _SCREEN_CENTERF, mC1, mC2, mR);
            
            mRefData_RGBA = new unsigned char[getWindowWidth()*getWindowHeight()*4];
			getFboData_RGBA(mReferenceFbo, mRefData_RGBA);
        }
        
        virtual void mouseDown( MouseEvent event ) {
			Vec2f clickPos = event.getPos();
			clickPos.y = getWindowHeight()-1-clickPos.y;
        }        

        virtual void update(float fDt) {         
			// if done at init() then fails because of unknown reason in cinder (one frame before fully operative...)
            if (mFirstTime) { 
                mFirstTime = false;								
                initRefImage();                   
				contextMngr::lockMain();
            }
        };
        
        virtual void draw() 
        {
			GraphicAppState::preDraw();
			{
				displayFbo(mReferenceFbo, Color(0.5,0.5,0.5));
                                
				gl::enableAdditiveBlending();
				{
					displayFbo(mFbo);
				}
				gl::disableAlphaBlending();
			}
			GraphicAppState::postDraw(false);

            contextMngr::unlockMain();

/*				if (m_annealingThread) {
					if (m_annealingThread->joinable()) {
						boost::system_time const timeout = boost::get_system_time() + boost::posix_time::milliseconds(0);
						m_annealingThread->timed_join(timeout);
					}
					else {
						printf("**** Thread finished !!!\n");
						delete m_annealingThread;
						m_annealingThread = 0;                    
					}
				}*/

			contextMngr::lockMain();
		}
	};
}