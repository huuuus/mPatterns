#include "BaseCirclePCH.h"
#include "ColorAnimator.h"
#include "cinder/rand.h"

using namespace mPatterns;
using namespace app;

t_IColorAnimatorVec IColorAnimator::mColorAnimators;

IColorAnimator::IColorAnimator() {
    mColorAnimators.push_back(this);
};

IColorAnimator::~IColorAnimator()
{
	t_IColorAnimatorVec::iterator s,e,f;
	s = mColorAnimators.begin();
	e = mColorAnimators.end();
	f = std::find(s, e, this);
	if (f != e) {
		mColorAnimators.erase(f);
	}
}
    
void IColorAnimator::tickAll() 
{
    t_IColorAnimatorVec::iterator it;
    for (it=mColorAnimators.begin();it!=mColorAnimators.end();++it) {
        (*it)->tick();
    }
};

softHueTechnique::softHueTechnique() {
    m_phase = randFloat(-M_PIf, M_PIf);
	m_speed = randFloat(0.1f, 0.3f);
};

void softHueTechnique::tick(cinder::ColorAf* pCol) {
    float s = sin(m_phase + m_speed*getElapsedSeconds()*M_PIf)*0.5f+0.5f;
    Colorf c;
    c.set(cinder::CM_HSV, Vec3f(s,0.85f,0.1f));
    *pCol = c;
};