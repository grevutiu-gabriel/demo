#pragma once

#include <math/Math.h>
#include <util/PiecewiseLinearFunction.h>
#include "Object.h"


#include <gfx/Camera.h>
#include <gfx/Geometry.h>





class Controller : public Object
{
public:
	typedef std::shared_ptr<Controller> Ptr;
	virtual void update( Property::Ptr prop, float time)=0;
	virtual bool isAnimated()const=0;
};

template<typename T>
class ControllerT : public Controller
{
public:
	typedef std::shared_ptr<ControllerT<T>> Ptr;

	ControllerT() : Controller()
	{
	}

	virtual void update(Property::Ptr prop, float time)override
	{
		PropertyT<T>::Ptr propt = std::dynamic_pointer_cast<PropertyT<T>>(prop);
		if(propt)
		{
			propt->set(evaluate(time));
		}
	}

	virtual T evaluate(float time)=0;
	virtual bool isAnimated()const=0;
};

typedef ControllerT<float> FloatController;
typedef ControllerT<math::V3f> V3fController;
typedef ControllerT<math::M44f> M44fController;
typedef ControllerT<base::Camera::Ptr> CameraController;
typedef ControllerT<base::Geometry::Ptr> GeometryController;


template<typename T>
class ConstantController : public ControllerT<T>
{
public:
	typedef std::shared_ptr<ConstantController<T>> Ptr;

	ConstantController( T value ) : ControllerT<T>(), m_value(value)
	{
	}

	static Ptr create( T value )
	{
		return std::make_shared<ConstantController<T>>(value);
	}

	virtual T evaluate(float time)override
	{
		return m_value;
	}
	virtual bool isAnimated()const override
	{
		return false;
	}

private:
	T m_value;
};

typedef ConstantController<float> ConstantFloatController;
typedef ConstantController<math::V3f> ConstantV3fController;
typedef ConstantController<math::M44f> ConstantM44fController;
typedef ConstantController<base::Camera::Ptr> ConstantCameraController;


template<typename T>
class CurveControllerTOld : public ControllerT<T>
{
public:
	typedef std::shared_ptr<CurveControllerTOld> Ptr;
	CurveControllerTOld( base::PiecewiseLinearFunction<T>& curve ) : ControllerT<T>(), curve(curve), m_isAnimated(false)
	{
		float miny, maxy;
		curve.getValueRange(miny, maxy);
		if( abs(miny-maxy)>0.0f )
			m_isAnimated = true;
	}
	static Ptr create( base::PiecewiseLinearFunction<T>& curve )
	{
		return std::make_shared<CurveControllerTOld<T>>(curve);
	}
	T evaluate(float time)override
	{
		return curve.evaluate(time);
	}
	virtual bool isAnimated()const override
	{
		return m_isAnimated;
	}

	base::PiecewiseLinearFunction<T> curve;

private:
	bool m_isAnimated;
};

typedef CurveControllerTOld<float> CurveFloatControllerOld;

template<typename T>
class CurveControllerT : public ControllerT<T>
{
public:
	typedef std::shared_ptr<CurveControllerT> Ptr;
	CurveControllerT( base::PiecewiseLinearFunction<T>& curve ) :
		ControllerT<T>(),
		curve(curve),
		m_isAnimated(false)
	{
		float miny, maxy;
		curve.getValueRange(miny, maxy);
		if( abs(miny-maxy)>0.0f )
			m_isAnimated = true;
	}
	static Ptr create( base::PiecewiseLinearFunction<T>& curve )
	{
		return std::make_shared<CurveControllerT<T>>(curve);
	}
	T evaluate(float time)override
	{
		return curve.evaluate(time);
	}
	virtual bool isAnimated()const override
	{
		return m_isAnimated;
	}

	base::PiecewiseLinearFunction<T> curve;

private:
	bool m_isAnimated;
};

typedef CurveControllerT<float> CurveFloatController;


class PRSController : public M44fController
{
	OBJECT
public:
	typedef std::shared_ptr<PRSController> Ptr;

	PRSController();

	static Ptr create()
	{
		return std::make_shared<PRSController>();
	}

	math::M44f evaluate(float time)override
	{
		//std::cout << "PRSController::evaluate " << m_tx << " " << m_ty << " " << m_tz << std::endl;
		math::M44f t = math::M44f::TranslationMatrix(m_tx, m_ty, m_tz);
		math::M44f rotationX = math::M44f::RotationMatrixX( -math::degToRad(m_rx) );
		math::M44f rotationY = math::M44f::RotationMatrixY( -math::degToRad(m_ry) );
		math::M44f rotationZ = math::M44f::RotationMatrixZ( -math::degToRad(m_rz) );
		//TODO: scale
		return rotationX*rotationY*rotationZ*t;
	}
	virtual bool isAnimated()const override
	{
		return true;
	}


	float getTx() const;
	void setTx(float tx);

	float getTy() const;
	void setTy(float ty);

	float getTz() const;
	void setTz(float tz);

	float getRx() const;
	void setRx(float rx);

	float getRy() const;
	void setRy(float ry);

	float getRz() const;
	void setRz(float rz);

	float getSx() const;
	void setSx(float sx);

	float getSy() const;
	void setSy(float sy);

	float getSz() const;
	void setSz(float sz);

private:
	float m_tx, m_ty, m_tz; // translation
	float m_rx, m_ry, m_rz; // rotation
	float m_sx, m_sy, m_sz; // scale
};

class SinusController : public FloatController
{
	OBJECT
public:
	typedef std::shared_ptr<SinusController> Ptr;

	SinusController() : FloatController()
	{
	}
	static Ptr create()
	{
		return std::make_shared<SinusController>();
	}

	float evaluate(float time)override
	{
		return sin(time);
	}
	virtual bool isAnimated()const override
	{
		return true;
	}
};

class ProjectionMatrixController : public M44fController
{
public:
	typedef std::shared_ptr<ProjectionMatrixController> Ptr;

	ProjectionMatrixController( FloatController::Ptr fovy, FloatController::Ptr aspect ) : M44fController(), fovy(fovy), aspect(aspect)
	{
	}
	static Ptr create( FloatController::Ptr fovy, FloatController::Ptr aspect )
	{
		return std::make_shared<ProjectionMatrixController>(fovy, aspect);
	}

	math::M44f evaluate(float time)override
	{
		return math::projectionMatrix( fovy->evaluate(time), aspect->evaluate(time), 0.1f, 1000.0f);
	}
	virtual bool isAnimated()const override
	{
		return fovy->isAnimated() || aspect->isAnimated();
	}

	FloatController::Ptr fovy;
	FloatController::Ptr aspect;

};

class FloatToV3fControllerOld : public V3fController
{
public:
	typedef std::shared_ptr<FloatToV3fControllerOld> Ptr;

	FloatToV3fControllerOld(FloatController::Ptr x, FloatController::Ptr y, FloatController::Ptr z) : V3fController(), m_x(x), m_y(y), m_z(z)
	{
		if(!m_x)
			m_x = ConstantFloatController::create(0.0f);
		if(!m_y)
			m_y = ConstantFloatController::create(0.0f);
		if(!m_z)
			m_z = ConstantFloatController::create(0.0f);
	}
	static Ptr create( FloatController::Ptr x, FloatController::Ptr y, FloatController::Ptr z )
	{
		return std::make_shared<FloatToV3fControllerOld>(x,y,z);
	}

	math::V3f evaluate(float time)override
	{
		return math::V3f(m_x->evaluate(time), m_y->evaluate(time), m_z->evaluate(time));
	}
	virtual bool isAnimated()const override
	{
		return m_x->isAnimated()||m_y->isAnimated()||m_z->isAnimated();
	}

	FloatController::Ptr m_x,m_y,m_z;
};


class FloatToV3fController : public V3fController
{
	OBJECT
public:
	typedef std::shared_ptr<FloatToV3fController> Ptr;

	FloatToV3fController() :
		V3fController(),
		m_x(0.0f),
		m_y(0.0f),
		m_z(0.0f)
	{
		addProperty<float>( "x", std::bind( &FloatToV3fController::getX, this ), std::bind( &FloatToV3fController::setX, this, std::placeholders::_1 ) );
		addProperty<float>( "y", std::bind( &FloatToV3fController::getY, this ), std::bind( &FloatToV3fController::setY, this, std::placeholders::_1 ) );
		addProperty<float>( "z", std::bind( &FloatToV3fController::getZ, this ), std::bind( &FloatToV3fController::setZ, this, std::placeholders::_1 ) );
	}
	static Ptr create()
	{
		return std::make_shared<FloatToV3fController>();
	}

	math::V3f evaluate(float time)override
	{
		return math::V3f(m_x, m_y, m_z);
	}
	virtual bool isAnimated()const override
	{
		return true;
	}

	void setX( float x ){m_x = x;}
	float getX()const{return m_x;}
	void setY( float y ){m_y = y;}
	float getY()const{return m_y;}
	void setZ( float z ){m_z = z;}
	float getZ()const{return m_z;}

	float m_x, m_y, m_z;
};

