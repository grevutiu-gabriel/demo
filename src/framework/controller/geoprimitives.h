#pragma once


#include <gfx/Geometry.h>
#include "../Controller.h"












class Grid : public GeometryController
{
	OBJECT
public:
	typedef std::shared_ptr<Grid> Ptr;

	Grid();

	// overrides from GeometryController
	virtual base::Geometry::Ptr evaluate(float time);
	virtual bool isAnimated()const;

	//void setFilename( const std::string& filename );
	//std::string getFilename();


	void serialize(Serializer &out);
	void deserialize(Deserializer &in);
private:
	bool                m_dirty;
	base::Geometry::Ptr m_geometry;
};
