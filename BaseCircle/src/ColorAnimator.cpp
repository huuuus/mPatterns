#include "BaseCirclePCH.h"
#include "ColorAnimator.h"

using namespace mPatterns;
using namespace app;

t_IColorAnimatorVec IColorAnimator::mColorAnimators;

IColorAnimator::IColorAnimator() {
    mColorAnimators.push_back(IColorAnimatorPtr(this));
};

void IColorAnimator::tickAll() 
{
    t_IColorAnimatorVec::iterator it;
    for (it=mColorAnimators.begin();it!=mColorAnimators.end();++it) {
        (*it)->tick();
    }
};

softHueTechnique::softHueTechnique() {
    
};

void softHueTechnique::tick(cinder::ColorAf* pCol) {
    float s = sin( getElapsedSeconds()*M_PIf)*0.5f+0.5f;
    Colorf c;
    c.set(cinder::CM_HSV, Vec3f(s,0.5f,0.5f));
    *pCol = c;
};