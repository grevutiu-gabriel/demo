#include "ParticleShop.h"

#include <gfx/Geometry.h>
#include <util/fs.h>


ParticleShop::ParticleShop() : Element()
{
}


ParticleShop::Ptr ParticleShop::create()
{
	return std::make_shared<ParticleShop>();
}

void ParticleShop::render(base::Context::Ptr context, float time)
{


}


REGISTERCLASS( ParticleShop )
