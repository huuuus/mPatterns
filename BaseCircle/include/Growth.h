#include "cinder/rand.h"
#include "ColorAnimator.h"
#include <boost/foreach.hpp>

using namespace ci;
using namespace std;

namespace mPatterns {

	DECL_SHARED_PTR(ISetting)

	class ISetting {
	public:
		enum TYPE {
			ST_BOOL,
			ST_FLOAT,
			ST_COLOR,
		};

		virtual TYPE getType() = 0;
		virtual float getf() = 0;
		virtual void _register(const string &name, cinder::params::InterfaceGl* pPrmIfaceGl) = 0;
	};

	class FloatSetting : public ISetting {
	public:
		FloatSetting() : mVal(0.f) {};
		FloatSetting(float v) : mVal(v) {};
		virtual ISetting::TYPE getType() {return ST_FLOAT;};
		virtual float getf() {return mVal;};
		
		virtual void _register(const string &name, cinder::params::InterfaceGl* pPrmIfaceGl) 
		{
			pPrmIfaceGl->addParam( name.c_str(), &mVal, "" );
		}
	protected:
		float mVal;
	};

	class ColorSetting : public ISetting {
	public:
		virtual ISetting::TYPE getType() {return ST_COLOR;};
		virtual float getf() {return (mCol.r+mCol.g+mCol.b)/3.f;};

		virtual void _register(const string &name, cinder::params::InterfaceGl* pPrmIfaceGl) 
		{
			pPrmIfaceGl->addParam( name.c_str(), &mCol, "" );
		}

	protected:
		ColorAf mCol;
	};

	class SettingsGroup {
	public:
		string mId;
		shared_ptr<params::InterfaceGl> mpPrmsGl;

		SettingsGroup(string name) 
		{
			unsigned long id = (unsigned long)(this);
			mId = (format(name + "_%d") % id).str();
		};

		void init() {
			mpPrmsGl.reset(new params::InterfaceGl(mId, Vec2i(200,200)));

			typedef std::pair<string, ISettingPtr> SettingPair_t;

			BOOST_FOREACH(SettingPair_t s, mSettings)
			{
				s.second->_register(s.first, mpPrmsGl.get());
			}
			mpPrmsGl->show();
		}

		// add Setting: takes ownership on pSetting
		void add(string name, ISetting* pSetting) {
			mSettings.insert(pair<string, ISettingPtr>(name, ISettingPtr(pSetting)));
		}

		float getf(const string &name) {
			return mSettings[name]->getf();
		}		

		map<string, ISettingPtr> mSettings;
	};

	SettingsGroup mGPrms("global");

	float GETF(const string &name) {
		return mGPrms.getf(name);
	}

    class Growthable {
    public:
        virtual float getRadius() = 0;
        virtual bool childAtAngle(float childRadius, float angleRad) = 0;
        virtual void randomize(int channelID, float strenght) = 0;       
    };

    DECL_SHARED_PTR(GrowthableCircle7);
    DECL_WEAK_PTR(GrowthableCircle7);
    class GrowthableCircle7 : public Growthable, public Circles7 {	
		public:
        GrowthableCircle7(float radius, NodeWeakPtr pParent) : 
            Circles7(radius, pParent, false)
        {
			vector<PrimitiveStylePtr>::const_iterator it;
			for (it=mStyles.begin();it!=mStyles.end();it++)
				mAnimators.push_back(
					shared_ptr<IColorAnimator>(new t_softHueAnimator( &((*it)->mMainColor) )));
        };

		virtual ~GrowthableCircle7() {
		}
        
		list<shared_ptr<IColorAnimator> > mAnimators;

        virtual float getRadius() {
            vector<CircleWeakPtr>& pCircles = getCircles();
            return pCircles[0]->mRadius + pCircles[1]->mRadius;            
        };
        
        virtual bool childAtAngle(float childRadius, float angleRad) {            
            GrowthableCircle7WeakPtr pC7(new GrowthableCircle7(childRadius, this));
            //= NODE_MGR.createCircles7(childRadius, false, this);
            Vec2f pos = mPos;
            Vec2f dir(cos(angleRad), sin(angleRad));
            pC7->mPos = dir * (childRadius*2.f + getRadius());
            pC7->randomize(0,0);

			mAnimators.push_back(shared_ptr<IColorAnimator>(new t_softHueAnimator(&(pC7->mStyles[0]->mMainColor))));
            
            return true;
        };
        
        virtual void randomize(int channelID, float strenght) {
            for (int i=0;i<2;i++) {
                Color c;
                
                ColorAf &curC = mStyles[i]->mMainColor;
                Vec3f hsv;
                hsv = rgbToHSV( curC );
                
                float max = 1.f*0.55f;
                hsv.x += randFloat(-max, max);
                hsv.y += randFloat(-max, max);
                hsv.z = randFloat(0.25f, 0.45f);
                c.set(cinder::CM_HSV, hsv);
                
                curC = ColorAf(c.r, c.g, c.b, curC.a);
            }
        };
    };
    
    DECL_SHARED_PTR(GrowthState)
    class GrowthState : public GraphicAppState {
    public:
        virtual void init() {
            GraphicAppState::init();
            
			mGPrms.add("rootRadius",			new FloatSetting(20.f));
			mGPrms.add("childMinRadiusFactor",	new FloatSetting(1.f/5.f));
			mGPrms.add("childMaxRadiusFactor",	new FloatSetting(4.f));
			mGPrms.add("nbChilds",				new FloatSetting(48.f));

			mGPrms.init();
        };

        virtual void doCrystalGrowth() {
			float rootRadius = GETF("rootRadius");
            mpRootCrystal = new GrowthableCircle7(rootRadius, NODE_MGR.mpRoot);
            mpRootCrystal->mPos = Vec2f((float)getWindowWidth()/2.f,(float)getWindowHeight()/2);

			int cnt = (int)GETF("nbChilds");
            for (int i=0;i<cnt;i++) {
                mpRootCrystal->childAtAngle(
					Rand::randFloat(rootRadius * GETF("childMinRadiusFactor"), 
									rootRadius * GETF("childMaxRadiusFactor")), 
					Rand::randFloat(0,2.f*M_PIf));
            }       
        };

        virtual void destroyScene() {
			NodeMngr::mpRoot->deleteChilds();
        };

        virtual void deInit() {
            
        };
        
        virtual void update(float fDt) {
            GraphicAppState::postUpdate();
        };
        
        virtual void draw() {
            GraphicAppState::preDraw();
            gl::enableAdditiveBlending();
            GraphicAppState::postDraw();            
        };
        
		virtual void mouseDown( MouseEvent event )
		{
			GraphicAppState::mouseDown(event);
			destroyScene();
			doCrystalGrowth();
		}

        //virtual void mouseWheel( MouseEvent event );
        
        GrowthableCircle7WeakPtr mpRootCrystal;
    };
    
};