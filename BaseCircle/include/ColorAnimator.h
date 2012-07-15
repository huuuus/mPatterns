namespace mPatterns {

DECL_WEAK_PTR(IColorAnimator)
DECL_SHARED_PTR(IColorAnimator)

typedef std::vector<IColorAnimatorPtr> t_IColorAnimatorVec;

class IColorAnimator {
public:
    IColorAnimator();
    static t_IColorAnimatorVec mColorAnimators;
    virtual void tick() = 0;
    static void tickAll();
};

template<class colorAnimatorTechnique>
class colorAnimator : public IColorAnimator {
public:
    colorAnimator(cinder::ColorAf* pCol):
        IColorAnimator()
    {
        mpCol = pCol;
    };
    
    cinder::ColorAf* mpCol;
    
    virtual void tick() {
        mTech.tick(mpCol);
    };
    
    colorAnimatorTechnique mTech;
};

class softHueTechnique {
public:
    softHueTechnique();
    virtual void tick(cinder::ColorAf* pCol);
	float m_phase;
	float m_speed;
};

typedef colorAnimator<softHueTechnique> t_softHueAnimator;

}
