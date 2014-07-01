#pragma once

#include <map>
#include "Property.h"








class PropertyGroup
{
public:
	typedef std::shared_ptr<PropertyGroup> Ptr;

	PropertyGroup( const std::string& name );
	virtual ~PropertyGroup();

	std::string                          getName()const;
	virtual void                         getPropertyNames( std::vector<std::string>& names )=0;
	virtual Property::Ptr                getProperty( const std::string& name )=0;
	virtual bool                         hasProperty( const std::string& name )const=0;

	// will be called once objects have been deserialized
	// (required for ListPropertyGroups to allow synchronization
	// after vectors have been changed through deserialization)
	virtual void                         synchronize(){}
private:
	std::string                          m_name;
};


