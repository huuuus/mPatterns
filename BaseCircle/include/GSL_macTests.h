#include "cinder/rand.h"
#include "cinder/gl/texture.h"
#include "cinder/timer.h"

#include "cinder/Thread.h"
#include <boost/thread/mutex.hpp>

#include <stdio.h>
#include <gsl/gsl_sf_bessel.h>
#include <gsl/gsl_multimin.h>
#include <gsl/gsl_siman.h>

#define _SCREEN_SIZE getWindowWidth(),getWindowHeight()
#define _SCREEN_CENTER getWindowWidth()/2,getWindowHeight()/2
#define _SCREEN_CENTER_VEC2F Vec2f(getWindowWidth()/2,getWindowHeight()/2)

#include "cinder/gl/Fbo.h"

namespace mPatterns {
    DECL_SHARED_PTR(GSL_macTestState)
    extern double my_fXY (const gsl_vector *v, void *params);
    extern double my_fXY_C1_C2 (const gsl_vector *v, void *params);
    
    enum EVAL_MODE {
        EVAL_LUM,
        EVAL_TONE,            
    };
    
    void test_annealing(float fx, float fy, float fr=32.f);
    
    class GSL_macTestState;
    GSL_macTestState* gSt;
    
    mutex glMutex;	

#ifdef WIN32
	HGLRC _WIN32_DEFAULT_CONTEXT;
	HDC _WIN32_DEFAULT_DC;
#endif

    class GSL_macTestState : public GraphicAppState {
        
    public:
        Circles7* m_pCircle;
        gl::Fbo mFbo;
        gl::Fbo mReferenceFbo;
        unsigned char *mRefData_RGB;
        //Vec2f mCurPos;
        bool mFirstTime;
        
        bool mDone;
    public:
        virtual void init() {
            gSt = this;
            GraphicAppState::init();
            
            m_pCircle = new Circles7(20.f, NODE_MGR.mpRoot, false);
            m_pCircle->mPos = _SCREEN_CENTER_VEC2F;
            
            gl::Fbo::Format format;
            format.setSamples( 4 );
            mFbo = gl::Fbo(_SCREEN_SIZE, format );  
            
            mFirstTime = true;
            mDone = false;            
        };
        
//        ~GSL_macTestState() {
//            deInit();
//        }
        
        virtual void deInit() {
            printf("deInit, unlock glmutex\n");
            glMutex.unlock();
        };
        
        virtual void update(float fDt) {
//            GraphicAppState::postUpdate();
        };
      
        void renderToFbo(gl::Fbo &fbo, float posX, float posY, ColorAf c1, ColorAf c2, float r) {
            gl::SaveFramebufferBinding bindingSaver;
            fbo.bindFramebuffer();
            gl::setViewport( fbo.getBounds() );
            
            gl::clear(Color( 0.0, 0.0, 0.0 ));
            
            gl::enableAdditiveBlending();
                m_pCircle->mPos = Vec2f(posX, posY);
            
                //vector<CircleWeakPtr>& circles = m_pCircle->getCircles();
                //vector<CircleWeakPtr>::iterator it;
                //for (it=circles.begin();it!=circles.end();it++)
                //    (*it)->mRadius = r;
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
        
        void displayFbo(gl::Fbo &fbo, Color c = Color::white()) 
        {
            gl::color( c );
            gl::draw(fbo.getTexture());            
        }
        
        static void getFboData_RGB(gl::Fbo &fbo, unsigned char *destRGB) {
            gl::Texture texFbo = fbo.getTexture();
            texFbo.bind();
                glPixelStorei( GL_PACK_ALIGNMENT, 1 );
                glGetTexImage( texFbo.getTarget(), 0, GL_RGB, GL_UNSIGNED_BYTE, destRGB );
            texFbo.unbind();            
        }
        
        ColorAf mC1,mC2;
        
        void initRefImage() {
            gl::Fbo::Format format;
            format.setSamples( 4 );
            mReferenceFbo = gl::Fbo(_SCREEN_SIZE, format );
            
            vector<PrimitiveStylePtr>& styles = m_pCircle->mStyles;
            mC1 = styles[0]->mMainColor; mC2 = styles[1]->mMainColor;
            
            float r = m_pCircle->getCircles()[0]->mRadius;
            testSimplex::setStartRadius(r);
            renderToFbo(mReferenceFbo, _SCREEN_CENTER, mC1, mC2, r);
            
            mRefData_RGB = new unsigned char[getWindowWidth()*getWindowHeight()*3];
            getFboData_RGB(mReferenceFbo, mRefData_RGB);
        }
        
        float fitness(float sample, float ref) {
            float diff = sample-ref;
            if (diff>0) {
                // more sample than ref
                diff = diff/100.f;
            }
            else {
                // more ref than sample
                diff = abs(diff);
            }  
            return diff;
        }
        
        float d_lum(Colorf sample, Colorf ref) {
            Vec3f ref_HSV = ref.get( CM_HSV );    
            Vec3f sample_HSV = sample.get( CM_HSV );
            return fitness(sample_HSV[2], ref_HSV[2]);
        }
 
        float d_tone(Colorf sample, Colorf ref) {
            Vec3f ref_HSV = ref.get( CM_HSV );    
            Vec3f sample_HSV = sample.get( CM_HSV );
            return abs(ref_HSV[1]-sample_HSV[1]) + abs(ref_HSV[0]-sample_HSV[0]);
        }
        
		static unsigned char gEvalBuffer[2048*2048*3];

        float evaluate_fXY_C1_C2_R(int evalMode, Vec2f pos, ColorAf c1, ColorAf c2,float r) {
            renderToFbo(mFbo, pos.x, pos.y, c1, c2, r);
            
            getFboData_RGB(mFbo, gEvalBuffer);
            unsigned char* curSample = gEvalBuffer;

            float d = 0.f;
            for (int i=0;i<getWindowWidth()*getWindowHeight();i++) {
                int off = i*3;
                Colorf ref(mRefData_RGB[off],mRefData_RGB[off+1],mRefData_RGB[off+2]);
                Colorf sample(curSample[off],curSample[off+1],curSample[off+2]);
                if (evalMode == EVAL_LUM)
                    d += d_lum(sample, ref);
                else
                    d += d_tone(sample, ref);
            }
            
//            printf("d=%.3f %.2f,%.2f C1=%.2f,%.2f,%.2f C2=%.2f,%.2f,%.2f\n", 
//                    d, pos.x, pos.y, 
 //                   c1.r, c1.g, c1.b, 
  //                  c2.r, c2.g, c2.b);
            
            return d;
        }
        
        float evaluate_fXY(Vec2f pos) {
            return evaluate_fXY_C1_C2_R(EVAL_LUM, pos, mC1, mC2, testSimplex::getRadius());
        }
        
        virtual void mouseDown( MouseEvent event ) {
            Vec2f clickPos = event.getPos();
            clickPos.y = getWindowHeight()-1-clickPos.y;  
//            test_annealing(clickPos.x,clickPos.y,32.f);
            
/*            delete pTs;
            testSimplex::setLumMode();
            testSimplex::setStartPos(clickPos);
            testSimplex::setStartRadius(Rand::randFloat()*32.f+4.f);
            pTs = new testSimplex(this);*/
            
            mDone = false;
        }
        
        Timer mFrameTimer;
        Timer mStepTimer;
  
        struct annealingProc
        {
        public:            
            annealingProc() {
            }
            
            void operator()() {
               // for (int i=0;i<_nbTimes;i++) {
               //   boost::posix_time::milliseconds duration(100);
               //   printf("*** mySimpleProc %03d\n", i);
               //   boost::this_thread::sleep(duration);                    
               // }
                
                test_annealing(0,0,32.f);
            }
        };
        
        thread* m_annealingThread;
        
        virtual void draw() 
        {
            //printf("one frame ...\n");

            // must init ref image at first frame            
            if (mFirstTime) {
                testSimplex::initGPrms();  
                initRefImage();
                // pTs = new testSimplex(this);
                mFirstTime = false;
                
                // test_annealing(0,0,32.f);
            
#ifdef WIN32
				_WIN32_DEFAULT_CONTEXT = wglGetCurrentContext();
				_WIN32_DEFAULT_DC = wglGetCurrentDC();
#endif

                //printf("*** create thread\n");
                glMutex.lock();
                m_annealingThread = 0;
                m_annealingThread = new thread(annealingProc());
            }
            
#ifdef WIN32
			wglMakeCurrent(NULL, NULL);
#endif
            //printf("main unlock\n");
            glMutex.unlock();
            
            //mFrameTimer.stop();
            //float dt = mFrameTimer.getSeconds();
            //printf("dt=%.2f\n", dt);
            //mFrameTimer.start();
            
            glMutex.lock();
            //printf("main is drawing...\n");
#ifdef WIN32
			BOOL test = wglMakeCurrent(_WIN32_DEFAULT_DC, _WIN32_DEFAULT_CONTEXT);
			DWORD err = GetLastError();
#endif
            GraphicAppState::preDraw();
            {
                
//                if (!mDone) {
//                  for (int i=0;i<1;i++) {
//                      mStepTimer.start();
//                          mDone = pTs->doOneStep(); 
//                      mStepTimer.stop(); 
//                      printf(" step time=%.2f\n", mStepTimer.getSeconds());
//                  }                                           
//              }
                
                displayFbo(mReferenceFbo, Color(0.5,0.5,0.5));
                                
                gl::enableAdditiveBlending();
                {
                    displayFbo(mFbo);
                }
                gl::disableAlphaBlending();
            }
            GraphicAppState::postDraw(false);

#ifdef WIN32
			wglMakeCurrent(NULL, NULL);
#endif
            glMutex.unlock();
            //printf("main finished drawing...\n");
                        
            if (m_annealingThread) {
                if (m_annealingThread->joinable()) {
                    //printf("main waiting...\n");
                    boost::system_time const timeout = boost::get_system_time() + boost::posix_time::milliseconds(1);
                    m_annealingThread->timed_join(timeout);
                    glMutex.lock();
                }
                else {
                    printf("**** Thread finished !!!\n");
                    delete m_annealingThread;
                    m_annealingThread = 0;                    
                    //m_annealingThread = new thread(mySimpleProc(Rand::randFloat()*300));
                }
            }

#ifdef WIN32
			test = wglMakeCurrent(_WIN32_DEFAULT_DC, _WIN32_DEFAULT_CONTEXT);
			err = GetLastError();
#endif
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

            static void setStartRadius(float r) {
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
            gsl_multimin_fminimizer *s; 
            gsl_vector *ss, *x; 
            gsl_multimin_function minex_func;
            
            size_t iter; int status; double size;
            
            testSimplex(GSL_macTestState* param) {                
                iter = 0;
                
                // Starting point 
                gsl_vector *allParams = gsl_vector_alloc(PRM_NB);                 
                gsl_vector_set (allParams, PRM_X, gMinPrms.startPos.x); 
                gsl_vector_set (allParams, PRM_Y, gMinPrms.startPos.y);
                gsl_vector_set (allParams, PRM_RADIUS, getRadius());
                
                for (int i=PRM_R1;i<=PRM_R2;i+=3) {
                    for (int j=0;j<3;j++)
                        gsl_vector_set (allParams, i+j, gMinPrms.startColors[(i==PRM_R1) ? 0 : 1][j]);
                }

                x = gsl_vector_alloc (gMinPrms.nbPrms); 
                for (int i=0;i<gMinPrms.nbPrms;i++)
                    gsl_vector_set(x, i, gsl_vector_get(allParams, i));
                gsl_vector_free(allParams);
                
                // Set initial step sizes 
                gsl_vector *all_ss = gsl_vector_alloc (PRM_NB);
                for (int i=PRM_X;i<=PRM_Y;i++)
                    gsl_vector_set (all_ss, i, 256.0);
                gsl_vector_set (all_ss, PRM_RADIUS, 15.f);
                for (int i=PRM_R1;i<=PRM_B2;i++)
                    gsl_vector_set (all_ss, i, 0.75);
                
                ss = gsl_vector_alloc (gMinPrms.nbPrms); 
                for (int i=0;i<gMinPrms.nbPrms;i++)
                    gsl_vector_set(ss, i, gsl_vector_get(all_ss, i));
                gsl_vector_free(all_ss);
                
                // Initialize method and minimizer 
                minex_func.n = gMinPrms.nbPrms;
                minex_func.f = my_fXY_C1_C2; 
                minex_func.params = param;
                
                const gsl_multimin_fminimizer_type *T = gsl_multimin_fminimizer_nmsimplex2;
                
                s = gsl_multimin_fminimizer_alloc (T, gMinPrms.nbPrms); 
                gsl_multimin_fminimizer_set (s, &minex_func, x, ss);
            }
            
            ~testSimplex() {
                gsl_vector_free(x); 
                gsl_vector_free(ss); 
                gsl_multimin_fminimizer_free (s);                
            }
            
            bool doOneStep() {
                iter++;
                
                status = gsl_multimin_fminimizer_iterate(s);
                if (status)
                    return true;
                
                size = gsl_multimin_fminimizer_size (s); 
                status = gsl_multimin_test_size (size, gMinPrms.error_tol);
                
                if (status == GSL_SUCCESS) {
                    printf ("converged to minimum at\n"); }
                
                printf ("%5d f() = %7.3f pos=%5.2f %5.2f size=%5.2f", (int)iter, s->fval,
                        gsl_vector_get (s->x, 0),
                        gsl_vector_get (s->x, 1),
                        gsl_vector_get (s->x, 2));
                
                return !(status == GSL_CONTINUE && iter < 10000);
            }
            
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
        
        testSimplex* pTs;
        
        
        
        //virtual void mouseWheel( MouseEvent event );        
    };
    
    GSL_macTestState::testSimplex::s_minimization_params GSL_macTestState::testSimplex::gMinPrms;

    // set up parameters for this simulated annealing run

    /// how many points do we try before stepping
    // NOT USED !!!
    #define N_TRIES   (-1)

    // how many iterations for each T?
    #define ITERS_FIXED_T 16

    // max step size in random walk 
    #define STEP_SIZE 0.1

    // Boltzmann constant 
    #define K 1.0

    // initial temperature 
    #define T_INITIAL 1.0         

    // damping factor for temperature 
    #define MU_T 1.05
    #define T_MIN 0.1

    gsl_siman_params_t param_siman
    = {N_TRIES, ITERS_FIXED_T, STEP_SIZE,
        K, T_INITIAL, MU_T, T_MIN};

    typedef struct {
        float x,y,r;
    }annealing_xyr_prms;

    // evaluate configuration
    double E1(void *xp)
    {
        glMutex.lock();
                
        App* pApp = App::get();
        assert(pApp);
        assert(pApp->getRenderer());

#ifdef WIN32
		BOOL test = wglMakeCurrent(_WIN32_DEFAULT_DC, _WIN32_DEFAULT_CONTEXT);
		DWORD err = GetLastError();
#else
        pApp->getRenderer()->makeCurrentContext();
#endif

        GSL_macTestState* pSt = gSt;
        annealing_xyr_prms x = *((annealing_xyr_prms*)xp);
        
        double res = pSt->evaluate_fXY_C1_C2_R(GSL_macTestState::testSimplex::getMode(),
                                               Vec2f(x.x,x.y),
                                               GSL_macTestState::testSimplex::getColor(0), 
                                               GSL_macTestState::testSimplex::getColor(1),
                                               x.r);
        
        //printf("at %.2f %.2f r=%.2f => %.2f\n",x.x,x.y,x.r,res);

#ifdef WIN32
		test = wglMakeCurrent(NULL, NULL);
		err = GetLastError();
#endif
        glMutex.unlock();
        //printf("E1 unlocked\n");
        
        return res;
    }

    #define SIMAN_X_SCALE getWindowWidth()*1.f
    #define SIMAN_Y_SCALE getWindowHeight()*1.f
    #define SIMAN_R_SCALE 64.0

    // distance between 2 configurations
    double M1(void *xp, void *yp)
    {
        return 0; // not used
    }
    
    float randomMove(const gsl_rng * r, double x, double step_size, double magnitude) {
        double v = gsl_rng_uniform(r);
        double newX = (v * 2.f * step_size - step_size) * magnitude + x;
        if (newX<0)
            newX += magnitude;
        newX = fmod(newX, magnitude);
        return newX;
    }
    
    // take a randon step of size step_size
    void S1(const gsl_rng * r, void *xp, double step_size)
    {
        annealing_xyr_prms old_x = *((annealing_xyr_prms*)xp);
        annealing_xyr_prms new_x;
        new_x.x = randomMove(r, old_x.x, step_size, SIMAN_X_SCALE);
        new_x.y = randomMove(r, old_x.y, step_size, SIMAN_Y_SCALE);
        new_x.r = randomMove(r, old_x.r, step_size, SIMAN_R_SCALE);        
        memcpy(xp, &new_x, sizeof(new_x));
    }

    // print func
    void P1(void *xp)
    {
        annealing_xyr_prms x = *((annealing_xyr_prms*)xp);
        printf (" pos=%.9f,%.9f r=%.9f",x.x,x.y,x.r);
    }

    void test_annealing(float fx, float fy, float fr)
    {
        const gsl_rng_type * T;
        gsl_rng * r;
        
        annealing_xyr_prms x_initial;
        x_initial.x = fx;
        x_initial.y = fy;
        x_initial.r = fr;
        
        printf("******** START AT at %.2f %.2f r=%.2f\n",fx,fy,fr);
        
        gsl_rng_env_setup();

        gsl_rng_default_seed = clock();
        
        T = gsl_rng_default;
        r = gsl_rng_alloc(T);
        
        gsl_siman_solve(r, &x_initial, E1, S1, M1, P1,
                        NULL, NULL, NULL, 
                        sizeof(annealing_xyr_prms), param_siman);
        
        printf("******** test_annealing finished\n");
        E1(&x_initial);
        P1(&x_initial);        
        gsl_rng_free (r);
    }
	unsigned char GSL_macTestState::gEvalBuffer[2048*2048*3];
};

