#include "houGeoIO.h"
#include "houGeo.h"

#include <fstream>


namespace houdini
{


	// HouGeoAdapter::RawPointer ==================================================
	HouGeoAdapter::RawPointer::~RawPointer()
	{
		if(freeOnDestruction)
			free(ptr);
	}

	HouGeoAdapter::RawPointer::Ptr HouGeoAdapter::RawPointer::create( void *ptr, bool freeOnDestruction )
	{
		HouGeoAdapter::RawPointer *rp = new HouGeoAdapter::RawPointer();
		rp->ptr = ptr;
		rp->freeOnDestruction = freeOnDestruction;
		return HouGeoAdapter::RawPointer::Ptr(rp);
	}

	// HouGeoAdapter::Attribute ==================================================
	HouGeoAdapter::Attribute::~Attribute()
	{
	}

	std::string HouGeoAdapter::Attribute::getName()const
	{
		return "";
	}

	HouGeoAdapter::Attribute::Type HouGeoAdapter::Attribute::getType()const
	{
		return ATTR_TYPE_INVALID;
	}

	int HouGeoAdapter::Attribute::getTupleSize()const
	{
		return 0;
	}

	HouGeoAdapter::Attribute::Storage HouGeoAdapter::Attribute::getStorage()const
	{
		return ATTR_STORAGE_INVALID;
	}

	void HouGeoAdapter::Attribute::getPacking( std::vector<int> &packing )const
	{
	}

	int HouGeoAdapter::Attribute::getNumElements()const
	{
		return 0;
	}

	HouGeoAdapter::RawPointer::Ptr HouGeoAdapter::Attribute::getRawPointer()
	{
		return HouGeoAdapter::RawPointer::Ptr();
	}

	HouGeoAdapter::Attribute::Type HouGeoAdapter::Attribute::type( const std::string &typeName )
	{
		if( typeName == "numeric" )
			return ATTR_TYPE_NUMERIC;
		else
		if( typeName == "string" )
			return ATTR_TYPE_STRING;
		return ATTR_TYPE_INVALID;
	}

	HouGeoAdapter::Attribute::Storage HouGeoAdapter::Attribute::storage( const std::string &storageName )
	{
		if( storageName == "fpreal32" )
			return ATTR_STORAGE_FPREAL32;
		else
		if( storageName == "fpreal64" )
			return ATTR_STORAGE_FPREAL64;
		else
		if( storageName == "int32" )
			return ATTR_STORAGE_INT32;
		return ATTR_STORAGE_INVALID;
	}

	int HouGeoAdapter::Attribute::storageSize( Storage storage )
	{
		switch(storage)
		{
		case ATTR_STORAGE_FPREAL32:return sizeof(float);break;
		case ATTR_STORAGE_FPREAL64:return sizeof(double);break;
		case ATTR_STORAGE_INT32:return sizeof(int);break;
		default: 0;break;
		};
		return 0;
	}

	// HouGeoAdapter::Topology ==================================================

	HouGeoAdapter::Topology::~Topology()
	{
	}


	// HouGeoAdapter::VolumePrimitive ==================================================


	math::Vec3i HouGeoAdapter::VolumePrimitive::getResolution()const
	{
		return math::Vec3i(0,0,0);
	}

	// returns raw pointer to the data
	HouGeoAdapter::RawPointer::Ptr HouGeoAdapter::VolumePrimitive::getRawPointer()
	{
		return HouGeoAdapter::RawPointer::Ptr();
	}

	// HouGeoAdapter::PolyPrimitive =====================================================

	int HouGeoAdapter::PolyPrimitive::numPolys()const
	{
		return 0;
	}

	int HouGeoAdapter::PolyPrimitive::numVertices( int poly )const
	{
		return 0;
	}

	int const *HouGeoAdapter::PolyPrimitive::vertices()const
	{
		return 0;
	}




	// HouGeoAdapter ==================================================
	sint64 HouGeoAdapter::pointcount()const
	{
		return 0;
	}

	sint64 HouGeoAdapter::vertexcount()const
	{
		return 0;
	}

	sint64 HouGeoAdapter::primitivecount()const
	{
		return 0;
	}


	void HouGeoAdapter::getPointAttributeNames( std::vector<std::string> &names )const
	{
	}

	HouGeoAdapter::Attribute::Ptr HouGeoAdapter::getPointAttribute( const std::string &name )
	{
		return Attribute::Ptr();
	}

	void HouGeoAdapter::getGlobalAttributeNames( std::vector<std::string> &names )const
	{
	}

	HouGeoAdapter::Attribute::Ptr HouGeoAdapter::getGlobalAttribute( const std::string &name )
	{
		return Attribute::Ptr();
	}

	HouGeoAdapter::Primitive::Ptr HouGeoAdapter::getPrimitive( int index )
	{
		return HouGeoAdapter::Primitive::Ptr();
	}

	HouGeoAdapter::Topology::Ptr HouGeoAdapter::getTopology()
	{
		return HouGeoAdapter::Topology::Ptr();
	}








} // namespace houdini
