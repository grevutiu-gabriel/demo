#pragma once

#include <util/shared_ptr.h>
#include <vector>
#include <map>
#include <string>
#include <functional>
#include <iostream>

//http://stackoverflow.com/questions/4484982/how-to-convert-typename-t-to-string-in-c
template <typename T>
struct TypeName
{
	static std::string get()
	{
		return typeid(T).name();
	}
};
#define SET_TYPENAME(A, NAME) template<> struct TypeName<A> { static std::string get() { return NAME; }};
SET_TYPENAME(std::string, "string")

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
	virtual std::string getType()const=0;

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

	virtual std::string getType()const override
	{
		return TypeName<T>::get();
	}



	Getter get;
	Setter set;
};
typedef PropertyT<float> FloatProperty;









