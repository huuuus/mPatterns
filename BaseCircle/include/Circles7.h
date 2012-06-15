namespace cinder {
	namespace params {
		class InterfaceGl;
	}
}

namespace mPatterns {
	// 7 circles unity
	DECL_WEAK_PTR(Circles7)

	class Circles7 : public Node<Vec2f> {	
	public:
		Circles7(float radius, NodeWeakPtr pParent, bool need7Styles=false);
		vector<PrimitiveStyleWeakPtr>& getStyles() {return mStyles;};
		virtual enum NODE_TYPE getType() {return NT_CIRCLES7;};
		virtual void draw() const;
		
		virtual void spawnParameters();
		virtual bool pick(Vec2f pickPos) const {return isInRadius(pickPos, mCircles[0]->mRadius*2.f);};

		vector<PrimitiveStyleWeakPtr> mStyles;
		vector<PrimitiveStyleWeakPtr> mAxisesStyles;

	protected:
		vector<CircleWeakPtr> mCircles;		
		cinder::params::InterfaceGl* mpParams;
	};
}