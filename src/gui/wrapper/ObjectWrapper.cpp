#include "ObjectWrapper.h"
#include <algorithm>

namespace gui
{
	float testtest = 1.123f;
	void setTest( float test )
	{
		std::cout << "setTest " << test << std::endl;std::flush(std::cout);
		testtest = test;
	}
	float getTest()
	{
		std::cout << "getTest " << testtest << std::endl;std::flush(std::cout);
		return testtest;
	}

	ObjectWrapper::ObjectWrapper(Object::Ptr object) : QObject(), m_object(object)
	{
		m_object->getPropertyNames(m_internalProps);

		//addExternalProperty( PropertyT<float>::create("test", std::bind(getTest), std::bind(setTest, std::placeholders::_1)));
	}

	ObjectWrapper::~ObjectWrapper()
	{
	}

	ObjectWrapper::Ptr ObjectWrapper::create(Object::Ptr object)
	{
		return std::make_shared<ObjectWrapper>(object);
	}

	std::string ObjectWrapper::getName() const
	{
		return m_object->getName();
	}

	void ObjectWrapper::setName(const std::string &name)
	{
		m_object->setName(name);
	}

	Object::Ptr ObjectWrapper::getObject()
	{
		return m_object;
	}

	void ObjectWrapper::getProperties(std::vector<Property::Ptr> &props)
	{
		props.clear();
		// propagate internal props...
		std::vector<std::string> propNames;
		m_object->getPropertyNames(propNames);
		for(auto propName:propNames)
			props.push_back(m_object->getProperty(propName));

		// as well as external props...
		for( auto prop:m_externalProps )
		{
			props.push_back(prop);
		}
	}

	void ObjectWrapper::addExternalProperty(Property::Ptr prop)
	{
		m_externalProps.push_back(prop);
	}

	void ObjectWrapper::updatePropertyList()
	{
		std::vector<std::string> internalProps( m_internalProps.begin(), m_internalProps.end() );
		// find out which properties have been removed
		std::vector<std::string> updatedInternalProps;
		m_object->getPropertyNames(updatedInternalProps);

		std::sort( internalProps.begin(), internalProps.end() );
		std::sort( updatedInternalProps.begin(), updatedInternalProps.end() );

		std::vector<std::string> addedProps( internalProps.size() + updatedInternalProps.size() );
		std::vector<std::string> removedProps( internalProps.size() + updatedInternalProps.size() );

		// http://www.cplusplus.com/reference/algorithm/set_difference/
		removedProps.resize( std::set_difference( internalProps.begin(), internalProps.end(), updatedInternalProps.begin(), updatedInternalProps.end(), removedProps.begin() ) - removedProps.begin() );
		addedProps.resize( std::set_difference( updatedInternalProps.begin(), updatedInternalProps.end(), internalProps.begin(), internalProps.end(), addedProps.begin() ) - addedProps.begin() );

		for( auto removed:removedProps )
		{
			//std::cout << "ObjectWrapper::updatePropertyList: prop removed: " << removed << std::endl;
			emit propertyRemoved(removed);
		}
		for( auto added:addedProps )
		{
			//std::cout << "ObjectWrapper::updatePropertyList: prop added: " << added << std::endl;
			emit propertyAdded(added);
		}

		m_internalProps = std::vector<std::string>( updatedInternalProps.begin(), updatedInternalProps.end() );
		/*
		auto it = m_nodes.find(objectWrapper);
		if(it!=m_nodes.end())
		{
			//return it->second.pos;
			// TODO: mnanage connections
			// -remove connections to props which disappeard
			// - rewire connections to props which are still there by name (but have a different property pointer behind)
			// -   -> maybe recompiling is enough (should be, actually)
		}
		emit objectPropertiesChanged( objectWrapper );


		QNEBlock* block = getNode(objectWrapper);
		if(!block)
			return;

		// get alle existing inputs (ignoring name and type ports)

		// get all object properties
		std::vector<std::string> propNames;
		objectWrapper->getObject()->getPropertyNames(propNames);
		for(auto propName:propNames)
		{
			m_inputs[m_nextInput] = propName;
			QNEPort*p = block->addInputPort(QString::fromStdString(propName));
			p->setPtr(m_nextInput++);
		}


		// remove connections

		// find out which ports have been added
		*/
	}
} // namespace gui
