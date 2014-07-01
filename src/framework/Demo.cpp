#include "Demo.h"
#include "elements/volumept/Volume.h"
#include "elements/postprocess/PostProcess.h"
#include "elements/stars/Stars.h"
#include "elements/LensFlare/LensFlare.h"
#include "elements/Nebulae/Nebulae.h"
#include "elements/basic.h"

#include "controller/LoadGeometry.h"
#include "controller/LoadShader.h"
#include "controller/LoadTexture.h"
#include "controller/LoadVolume.h"

//#include "FileWatcher.h"
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

	DemoSerializer( Demo* demo, std::ostream *out, Demo::GuiInfoSerializationCallback serializeGuiInfo )
		:m_serializeGuiInfo(serializeGuiInfo)
	{
		m_writer= new JSONWriter( out );

		houdini::json::ObjectPtr root = houdini::json::Object::create();
		m_jsonObjectStack.push( root );
		demo->serialize(*this);
	}
	~DemoSerializer()
	{
		houdini::json::ObjectPtr root = m_jsonObjectStack.top();

		// serialize gui
		{
			houdini::json::ObjectPtr guiInfo = houdini::json::Object::create();
			m_jsonObjectStack.push( guiInfo );
			if(m_serializeGuiInfo)
				m_serializeGuiInfo(*this);
			m_jsonObjectStack.pop();
			root->append("gui", guiInfo);
		}


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

	virtual void write( const std::string& key, math::V3f value )
	{
		houdini::json::ArrayPtr array = houdini::json::Array::create();
		array->append( houdini::json::Value::create<float>(value.x) );
		array->append( houdini::json::Value::create<float>(value.y) );
		array->append( houdini::json::Value::create<float>(value.z) );
		m_jsonObjectStack.top()->append( key, array );
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
	Demo::GuiInfoSerializationCallback                m_serializeGuiInfo;
};


struct DemoDeserializer : public Deserializer
{

	DemoDeserializer(Demo* demo, std::istream* in, Demo::GuiInfoDeserializationCallback deserializeGuiInfo)
	{
		houdini::json::JSONLogger logger(std::cout);
		houdini::json::JSONReader reader;
		houdini::json::Parser p;
		//p.parse( &in, &logger );
		p.parse( in, &reader );

		//
		houdini::json::ObjectPtr root = reader.getRoot().asObject();

		// read objects
		m_serializedObjects = root->getArray( "objects" );

		// finally deserialize demo object
		m_jsonObjectStack.push(root);
		demo->deserialize( *this );

		// update property groups
		// see ListPropertyT for details
		for( auto it : m_deserializeMap )
		{
			std::vector<PropertyGroup::Ptr> propGroups;
			it.second->getPropertyGroups( propGroups );
			for(auto propGroup:propGroups)
				propGroup->synchronize();
		}

		//gui
		if(deserializeGuiInfo)
		{
			houdini::json::ObjectPtr guiInfo = root->getObject("gui");
			m_jsonObjectStack.push(guiInfo);
			deserializeGuiInfo(*this);
			m_jsonObjectStack.pop();
		}
		m_jsonObjectStack.pop();
	}

	virtual bool hasKey( const std::string& key )
	{
		return m_jsonObjectStack.top()->hasKey(key);
	}

	houdini::json::ArrayPtr readArray( const std::string& key )override
	{
		return m_jsonObjectStack.top()->getArray(key);
	}

	virtual std::string readString( const std::string& key, const std::string& defaultValue = "" )override
	{
		if( m_jsonObjectStack.top()->hasKey(key) )
			return m_jsonObjectStack.top()->get<std::string>(key);
		return defaultValue;
	}

	virtual float readFloat( const std::string& key, float defaultValue)
	{
		if( m_jsonObjectStack.top()->hasKey(key) )
			return m_jsonObjectStack.top()->get<float>(key);
		return defaultValue;
	}

	virtual float readInt( const std::string& key, int defaultValue )
	{
		if( m_jsonObjectStack.top()->hasKey(key) )
			return m_jsonObjectStack.top()->get<int>(key);
		return defaultValue;
	}

	virtual math::V3f readV3f( const std::string& key, math::V3f defaultValue )
	{
		math::V3f result = defaultValue;
		if( m_jsonObjectStack.top()->hasKey(key) )
		{
			houdini::json::ArrayPtr array = m_jsonObjectStack.top()->getArray(key);
			if(array)
			{
				result.x = array->get<float>(0);
				result.y = array->get<float>(1);
				result.z = array->get<float>(2);
			}
		}
		return result;
	}

	virtual Object::Ptr deserializeObject( houdini::json::Value value )
	{
		int id = value.as<int>();

		// find deserialized object
		auto it = m_deserializeMap.find(id);
		if(it!=m_deserializeMap.end())
			return it->second;

		// object not deserialized yet...deserialize it
		houdini::json::ObjectPtr serializedObject = m_serializedObjects->getObject(id);
		m_jsonObjectStack.push(serializedObject);
		std::string className = readString("type");
		Object::Ptr object = ObjectFactory::create(className);
		// register created object (done before deserialize in order to avoid infinit loops)
		m_deserializeMap[id] = object;
		object->deserialize(*this);
		m_jsonObjectStack.pop();



		return object;
	}

	virtual houdini::json::Value readValue( const std::string& key ) override
	{
		return m_jsonObjectStack.top()->getValue(key);
	}

private:

	houdini::json::ArrayPtr              m_serializedObjects;
	std::map<int, Object::Ptr>           m_deserializeMap; // objects to be serialized with their associated id
	std::stack<houdini::json::ObjectPtr> m_jsonObjectStack;
};




base::Texture2d::Ptr Demo::m_nocomp;


Demo::Demo( bool doAudio ) :
	Object(),
	m_currentCompositionIndex(0)
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

Composition::Ptr Demo::getComposition( int index )
{
	return m_compositions[index];
}

int Demo::getNumCompositions() const
{
	return int(m_compositions.size());
}

float Demo::getDuration() const
{
	if(m_scenes.empty())
		return 10.0f;
	return m_scenes[0]->getEndTime();
}

std::vector<Scene::Ptr> &Demo::getScenes()
{
	return m_scenes;
}

std::vector<Composition::Ptr> &Demo::getCompositions()
{
	return m_compositions;
}

// TODO: remove
void Demo::loadScene( const std::string& filename )
{
	Scene::Ptr scene = Scene::create();
	scene->load(filename);
	scene->setName(filename);
	addScene(scene);
}

void Demo::load( const std::string& filename, GuiInfoDeserializationCallback callback )
{
	std::string basePathData = base::path("data");
	std::string basePathSrc = base::path("src");

	///*
	m_filename = filename;
	std::cout << "Demo::load " << filename << std::endl;
	std::ifstream in( base::expand(filename).c_str(), std::ios_base::in);
	if(!in)
		return;

	int contentsSize = 0;
	{
		in.seekg(0, std::ios::end);
		contentsSize = in.tellg();
		in.seekg(0, std::ios::beg);
	}

	if(!contentsSize)
		return;

	DemoDeserializer deserializer(this, &in, callback);

	//*/
	//




//	loadScene("$DATA/artifix.scn");
//	loadScene("$DATA/test.scn");
//	loadScene("$DATA/nebulae.scn");
/*
	// create all object instances ---
	m_deserializeMap[0] = ObjectFactory::create("Clear");

	m_deserializeMap[1] = ObjectFactory::create("Stars");
	m_deserializeMap[2] = ObjectFactory::create("Volume");
	m_deserializeMap[3] = ObjectFactory::create("PostProcess");
	m_deserializeMap[4] = ObjectFactory::create("Nebulae");
	m_deserializeMap[5] = ObjectFactory::create("RenderGeometry");
	m_deserializeMap[6] = ObjectFactory::create("FlareShop");
	m_deserializeMap[7] = ObjectFactory::create("Composition");

	// deserialize all objects in a second pass (allows objects to reference each other in their serialization) ---
	//...

	// deserialize compositions ---
	addComposition( std::dynamic_pointer_cast<Composition>(m_deserializeMap[7]) );
*/

	/*
	Composition::Ptr composition = Composition::create();
	addComposition(composition);

	Element::Ptr clear = ObjectFactory::create<Element>("Clear");
	composition->addElement( clear );
	Element::Ptr renderGeo = ObjectFactory::create<Element>("RenderGeometry");
	//composition->addElement( renderGeo );
	Element::Ptr renderTex = ObjectFactory::create<Element>("RenderTexture");
	//composition->addElement( renderTex );

	Controller::Ptr toV3f = ObjectFactory::create<Controller>("FloatToV3fController");
	Controller::Ptr test = ObjectFactory::create<Controller>("SinusController");
	LoadGeometry::Ptr loadGeometry = ObjectFactory::create<LoadGeometry>("LoadGeometry");
	LoadShader::Ptr loadShader = ObjectFactory::create<LoadShader>("LoadShader");
	LoadTexture2d::Ptr loadTexture = ObjectFactory::create<LoadTexture2d>("LoadTexture2d");
	//loadGeometry->setFilename( "$DATA/test.bgeo" );
	loadShader->setFilename( "$DATA/matcap" );
	//loadTexture->setFilename( "$DATA/scary-light.jpg" );
	loadTexture->setFilename( "$DATA/00ZBrush_RedWax.png" );
	//composition->setPropertyController( clear, "color", toV3f );
	composition->setPropertyController( renderGeo, "geometry", loadGeometry );
	composition->setPropertyController( renderGeo, "shader", loadShader );
	composition->setPropertyController( loadShader, "tex", loadTexture );
	//composition->setPropertyController( toV3f, "x", test );
	//composition->setPropertyController( toV3f, "y", test );
	//composition->setPropertyController( toV3f, "z", test );

	//composition->setPropertyController( renderTex, "texture", loadTexture );
	//composition->setPropertyController( renderTex, "texture", SceneController::create(m_scenes[2],"/obj/geo1/volumeramp1/baked") );


	SceneController::Ptr ch1 = SceneController::create(m_scenes[0],"/ch/ch1.x");
	SceneController::Ptr cam1 = SceneController::create(m_scenes[0],"/obj/cam1");
	SceneController::Ptr switch1 = SceneController::create(m_scenes[1],"/obj/switcher1");
	composition->setPropertyController( toV3f, "y", ch1 );
	composition->setPropertyController( composition, "camera", switch1 );

	composition->prepareForRendering();
	*/
///*
///
///
	/*
	// volume -------
	Composition::Ptr composition = Composition::create();
	addComposition(composition);

	Element::Ptr clear = ObjectFactory::create<Element>("Clear");
	composition->addElement( clear );
	Volume::Ptr volume = ObjectFactory::create<Volume>("Volume");
	//FlareShop::Ptr flareshop = ObjectFactory::create<FlareShop>("FlareShop");
	//composition->addElement( flareshop );

	LoadVolume::Ptr loadVolume = ObjectFactory::create<LoadVolume>("LoadVolume");
	loadVolume->setFilename("$DATA/artifix_resized_moved.bgeo");
	composition->addElement( volume );

	composition->setPropertyController( composition, "camera", SceneController::create(m_scenes[0],"/obj/switcher1") );
	composition->setPropertyController( volume, "normalizedDensity", loadVolume);
	composition->setPropertyController( volume, "transferfunction", SceneController::create(m_scenes[0],"/obj/geo1/volumeramp1/baked"));
	composition->setPropertyController( volume, "localToWorld", loadVolume);
	composition->setPropertyController( volume, "PointLightPosition", SceneController::create(m_scenes[0],"/obj/null1/transform.translation"));
	composition->setPropertyController( volume, "PointLightColor", SceneController::create(m_scenes[0],"/obj/pointlight1/light_color"));
	composition->setPropertyController( volume, "PointLightIntensity", SceneController::create(m_scenes[0],"/obj/pointlight1/light_intensity"));
	*/

	/*
	// neublae
	Composition::Ptr composition = Composition::create();
	composition->setName("nebulae");
	addComposition(composition);

	Element::Ptr clear = ObjectFactory::create<Element>("Clear");
	clear->setName("clear");
	composition->addElement( clear );
	Volume::Ptr volume = ObjectFactory::create<Volume>("Volume");
	volume->setName("volume");

	LoadVolume::Ptr loadVolume = ObjectFactory::create<LoadVolume>("LoadVolume");
	loadVolume->setFilename("$DATA/nebulae200.bgeo");
	composition->addElement( volume );

	composition->setPropertyController( composition, "camera", SceneController::create(m_scenes[2],"/obj/cam1") );
	composition->setPropertyController( volume, "normalizedDensity", loadVolume);
	composition->setPropertyController( volume, "transferfunction", SceneController::create(m_scenes[2],"/obj/geo1/volumeramp1/baked"));
	composition->setPropertyController( volume, "localToWorld", loadVolume);
	composition->setPropertyController( volume, "PointLightPosition", SceneController::create(m_scenes[2],"/obj/pointlight1/transform.translation"));
	composition->setPropertyController( volume, "PointLightColor", SceneController::create(m_scenes[0],"/obj/pointlight1/light_color"));
	composition->setPropertyController( volume, "PointLightIntensity", SceneController::create(m_scenes[2],"/obj/pointlight1/light_intensity"));
	composition->setPropertyController( volume, "stepSize", SceneController::create(m_scenes[2],"/ch/stepsize"));




	composition->prepareForRendering();
	*/
//*/
//	// load elements ------
//	addElement( ObjectFactory::create<Element>("Clear") );
//	addElement( ObjectFactory::create<Element>("Stars") );
//	addElement( ObjectFactory::create<Element>("Volume") );
//	addElement( ObjectFactory::create<Element>("PostProcess") );
//	addElement( ObjectFactory::create<Element>("Nebulae") );
//	addElement( ObjectFactory::create<Element>("RenderGeometry") );
//	addElement( ObjectFactory::create<Element>("FlareShop") );

	// load compositions -----


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

	// manix composition ---------
	{
		// create composition
		Composition::Ptr composition = Composition::create();
		composition->m_cameraController = scene->getCamera("switcher1");


		CompositionElement::Ptr se = CompositionElement::create(post);
		se->addChild(black);
		se->addChild(volume);
		composition->setController(volume, "PointLightPosition", scene->getLocator("null1")->getProperty("translation"));
		composition->setController(volume, "PointLightIntensity", scene->getChannel("ch1.x"));
		//volumese->setController(volume->m_transferFunction->getNode(1), "density", scene->getChannel("tfnode.density"));
		//se->addChild(stars);
		composition->addElement(se);
		addComposition(composition);
	}

	// geometry --------
	{
		Composition::Ptr composition = Composition::create();
		composition->m_cameraController = scene_test->getCamera("switcher1");

		CompositionElement::Ptr se = CompositionElement::create(post);
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

		composition->addElement(se);

		//composition->addElement(black);
		//composition->addElement(renderGeo);

		addComposition(composition);
	}

	// Nebulae
	{
		Composition::Ptr composition = Composition::create();
		composition->m_cameraController = scene_test->getCamera("cam1");


		CompositionElement::Ptr se = CompositionElement::create(post);
		Nebulae::Ptr nebulae = Nebulae::create();
		nebulae->generate();

		se->addChild(black);
		se->addChild(stars);
		se->addChild(nebulae);

		composition->addElement(se);

		addComposition(composition);
	}



	// add clips
	// clips define when on the global timeline which composition will be rendered
	addClip( 0, 0.0f, 24.0f, 24.0f );
	addClip( 1, 24.0f, 48.0f, 24.0f );
	//addClip( 2, 36.0f, 72.0f, 24.0f );

//	// TEMP ----------
//	{
//		Element::Ptr black = Clear::create(math::V3f(0.0f, 0.0f, 0.0f));
//		FlareShop::Ptr fs = FlareShop::create();
//		Composition::Ptr composition = Composition::create();
//		composition->addElement(black);
//		composition->addElement(fs);
//		this->addComposition(composition);
//		addClip( 0, 0.0f, 1.0f, 1.0f );
//	}
	*/

	for( auto composition:m_compositions )
		composition->prepareForRendering();

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
	int newCompositionIndex = clip.compositionIndex;
//	//std::cout << time << " rendering composition " << newCompositionIndex << std::endl;

*/
	if( m_currentCompositionIndex < m_compositions.size() )
	{
		int newCompositionIndex = m_currentCompositionIndex;
		Composition::Ptr composition = m_compositions[newCompositionIndex];
		composition->render(context, time, overrideCamera);
	}else
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if(!m_nocomp)
			m_nocomp = base::Texture2d::load( base::expand("$DATA/framework/nocomp.png") );
		context->renderScreen(m_nocomp);
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
			scenes->append( out.serialize(scene) );
		}
		out.write( "scenes", scenes );
	}

	// compositions
	{
		houdini::json::ArrayPtr compositions = houdini::json::Array::create();
		for( auto it = m_compositions.begin(), end=m_compositions.end();it!=end;++it )
		{
			Composition::Ptr composition = *it;
			compositions->append( out.serialize(composition) );
		}
		out.write( "compositions", compositions );
	}
}

void Demo::deserialize(Deserializer &in)
{
	// scenes ------
	houdini::json::ArrayPtr scenes = in.readArray( "scenes" );
	for( int i=0,numElements=scenes->size();i<numElements;++i )
	{
		Scene::Ptr scene = std::dynamic_pointer_cast<Scene>(in.deserializeObject( scenes->getValue(i) ));
		addScene(scene);
	}
	// compositions ------
	houdini::json::ArrayPtr compositions = in.readArray( "compositions" );
	for( int i=0,numElements=compositions->size();i<numElements;++i )
	{
		Composition::Ptr composition = std::dynamic_pointer_cast<Composition>(in.deserializeObject( compositions->getValue(i) ));
		addComposition(composition);
	}
}

void Demo::addScene(Scene::Ptr scene)
{
	m_scenes.push_back(scene);
}

void Demo::save(const std::string &filename, GuiInfoSerializationCallback serializeGuiInfo)
{
	std::ofstream out( filename, std::ios::trunc );

	//DemoSerializer de( this, &std::cout );
	DemoSerializer de( this, &out, serializeGuiInfo );
}


int Demo::addComposition( Composition::Ptr composition )
{
	int index = int(m_compositions.size());
	m_compositions.push_back(composition);
	return index;
}

void Demo::addClip(int compositionIndex, float compositionStart , float compositionEnd, float clipDuration)
{
	Clip clip;
	clip.compositionStart = compositionStart;
	clip.compositionEnd = compositionEnd;
	clip.compositionIndex = compositionIndex;
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
