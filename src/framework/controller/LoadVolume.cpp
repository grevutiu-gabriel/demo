#include "LoadVolume.h"

#include <util/Path.h>

#include "../houdini/HouGeoIO.h"



base::Texture3d::Ptr LoadVolume::evaluateTexture3d(float time)
{
	return m_texture3d;
}

base::ScalarField::Ptr LoadVolume::evaluateScalarField(float time)
{
	return m_field;
}

math::M44f LoadVolume::evaluateLocalToWorld(float time)
{
	return m_field->m_localToWorld;
}

void LoadVolume::update(Property::Ptr prop, float time)
{
	//PropertyT<base::ScalarField::Ptr>::Ptr propField = std::dynamic_pointer_cast<PropertyT<base::ScalarField::Ptr>>(prop);
	PropertyT<base::Texture3d::Ptr>::Ptr propTexture3d = std::dynamic_pointer_cast<PropertyT<base::Texture3d::Ptr>>(prop);
	PropertyT<math::M44f>::Ptr propLocalToWorld = std::dynamic_pointer_cast<PropertyT<math::M44f>>(prop);
	if(propTexture3d)
	{
		propTexture3d->set(evaluateTexture3d(time));
	}else
	if(propLocalToWorld)
	{
		propLocalToWorld->set(evaluateLocalToWorld(time));
	}
}

bool LoadVolume::isAnimated()const
{
	return false;
}

void LoadVolume::setFilename( const std::string& filename )
{
	// load the file immediately
	m_filename = filename;

	m_field = base::ScalarField::Ptr();

	// load houdini file ================
	std::ifstream in( base::expand(m_filename).c_str(), std::ios_base::in | std::ios_base::binary );
	houdini::HouGeo::Ptr hgeo = houdini::HouGeoIO::import( &in );
	if( hgeo )
	{
		int primIndex = 0;
		houdini::HouGeo::Primitive::Ptr prim = hgeo->getPrimitive(primIndex);

		//Volume
		if(std::dynamic_pointer_cast<houdini::HouGeo::HouVolume>(prim) )
		{
			houdini::HouGeo::HouVolume::Ptr volprim = std::dynamic_pointer_cast<houdini::HouGeo::HouVolume>(prim);
			m_normalizedDensity = volprim->field;
		}
	}

	if(!m_field)
		std::cerr << "unable to load " << filename << std::endl;


	// find density value range ---
	float densityMin=std::numeric_limits<float>::infinity();
	float densityMax=-std::numeric_limits<float>::infinity();
	for( int k = 0; k<m_field->m_resolution.z;++k )
		for( int j = 0; j<m_field->m_resolution.y;++j )
			for( int i = 0; i<m_field->m_resolution.x;++i )
			{
				float value = m_field->lvalue(i,j,k);
				densityMax = std::max(densityMax, value );
				densityMin = std::min(densityMin, value );
			}

	// normalize density ---
	for( int k = 0; k<m_normalizedDensity->m_resolution.z;++k )
		for( int j = 0; j<m_normalizedDensity->m_resolution.y;++j )
			for( int i = 0; i<m_normalizedDensity->m_resolution.x;++i )
			{
				float value = math::mapValueTo0_1(densityMin, densityMax, m_field->lvalue(i,j,k));
				m_field->lvalue(i,j,k) = value;
			}

	std::cout << "Volume::load: densityMax " << densityMax << std::endl;
	m_texture3d->uploadFloat32( m_field->m_resolution.x, m_field->m_resolution.y, m_field->m_resolution.z, m_field->getRawPointer() );
}


std::string LoadVolume::getFilename()
{
	return m_filename;
}

void LoadVolume::serialize(Serializer &out)
{
	Controller::serialize(out);

	out.write( "filename", m_filename );
}


REGISTERCLASS( LoadVolume )





