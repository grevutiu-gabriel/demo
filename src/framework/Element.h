#pragma once

#include <util/shared_ptr.h>
#include <gfx/Context.h>
#include "Controller.h"


typedef std::function<void(base::Context::Ptr, float)> RenderFunction;
class Element : public ControllerT<RenderFunction>
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

	// overrides from ControllerT
	virtual RenderFunction evaluate(float time);
};
