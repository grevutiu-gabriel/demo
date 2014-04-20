#pragma once

#include <util/shared_ptr.h>
#include <gfx/Context.h>
#include "Object.h"



struct Element : public Object
{
	typedef std::shared_ptr<Element> Ptr;


	virtual void begin(base::Context::Ptr context)
	{
	}
	virtual void end(base::Context::Ptr context)
	{
	}
	virtual void render(base::Context::Ptr context, float time)
	{
	}


};
