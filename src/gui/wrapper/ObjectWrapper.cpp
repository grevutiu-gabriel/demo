#include "ObjectWrapper.h"


namespace gui
{


	ObjectWrapper::ObjectWrapper(Object::Ptr object) : QObject(), m_object(object)
	{

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

} // namespace gui
