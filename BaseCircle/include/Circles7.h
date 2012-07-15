namespace cinder {
	namespace params {
		class InterfaceGl;
	}
}

// 7 circles unity
namespace mPatterns {
	DECL_WEAK_PTR(Circles7)

	class Circles7 : public Node<Vec2f> {	
	public:
		Circles7(float radius, NodeWeakPtr pParent, bool need7Styles=false);

		virtual ~Circles7() {
			DEBUG_MSG(L"delete a Circles7 %x\n",(int)(this));			
		};

		vector<PrimitiveStylePtr>& getStyles() {return mStyles;};
		virtual enum NODE_TYPE getType() {return NT_CIRCLES7;};
		virtual void draw() const;
		
		virtual void spawnParameters();
		virtual bool pick(Vec2f pickPos) const {return isInRadius(pickPos, mCircles[0]->mRadius*2.f);};

		vector<PrimitiveStylePtr> mStyles;
		vector<PrimitiveStylePtr> mAxisesStyles;

        vector<CircleWeakPtr>& getCircles() {return mCircles;};
        
	protected:
		vector<CircleWeakPtr> mCircles;		
		 boost::shared_ptr<cinder::params::InterfaceGl> mpParams;
        
        virtual void onSelected();
        virtual void onUnselected();
	};
}