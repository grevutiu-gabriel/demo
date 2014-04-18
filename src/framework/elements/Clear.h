#pragma once


#include "../Element.h"


struct Clear : public Element
{
	typedef std::shared_ptr<Clear> Ptr;

	Clear( math::V3f color ) : Element(), m_color(color)
	{
		addProperty<math::V3f>( "color", std::bind( &Clear::getColor, this ), std::bind( &Clear::setColor, this, std::placeholders::_1 ) );
	}

	static Ptr create( math::V3f color )
	{
		return std::make_shared<Clear>(color);
	}

	math::V3f getColor()const
	{
		return m_color;
	}
	void setColor(math::V3f color)
	{
		m_color=color;
	}


	virtual void render(base::Context::Ptr context)override
	{
		glClearColor(m_color.x, m_color.y, m_color.z, 1.0f);
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glEnable(GL_DEPTH_TEST);
	}


private:
	math::V3f m_color;
};







