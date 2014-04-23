#include "Demo.h"
#include "elements/volumept/Volume.h"
#include "elements/postprocess/PostProcess.h"
#include "elements/stars/Stars.h"

Demo::Demo( bool doAudio )
{
	if(doAudio)
		m_audio = std::make_shared<Audio>();
}

Shot::Ptr Demo::getShot( int index )
{
	return m_shots[index];
}

void Demo::load( const std::string& filename )
{
	// load scenes ------
	Scene::Ptr scene = Scene::create();
	scene->load("c:\\projects\\demo\\git\\bin\\data\\artifix.scn");
	Scene::Ptr scene_test = Scene::create();
	scene_test->load("c:\\projects\\demo\\git\\bin\\data\\test.scn");

	// load elements ----
	// black background
	Element::Ptr black = Clear::create(math::V3f(0.0f, 0.0f, 0.0f));
	//stars
	Stars::Ptr stars = Stars::create();
	stars->setFixedRotationEnabled(true);
	stars->setFixedRotation( math::V3f(1.0f, 1.0f, 0.0f).normalized(), math::degToRad(5.0f) );
	stars->setMotionblurScale(1.0f);
	// volume
	Volume::Ptr volume = Volume::create();
	volume->load( "c:\\projects\\demo\\git\\bin\\data\\artifix_resized_moved.bgeo" );
	// post process
	PostProcess::Ptr post = PostProcess::create();
	post->setHDREnabled(true);
	post->setGlareEnabled(true);
	post->setGlareBlurIterations(4);
	post->setGlareAmount(0.8f);


	// manix shot ---------
	{
		// create shot
		SwitchedShot::Ptr shot = SwitchedShot::create( scene->getSwitcher("switcher1") );

		Shot::ShotElement::Ptr se = Shot::ShotElement::create(post);
		se->addChild(black);
		Shot::ShotElement::Ptr volumese = se->addChild(volume);
		volumese->setController("PointLightPosition", scene->getLocator("null1")->xform->translation);
		volumese->setController("PointLightIntensity", scene->getChannel("ch1.x"));
		//volumese->setController(volume->m_transferFunction->getNode(1), "density", scene->getChannel("tfnode.density"));
		//se->addChild(stars);
		shot->addElement(se);
		addShot(shot);
	}

	// geometry --------
	{
		SwitchedShot::Ptr shot = SwitchedShot::create( scene_test->getSwitcher("switcher1") );

		Shot::ShotElement::Ptr se = Shot::ShotElement::create(post);
		// ------------
		base::Geometry::Ptr geo = houdini::HouGeoIO::importGeometry("c:\\projects\\demo\\git\\bin\\data\\test.bgeo");
		base::Shader::Ptr shader = base::Shader::loadFromFile( "c:\\projects\\demo\\git\\src\\core\\glsl\\genericShader" );
		shader->setUniform("l", math::V3f(1.0f).normalized());
		shader->setUniform("ka", 0.1f);
		shader->setUniform("ambient", math::V3f(1.0f));
		shader->setUniform("kd", 1.0f);
		shader->setUniform("diffuse", math::V3f(.5f));
		Element::Ptr renderGeo = RenderGeometry::create(geo, shader);

		se->addChild(black);
		//se->addChild(stars);
		se->addChild(renderGeo);

		shot->addElement(se);
		addShot(shot);
	}



	// add clips
	// clips define when on the global timeline which shot will be rendered
	addClip( 0, 0.0f, 24.0f, 24.0f );
	//addClip( 1, 24.0f, 48.0f, 24.0f );



	// load audio ----
	if(m_audio)
		m_audio->load("c:\\projects\\demo\\git\\bin\\data\\heart_of_courage.ogg");
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
