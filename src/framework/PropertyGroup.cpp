#include "PropertyGroup.h"








PropertyGroup::PropertyGroup( const std::string& name ):
	m_name(name)
{

}

PropertyGroup::~PropertyGroup()
{

}

std::string PropertyGroup::getName() const
{
	return m_name;
}

