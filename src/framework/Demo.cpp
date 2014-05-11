#include "Demo.h"
#include "elements/volumept/Volume.h"
#include "elements/postprocess/PostProcess.h"
#include "elements/stars/Stars.h"
#include "elements/FlareShop/FlareShop.h"
#include "elements/Nebulae/Nebulae.h"

#include <stack>



struct JSONWriter
{
	JSONWriter( std::ostream *out )
	{
		m_writer= new houdini::json::ASCIIWriter( out );
	}
	~JSONWriter()
	{
		delete m_writer;
	}

	void writeValue( houdini::json::Value value )
	{
		if( value.isArray() )
		{
			houdini::json::ArrayPtr array = value.asArray();
			int numElements = array->size();
			m_writer->jsonBeginArray();
			for( int i=0;i<numElements;++i )
			{
				houdini::json::Value v = array->getValue(i);
				writeValue(v);
			}
			m_writer->jsonEndArray();
		}else
		if( value.isObject() )
		{
			houdini::json::ObjectPtr obj = value.asObject();
			std::vector<std::string> keys;
			obj->getKeys(keys);
			m_writer->jsonBeginMap();
			for( auto it = keys.begin(), end=keys.end();it!=end;++it )
			{
				std::string key = *it;
				m_writer->jsonKey( key );
				writeValue( obj->getValue(key) );
			}
			m_writer->jsonEndMap();
		}else
		{
			houdini::json::Value::Variant variant = value.getVariant();
			ttl::var::apply_visitor(*this, variant);
		}
	}
	void operator()( bool value )
	{
		m_writer->jsonBool(value);
	}
	void operator()( float value )
	{
		m_writer->jsonReal32(value);
	}
	void operator()( double value )
	{
		m_writer->jsonReal64(value);
	}
	void operator()( int value )
	{
		m_writer->jsonInt32(value);
	}
	void operator()( sint64 value )
	{
		m_writer->jsonInt64(value);
	}
	void operator()( ubyte value )
	{
		m_writer->jsonUInt8(value);
	}
	void operator()( std::string value )
	{
		m_writer->jsonString(value);
	}
private:
	houdini::json::ASCIIWriter* m_writer;
};




struct DemoSerializer : public Serializer
{
	struct SerializedObject
	{
		int id;
		houdini::json::Value value;
	};

	DemoSerializer( Demo* demo, std::ostream *out )
	{
		m_writer= new JSONWriter( out );

		houdini::json::ObjectPtr root = houdini::json::Object::create();
		m_jsonObjectStack.push( root );

		demo->serialize(*this);
	}
	~DemoSerializer()
	{
		houdini::json::ObjectPtr root = m_jsonObjectStack.top();
		m_jsonObjectStack.pop();

		// objects
		{
			int numObjects = int(m_serializeMap.size());
			houdini::json::ArrayPtr objects = houdini::json::Array::create();
			std::vector<Object::Ptr> sorted(numObjects);
			for( auto it=m_serializeMap.begin(),end=m_serializeMap.end();it!=end;++it )
				sorted[it->second.id] = it->first;

			for( auto it=sorted.begin(),end=sorted.end();it!=end;++it )
			{
				Object::Ptr obj = *it;
				objects->append( m_serializeMap[obj].value );
			}
			root->append( "objects", objects );
		}

		// now, finally write out the json object hierarchy
		m_writer->writeValue( houdini::json::Value::createObject(root) );
		delete m_writer;
	}

	// overrides from Serializer ---
	virtual void write( const std::string& key, unsigned char value )override
	{
		m_jsonObjectStack.top()->appendValue<unsigned char>( key, value );
	}

	virtual void write( const std::string& key, const std::string& value )override
	{
		m_jsonObjectStack.top()->appendValue<std::string>( key, value );
	}

	virtual void write( const std::string& key, float value )override
	{
		m_jsonObjectStack.top()->appendValue<float>( key, value );
	}

	virtual void write( const std::string& key, int value )override
	{
		m_jsonObjectStack.top()->appendValue<int>( key, value );
	}

	virtual void write( const std::string& key, Object::Ptr object )override
	{
		m_jsonObjectStack.top()->append( key, serialize(object) );
	}

	virtual void write( const std::string& key, houdini::json::ObjectPtr jsonObject )
	{
		m_jsonObjectStack.top()->append( key, jsonObject );
	}

	virtual void write( const std::string& key, houdini::json::ArrayPtr array )
	{
		m_jsonObjectStack.top()->append( key, array );
	}

	virtual void write( const std::string& key, houdini::json::Value value )override
	{
		m_jsonObjectStack.top()->append( key, value );
	}

	virtual houdini::json::Value serialize( Object::Ptr object )override
	{
		auto it = m_serializeMap.find( object );
		if( it != m_serializeMap.end() )
			return houdini::json::Value::create<int>(it->second.id);

		// object has not been serialized yet --- so serialize it now
		SerializedObject so;
		// get id
		so.id = int(m_serializeMap.size());
		houdini::json::ObjectPtr json = houdini::json::Object::create();
		so.value = houdini::json::Value::createObject(json);
		// store object for serialization
		// this is done before serialization because otherwise the ids wont
		// get incremented
		m_serializeMap.insert(std::make_pair(object, so));


		// push json object onto stack which will hold serialized data from object
		m_jsonObjectStack.push( json );
		// now serialize object...everything goes into the object  on the object stack
		object->serialize(*this);
		m_jsonObjectStack.pop();


		// done - return reference
		return houdini::json::Value::create<int>(so.id);
	}


private:




	JSONWriter* m_writer;
	std::map<Object::Ptr, SerializedObject>           m_serializeMap; // objects to be serialized with their associated id
	std::stack<houdini::json::ObjectPtr>              m_jsonObjectStack;

};









Demo::Demo( bool doAudio )
{
	base::setVariable("$DATA", base::path("data").str());
	if(doAudio)
		m_audio = std::make_shared<Audio>();
}



//void Demo::addElement(Element::Ptr element)
//{
//	if(!element)
//		throw std::runtime_error( "Demo::addElement: nullptr" );
//	m_elements.push_back(element);
//}

Shot::Ptr Demo::getShot( int index )
{
	return m_shots[index];
}

int Demo::getNumShots() const
{
	return int(m_shots.size());
}

void Demo::loadScene( const std::string& filename )
{
	Scene::Ptr scene = Scene::create();
	scene->load(filename);
	m_scenes.push_back(scene);
}

void Demo::load( const std::string& filename )
{
	std::string basePathData = base::path("data");
	std::string basePathSrc = base::path("src");


	// load scenes ------
	loadScene("$DATA/artifix.scn");
	loadScene("$DATA/test.scn");
/*
	// create all object instances ---
	m_deserializeMap[0] = ObjectFactory::create("Clear");

	m_deserializeMap[1] = ObjectFactory::create("Stars");
	m_deserializeMap[2] = ObjectFactory::create("Volume");
	m_deserializeMap[3] = ObjectFactory::create("PostProcess");
	m_deserializeMap[4] = ObjectFactory::create("Nebulae");
	m_deserializeMap[5] = ObjectFactory::create("RenderGeometry");
	m_deserializeMap[6] = ObjectFactory::create("FlareShop");
	m_deserializeMap[7] = ObjectFactory::create("Shot");

	// deserialize all objects in a second pass (allows objects to reference each other in their serialization) ---
	//...

	// deserialize shots ---
	addShot( std::dynamic_pointer_cast<Shot>(m_deserializeMap[7]) );
*/

	Shot::Ptr shot = Shot::create();
	addShot(shot);

	Element::Ptr clear = ObjectFactory::create<Element>("Clear");
	shot->addElement( clear );

	Controller::Ptr toV3f = ObjectFactory::create<Controller>("FloatToV3fController");
	Controller::Ptr test = ObjectFactory::create<Controller>("SinusController");
	shot->setPropertyController( clear, "color", toV3f );
	//shot->setPropertyController( toV3f, "x", test );
	//shot->setPropertyController( toV3f, "y", test );
	//shot->setPropertyController( toV3f, "z", test );

	SceneController::Ptr ch1 = SceneController::create(m_scenes[0],"ch1.x");
	shot->setPropertyController( toV3f, "y", ch1 );

	shot->prepareForRendering();


//	// load elements ------
//	addElement( ObjectFactory::create<Element>("Clear") );
//	addElement( ObjectFactory::create<Element>("Stars") );
//	addElement( ObjectFactory::create<Element>("Volume") );
//	addElement( ObjectFactory::create<Element>("PostProcess") );
//	addElement( ObjectFactory::create<Element>("Nebulae") );
//	addElement( ObjectFactory::create<Element>("RenderGeometry") );
//	addElement( ObjectFactory::create<Element>("FlareShop") );

	// load shots -----


	/*
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
	*/

	// load audio ----
	//if(m_audio)
	//	m_audio->load(basePathData + "/heart_of_courage.ogg");
}


void Demo::render( base::Context::Ptr context, float time, base::Camera::Ptr overrideCamera )
{
/*
	//std::cout << "Demo::render\n";

	//TODO: update time controller

	int newClipIndex = m_clipIndex.evaluate(time);
	Clip& clip = m_clips[newClipIndex];
	int newShotIndex = clip.shotIndex;
//	//std::cout << time << " rendering shot " << newShotIndex << std::endl;

*/
	if( !m_shots.empty() )
	{
		int newShotIndex = 0;
		Shot::Ptr shot = m_shots[newShotIndex];
		shot->render(context, time, overrideCamera);
	}
}

void Demo::serialize(Serializer &out)
{
	// scenes
	{
		houdini::json::ArrayPtr scenes = houdini::json::Array::create();
		for( auto it = m_scenes.begin(), end=m_scenes.end();it!=end;++it )
		{
			Scene::Ptr scene = *it;
			//scenes->append( houdini::json::Value::create<std::string>(scene->getFilename()) );
			scenes->append( out.serialize(scene) );
		}
		out.write( "scenes", scenes );
	}

	// shots
	{
		houdini::json::ArrayPtr shots = houdini::json::Array::create();
		for( auto it = m_shots.begin(), end=m_shots.end();it!=end;++it )
		{
			Shot::Ptr shot = *it;
			shots->append( out.serialize(shot) );
		}
		out.write( "shots", shots );
	}
}

void Demo::store(const std::string &filename)
{
	DemoSerializer de( this, &std::cout );
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


REGISTERCLASS(Demo)
