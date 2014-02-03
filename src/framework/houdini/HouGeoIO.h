#pragma once
#include "HouGeo.h"

#include <gfx/Geometry.h>

namespace houdini
{
	struct HouGeoIO
	{
		//static void      exportGeo( const std::string &path, Geometry::Ptr geo );
		//static Geometry::Ptr                  loadGeo( const std::string &path );
		static HouGeo::Ptr                      import( std::istream *in );
		static bool                             xport(std::ostream *out, HouGeoAdapter::Ptr geo , bool binary = true);
		static void                             makeLog( const std::string &path, std::ostream *out );

		static base::Geometry::Ptr              convertToGeometry( HouGeo::Ptr houGeo, int prim ); // converts primitive with the given index to geometry

	private:
		static bool                             exportAttribute( HouGeoAdapter::Attribute::Ptr attr );
		static bool                             exportTopology( HouGeoAdapter::Topology::Ptr topo );
		static bool                             exportPrimitive( HouGeoAdapter::VolumePrimitive::Ptr volume );
		static HouGeoAdapter*                   g_geo;
		static json::BinaryWriter*              g_writer;
	};
}
