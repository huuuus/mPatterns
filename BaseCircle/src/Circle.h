#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

//---------------------------------------------------------------------------
#define CIRCLE_MGR CircleMngr::instance()

//---------------------------------------------------------------------------
const int CIRCLE_DEFAULT_STROKE = 4;

//---------------------------------------------------------------------------
class Axis;
class Circle;
class Style;

typedef boost::shared_ptr<Circle> CirclePtr;
typedef boost::shared_ptr<Axis> AxisPtr;
typedef boost::shared_ptr<Style> StylePtr;

enum NODE_TYPE {
	NT_ROOT,
	NT_CIRCLE,
	NT_AXIS
};

/*enum FX_STYLE_FLAGS {
	GLOW_ADDITIVE,
};
*/
class Style
{
public:
	Style() {
	};

	Color mMainColor;
	Color mSecondColor;
};

template<class CPos>
class Node;

typedef const Node<Vec2f>* NodeConstPtr;
typedef boost::shared_ptr<Node<Vec2f> > NodePtr;

typedef std::vector<NodePtr>::const_iterator const_itNodeVec;
typedef std::list<NodePtr>::const_iterator const_itNodeList;

template<class CPos>
class Node {
public:
	Node(CPos p, NodeConstPtr pParent) : mPos(p) , mpParent(pParent) 
	{
	};

	CPos mPos;
	NodeConstPtr mpParent;
	std::list<NodePtr> mChilds;

	virtual enum NODE_TYPE getType() = 0;
	virtual void draw() const = 0;

	void addChild(NodePtr pChild) {mChilds.push_back(pChild);};

	CPos mPosWorld;
};

//---------------------------------------------------------------------------
class Root : public Node<Vec2f> {
public:
	Root() : Node<Vec2f>(Vec2f(0,0), NULL) {};
	virtual void draw() const {};
	virtual enum NODE_TYPE getType() {return NT_ROOT;};
	virtual Style* getStyle() const {return 0;};
};

typedef boost::shared_ptr<Node<Vec2f> > RootPtr;

//---------------------------------------------------------------------------
class CircleMngr {
public:
	CircleMngr() {};

	static CircleMngr& instance() {static CircleMngr inst; return inst;};
	static CirclePtr createCircle(Vec2f pos, Color c, float r, NodePtr pParent = mpRoot, Style* s=0);
	
	static CirclePtr spawnChildOnCircleAxis(CirclePtr pCircle, unsigned int axis, float distInRadiusUnits, float radius, Style* s=0);

	static RootPtr mpRoot;

	static void draw(NodePtr pNode=mpRoot);
	static void updateNodes(NodePtr pNode = mpRoot, Vec2f curPos = Vec2f(0,0));	
};

//---------------------------------------------------------------------------
class hasStyle {
public:
	hasStyle() {
		mStyle = 0;
	};

	Style* mStyle;	

	Style* getStyle() const {return mStyle;};
	void setStyle(Style* s) {mStyle=s;};
};

//---------------------------------------------------------------------------
class Circle : public Node<Vec2f>, public hasStyle {
public:
	Circle(Vec2f pos, Color c, float r, NodeConstPtr pParent);

	float mRadius;
	Color mColor;

	virtual void draw() const;
	void addAxis(float angle, Style* s=0);
	void setAxis(const Axis* pAxis);

	void positionAlongAxis(float dist);
		
	std::vector<Axis*> mAxises;

	const Axis* mpAxis;

	virtual enum NODE_TYPE getType() {return NT_CIRCLE;};
};

//---------------------------------------------------------------------------
class Axis : public Node<Vec2f>, public hasStyle {
public:
	Axis(Vec2f pos, float angle, NodeConstPtr pParent) : 
		Node(pos, pParent)
	{
		mDir.set(cos(angle),-sin(angle));
	};

	virtual void draw() const {
		if (getStyle())
			gl::color( getStyle()->mMainColor );
		else
			gl::color( Color(255,0,0) );

		//gl::color( Color(64/255.f,64/255.f,255/255.f) );
//		glLineWidth( 1.f );
		const float LONG_DIST = 5000.f;
		gl::drawLine(	mpParent->mPosWorld - mDir*LONG_DIST, 
						mpParent->mPosWorld + mDir*LONG_DIST);
//		glLineWidth( 1.0f );
	}

	Vec2f mDir;
	virtual enum NODE_TYPE getType() {return NT_AXIS;};

	
};