#include "PLFController.h"
#include <util/StringManip.h>




FloatPLFController::PLFPTR FloatPLFController::evaluate(float time)
{
	return m_plf;
}

bool FloatPLFController::isAnimated()const
{
	return true;
}


void FloatPLFController::serialize(Serializer &out)
{
	Controller::serialize(out);
}



FloatPLFController::FloatPLFController()
{
	m_plf = std::make_shared<base::PiecewiseLinearFunction<float>>();
}

void FloatPLFController::addPoint(float x, float y)
{
	int index = m_plf->getNumPoints();
	m_plf->addSample( x, y );
	// add properties for controlling sample position and value
	std::string pointName = "point"+base::toString(index);
	addProperty<float>( pointName + ".x", PropertyT<float>::Getter(), std::bind( static_cast<void(PLF::*)(int, float)>(&PLF::setPosition), m_plf, index, std::placeholders::_1 ) );
	addProperty<float>( pointName + ".y", PropertyT<float>::Getter(), std::bind( static_cast<void(PLF::*)(int, float)>(&PLF::setValue), m_plf, index, std::placeholders::_1 ) );
}

REGISTERCLASS( FloatPLFController )




// --------------------------------------------



V3fPLFController::PLFPTR V3fPLFController::evaluate(float time)
{
	return m_plf;
}

bool V3fPLFController::isAnimated()const
{
	return true;
}


void V3fPLFController::serialize(Serializer &out)
{
	Controller::serialize(out);
}



V3fPLFController::V3fPLFController()
{
	m_plf = std::make_shared<base::PiecewiseLinearFunction<math::V3f>>();
}

void V3fPLFController::addPoint(float x, math::V3f value)
{
	int index = m_plf->getNumPoints();
	m_plf->addSample( x, value );
	// add properties for controlling sample position and value
	std::string pointName = "point"+base::toString(index);
	addProperty<float>( pointName + ".x", PropertyT<float>::Getter(), std::bind( static_cast<void(PLF::*)(int, float)>(&PLF::setPosition), m_plf, index, std::placeholders::_1 ) );
	addProperty<math::V3f>( pointName + ".value", PropertyT<math::V3f>::Getter(), std::bind( static_cast<void(PLF::*)(int, math::V3f)>(&PLF::setValue), m_plf, index, std::placeholders::_1 ) );
}

REGISTERCLASS( V3fPLFController )








