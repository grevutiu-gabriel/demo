#include "math.h"


MultiplyFloat::MultiplyFloat() :
	FloatController(),
	m_x(0.0f),
	m_y(0.0f)
{
	addProperty<float>( "x", std::bind( &MultiplyFloat::getX, this ), std::bind( &MultiplyFloat::setX, this, std::placeholders::_1 ) );
	addProperty<float>( "y", std::bind( &MultiplyFloat::getY, this ), std::bind( &MultiplyFloat::setY, this, std::placeholders::_1 ) );

}

float MultiplyFloat::evaluate(float time)
{
	return m_x*m_y;
}

void MultiplyFloat::setX(float x)
{
	m_x = x;
}

float MultiplyFloat::getX()
{
	return m_x;
}

void MultiplyFloat::setY(float y)
{
	m_y = y;
}

float MultiplyFloat::getY()
{
	return m_y;
}


void MultiplyFloat::serialize(Serializer &out)
{
	FloatController::serialize(out);
	out.write("x", m_x);
	out.write("y", m_y);
}

void MultiplyFloat::deserialize(Deserializer &in)
{
	FloatController::deserialize(in);
	m_x = in.readFloat("x");
	m_y = in.readFloat("y");
}

REGISTERCLASS2( MultiplyFloat, Controller )
