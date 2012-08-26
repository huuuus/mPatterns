#include <stdio.h>

#include "cinder/rand.h"
#include "cinder/gl/texture.h"
#include "cinder/timer.h"
#include "cinder/gl/Fbo.h"

#include "contextMngr.h"

namespace mPatterns {
    DECL_SHARED_PTR(GSL_macTestState)
    
    extern double my_fXY (const gsl_vector *v, void *params);
    extern double my_fXY_C1_C2 (const gsl_vector *v, void *params);
    
    enum EVAL_MODE {
        EVAL_LUM,
        EVAL_TONE,            
    };
    
    class GSL_macTestState;
    GSL_macTestState* gSt;        

    class GSL_macTestState : public GraphicAppState {
        
    public:
        Circles7* m_pCircle;
        gl::Fbo mFbo;
        gl::Fbo mReferenceFbo;
        unsigned char *mRefData_RGBA;
        //Vec2f mCurPos;
        bool mFirstTime;
        
        bool mDone;
    public:
        virtual void init() {
            gSt = this;
            GraphicAppState::init();
            
            m_pCircle = new Circles7(20.f, NODE_MGR.mpRoot, false);
            m_pCircle->mPos = _SCREEN_CENTER_VEC2F;
            
            mFbo = createFbo(GL_RGBA8, 4);
            
            mFirstTime = true;
            mDone = false;            
        };
        
        virtual void deInit() {
            contextMngr::unlockMain();
        };
      
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
        };
        
        ColorAf mC1,mC2;
        
        void initRefImage() {
            mReferenceFbo = createFbo(_SCREEN_SIZE, GL_RGBA8, 4);
                        
            vector<PrimitiveStylePtr>& styles = m_pCircle->mStyles;
            mC1 = styles[0]->mMainColor; 
            mC2 = styles[1]->mMainColor;            
            float r = m_pCircle->getCircles()[0]->mRadius;            
            renderToFbo(mReferenceFbo, _SCREEN_CENTER, mC1, mC2, r);
            
            mRefData_RGBA = new unsigned char[getWindowWidth()*getWindowHeight()*4];
           getFboData_RGBA(mReferenceFbo, mRefData_RGBA);
        }
        
        #define GET_R(c) ((c&0xFF000000)>>24) 
        #define GET_G(c) ((c&0x00FF0000)>>16) 
        #define GET_B(c) ((c&0x0000FF00)>>8) 
        
        typedef unsigned int u32;
        typedef int s32;
        
        inline float fastFitness(u32 s, u32 r) {
            s32 lumS = (GET_R(s)+GET_G(s)+GET_B(s))/3;
            s32 lumR = (GET_R(r)+GET_G(r)+GET_B(r))/3;
            //printf("s=%x\n",lumS);
            float diff = lumS-lumR;
            
            // more sample than ref, this is bad
            if (diff>0)
                return diff/64.f;
            else
                return -diff;
        }
        
//        float fitness(float sample, float ref) {
//            float diff = sample-ref;
//            if (diff>0) {
//                // more sample than ref
//                diff = diff/100.f;
//            }
//            else {
//                // more ref than sample
//                diff = abs(diff);
//            }  
//            return diff;
//        }
//        
//        float d_lum(Colorf sample, Colorf ref) {
//            Vec3f ref_HSV = ref.get( CM_HSV );    
//            Vec3f sample_HSV = sample.get( CM_HSV );
//            return fitness(sample_HSV[2], ref_HSV[2]);
//        }
// 
//        float d_tone(Colorf sample, Colorf ref) {
//            Vec3f ref_HSV = ref.get( CM_HSV );    
//            Vec3f sample_HSV = sample.get( CM_HSV );
//            return abs(ref_HSV[1]-sample_HSV[1]) + abs(ref_HSV[0]-sample_HSV[0]);
//        }

        class TimerScope {
        public:
            Timer mStepTimer;
            
            TimerScope(string desc) {                
                mFrameTimer.start();                
            }
            
            ~TimerScope() {
                mFrameTimer.stop();
                float dt = mFrameTimer.getSeconds();
                printf("%s dt=%.4fms\n", mDesc.c_str(), dt*1000.f);
            }
            
            string mDesc;
        };
                
        
        float evaluate_fXY_C1_C2_R(int evalMode, Vec2f pos, ColorAf c1, ColorAf c2,float r) {

            {   TimerScope("renderToFbo");
                renderToFbo(mFbo, pos.x, pos.y, c1, c2, r); }
            
            {   TimerScope("getFboData_RGBA");
                int nbPixels = getWindowWidth()*getWindowHeight();
                static unsigned char *curSample = 0;
                if (!curSample)
                    curSample = new unsigned char[nbPixels*4];
                getFboData_RGBA(mFbo, curSample); }            
            
            {   TimerScope("rate");
                float d = 0.f;
                u32* pRef = (u32*)mRefData_RGBA;
                u32* pSample = (u32*)curSample;            
                for (int i=0;i<nbPixels;i++)
                    d += fastFitness(pSample[i], pRef[i]); }
            
            return d;
        }
        
        float evaluate_fXY(Vec2f pos) {
            return evaluate_fXY_C1_C2_R(EVAL_LUM, pos, mC1, mC2, testSimplex::getRadius());
        }
        
        virtual void mouseDown( MouseEvent event ) {
//            Vec2f clickPos = event.getPos();
//            clickPos.y = getWindowHeight()-1-clickPos.y;  
        }
        
        virtual void update(float fDt) {
            // must init ref image at first frame            
            if (mFirstTime) {
                testSimplex::initGPrms();  
                initRefImage();
                mFirstTime = false;

                // -- simplex
                // pTs = new testSimplex(this);

                // -- annealing
                contextMngr::lockMain();
                m_annealingThread = 0;
                m_annealingThread = new thread(annealingProc());
            }
        };
        
        virtual void draw() 
        {
            if (mFirstTime) {
                GraphicAppState::preDraw();
                GraphicAppState::postDraw(true);
                return;
            }
            
            GraphicAppState::preDraw();
            {
                // -- simplex
/*                if (!mDone) {
                    for (int i=0;i<1;i++) {
                        mStepTimer.start();
                            mDone = pTs->doOneStep(); 
                        mStepTimer.stop(); 
                        printf(" step time=%.2f\n", mStepTimer.getSeconds());
                    }                                           
                }*/
                
                displayFbo(mReferenceFbo, Color(0.5,0.5,0.5));
                                
                gl::enableAdditiveBlending();
                {
                    displayFbo(mFbo);
                }
                gl::disableAlphaBlending();
            }
            GraphicAppState::postDraw(false);
            contextMngr::unlockMain();
                        
            if (m_annealingThread) {
                if (m_annealingThread->joinable()) {
                    boost::system_time const timeout = boost::get_system_time() + boost::posix_time::milliseconds(0);
                    m_annealingThread->timed_join(timeout);
                    contextMngr::lockMain();
                }
                else {
                    printf("**** Thread finished !!!\n");
                    delete m_annealingThread;
                    m_annealingThread = 0;                    
                }
            }
        };
        
        // ---------------------------
        class testSimplex {
          public:
            enum {
                PRM_X=0, PRM_Y,
                PRM_RADIUS,
                PRM_R1, PRM_G1, PRM_B1,
                PRM_R2, PRM_G2, PRM_B2,
                PRM_NB
            };
            
          public:  
            typedef struct {
                EVAL_MODE evalMode;
                int firstPrmId;
                int nbPrms;
                Vec2f startPos;
                ColorAf startColors[2];
                float error_tol;
                float startRadius;
            }s_minimization_params;
            
            static s_minimization_params gMinPrms;
            
            static void initGPrms() {
                randPosition();
                randColor();
                setLumMode();
                gMinPrms.startRadius = 32.f;
            }
            
            static void setErrorTol(float tol) {
                gMinPrms.error_tol = tol;
            }
            
            static void setStartPos(Vec2f p) {
                gMinPrms.startPos = p;
            }
            
            static void randPosition() {
                gMinPrms.startPos.set(Rand::randFloat()*getWindowWidth(),
                                      Rand::randFloat()*getWindowHeight());
            }

            static void randColor() {
                for (int i=0;i<2;i++)
                   for (int j=0;j<3;j++)
                       gMinPrms.startColors[i][j] = Rand::randFloat();
            }
            
            static ColorAf getColor(int i) {
                return gMinPrms.startColors[i];
            }

            static float setStartRadius(float r) {
                gMinPrms.startRadius = r;
            }
            
            static float getRadius() {
                return gMinPrms.startRadius;
            }
            
            static void setLumMode() {
                gMinPrms.firstPrmId = 0;
                gMinPrms.nbPrms = 3;
                gMinPrms.evalMode = EVAL_LUM;
                setErrorTol(1e-1);
            }

            static int getMode() {return gMinPrms.evalMode;};

        public:                                    
            static double my_fXY_C1_C2 (const gsl_vector *v, void *params) 
            {
                GSL_macTestState* pSt = (GSL_macTestState*)params;
                
                if (getMode()==EVAL_LUM) {
                    return pSt->evaluate_fXY_C1_C2_R(getMode(),
                                                     Vec2f(gsl_vector_get(v, 0),
                                                           gsl_vector_get(v, 1)),                                       
                                                     getColor(0), 
                                                     getColor(1),
                                                     gsl_vector_get(v,2));
                }
                else {
                    assert(0);
                }
            }
        };        
    };
 };