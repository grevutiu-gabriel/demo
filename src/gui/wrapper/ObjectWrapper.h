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
		void updatePropertyList();

		Property::Ptr addProperty( const std::string listName ); // calls addProperty on name-specified ListProperty

	signals:
		void propertyRemoved( const std::string& name );
		void propertyAdded( const std::string& name );
	private:
		Object::Ptr m_object;
		std::vector<std::string> m_internalProps;
		std::vector<Property::Ptr> m_externalProps;
	};

	Q_DECLARE_METATYPE(std::shared_ptr<ObjectWrapper>)
} // namespace gui
