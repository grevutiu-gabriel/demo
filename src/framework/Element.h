#pragma once

#include <util/shared_ptr.h>
#include <gfx/Context.h>
#include "Object.h"



class Element : public Object
{
	OBJECT
public:
	typedef std::shared_ptr<Element> Ptr;


	virtual void begin(base::Context::Ptr context, float time)
	{
	}
	virtual void end(base::Context::Ptr context, float time)
	{
	}
	virtual void render(base::Context::Ptr context, float time)
	{
	}
};
