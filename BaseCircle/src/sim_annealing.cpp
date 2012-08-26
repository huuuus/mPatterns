#include "baseCirclePCH.h"
#include <gsl/gsl_siman.h>

//struct annealingProc
//{
//public:            
//    annealingProc() {
//        
//    }
//    
//    void operator()() {
//        contextMngr::setMainAsCurrent();
//        test_annealing(0,0,32.f);
//    }
//};
//
//thread* m_annealingThread;
//
//void test_annealing(float fx, float fy, float fr=32.f);
//
///* set up parameters for this simulated annealing run */
//
///* how many points do we try before stepping */
//// NOT USED !!!
//#define N_TRIES   (-1)
//
///* how many iterations for each T? */
//#define ITERS_FIXED_T 16
//
///* max step size in random walk */
//#define STEP_SIZE 0.15
//
///* Boltzmann constant */
//#define K 1.0
//
///* initial temperature */
//#define T_INITIAL 1.0         
//
///* damping factor for temperature */
//#define MU_T 1.05
//#define T_MIN 0.1
//
//gsl_siman_params_t param_siman
//= {N_TRIES, ITERS_FIXED_T, STEP_SIZE,
//    K, T_INITIAL, MU_T, T_MIN};
//
//typedef struct {
//    float x,y,r;
//}annealing_xyr_prms;
//
//// evaluate configuration
//double E1(void *xp)
//{
//    contextMngr::lockMain();
//    
//    TestMinimizersState* pSt = gSt;
//    annealing_xyr_prms x = *((annealing_xyr_prms*)xp);
//    
//    double res = pSt->evaluate_fXY_C1_C2_R(TestMinimizersState::testSimplex::getMode(),
//                                           Vec2f(x.x,x.y),
//                                           TestMinimizersState::testSimplex::getColor(0), 
//                                           TestMinimizersState::testSimplex::getColor(1),
//                                           x.r);
//    
//    //printf("at %.2f %.2f r=%.2f => %.2f\n",x.x,x.y,x.r,res);
//    
//    contextMngr::unlockMain();
//    //printf("E1 unlocked\n");
//    
//    return res;
//}
//
//#define SIMAN_X_SCALE getWindowWidth()*1.f
//#define SIMAN_Y_SCALE getWindowHeight()*1.f
//#define SIMAN_R_SCALE 64.0
//
//// distance between 2 configurations
//double M1(void *xp, void *yp)
//{
//    return 0; // not used
//}
//
//float randomMove(const gsl_rng * r, double x, double step_size, double magnitude) {
//    double v = gsl_rng_uniform(r);
//    double newX = (v * 2.f * step_size - step_size) * magnitude + x;
//    if (newX<0)
//        newX += magnitude;
//    newX = fmod(newX, magnitude);
//    return newX;
//}
//
//// take a randon step of size step_size
//void S1(const gsl_rng * r, void *xp, double step_size)
//{
//    annealing_xyr_prms old_x = *((annealing_xyr_prms*)xp);
//    annealing_xyr_prms new_x;
//    new_x.x = randomMove(r, old_x.x, step_size, SIMAN_X_SCALE);
//    new_x.y = randomMove(r, old_x.y, step_size, SIMAN_Y_SCALE);
//    new_x.r = randomMove(r, old_x.r, step_size, SIMAN_R_SCALE);        
//    memcpy(xp, &new_x, sizeof(new_x));
//}
//
//// print func
//void P1(void *xp)
//{
//    annealing_xyr_prms x = *((annealing_xyr_prms*)xp);
//    printf (" pos=%.9f,%.9f r=%.9f",x.x,x.y,x.r);
//}
//
//void test_annealing(float fx, float fy, float fr)
//{
//    const gsl_rng_type * T;
//    gsl_rng * r;
//    
//    annealing_xyr_prms x_initial;
//    x_initial.x = fx;
//    x_initial.y = fy;
//    x_initial.r = fr;
//    
//    printf("******** START AT at %.2f %.2f r=%.2f\n",fx,fy,fr);
//    
//    gsl_rng_env_setup();
//    gsl_rng_default_seed = clock();
//    
//    T = gsl_rng_default;
//    r = gsl_rng_alloc(T);
//    
//    gsl_siman_solve(r, &x_initial, E1, S1, M1, P1,
//                    NULL, NULL, NULL, 
//                    sizeof(annealing_xyr_prms), param_siman);
//    
//    printf("******** test_annealing finished\n");
//    E1(&x_initial);
//    P1(&x_initial);        
//    gsl_rng_free (r);
//}
//
////				m_annealingThread = 0;
////              m_annealingThread = new thread(annealingProc());
