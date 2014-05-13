#include "Controller.h"





PRSController::PRSController():
	   M44fController()
{

	addProperty<float>( "tx", std::bind( &PRSController::getTx, this ), std::bind( &PRSController::setTx, this, std::placeholders::_1 ) );
	addProperty<float>( "ty", std::bind( &PRSController::getTy, this ), std::bind( &PRSController::setTy, this, std::placeholders::_1 ) );
	addProperty<float>( "tz", std::bind( &PRSController::getTz, this ), std::bind( &PRSController::setTz, this, std::placeholders::_1 ) );
	addProperty<float>( "rx", std::bind( &PRSController::getRx, this ), std::bind( &PRSController::setRx, this, std::placeholders::_1 ) );
	addProperty<float>( "ry", std::bind( &PRSController::getRy, this ), std::bind( &PRSController::setRy, this, std::placeholders::_1 ) );
	addProperty<float>( "rz", std::bind( &PRSController::getRz, this ), std::bind( &PRSController::setRz, this, std::placeholders::_1 ) );
	addProperty<float>( "sx", std::bind( &PRSController::getSx, this ), std::bind( &PRSController::setSx, this, std::placeholders::_1 ) );
	addProperty<float>( "sy", std::bind( &PRSController::getSy, this ), std::bind( &PRSController::setSy, this, std::placeholders::_1 ) );
	addProperty<float>( "sz", std::bind( &PRSController::getSz, this ), std::bind( &PRSController::setSz, this, std::placeholders::_1 ) );

}
float PRSController::getTx() const
{
	return m_tx;
}

void PRSController::setTx(float tx)
{
	m_tx = tx;
}
float PRSController::getTy() const
{
	return m_ty;
}

void PRSController::setTy(float ty)
{
	m_ty = ty;
}
float PRSController::getTz() const
{
	return m_tz;
}

void PRSController::setTz(float tz)
{
	m_tz = tz;
}
float PRSController::getRx() const
{
	return m_rx;
}

void PRSController::setRx(float rx)
{
	m_rx = rx;
}
float PRSController::getRy() const
{
	return m_ry;
}

void PRSController::setRy(float ry)
{
	m_ry = ry;
}
float PRSController::getRz() const
{
	return m_rz;
}

void PRSController::setRz(float rz)
{
	m_rz = rz;
}
float PRSController::getSx() const
{
	return m_sx;
}

void PRSController::setSx(float sx)
{
	m_sx = sx;
}
float PRSController::getSy() const
{
	return m_sy;
}

void PRSController::setSy(float sy)
{
	m_sy = sy;
}
float PRSController::getSz() const
{
	return m_sz;
}

void PRSController::setSz(float sz)
{
	m_sz = sz;
}











REGISTERCLASS( SinusController )
REGISTERCLASS( FloatToV3fController )
REGISTERCLASS( PRSController )
