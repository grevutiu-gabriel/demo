#pragma once

#include <util/shared_ptr.h>
#include "Controller.h"
#include <map>
#include <string>

#include "houdini/HouGeoIO.h"

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
	}
	static Ptr create()
	{
		return std::make_shared<PerspectiveCameraController>();
	}
	virtual base::Camera::Ptr evaluate(float time)override
	{
		// assemble camera ----

		// projectionMatrix changes
		if( m_fovIsAnimated )
			m_camera->setProjection(math::projectionMatrix( fov->evaluate(time), aspect->evaluate(time), m_camera->m_znear, m_camera->m_zfar ));
		// transform changes
		if( m_xformIsAnimated )
			m_camera->setViewToWorld( xform->evaluate(time) );

		return m_camera;
	}

	virtual bool isAnimated()const override
	{
		return fov->isAnimated()||xform->isAnimated();
	}


	FloatController::Ptr fov; // in radians
	FloatController::Ptr aspect;
	M44fController::Ptr  xform;

private:
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
		int sw = int(m_switch->evaluate(time));
		return m_cameras[ sw ]->evaluate(time);
	}
	virtual bool isAnimated()const override
	{
		return m_switch->isAnimated();
	}
	FloatController::Ptr               m_switch;
	std::vector<CameraController::Ptr> m_cameras;
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

	CameraController::Ptr getCamera( const std::string& name )
	{
		auto it = m_cameras.find(name);
		if( it!=m_cameras.end() )
			return it->second;
		return CameraController::Ptr();
	}
	M44fController::Ptr getLocator( const std::string& name )
	{
		auto it = m_locators.find(name);
		if( it!=m_locators.end() )
			return it->second;
		return M44fController::Ptr();
	}
	Controller::Ptr getChannel( const std::string& name )
	{
		auto it = m_channels.find(name);
		if( it!=m_channels.end() )
			return it->second;
		return Controller::Ptr();
	}

	Controller::Ptr getController( const std::string& name )
	{
		auto it = m_controller.find(name);
		if( it!=m_controller.end() )
			return it->second;
		return Controller::Ptr();
	}

	std::map<std::string, CameraController::Ptr> m_cameras;
	std::map<std::string, M44fController::Ptr>   m_locators;
	std::map<std::string, Controller::Ptr>       m_channels; // generic animations

	std::map<std::string, Controller::Ptr> m_controller;

	virtual void serialize(Serializer &out)override;
private:
	M44fController::Ptr loadTransform( houdini::json::ObjectPtr transform );
	M44fController::Ptr loadLocator( houdini::json::ObjectPtr transform );
	CameraController::Ptr loadCamera( houdini::json::ObjectPtr camera );
	CameraController::Ptr loadSwitcher( houdini::json::ObjectPtr switcher );
	void loadChannel( const std::string& name, houdini::json::ObjectPtr channel );
	FloatController::Ptr loadTrack( houdini::json::ObjectPtr track );
	std::string m_filename;
};
