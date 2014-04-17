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

	void load( const std::string& filename );

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
