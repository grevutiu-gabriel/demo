//
//Subclasses from QMimeData which is used to transport data during drag
//and drop operations. This particular class MimeDataCreateProcess is
//used to transport data which is used to kick of creation of a node.
//
//application/draw-connection is the mimetype of this data
//
#include "ObjectWrapperMimeData.h"

#include <QStringList>

namespace gui
{

	//
	// constructor - provide info about which process to create
	//
	ObjectWrapperMimeData::ObjectWrapperMimeData(const std::string& className, GetObjectWrapperCallback callback, bool creatingObject ) :
		QMimeData(),
		m_callback(callback),
		m_className(className),
		m_creatingObject(creatingObject)
	{
	}

	//
	// destructor
	//
	ObjectWrapperMimeData::~ObjectWrapperMimeData()
	{
	}


	//
	// returns string identifier of the processtype to create
	//
	ObjectWrapper::Ptr ObjectWrapperMimeData::getObjectWrapper() const
	{
		if( m_callback )
			return m_callback();
		return ObjectWrapper::Ptr();
	}

	const std::string &ObjectWrapperMimeData::getClassName() const
	{
		return m_className;
	}

	//
	// returns true if the mimedata has given mimetype (which is application/create-process)
	//
	bool ObjectWrapperMimeData::hasFormat( const QString &mimetype )const
	{
		if( mimetype == QString("application/objectwrapper") )
			return true;
		if( m_creatingObject && (mimetype == QString("application/createobject")) )
			return true;
		return false;
	}

	QStringList ObjectWrapperMimeData::formats() const
	{
		QStringList f;
		f.push_back("application/objectwrapper");
		if(m_creatingObject)
			f.push_back("application/createobject");
		return f;
	}
}
