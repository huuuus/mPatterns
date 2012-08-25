#include "cinder/rand.h"
#include "cinder/gl/texture.h"
#include "cinder/timer.h"

#include <stdio.h>
#include <gsl/gsl_sf_bessel.h>
#include <gsl/gsl_multimin.h>
#include <gsl/gsl_siman.h>

#define _SCREEN_SIZE getWindowWidth(),getWindowHeight()
#define _SCREEN_CENTER getWindowWidth()/2,getWindowHeight()/2
#define _SCREEN_CENTER_VEC2F Vec2f(getWindowWidth()/2,getWindowHeight()/2)

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
    
    void test_annealing(float fx, float fy, float fr=32.f);
    
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
            
            gl::Fbo::Format format;
            format.setSamples( 4 );
            format.setColorInternalFormat( GL_RGBA8 );
            mFbo = gl::Fbo(_SCREEN_SIZE, format );  
            
            mFirstTime = true;
            mDone = false;            
        };
        
        virtual void deInit() {
            contextMngr::unlockMain();
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
        
        static void getFboData_RGBA(gl::Fbo &fbo, void* destRGBA) {
            gl::Texture texFbo = fbo.getTexture();
            texFbo.bind();
                glPixelStorei( GL_PACK_ALIGNMENT, 1 );
                glGetTexImage( texFbo.getTarget(), 0, GL_RGBA, GL_UNSIGNED_BYTE, destRGBA );
                GLenum err = glGetError();
                if (err!=GL_NO_ERROR)
                    printf("err=%x\n",err);
            texFbo.unbind();
        }
        
        ColorAf mC1,mC2;
        
        void initRefImage() {
            gl::Fbo::Format format;
            
            format.setSamples( 4 );
            format.setColorInternalFormat( GL_RGBA8 );
            mReferenceFbo = gl::Fbo(_SCREEN_SIZE, format );
            
            vector<PrimitiveStylePtr>& styles = m_pCircle->mStyles;
            mC1 = styles[0]->mMainColor; mC2 = styles[1]->mMainColor;
            
            float r = m_pCircle->getCircles()[0]->mRadius;
            testSimplex::setStartRadius(r);
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
        
        float evaluate_fXY_C1_C2_R(int evalMode, Vec2f pos, ColorAf c1, ColorAf c2,float r) {

            mFrameTimer.start();
                renderToFbo(mFbo, pos.x, pos.y, c1, c2, r);
                mFrameTimer.stop();
            float dt = mFrameTimer.getSeconds();
            printf("renderToFbo dt=%.4fms\n", dt*1000.f);

            mFrameTimer.start();
                int nbPixels = getWindowWidth()*getWindowHeight();
                static unsigned char *curSample = 0;
                if (!curSample)
                    curSample = new unsigned char[nbPixels*4];
                getFboData_RGBA(mFbo, curSample);
            dt = mFrameTimer.getSeconds();
            printf("getFboData_RGBA dt=%.4fms\n", dt*1000.f);
                        
            mFrameTimer.start();
                float d = 0.f;
                u32* pRef = (u32*)mRefData_RGBA;
                u32* pSample = (u32*)curSample;            
                for (int i=0;i<nbPixels;i++) {
                    d += fastFitness(pSample[i], pRef[i]);                    
                }
                //delete [] curSample;
            dt = mFrameTimer.getSeconds();
            printf("rate dt=%.4fms d=%.4f\n", dt*1000.f, d);
            
/*            printf("d=%.3f %.2f,%.2f C1=%.2f,%.2f,%.2f C2=%.2f,%.2f,%.2f\n", 
                    d, pos.x, pos.y, 
                    c1.r, c1.g, c1.b, 
                    c2.r, c2.g, c2.b);*/
            

            return d;
        }
        
        float evaluate_fXY(Vec2f pos) {
            return evaluate_fXY_C1_C2_R(EVAL_LUM, pos, mC1, mC2, testSimplex::getRadius());
        }
        
        virtual void mouseDown( MouseEvent event ) {
            Vec2f clickPos = event.getPos();
            clickPos.y = getWindowHeight()-1-clickPos.y;  
            
            // -- simplex
            //delete pTs;
            //testSimplex::setLumMode();
            //testSimplex::setStartPos(clickPos);
            //testSimplex::setStartRadius(Rand::randFloat()*32.f+4.f);
            //pTs = new testSimplex(this);
            //mDone = false;
        }
        
        Timer mFrameTimer;
        Timer mStepTimer;
  
        struct annealingProc
        {
        public:            
            annealingProc() {
                
            }
            
            void operator()() {
                contextMngr::setMainAsCurrent();
                test_annealing(0,0,32.f);
            }
        };
        
        thread* m_annealingThread;
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
            gsl_multimin_fminimizer *s; 
            gsl_vector *ss, *x; 
            gsl_multimin_function minex_func;
            
            size_t iter; int status; double size;
            
            testSimplex(GSL_macTestState* param) {                
                iter = 0;
                
                /* Starting point */
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
                
                /* Set initial step sizes */
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
                
                /* Initialize method and minimizer */
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

    /* set up parameters for this simulated annealing run */

    /* how many points do we try before stepping */
    // NOT USED !!!
    #define N_TRIES   (-1)

    /* how many iterations for each T? */
    #define ITERS_FIXED_T 16

    /* max step size in random walk */
    #define STEP_SIZE 0.15

    /* Boltzmann constant */
    #define K 1.0

    /* initial temperature */
    #define T_INITIAL 1.0         

    /* damping factor for temperature */
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
        contextMngr::lockMain();
                
        GSL_macTestState* pSt = gSt;
        annealing_xyr_prms x = *((annealing_xyr_prms*)xp);
        
        double res = pSt->evaluate_fXY_C1_C2_R(GSL_macTestState::testSimplex::getMode(),
                                               Vec2f(x.x,x.y),
                                               GSL_macTestState::testSimplex::getColor(0), 
                                               GSL_macTestState::testSimplex::getColor(1),
                                               x.r);
        
        //printf("at %.2f %.2f r=%.2f => %.2f\n",x.x,x.y,x.r,res);

        contextMngr::unlockMain();
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
};









/*
double my_f (const gsl_vector *v, void *params) 
{
    double x, y;
    double *p = (double *)params;
    x = gsl_vector_get(v, 0); y = gsl_vector_get(v, 1);
    return p[2] * (x - p[0]) * (x - p[0]) +
    p[3] * (y - p[1]) * (y - p[1]) + p[4];
}

void test_simplex(void) 
{    
    double par[5] = {1.0, 2.0, 10.0, 20.0, 30.0};
    
    const gsl_multimin_fminimizer_type *T = gsl_multimin_fminimizer_nmsimplex2;
    
    gsl_multimin_fminimizer *s = NULL; 
    gsl_vector *ss, *x; 
    gsl_multimin_function minex_func;
    
    size_t iter = 0; int status; double size;
    
    // Starting point
    x = gsl_vector_alloc (2); 
    gsl_vector_set (x, 0, 5.0); 
    gsl_vector_set (x, 1, 7.0);
    
    // Set initial step sizes to 1
    ss = gsl_vector_alloc (2); 
    gsl_vector_set_all (ss, 1.0);
    
    // Initialize method and iterate
    minex_func.n = 2;
    minex_func.f = my_f; 
    minex_func.params = par;
    
    s = gsl_multimin_fminimizer_alloc (T, 2); 
    gsl_multimin_fminimizer_set (s, &minex_func, x, ss);
    do {
        iter++;
        
        status = gsl_multimin_fminimizer_iterate(s);
        if (status)
            break;
        
        size = gsl_multimin_fminimizer_size (s); 
        status = gsl_multimin_test_size (size, 1e-2);
        
        if (status == GSL_SUCCESS) {
            printf ("converged to minimum at\n"); }
        
        printf ("%5d %10.3e %10.3e f() = %7.3f size = %.3f\n", iter,
                gsl_vector_get (s->x, 0), gsl_vector_get (s->x, 1), s->fval, size);
    }while (status == GSL_CONTINUE && iter < 100);
    
    gsl_vector_free(x); 
    gsl_vector_free(ss); 
    gsl_multimin_fminimizer_free (s);
    
    // return status;
}*/