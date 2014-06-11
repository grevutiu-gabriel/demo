#pragma once

#include <QObject>

#include "../framework/Object.h"


namespace gui
{

	class ObjectWrapper : public QObject
	{
		Q_OBJECT
	public:
		typedef std::shared_ptr<ObjectWrapper> Ptr;

		ObjectWrapper( Object::Ptr object );
		virtual ~ObjectWrapper();
		static Ptr create( Object::Ptr object );

		std::string getName()const;
		void setName( const std::string& name );
		Object::Ptr getObject();
		void getProperties( std::vector<Property::Ptr>& props );
		void addExternalProperty( Property::Ptr prop );
	private:
		Object::Ptr m_object;
		std::vector<Property::Ptr> m_externalProps;
	};

} // namespace gui
