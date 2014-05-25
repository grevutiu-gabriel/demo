#pragma once


#include "../Element.h"




class RenderTexture : public Element
{
	OBJECT
public:
	typedef std::shared_ptr<RenderTexture> Ptr;
	RenderTexture():Element()
	{
		addProperty<base::Texture2d::Ptr>( "texture", std::bind( &RenderTexture::getTexture, this ), std::bind( &RenderTexture::setTexture, this, std::placeholders::_1 ) );
	}

	static Ptr create()
	{
		return std::make_shared<RenderTexture>();
	}

	virtual void render(base::Context::Ptr context, float time)override
	{
		context->renderScreen(m_texture);
	}

	base::Texture2d::Ptr getTexture()const
	{
		return m_texture;
	}
	void setTexture( base::Texture2d::Ptr texture )
	{
		m_texture = texture;
	}

private:
	base::Texture2d::Ptr m_texture;
};

