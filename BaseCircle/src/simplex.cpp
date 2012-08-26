#include <gsl/gsl_sf_bessel.h>
#include <gsl/gsl_multimin.h>

//virtual void mouseDown( MouseEvent event ) {
    // -- simplex
    //delete pTs;
    //testSimplex::setLumMode();
    //testSimplex::setStartPos(clickPos);
    //testSimplex::setStartRadius(Rand::randFloat()*32.f+4.f);
    //pTs = new testSimplex(this);
    //mDone = false;
//}

testSimplex::setStartRadius(r);

testSimplex* pTs;

GSL_macTestState::testSimplex::s_minimization_params GSL_macTestState::testSimplex::gMinPrms;

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
