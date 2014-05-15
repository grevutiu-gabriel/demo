#include "LoadGeometry.h"

#include <util/Path.h>

#include "../houdini/HouGeoIO.h"


base::Geometry::Ptr LoadGeometry::evaluate(float time)
{
	return m_geometry;
}

bool LoadGeometry::isAnimated()const
{
	return false;
}

void LoadGeometry::setFilename( const std::string& filename )
{
	// load the file immediately
	m_filename = filename;
	m_geometry = houdini::HouGeoIO::importGeometry(base::expand(filename));
}


std::string LoadGeometry::getFilename()
{
	return m_filename;
}

void LoadGeometry::serialize(Serializer &out)
{
	GeometryController::serialize(out);

	out.write( "filename", m_filename );
}

REGISTERCLASS( LoadGeometry )





