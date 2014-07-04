#pragma once

#include <util/shared_ptr.h>
#include <gfx/Context.h>
#include <gfx/Camera.h>

#include "Scene.h"
#include "Element.h"
#include "Controller.h"
#include "UpdateGraph.h"

#include <iostream>





class Composition : public ControllerT<RenderFunction>
{
	OBJECT
public:
	typedef std::shared_ptr<Composition> Ptr;


	Composition();

	static Ptr                            create();


	virtual void                          prepareForRendering();

	virtual void                          render( base::Context::Ptr context, float time, base::Camera::Ptr overrideCamera );
	virtual RenderFunction                evaluate(float time);

	void                                  setPropertyController(Object::Ptr object, const std::string& name, Controller::Ptr controller);
	UpdateGraph::Ptr                      getUpdateGraph();

	base::Camera::Ptr                     getCamera()const;
	void                                  setCamera( base::Camera::Ptr camera );

	virtual void                          serialize(Serializer &out)override;
	virtual void                          deserialize(Deserializer &in)override;


private:
	base::Camera::Ptr                     m_camera;
	UpdateGraph::Ptr                      m_updateGraph;
	std::vector<RenderFunction>           m_renderElements;

	static base::Texture2d::Ptr           m_noelement;
	static base::Texture2d::Ptr           m_nocamera;
};


