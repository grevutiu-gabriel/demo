#pragma once

#include <map>
#include "Property.h"
#include "houdini/json.h"


struct MetaObject;
struct Serializer;


class Object
{
public:
	typedef std::shared_ptr<Object> Ptr;
	typedef std::vector<Ptr> ChildList;

	Object()
	{
	}
	virtual ~Object()
	{
	}

	virtual const MetaObject* getMetaObject()const
	{
		return 0;
	}



	template<typename T>
	void addProperty( const std::string& name, typename PropertyT<T>::Getter get, typename PropertyT<T>::Setter set)
	{
		PropertyT<T>::Ptr prop = PropertyT<T>::create(name, get, set);

		if( !hasProperty(name) )
		{
			m_props[name] = prop;
		}else
		{
			// if property exists already, we do what?
		}
	}
	template<typename T>
	void addRefProperty( const std::string& name, typename RefPropertyT<T>::Getter get, typename RefPropertyT<T>::Setter set)
	{
		RefPropertyT<T>::Ptr prop = RefPropertyT<T>::create(name, get, set);

		if( !hasProperty(name) )
		{
			m_props[name] = prop;
		}else
		{
			// if property exists already, we do what?
		}
	}


	Property::Ptr getProperty( const std::string& name )
	{
		auto it = m_props.find( name );
		if( it!=m_props.end() )
			return it->second;
		return Property::Ptr();
	}

	bool hasProperty( const std::string& name )const
	{
		return m_props.find( name ) != m_props.end();
	}

	ChildList& getChildren()
	{
		return m_childs;
	}

	void print( std::ostream& out )const
	{
		for(auto it = m_props.begin(), end=m_props.end();it!=end;++it)
			it->second->print( out );
	}

	virtual void serialize(Serializer &out);

private:
	std::map<std::string, Property::Ptr> m_props;
	std::vector<Ptr>                     m_childs;
};

struct Serializer
{
	~Serializer(){}
	virtual void write( const std::string& key, unsigned char value )=0;
	virtual void write( const std::string& key, const std::string& value )=0;
	virtual void write( const std::string& key, float value )=0;
	virtual void write( const std::string& key, int value )=0;
	virtual void write( const std::string& key, houdini::json::ObjectPtr jsonObject )=0;
	virtual void write( const std::string& key, houdini::json::ArrayPtr array )=0;
	virtual void write( const std::string& key, Object::Ptr object )=0;
	virtual void write( const std::string& key, houdini::json::Value value )=0;
	virtual houdini::json::Value serialize( Object::Ptr object )=0;
};


struct MetaObject
{
	//const MetaObject* getParent()const=0;
	virtual Object::Ptr create()const=0;
	virtual const std::string& getTypeName()const=0;
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

	static void print(std::ostream &out);
private:
	static std::map<std::string, const MetaObject*>* m_register;
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
					virtual const std::string& getTypeName()const override \
					{ \
						return m_name; \
					} \
				private:\
					std::string m_name; \
				}; \
				const MetaObject* name::metaObject = 0; \
				name ## MetaObject g_ ## name ## MetaObject;

#define OBJECT \
		public: static const MetaObject* metaObject; \
		public: virtual const MetaObject* getMetaObject()const override{return metaObject;}
















