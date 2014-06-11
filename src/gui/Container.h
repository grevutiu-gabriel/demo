#pragma once

#include <QObject>
#include <vector>

#include "../framework/Object.h"





namespace gui
{


class ObjectContainer : public QObject
{
	Q_OBJECT
	typedef Object::Ptr ItemPtr;
public:
	ObjectContainer();

	void push_back( ItemPtr pixel );
	void append( const std::vector<ItemPtr>& pixels ); // useful for adding a bunch of pixels without getting a changed event fired for ever single item
	bool empty()const;
	int size()const;
	void clear();
	ItemPtr operator[](int index);

signals:
	void changed(); // will be emitted whenever items are removed or inserted

private:
	std::vector<ItemPtr> m_objects;
};


} // gui
