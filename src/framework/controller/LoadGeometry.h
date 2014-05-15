#pragma once


#include <gfx/Geometry.h>
#include "../Controller.h"














class LoadGeometry : public GeometryController
{
	OBJECT
public:
	typedef std::shared_ptr<LoadGeometry> Ptr;

	// overrides from GeometryController
	virtual base::Geometry::Ptr evaluate(float time);
	virtual bool isAnimated()const;

	void setFilename( const std::string& filename );
	std::string getFilename();


	void serialize(Serializer &out);
private:
	base::Geometry::Ptr m_geometry;
	std::string         m_filename;
};
