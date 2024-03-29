#pragma once


#include <gfx/Shader.h>
#include "../Controller.h"














class LoadShader : public ShaderController
{
	OBJECT
public:
	typedef std::shared_ptr<LoadShader> Ptr;

	LoadShader();

	// overrides from GeometryController
	virtual base::Shader::Ptr evaluate(float time);
	virtual bool isAnimated()const;

	void setFilename( const std::string& filename );
	std::string getFilename();

	void setUniformSampler2D( const std::string& name, base::Texture2d::Ptr texture );

	void serialize(Serializer &out);
	void deserialize(Deserializer &in);

	void reload();

	base::Shader::Ptr getShader();
private:

	void updateProperties();
	void clearProperties();

	base::Shader::Ptr          m_shader;
	std::string                m_filename;
	std::vector<std::string>   m_props;
};

