#pragma once

#include <math/Math.h>
#include <util/PiecewiseLinearFunction.h>
#include "Property.h"








struct Controller
{
	typedef std::shared_ptr<Controller> Ptr;
	virtual void update( Property::Ptr prop, float time)=0;
	virtual bool isAnimated()const=0;
};

template<typename T>
struct ControllerT : public Controller
{
	typedef std::shared_ptr<ControllerT<T>> Ptr;

	ControllerT() : Controller()
	{
	}

	virtual void update(Property::Ptr prop, float time)override
	{
		PropertyT<T>::Ptr propt = std::dynamic_pointer_cast<PropertyT<T>>(prop);
		if(propt)
			propt->set(evaluate(time));
	}

	virtual T evaluate(float time)=0;
	virtual bool isAnimated()const=0;
};

typedef ControllerT<float> FloatController;
typedef ControllerT<math::V3f> V3fController;
typedef ControllerT<math::M44f> M44fController;

template<typename T>
struct ConstantController : public ControllerT<T>
{
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


template<typename T>
struct CurveControllerT : public ControllerT<T>
{
	typedef std::shared_ptr<CurveControllerT> Ptr;
	CurveControllerT( base::PiecewiseLinearFunction<T>& curve ) : ControllerT<T>(), curve(curve), m_isAnimated(false)
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


struct PRSController : public M44fController
{
	typedef std::shared_ptr<PRSController> Ptr;

	PRSController( V3fController::Ptr translation, V3fController::Ptr rotation, V3fController::Ptr scale ) :
		M44fController(),
		translation(translation),
		rotation(rotation),
		scale(scale)
	{
	}
	static Ptr create(V3fController::Ptr translation, V3fController::Ptr rotation, V3fController::Ptr scale)
	{
		return std::make_shared<PRSController>(translation, rotation, scale);
	}
	math::M44f evaluate(float time)override
	{
		math::M44f t = math::M44f::TranslationMatrix(translation->evaluate(time));
		math::V3f r = rotation->evaluate(time);
		math::M44f rotationX = math::M44f::RotationMatrixX( -math::degToRad(r.x) );
		math::M44f rotationY = math::M44f::RotationMatrixY( -math::degToRad(r.y) );
		math::M44f rotationZ = math::M44f::RotationMatrixZ( -math::degToRad(r.z) );
		//TODO: scale
		return rotationX*rotationY*rotationZ*t;
	}
	virtual bool isAnimated()const override
	{
		return translation->isAnimated()||rotation->isAnimated()||scale->isAnimated();
	}

	V3fController::Ptr translation;
	V3fController::Ptr rotation;
	V3fController::Ptr scale;
};

struct SinusController : public FloatController
{
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

struct ProjectionMatrixController : public M44fController
{
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

struct FloatToV3fController : public V3fController
{
	typedef std::shared_ptr<FloatToV3fController> Ptr;

	FloatToV3fController(FloatController::Ptr x, FloatController::Ptr y, FloatController::Ptr z) : V3fController(), m_x(x), m_y(y), m_z(z)
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
		return std::make_shared<FloatToV3fController>(x,y,z);
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
