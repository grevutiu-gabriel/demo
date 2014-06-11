#include "ObjectWrapper.h"


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
} // namespace gui
