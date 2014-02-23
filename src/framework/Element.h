#pragma once

#include <util/shared_ptr.h>
#include <gfx/Context.h>
#include "Property.h"



struct Object
{
	Object()
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


	void print( std::ostream& out )const
	{
		for(auto it = m_props.begin(), end=m_props.end();it!=end;++it)
			it->second->print( out );
	}

private:
	std::map<std::string, Property::Ptr> m_props;
};


struct Element : public Object
{
	typedef std::shared_ptr<Element> Ptr;


	virtual void begin(base::Context::Ptr context)
	{
	}
	virtual void end(base::Context::Ptr context)
	{
	}
	virtual void render(base::Context::Ptr context)
	{
	}


};
