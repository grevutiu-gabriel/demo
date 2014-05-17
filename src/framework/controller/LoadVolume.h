#pragma once

#include <gfx/Field.h>
#include "../Controller.h"














class LoadVolume : public Controller
{
	OBJECT
public:
	typedef std::shared_ptr<LoadVolume> Ptr;

	// overrides from Controller
	virtual void update( Property::Ptr prop, float time)=0;
	virtual bool isAnimated()const;

	void setFilename( const std::string& filename );
	std::string getFilename();

	void serialize(Serializer &out);
private:
	base::Texture3d::Ptr evaluateTexture3d(float time);
	base::ScalarField::Ptr evaluateScalarField(float time);
	math::M44f evaluateLocalToWorld(float time);

	base::Texture3d::Ptr   m_texture3d;
	base::ScalarField::Ptr m_field;
	std::string            m_filename;
};
