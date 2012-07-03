#include "BaseCirclePCH.h"
#include "cinder/gl/gl.h"
#include "Node.h"
#include "Style.h"
#include "Axis.h"
#include "cinder/gl/gl.h"
#include "ColorAnimator.h"

using namespace mPatterns;

t_IColorAnimatorVec IColorAnimator::mColorAnimators;

IColorAnimator::IColorAnimator() {
    mColorAnimators.push_back(IColorAnimatorPtr(this));
};

template<class T>
colorAnimator<T>::colorAnimator(cinder::ColorAf* pCol) :
    IColorAnimator()
{
    mpCol = pCol;
    mColorAnimators.push_back(this);
}

template<class T>
void colorAnimator<T>::tick() {
    mTech.tick(mpCol);
}

template<class T>
void colorAnimator<T>::tickAll() 
{
    t_IColorAnimatorVec::iterator it;
    for (it=mColorAnimators.begin();it!=mColorAnimators.end();++it) {
        (*it)->tick();
    }
};

softHueTechnique::softHueTechnique() {
    
};

void softHueTechnique::tick(cinder::ColorAf* pCol) {
    float s = sin(gT_s*34.f)*0.5f+0.5f;
    Colorf c;
    c.set(cinder::CM_HSV, Vec3f(s,0.5f,0.5f));
    *pCol = c;
};