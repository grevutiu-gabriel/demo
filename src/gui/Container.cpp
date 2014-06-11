#include "Container.h"







namespace gui
{

// ObjectContainer ==================
ObjectContainer::ObjectContainer() : QObject()
{
}


void ObjectContainer::push_back( Object::Ptr object )
{
	if(object)
	{
		m_objects.push_back(object);
		emit changed();
	}
}
// useful for adding a bunch of pixels without getting a changed event fired for ever single item
void ObjectContainer::append( const std::vector<Object::Ptr>& objects )
{
	if( !objects.empty() )
	{
		m_objects.insert( m_objects.end(), objects.begin(), objects.end() );
		emit changed();
	}
}

bool ObjectContainer::empty()const
{
	return m_objects.empty();
}
void ObjectContainer::clear()
{
	m_objects.clear();
	emit changed();
}
int ObjectContainer::size()const
{
	return (int)m_objects.size();
}
Object::Ptr ObjectContainer::operator[](int index )
{
	return m_objects[index];
}

} // gui
