#include "Demo.h"
#include "elements/volumept/Volume.h"
#include "elements/postprocess/PostProcess.h"
#include "elements/stars/Stars.h"
#include "elements/FlareShop/FlareShop.h"
#include "elements/Nebulae/Nebulae.h"

Demo::Demo( bool doAudio )
{
	if(doAudio)
		m_audio = std::make_shared<Audio>();
}

Shot::Ptr Demo::getShot( int index )
{
	return m_shots[index];
}

int Demo::getNumShots() const
{
	return int(m_shots.size());
}

void Demo::load( const std::string& filename )
{
	std::string basePathData = base::path("data");
	std::string basePathSrc = base::path("src");

	///*
	// load scenes ------
	Scene::Ptr scene = Scene::create();
	scene->load(basePathData + "/artifix.scn");
	Scene::Ptr scene_test = Scene::create();
	scene_test->load(basePathData + "/test.scn");

	// load elements ----
	// black background
	Element::Ptr black = Clear::create(math::V3f(0.0f, 0.0f, 0.0f));
	//stars
	Stars::Ptr stars = Stars::create();
	stars->setFixedRotationEnabled(true);
	stars->setFixedRotation( math::V3f(1.0f, 1.0f, 0.0f).normalized(), math::degToRad(5.0f) );
	stars->setMotionblurScale(1.0f);
	stars->setDomeBrightnessScale(0.0061f);
	stars->setStarBrightnessScale(0.7f);
	// volume
	Volume::Ptr volume = Volume::create();
	volume->load( basePathData + "/artifix_resized_moved.bgeo" );
	// post process
	PostProcess::Ptr post = PostProcess::create();
//	post->setHDREnabled(true);
//	post->setGlareEnabled(true);
//	post->setGlareBlurIterations(4);
//	post->setGlareAmount(0.8f);
	post->setHDREnabled(true);
	post->setGlareEnabled(true);
	post->setGlareBlurIterations(3);
	post->setGlareAmount(0.1f);
	post->setVignetteEnabled(true);
	post->setVignetteStrength(1.0f);
	post->setVignetteSoftness( 1.0f );
	post->setVignetteScale(0.7f);

	// manix shot ---------
	{
		// create shot
		Shot::Ptr shot = Shot::create();
		shot->m_cameraController = scene->getCamera("switcher1");


		ShotElement::Ptr se = ShotElement::create(post);
		se->addChild(black);
		se->addChild(volume);
		shot->setController(volume, "PointLightPosition", scene->getLocator("null1")->getProperty("translation"));
		shot->setController(volume, "PointLightIntensity", scene->getChannel("ch1.x"));
		//volumese->setController(volume->m_transferFunction->getNode(1), "density", scene->getChannel("tfnode.density"));
		//se->addChild(stars);
		shot->addElement(se);
		addShot(shot);
	}

	// geometry --------
	{
		Shot::Ptr shot = Shot::create();
		shot->m_cameraController = scene_test->getCamera("switcher1");

		ShotElement::Ptr se = ShotElement::create(post);
		// ------------
		base::Geometry::Ptr geo = houdini::HouGeoIO::importGeometry(basePathData + "/test.bgeo");
		//base::Geometry::Ptr geo = houdini::HouGeoIO::importGeometry(basePathData + "/mountain.bgeo");

//		base::Shader::Ptr shader = base::Shader::loadFromFile( basePathSrc + "/core/glsl/genericShader" );
//		shader->setUniform("l", math::V3f(1.0f).normalized());
//		shader->setUniform("ka", 0.1f);
//		shader->setUniform("ambient", math::V3f(1.0f));
//		shader->setUniform("kd", 1.0f);
//		shader->setUniform("diffuse", math::V3f(.5f));

		base::Shader::Ptr shader = base::Shader::loadFromFile( basePathData + "/matcap" );
		//base::Texture2d::Ptr tex = base::Texture2d::load( basePathData + "/droplet_01.png", GL_SRGB8 );
		//base::Texture2d::Ptr tex = base::Texture2d::load( basePathData + "/00ZBrush_RedWax.png", GL_SRGB8 );
		base::Texture2d::Ptr tex = base::Texture2d::load( basePathData + "/scary-light.jpg", GL_SRGB8 );
		//base::Texture2d::Ptr tex = base::Texture2d::load( basePathData + "/bronze1.jpg", GL_SRGB8 );
		//base::Texture2d::Ptr tex = base::Texture2d::load( basePathData + "/night2.jpg", GL_SRGB8 );

		base::Context::getCurrentContext()->addTexture2d("droplet_01.png", tex);
		shader->setUniform("tex", tex);


		Element::Ptr renderGeo = RenderGeometry::create(geo, shader);

		se->addChild(black);
		se->addChild(stars);
		se->addChild(renderGeo);

		shot->addElement(se);

		//shot->addElement(black);
		//shot->addElement(renderGeo);

		addShot(shot);
	}

	// Nebulae
	{
		Shot::Ptr shot = Shot::create();
		shot->m_cameraController = scene_test->getCamera("cam1");


		ShotElement::Ptr se = ShotElement::create(post);
		Nebulae::Ptr nebulae = Nebulae::create();
		nebulae->generate();

		se->addChild(black);
		se->addChild(stars);
		se->addChild(nebulae);

		shot->addElement(se);

		addShot(shot);
	}



	// add clips
	// clips define when on the global timeline which shot will be rendered
	addClip( 0, 0.0f, 24.0f, 24.0f );
	addClip( 1, 24.0f, 48.0f, 24.0f );
	//addClip( 2, 36.0f, 72.0f, 24.0f );
	//*/

//	// TEMP ----------
//	{
//		Element::Ptr black = Clear::create(math::V3f(0.0f, 0.0f, 0.0f));
//		FlareShop::Ptr fs = FlareShop::create();
//		Shot::Ptr shot = Shot::create();
//		shot->addElement(black);
//		shot->addElement(fs);
//		this->addShot(shot);
//		addClip( 0, 0.0f, 1.0f, 1.0f );
//	}


	// load audio ----
	if(m_audio)
		m_audio->load(basePathData + "/heart_of_courage.ogg");
}


void Demo::render( base::Context::Ptr context, float time, base::Camera::Ptr overrideCamera )
{
	//std::cout << "Demo::render\n";

	//TODO: update time controller

	int newClipIndex = m_clipIndex.evaluate(time);
	Clip& clip = m_clips[newClipIndex];
	int newShotIndex = clip.shotIndex;
//	//std::cout << time << " rendering shot " << newShotIndex << std::endl;

	Shot::Ptr shot = m_shots[newShotIndex];
	shot->render(context, clip.toShotTime(time), overrideCamera);

}

int Demo::addShot( Shot::Ptr shot )
{
	int index = int(m_shots.size());
	m_shots.push_back(shot);
	return index;
}

void Demo::addClip(int shotIndex, float shotStart , float shotEnd, float clipDuration)
{
	Clip clip;
	clip.shotStart = shotStart;
	clip.shotEnd = shotEnd;
	clip.shotIndex = shotIndex;
	clip.duration = clipDuration;
	m_clips.push_back( clip );

	m_clipIndex.reset();
	float lastEnd = 0.0f;
	int clipIndex = 0;
	for( auto it = m_clips.begin(), end=m_clips.end();it!=end;++it, ++clipIndex)
	{
		Clip& c = *it;
		m_clipIndex.addSample( lastEnd, clipIndex );
		lastEnd += c.duration;
	}
}
