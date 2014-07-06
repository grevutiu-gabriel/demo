#include "LoadGeometry.h"

#include <util/Path.h>
#include <util/fs.h>

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
	m_filename = filename;

	if(!base::fs::exists(base::expand(m_filename)))
		return;

	// load houdini file ================
	m_geometry = houdini::HouGeoIO::importGeometry(base::expand(m_filename));

	if(!m_geometry)
	{
		std::cout << "unable to load " << m_filename << std::endl;std::flush(std::cout);
		return;
	}
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

void LoadGeometry::deserialize(Deserializer &in)
{
	GeometryController::deserialize(in);
	setFilename( in.readString("filename") );
}


REGISTERCLASS2( LoadGeometry, Controller )





