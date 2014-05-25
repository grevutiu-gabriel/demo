#pragma once


#include <util/PiecewiseLinearFunction.h>
#include "../Controller.h"

















class FloatPLFController : public ControllerT<base::PiecewiseLinearFunction<float>::Ptr>
{
	OBJECT
public:
	typedef std::shared_ptr<FloatPLFController> Ptr;
	typedef base::PiecewiseLinearFunction<float> PLF;
	typedef base::PiecewiseLinearFunction<float>::Ptr PLFPTR;

	FloatPLFController();

	void addPoint( float x, float y );

	// overrides from object
	void serialize(Serializer &out);

	// overrides from Controller
	virtual PLFPTR evaluate(float time);
	virtual bool isAnimated()const;


private:
	PLFPTR m_plf;
};




class V3fPLFController : public ControllerT<base::PiecewiseLinearFunction<math::V3f>::Ptr>
{
	OBJECT
public:
	typedef std::shared_ptr<V3fPLFController> Ptr;
	typedef base::PiecewiseLinearFunction<math::V3f> PLF;
	typedef base::PiecewiseLinearFunction<math::V3f>::Ptr PLFPTR;

	V3fPLFController();

	void addPoint( float x, math::V3f value );

	// overrides from object
	void serialize(Serializer &out);

	// overrides from Controller
	virtual PLFPTR evaluate(float time);
	virtual bool isAnimated()const;


private:
	PLFPTR m_plf;
};

