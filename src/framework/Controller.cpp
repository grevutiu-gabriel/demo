#include "Controller.h"





PRSController::PRSController(V3fController::Ptr translation, V3fController::Ptr rotation, V3fController::Ptr scale):
	   M44fController(),
	   translation(translation),
	   rotation(rotation),
	   scale(scale)
{
	addRefProperty<V3fController>( "translation", std::bind( &PRSController::getTranslation, this ), std::bind( &PRSController::setTranslation, this, std::placeholders::_1 ) );
}

V3fController::Ptr PRSController::getTranslation() const
{
	return translation;
}

void PRSController::setTranslation(const V3fController::Ptr &value)
{
	translation = value;
}

REGISTERCLASS( SinusController )
REGISTERCLASS( FloatToV3fController )
