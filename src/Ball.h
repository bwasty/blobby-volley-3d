/// Ball.h: declare class BV3D::Ball

#ifndef BV3D_BALL
#define BV3D_BALL

#include <vrs/sharedobj.h>
#include <vrs/sg/scenething.h>

namespace BV3D
{
	class Ball : public VRS::SharedObj
	{
	public:
		Ball();
		~Ball();
		VRS::SO<VRS::SceneThing> getScene() {return m_Scene;}
		void setTransformation(VRS::Matrix matrix);
		double getRadius() {return m_radius;}
	private:
		double	m_radius;
		VRS::SO<VRS::SceneThing>	m_Scene;
	};
}

#endif	// BV3D_BALL