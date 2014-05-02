#pragma once

#include <map>
#include "Property.h"



class Object
{
public:
	typedef std::shared_ptr<Object> Ptr;
	typedef std::vector<Ptr> ChildList;

	Object()
	{
	}
	virtual ~Object()
	{
	}



	template<typename T>
	void addProperty( const std::string& name, typename PropertyT<T>::Getter get, typename PropertyT<T>::Setter set)
	{
		PropertyT<T>::Ptr prop = PropertyT<T>::create(name, get, set);

		if( !hasProperty(name) )
		{
			m_props[name] = prop;
		}else
		{
			// if property exists already, we do what?
		}
	}
	template<typename T>
	void addRefProperty( const std::string& name, typename RefPropertyT<T>::Getter get, typename RefPropertyT<T>::Setter set)
	{
		RefPropertyT<T>::Ptr prop = RefPropertyT<T>::create(name, get, set);

		if( !hasProperty(name) )
		{
			m_props[name] = prop;
		}else
		{
			// if property exists already, we do what?
		}
	}


	Property::Ptr getProperty( const std::string& name )
	{
		auto it = m_props.find( name );
		if( it!=m_props.end() )
			return it->second;
		return Property::Ptr();
	}

	bool hasProperty( const std::string& name )const
	{
		return m_props.find( name ) != m_props.end();
	}

	ChildList& getChildren()
	{
		return m_childs;
	}

	void print( std::ostream& out )const
	{
		for(auto it = m_props.begin(), end=m_props.end();it!=end;++it)
			it->second->print( out );
	}

private:
	std::map<std::string, Property::Ptr> m_props;
	std::vector<Ptr>                     m_childs;
};

