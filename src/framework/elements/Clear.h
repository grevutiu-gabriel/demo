#pragma once


#include "../Element.h"


class Clear : public Element
{
	OBJECT
public:
	typedef std::shared_ptr<Clear> Ptr;

	Clear() :
		Element(),
		m_color(math::V3f(0.0f))
	{
		addProperty<math::V3f>( "color", std::bind( &Clear::getColor, this ), std::bind( &Clear::setColor, this, std::placeholders::_1 ) );
	}

//	Clear( math::V3f color ) : Element(), m_color(color)
//	{
//		addProperty<math::V3f>( "color", std::bind( &Clear::getColor, this ), std::bind( &Clear::setColor, this, std::placeholders::_1 ) );
//	}

	static Ptr create()
	{
		return std::make_shared<Clear>();
	}

	math::V3f getColor()const
	{
		return m_color;
	}
	void setColor(math::V3f color)
	{
		m_color=color;
	}


	virtual void render(base::Context::Ptr context, float time)override
	{
		//std::cout << m_color.x << " " << m_color.y << " " << m_color.z << std::endl;
		glClearColor(m_color.x, m_color.y, m_color.z, 1.0f);
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}


private:
	math::V3f m_color;
};







