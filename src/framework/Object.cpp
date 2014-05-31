#include "Object.h"

std::map<std::string, const MetaObject*>* ObjectFactory::m_register = 0;

void ObjectFactory::registerClass(const MetaObject *moc)
{
	if(!m_register)
		m_register = new std::map<std::string, const MetaObject*>();
	(*m_register)[moc->getTypeName()] = moc;
}

Object::Ptr ObjectFactory::create(const std::string &name)
{
	auto it = m_register->find(name);

	if(it!=m_register->end())
		return it->second->create();

	std::cerr << "unable to create " << name << std::endl;
	return Object::Ptr();
}

void ObjectFactory::print(std::ostream &out)
{
	for( auto it=m_register->begin(), end=m_register->end();it!=end;++it )
	{
		std::string name = it->first;
		out << name << std::endl;
	}
}


void Object::serialize(Serializer &out)
{
	out.write("type", getMetaObject()->getTypeName());
	out.write("name", m_name);
}
std::string Object::getName() const
{
    return m_name;
}

void Object::setName(const std::string &name)
{
    m_name = name;
}

