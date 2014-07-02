#pragma once

#include "PropertyGroup.h"
#include "ListProperty.h"
#include "houdini/json.h"
#include <math/Vec3.h>


struct MetaObject;
struct Serializer;
struct Deserializer;


class Object
{
public:
	typedef std::shared_ptr<Object> Ptr;
	typedef std::vector<Ptr> ChildList;

	Object();
	virtual ~Object();

	virtual const MetaObject*    getMetaObject()const;


	virtual void                 serialize(Serializer &out);
	virtual void                 deserialize(Deserializer &in);

	std::string                  getName() const;
	void                         setName(const std::string &name);

	virtual void                 print( std::ostream& out )const;

	template<typename T>
	void                         addProperty( const std::string& name, typename PropertyT<T>::Getter get, typename PropertyT<T>::Setter set);
	template<typename T>
	void                         addProperty( const std::string& name, std::vector<T>* list );

	//property management
	virtual void                 addProperty( const std::string& name, Property::Ptr prop );
	virtual void                 removeProperty( const std::string& name );
	virtual void                 getPropertyNames( std::vector<std::string>& names );
	virtual Property::Ptr        getProperty( const std::string& name );
	virtual bool                 hasProperty( const std::string& name )const;
	virtual void                 addPropertyGroup( PropertyGroup::Ptr group );
	virtual PropertyGroup::Ptr   getPropertyGroup( const std::string& name );
	virtual void                 getPropertyGroups( std::vector<PropertyGroup::Ptr>& groups );

private:
	std::map<std::string, Property::Ptr>      m_props;
	std::map<std::string, PropertyGroup::Ptr> m_propGroups;
	std::string                               m_name;
};



template<typename T>
void Object::addProperty( const std::string& name, typename PropertyT<T>::Getter get, typename PropertyT<T>::Setter set)
{
	PropertyT<T>::Ptr prop = PropertyT<T>::create(name, get, set);

	if( !hasProperty(name) )
	{
		addProperty( name, prop );
	}else
	{
		// if property exists already, we do what?
	}
}

template<typename T>
void Object::addProperty( const std::string& name, std::vector<T>* list )
{
	ListPropertyT<T>::Ptr listProp = std::make_shared<ListPropertyT<T>>( name, list );
	addPropertyGroup( listProp );
}

struct Serializer
{
	~Serializer(){}
	virtual void write( const std::string& key, unsigned char value )=0;
	virtual void write( const std::string& key, const std::string& value )=0;
	virtual void write( const std::string& key, float value )=0;
	virtual void write( const std::string& key, int value )=0;
	virtual void write( const std::string& key, math::V3f value )=0;
	virtual void write( const std::string& key, houdini::json::ObjectPtr jsonObject )=0;
	virtual void write( const std::string& key, houdini::json::ArrayPtr array )=0;
	virtual void write( const std::string& key, Object::Ptr object )=0;
	virtual void write( const std::string& key, houdini::json::Value value )=0;
	virtual houdini::json::Value serialize( Object::Ptr object )=0;
};
struct Deserializer
{
	~Deserializer(){}

	virtual
	houdini::json::ArrayPtr readArray( const std::string& key )=0;

	//virtual void read( const std::string& key, unsigned char value )=0;
	virtual std::string readString( const std::string& key, const std::string& defaultValue ="")=0;
	virtual float readFloat( const std::string& key, float defaultValue =0.0f)=0;
	virtual float readInt( const std::string& key, int defaultValue =0)=0;
	virtual math::V3f readV3f( const std::string& key, math::V3f defaultValue =0)=0;
	virtual houdini::json::Value readValue( const std::string& key )=0;
	virtual bool hasKey( const std::string& key )=0;
	/*
	virtual void read( const std::string& key, float value )=0;
	virtual void read( const std::string& key, int value )=0;
	virtual void read( const std::string& key, houdini::json::ObjectPtr jsonObject )=0;
	virtual void read( const std::string& key, houdini::json::ArrayPtr array )=0;
	virtual void read( const std::string& key, Object::Ptr object )=0;
	*/
	virtual Object::Ptr deserializeObject( houdini::json::Value )=0;
};

struct MetaObject
{
	virtual Object::Ptr create()const=0;
	virtual const std::string& getClassName()const=0;
	virtual const std::string& getSuperClassName()const=0;
	//virtual const MetaObject* getSuperClass()const=0;
};

struct ObjectFactory
{
	static void registerClass(const MetaObject* moc);
	static Object::Ptr create( const std::string& name );
	template<class T>
	static std::shared_ptr<T> create(const std::string& name)
	{
		return std::dynamic_pointer_cast<T>(create(name));
	}

	static void getClassNames( std::vector<std::string>& classNames );
	static const MetaObject* getMetaObject(const std::string& className);
	static bool derivesFrom( const MetaObject*moc, const std::string& superClass );
	static void print(std::ostream &out);
private:
	static std::map<std::string, const MetaObject*>* m_register; // className:metaobject
};

#define REGISTERCLASS( name ) \
				struct name ## MetaObject : public MetaObject \
				{ \
					name ## MetaObject() : MetaObject(), m_name( #name ) \
					{ \
						name::metaObject = this; \
						ObjectFactory::registerClass(this); \
					} \
					virtual Object::Ptr create()const override \
					{ \
						return std::make_shared<name>(); \
					} \
					virtual const std::string& getClassName()const override \
					{ \
						return m_name; \
					} \
					virtual const std::string& getSuperClassName()const \
					{ \
						return m_superClassName; \
					} \
				private:\
					std::string m_name; \
					std::string m_superClassName; \
				}; \
				const MetaObject* name::metaObject = 0; \
				name ## MetaObject g_ ## name ## MetaObject;

#define REGISTERCLASS2( name, superclass ) \
				struct name ## MetaObject : public MetaObject \
				{ \
					name ## MetaObject() : MetaObject(), \
					m_className( #name ), \
					m_superClassName( #superclass ) \
					{ \
						name::metaObject = this; \
						ObjectFactory::registerClass(this); \
					} \
					virtual Object::Ptr create()const override \
					{ \
						return std::make_shared<name>(); \
					} \
					virtual const std::string& getClassName()const override \
					{ \
						return m_className; \
					} \
					virtual const std::string& getSuperClassName()const override \
					{ \
						return m_superClassName; \
					} \
				private:\
					std::string m_className; \
					std::string m_superClassName; \
				}; \
				const MetaObject* name::metaObject = 0; \
				name ## MetaObject g_ ## name ## MetaObject;

#define OBJECT \
		public: static const MetaObject* metaObject; \
		public: virtual const MetaObject* getMetaObject()const override{return metaObject;}
















