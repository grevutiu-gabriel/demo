#pragma once

#include <util/shared_ptr.h>
#include <vector>
#include <map>
#include <string>
#include <functional>
#include <iostream>


/*

  Property system features:
	-properties can be serialized and deserialized
	-Properties can be updated from controllers (1 to n relationship possible)
	-controllers can be linked such that controller chains are established
	- not fully a pll mechanism because we want a callback when the property changes (the set function)



 */




struct Property
{
	typedef std::shared_ptr<Property> Ptr;

	Property( const std::string& name ) : m_name(name)
	{
	}

	std::string getName()const
	{
		return m_name;
	}

	virtual void print( std::ostream& out )const
	{
		//out << getName() << " " << get() << std::endl;
		out << getName() << " " << std::endl;
	}

private:
	std::string m_name;
};

template<typename T>
struct PropertyT : public Property
{
	typedef std::shared_ptr<PropertyT<T>> Ptr;
	typedef std::function<T()> Getter;
	typedef std::function<void(T)> Setter;

	static Ptr create( const std::string& name, Getter getter, Setter setter )
	{
		return std::make_shared<PropertyT<T>>(name, getter, setter);
	}

	PropertyT( const std::string& name, Getter getter, Setter setter ) :
		Property(name),
		get(getter),
		set(setter)
	{
	}



	Getter get;
	Setter set;
};
typedef PropertyT<float> FloatProperty;



// object forward declaration
class Object;


// this property wraps a reference to a object or any derived class
struct RefProperty : public Property
{
	typedef std::shared_ptr<RefProperty> Ptr;

	RefProperty( const std::string& name ) : Property(name)
	{

	}

	virtual std::shared_ptr<Object> getRef()=0;

private:
};

template<class T>
struct RefPropertyT : public RefProperty
{
	typedef std::shared_ptr<RefPropertyT<T>> Ptr;
	typedef std::function<std::shared_ptr<T>()> Getter;
	typedef std::function<void(std::shared_ptr<T>)> Setter;

	RefPropertyT( const std::string& name, Getter getter, Setter setter ) :
		RefProperty(name),
		get(getter),
		set(setter)
	{
	}
	static Ptr create( const std::string& name, Getter getter, Setter setter )
	{
		return std::make_shared<RefPropertyT<T>>(name, getter, setter);
	}
	virtual std::shared_ptr<Object> getRef()
	{
		return get();
	}

	Getter get;
	Setter set;
};







