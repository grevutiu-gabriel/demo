#pragma once


#include <gfx/Texture.h>
#include "../Controller.h"














class LoadTexture : public TextureController
{
	OBJECT
public:
	typedef std::shared_ptr<LoadTexture> Ptr;

	LoadTexture();

	// overrides from GeometryController
	virtual base::Texture2d::Ptr evaluate(float time);
	virtual bool isAnimated()const;

	void setFilename( const std::string& filename );
	std::string getFilename();


	void serialize(Serializer &out);
private:
	base::Texture2d::Ptr  m_texture;
	std::string           m_filename;
};
