#pragma once

#include <util/shared_ptr.h>
#include "Controller.h"
#include <map>
#include <string>

#include "houdini/HouGeoIO.h"
#include "UpdateGraph.h"

struct Transform
{
	typedef std::shared_ptr<Transform> Ptr;
	Transform()
	{
	}
	static Ptr create()
	{
		return std::make_shared<Transform>();
	}

	PRSController::Ptr xform;

};


struct PerspectiveCameraController : public CameraController
{
	typedef std::shared_ptr<PerspectiveCameraController> Ptr;

	PerspectiveCameraController() : CameraController()
	{
		m_camera = std::make_shared<base::Camera>();
		// TODO, make sure proj is not rebuild all the time when fov is not animated
		m_fovIsAnimated = true;
		m_xformIsAnimated = true;

		addProperty<float>( "fovy", std::bind( &PerspectiveCameraController::getFovy, this ), std::bind( &PerspectiveCameraController::setFovy, this, std::placeholders::_1 ) );
		addProperty<float>( "aspect", std::bind( &PerspectiveCameraController::getAspect, this ), std::bind( &PerspectiveCameraController::setAspect, this, std::placeholders::_1 ) );
		addProperty<math::M44f>( "transform", std::bind( &PerspectiveCameraController::getTransform, this ), std::bind( &PerspectiveCameraController::setTransform, this, std::placeholders::_1 ) );
	}
	static Ptr create()
	{
		return std::make_shared<PerspectiveCameraController>();
	}
	virtual base::Camera::Ptr evaluate(float time)override
	{
		// assemble camera ----
		std::cout << "PerspectiveCameraController::evaluate\n";

		// projectionMatrix changes
		if( m_fovIsAnimated )
			m_camera->setProjection(math::projectionMatrix( m_fovy, m_aspect, m_camera->m_znear, m_camera->m_zfar ));
		// transform changes
		if( m_xformIsAnimated )
			m_camera->setViewToWorld( m_xform );

		return m_camera;
	}

	virtual bool isAnimated()const override
	{
		return true;
	}


	float getFovy() const
	{
		return m_fovy;
	}
	void setFovy(float fovy)
	{
		m_fovy = fovy;
	}

	float getAspect() const
	{
		return m_aspect;
	}
	void setAspect(float aspect)
	{
		m_aspect = aspect;
	}

	math::M44f getTransform() const
	{
		return m_xform;
	}
	void setTransform(const math::M44f &xform)
	{
		m_xform = xform;
	}

private:
	float             m_fovy; // in radians
	float             m_aspect;
	math::M44f        m_xform;
	base::Camera::Ptr m_camera;
	bool m_fovIsAnimated;
	bool m_xformIsAnimated;
};

struct CameraSwitchController : public CameraController
{
	typedef std::shared_ptr<CameraSwitchController> Ptr;
	CameraSwitchController() : CameraController()
	{
	}
	static Ptr create()
	{
		return std::make_shared<CameraSwitchController>();
	}
	virtual base::Camera::Ptr evaluate(float time)override
	{
		int sw = int(m_switch);
		return m_cameras[ sw ];
	}
	virtual bool isAnimated()const override
	{
		return true;
	}
	float                              m_switch;
	std::vector<base::Camera::Ptr>     m_cameras;
};

class Scene;
class SceneController : public Controller
{
	OBJECT
public:
	typedef std::shared_ptr<Scene> ScenePtr;
	typedef std::shared_ptr<SceneController> Ptr;

	SceneController() : Controller()
	{
	}

	SceneController( ScenePtr scene, const std::string& controllerId )
		:Controller(),
		 m_scene(scene),
		 m_controllerId(controllerId)
	{
		getController();
	}

	static Ptr create( ScenePtr scene, const std::string& controllerId )
	{
		return std::make_shared<SceneController>( scene, controllerId );
	}

	virtual void update( Property::Ptr prop, float time)override;


	virtual bool isAnimated()const override
	{
		return true;
	}

	virtual void serialize(Serializer &out)override;

private:
	void getController();


	ScenePtr        m_scene;
	std::string     m_controllerId;
	Controller::Ptr m_controller;
	UpdateGraph     m_updateGraph;
};



class Scene : public Object
{
	OBJECT
public:
	typedef std::shared_ptr<Scene> Ptr;

	Scene()
	{
	}
	static Ptr create()
	{
		return std::make_shared<Scene>();
	}

	void load( const std::string& filename );
	const std::string& getFilename()const;


	Controller::Ptr getController( const std::string& name, UpdateGraph& updateGraph )
	{
		updateGraph.clear();

		auto it = m_controller.find(name);
		if( it!=m_controller.end() )
		{
			Controller::Ptr controller = it->second;
			updateGraph.copyFrom( m_updateGraph, controller );
			return controller;
		}
		return Controller::Ptr();
	}


	virtual void serialize(Serializer &out)override;
private:
	M44fController::Ptr loadTransform( houdini::json::ObjectPtr transform, const std::string& name );
	M44fController::Ptr loadLocator( houdini::json::ObjectPtr transform, const std::string& name );
	CameraController::Ptr loadCamera( houdini::json::ObjectPtr camera, const std::string& name );
	CameraController::Ptr loadSwitcher( houdini::json::ObjectPtr switcher, const std::string& name );
	void loadChannel( houdini::json::ObjectPtr channel, const std::string& name );
	FloatController::Ptr loadTrack( houdini::json::ObjectPtr track, const std::string& name );

	std::string                            m_filename; // scene filename
	std::map<std::string, Controller::Ptr> m_controller; // contains all channels etc.
	UpdateGraph                            m_updateGraph; // holds information about how controllers are connected
};
