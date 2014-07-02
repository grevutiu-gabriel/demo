#pragma once


#include "PropertyGroup.h"

#include <util/StringManip.h>







class ListProperty : public PropertyGroup
{
public:
	typedef std::shared_ptr<ListProperty> Ptr;
	ListProperty(const std::string& name);
	virtual ~ListProperty();

	virtual Property::Ptr                addProperty()=0;
	virtual void                         removeProperty( const std::string& name )=0;

	// overrides from propertgroup
//	virtual void                         getPropertyNames( std::vector<std::string>& names )=0;
//	virtual Property::Ptr                getProperty( const std::string& name )=0;
//	virtual bool                         hasProperty( const std::string& name )const=0;
};


template<typename T>
class ListPropertyT : public ListProperty
{
public:
	ListPropertyT( const std::string& name, std::vector<T>* vector );
	~ListPropertyT();

	// overrides from propertygroup
	virtual void                         synchronize()override;

	// overrides from listproperty
	virtual Property::Ptr                addProperty()override;
	virtual void                         removeProperty( const std::string& name )override;
	// overrides from propertgroup
	virtual void                         getPropertyNames( std::vector<std::string>& names )override;
	virtual Property::Ptr                getProperty( const std::string& name )override;
	virtual bool                         hasProperty( const std::string& name )const override;

private:
	Property::Ptr                        createPropertyForIndex( int index );
	std::vector<T>*                      m_vector;
	std::vector<Property::Ptr>           m_props;
};

template<typename T>
ListPropertyT<T>::ListPropertyT(const std::string &name, std::vector<T>* vector ):
	ListProperty(name),
	m_vector(vector)
{
	synchronize();
}

template<typename T>
ListPropertyT<T>::~ListPropertyT()
{

}

template<typename T>
Property::Ptr ListPropertyT<T>::addProperty()
{
	int index = int(m_props.size());
	// add element
	m_vector->push_back( T() );

	// create property which wraps the new element
	Property::Ptr prop = createPropertyForIndex(index);
	m_props.push_back(prop);

	return prop;
}

template<typename T>
void ListPropertyT<T>::removeProperty(const std::string &name)
{
	int removeIndex = -1;
	for( int i=0, numElements=int(m_props.size());i<numElements;++i )
		if( m_props[i]->getName()==name )
		{
			removeIndex = i;
			break;
		}

	if( removeIndex >= 0 )
	{
		m_vector->erase( m_vector->begin() + removeIndex );
		m_props.erase( m_props.begin() + removeIndex );
	}
}

template<typename T>
void ListPropertyT<T>::synchronize()
{
	if(m_vector->size() != m_props.size())
	{
		// clear props
		m_props.clear();

		// TODO:identify added and removed vector items
		//  for now: just add props
		for( int i=0,end=int(m_vector->size());i<end;++i )
			m_props.push_back(createPropertyForIndex(i));
	}

}

template<typename T>
Property::Ptr ListPropertyT<T>::createPropertyForIndex(int index)
{
	PropertyT<T>::Getter get = [=]{return (*m_vector)[index];};
	PropertyT<T>::Setter set = [=](T& arg){(*m_vector)[index] = arg;};
	std::string name = getName() + "[" + base::toString<int>(index) + "]";
	PropertyT<T>::Ptr prop = PropertyT<T>::create(name, get, set);
	return prop;
}

template<typename T>
void ListPropertyT<T>::getPropertyNames( std::vector<std::string>& names )
{
	for( auto prop:m_props )
		names.push_back(prop->getName());
}

template<typename T>
Property::Ptr ListPropertyT<T>::getProperty( const std::string& name )
{
	for( auto prop:m_props )
		if(prop->getName()==name)
			return prop;
	return Property::Ptr();
}

template<typename T>
bool ListPropertyT<T>::hasProperty( const std::string& name )const
{
	for( auto prop:m_props )
		if(prop->getName()==name)
			return true;
	return false;
}
