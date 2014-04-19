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
	Transform::Ptr getLocator( const std::string& name )
	{
		auto it = m_locators.find(name);
		if( it!=m_locators.end() )
			return it->second;
		return Transform::Ptr();
	}
	Controller::Ptr getChannel( const std::string& name )
	{
		auto it = m_channels.find(name);
		if( it!=m_channels.end() )
			return it->second;
		return Controller::Ptr();
	}

	std::map<std::string, Camera::Ptr> m_cameras;
	std::map<std::string, Transform::Ptr> m_locators;
	std::map<std::string, Controller::Ptr> m_channels; // generic animations

private:
	void loadTransform( houdini::json::ObjectPtr transform, Transform::Ptr xform );
	Transform::Ptr loadLocator( houdini::json::ObjectPtr transform );
	Camera::Ptr loadCamera( houdini::json::ObjectPtr camera );
	FloatController::Ptr loadChannel( houdini::json::ObjectPtr channel );
};
