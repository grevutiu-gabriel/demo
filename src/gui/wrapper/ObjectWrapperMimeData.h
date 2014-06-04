//
//Subclasses from QMimeData which is used to transport data during drag
//and drop operations. This particular class MimeDataCreateProcess is
//used to transport data which is used to kick of creation of a node.
//
//application/draw-connection is the mimetype of this data
//

#pragma once
#include <QMimeData>

#include "ObjectWrapper.h"

namespace gui
{

	//
	// used to transport data which is used to draw a connection between two nodes in a graph.
	//
	class ObjectWrapperMimeData : public QMimeData
	{
		Q_OBJECT
	public:
		ObjectWrapperMimeData( ObjectWrapper::Ptr objectWrapper );  // constructor - provide info about which process to create
		~ObjectWrapperMimeData();                                    // destructor

		// QMimeData overrides
		virtual bool                   hasFormat( const QString &mimetype )const override;  // returns true if the mimedata has given mimetype (which is application/draw-connection)
		virtual QStringList formats () const override;

		ObjectWrapper::Ptr getObjectWrapper() const;  // returns string identifier of the processtype to create

	private:
		ObjectWrapper::Ptr             m_objectWrapper;
	};
}
