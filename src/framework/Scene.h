#pragma once

#include <util/shared_ptr.h>
#include "Controller.h"
#include <map>
#include <string>

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

	M44fController::Ptr xformMatrix;

};


struct Camera : public Transform
{
	typedef std::shared_ptr<Camera> Ptr;
	Camera() : Transform()
	{
	}
	static Ptr create()
	{
		return std::make_shared<Camera>();
	}


	FloatController::Ptr fov;
	FloatController::Ptr aspect;
	M44fController::Ptr projectionMatrix;
};



struct Scene
{
	typedef std::shared_ptr<Scene> Ptr;

	Scene()
	{
	}
	static Ptr create()
	{
		return std::make_shared<Scene>();
	}

	void load( const std::string& filename )
	{
		//get content from json object
		Camera::Ptr cam1 = Camera::create();
		cam1->xformMatrix = ConstantM44fController::create( math::M44f::TranslationMatrix(0.0f, 1.0f, 2.0f) );
		cam1->fov = ConstantFloatController::create( 54.0f );
		cam1->aspect = ConstantFloatController::create( 1.0f );
		cam1->projectionMatrix = ProjectionMatrixController::create( cam1->fov, cam1->aspect );
		m_cameras["cam1"] = cam1;

		// add some dummy channel
		m_channels["color"] = FloatToV3fController::create( ConstantFloatController::create(1.0f), SinusController::create(), ConstantFloatController::create(0.0f) );
	}

	Camera::Ptr getCamera( const std::string& name )
	{
		auto it = m_cameras.find(name);
		if( it!=m_cameras.end() )
			return it->second;
		return Camera::Ptr();
	}
	Controller::Ptr getChannel( const std::string& name )
	{
		auto it = m_channels.find(name);
		if( it!=m_channels.end() )
			return it->second;
		return Controller::Ptr();
	}

	std::map<std::string, Camera::Ptr> m_cameras;
	std::map<std::string, Controller::Ptr> m_channels; // generic animations
};
