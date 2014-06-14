#include "Object.h"

std::map<std::string, const MetaObject*>* ObjectFactory::m_register = 0;
//std::map<const MetaObject*, const MetaObject*>* ObjectFactory::m_superClasses = 0;

void ObjectFactory::registerClass(const MetaObject *moc)
{
	const std::string& className = moc->getClassName();
	if(!m_register)
	{
		m_register = new std::map<std::string, const MetaObject*>();
		//m_superClasses = new std::map<const MetaObject*, const MetaObject*>();
	}
	(*m_register)[className] = moc;

	/*
	// check if super class has already been registered
	auto it = (*m_register).find(moc->getSuperClassName());
	if(it != m_register->end())
	{
		// super class has already been registered which allows us to store the new class
		// in the superclass register
		const MetaObject* superClass = it->second;
		(*m_superClasses)[moc] = superClass;
	}
	// since we dont enforce an order in which classes are registered, there might be
	// the case that superclasses register after derived classes have done
	// in that case we need to update the superclass register for all derived classes
	for( auto it :(*m_register) )
	{
		const MetaObject* derivedMoc = it.second;
		if( derivedMoc->getSuperClassName() == className )
			// current item has the new class as super class
			(*m_superClasses)[derivedMoc] = moc;
	}
	*/
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
		out << name << " : " << it->second->getSuperClassName() << std::endl;
	}
}

void ObjectFactory::getClassNames(std::vector<std::string> &classNames)
{
	classNames.clear();
	for( auto it=m_register->begin(), end=m_register->end();it!=end;++it )
		classNames.push_back(it->first);
}

const MetaObject* ObjectFactory::getMetaObject(const std::string& className)
{
	auto it = (*m_register).find(className);
	if(it != m_register->end())
	{
		return it->second;
	}
	return 0;
}

bool ObjectFactory::derivesFrom(const MetaObject *moc, const std::string &superClassName)
{
	// traverse up the hierarchy
	const MetaObject* parent = moc;

	while(parent)
	{
		if(parent->getClassName() == superClassName)
			return true;
		parent = getMetaObject(parent->getSuperClassName());
	}

	return false;
}

void Object::getPropertyNames(std::vector<std::string>& names)
{
	names.clear();
	for(auto it:m_props)
		names.push_back(it.first);
}

void Object::serialize(Serializer &out)
{
	out.write("type", getMetaObject()->getClassName());
	out.write("name", m_name);
}

void Object::deserialize(Deserializer &in)
{
	// we dont need to read the type as this was needed
	// before this object was created...
	m_name = in.readString( "name" );
}
std::string Object::getName() const
{
    return m_name;
}

void Object::setName(const std::string &name)
{
    m_name = name;
}



