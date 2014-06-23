#pragma once


#include <math/math.h>
#include "../Controller.h"












class MultiplyFloat : public FloatController
{
	OBJECT
public:
	typedef std::shared_ptr<MultiplyFloat> Ptr;

	MultiplyFloat();

	// overrides from GeometryController
	virtual float evaluate(float time);

	void setX( float x );
	float getX();
	void setY( float y );
	float getY();


	void serialize(Serializer &out);
	void deserialize(Deserializer &in);
private:
	float m_x;
	float m_y;
};
