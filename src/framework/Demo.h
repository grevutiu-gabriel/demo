#pragma once
#include <vector>
#include <util/shared_ptr.h>
#include <util/PiecewiseLinearFunction.h>
#include "Scene.h"
#include "Shot.h"
#include "elements/Clear.h"
#include "elements/RenderGeometry.h"


#include "houdini/HouGeoIO.h"





template<typename T>
struct PiecewiseConstantFunction
{
	PiecewiseConstantFunction()
	{
	}

	~PiecewiseConstantFunction()
	{
	}

	void reset()
	{
		m_domain.clear();
		m_values.clear();
		m_numSamples = 0;
	}

	void addSample( float x, T y )
	{
		m_domain.push_back(x);
		m_values.push_back(y);
		m_numSamples = (int)m_domain.size();
	}

	T evaluate( float x )
	{
		if( m_numSamples == 0 )
			return T(0.0f);

		// out of bound cases
		if( x <= m_domain[0] )
			return m_values[0];
		if( x >= m_domain[m_numSamples-1] )
			return m_values[m_numSamples-1];

		// find interval using binary search http://en.wikipedia.org/wiki/Binary_search_algorithm#Deferred_detection_of_equality
		int imin = 0;
		int imax = m_numSamples - 1;

		while( imin < imax )
		{
			int imid = (imax + imin)/2 + 1;
			if( m_domain[imid] > x )
				imax = imid - 1;
			else
				imin = imid;
		};

		return T(m_values[imax]);
	}

	std::vector<float>                  m_domain;
	std::vector<T>                      m_values;
	int                                 m_numSamples;
};


struct Demo
{
	typedef std::shared_ptr<Demo> Ptr;

	Demo()
	{
	}

	static Ptr create()
	{
		return std::make_shared<Demo>();
	}

	void load( const std::string& filename)
	{
		// load scenes
		Scene::Ptr scene = Scene::create();
		scene->load("c:\\projects\\demo\\git\\bin\\data\\untitled.scn");
		// load elements
		// load shots
		Shot::Ptr shot0 = Shot::create( scene->getCamera("cam1") );
		Shot::Ptr shot1 = Shot::create( scene->getCamera("cam1") );
		m_shots.push_back(shot0);
		m_shots.push_back(shot1);

		Element::Ptr red = Clear::create(math::V3f(1.0f, 0.0f, 0.0f));
		shot0->addElement(red);
		Element::Ptr green = Clear::create(math::V3f(0.0f, 1.0f, 0.0f));
		shot1->addElement(red)->setController("color", scene->getChannel("color"));

		base::Geometry::Ptr geo = houdini::HouGeoIO::importGeometry("c:\\projects\\demo\\git\\bin\\data\\test.bgeo");
		//base::Shader::Ptr shader = base::Shader::createSimpleConstantShader();
		base::Shader::Ptr shader = base::Shader::loadFromFile( "c:\\projects\\demo\\git\\src\\core\\glsl\\genericShader" );
		shader->setUniform("l", math::V3f(1.0f).normalized());
		shader->setUniform("ka", 0.2f);
		shader->setUniform("ambient", math::V3f(1.0f));
		shader->setUniform("kd", 1.0f);
		shader->setUniform("diffuse", math::V3f(.5f));
		Element::Ptr renderGeo = RenderGeometry::create(geo, shader);
		shot0->addElement(renderGeo);
		shot1->addElement(renderGeo);

		m_duration = 10.0f;
		m_shotIndex.addSample(0.0f, 0);
		m_shotIndex.addSample(5.0f, 1);

	}

	void render( base::Context::Ptr context, float time, base::Camera::Ptr overrideCamera = base::Camera::Ptr() )
	{
		//std::cout << "Demo::render\n";

		//TODO: update time controller

		int newShotIndex = m_shotIndex.evaluate(time);
		//std::cout << time << " rendering shot " << newShotIndex << std::endl;

		Shot::Ptr shot = m_shots[newShotIndex];
		shot->render(context, time, overrideCamera);

	}

	float                                m_duration; // how long does the thing go? (in s)
	std::vector<Scene::Ptr>              m_scenes;
	std::vector<Shot::Ptr>               m_shots;
	PiecewiseConstantFunction<int>       m_shotIndex; // tells which shot to render when (should cover the range [0, m_duration])


};
