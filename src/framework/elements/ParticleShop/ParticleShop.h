#pragma once


#include "../../Element.h"





class ParticleShop : public Element
{
	OBJECT
public:
	typedef std::shared_ptr<ParticleShop> Ptr;

	ParticleShop();

	static Ptr create();

	virtual void render(base::Context::Ptr context, float time)override;



private:
};

