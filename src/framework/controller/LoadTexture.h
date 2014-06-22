#pragma once


#include <gfx/Texture.h>
#include "../Controller.h"














class LoadTexture2d : public Texture2dController
{
	OBJECT
public:
	typedef std::shared_ptr<LoadTexture2d> Ptr;

	LoadTexture2d();

	// overrides from GeometryController
	virtual base::Texture2d::Ptr evaluate(float time);
	virtual bool isAnimated()const;

	void setFilename( const std::string& filename );
	std::string getFilename();

	virtual void serialize(Serializer &out)override;
	virtual void deserialize(Deserializer &in)override;
private:
	base::Texture2d::Ptr  m_texture;
	std::string           m_filename;
};


class LoadTexture3d : public Texture3dController
{
	OBJECT
public:
	typedef std::shared_ptr<LoadTexture3d> Ptr;

	LoadTexture3d();

	// overrides from GeometryController
	virtual base::Texture3d::Ptr evaluate(float time);
	virtual bool isAnimated()const;

	void setFilename( const std::string& filename );
	std::string getFilename();


	void serialize(Serializer &out);
private:
	base::Texture3d::Ptr  m_texture;
	std::string           m_filename;
};
