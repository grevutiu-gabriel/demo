#include "geoprimitives.h"


Grid::Grid() :
	GeometryController(),
	m_dirty(true)
{
}

base::Geometry::Ptr Grid::evaluate(float time)
{
	if(m_dirty)
	{
		m_geometry = base::Geometry::createGrid(2, 2);
		m_dirty = false;
	}
	return m_geometry;
}

bool Grid::isAnimated() const
{
	return true;
}

void Grid::serialize(Serializer &out)
{
	GeometryController::serialize(out);
}

void Grid::deserialize(Deserializer &in)
{
	GeometryController::deserialize(in);
}

REGISTERCLASS2( Grid, Controller )
