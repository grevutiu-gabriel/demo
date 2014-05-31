#include "Context.h"
#include <gltools/gl.h>
#include <gltools/misc.h>



namespace base
{


	Context::Context()
	{
		/*
//		// setup path
//		if( !pathIsRegistered("src") )
//			pathRegister( "src", base::Path( SRC_PATH ) );
//		if( !pathIsRegistered("base") )
//			pathRegister( "base", base::Path( BASE_PATH ) );
//#ifdef OPERATORS_SRC_PATH
//		pathRegister( "ops_src", base::Path( OPERATORS_SRC_PATH ) );
//#endif
//#ifdef OPERATORS_DATA_PATH
//		pathRegister( "ops_data", base::Path( OPERATORS_DATA_PATH ) );
//#endif
*/

		//
		// setup transform stuff
		//
		m_currentTransformState.modelMatrix = math::Matrix44f::Identity();
		m_currentTransformState.viewMatrix = math::Matrix44f::Identity();
		m_currentTransformState.projectionMatrix = math::Matrix44f::Identity();
		m_currentTransformState.viewInverseMatrix = math::Matrix44f::Identity();
		m_currentTransformState.modelViewMatrix = math::Matrix44f::Identity();

		// projection matrix
		m_pmAttr = Attribute::createM44f();
		m_pmAttr->appendElement( math::Matrix44f::Identity() );

		// view projection matrix
		m_mvpmAttr = Attribute::createM44f();
		m_mvpmAttr->appendElement( math::Matrix44f::Identity() );

		// model view matrix
		m_mvmAttr = Attribute::createM44f();
		m_mvmAttr->appendElement( math::Matrix44f::Identity() );

		// model view matrix inverse transpose
		m_mvminvtAttr = Attribute::createM33f();
		m_mvminvtAttr->appendElement( math::Matrix33f::Identity() );

		m_mmAttr = Attribute::createM44f();
		m_mmAttr->appendElement( math::Matrix44f::Identity() );


		m_vmAttr = Attribute::createM44f();
		m_vmAttr->appendElement( math::Matrix44f::Identity() );

		// view matrix inverse (camera transform)
		m_vminvAttr = Attribute::createM44f();
		m_vminvAttr->appendElement( math::Matrix44f::Identity() );


		// register predefined globals ----
		m_globalUniforms.insert( std::make_pair( "mvpm", m_mvpmAttr ) );
		m_globalUniforms.insert( std::make_pair( "mvm", m_mvmAttr ) );
		m_globalUniforms.insert( std::make_pair( "mvminvt", m_mvminvtAttr ) );
		m_globalUniforms.insert( std::make_pair( "vminv", m_vminvAttr ) );
		m_globalUniforms.insert( std::make_pair( "vm", m_vmAttr ) );
		m_globalUniforms.insert( std::make_pair( "mm", m_mmAttr ) );
		m_globalUniforms.insert( std::make_pair( "pm", m_pmAttr ) );


		// initialize standard resources ----
		m_screenQuad = Geometry::createQuad();

		//m_screenQuadVFlipped = geo_quad( Geometry::TRIANGLE );
		//gfx::apply_transform(m_screenQuadVFlipped, math::M44f::ScaleMatrix( 1.0f, -1.0f, 1.0f ));
		/*
		// we initialize mvpm uniform to identity. we can do this here because currently m_simpleTextureShader is only used for
		// screenquad rendering
		m_simpleTextureShader = Shader::create().attachPSFromFile("$APPDATA/glsl/simpleTexture.ps.glsl").attachPSFromFile("$APPDATA/glsl/simpleTexture.vs.glsl");
		*/
		//m_simpleTextureShaderScreen = Shader::create().attachPSFromFile("$APPDATA/glsl/simpleTexture.ps.glsl").attachVSFromFile("$APPDATA/glsl/simpleTexture.vs.glsl");
		//m_simpleTextureShaderScreen->setUniform( "mvpm", math::Matrix44f::Identity() );
		m_simpleTextureShaderScreen = Shader::createSimpleTextureShader();
		m_simpleTextureShaderScreen->setUniform( "mvpm", math::Matrix44f::Identity() );

		/*
		m_constantShader = Shader::createSimpleConstantShader(1.0f, 1.0f, 1.0f);

		// initialize noise glsl module
		//setUniform("noise_permTexture", base::glsl::noisePermutationTableTex()->getUniform());
		*/
	}

	Context::~Context()
	{
	}


	Context::Ptr Context::create()
	{
		Context::Ptr c = std::make_shared<Context>();
		setCurrentContext(c);
		return c;
	}


	void Context::setUniform( const std::string &name, Attribute::Ptr uniform )
	{
		m_globalUniforms[name] = uniform;
	}


	Context::Ptr Context::g_currentContext = Context::Ptr();

	// sets current context
	Context::Ptr Context::getCurrentContext()
	{
		return g_currentContext;
	}

	// sets current context
	void Context::setCurrentContext( Context::Ptr context )
	{
		g_currentContext = context;
	}


	Attribute::Ptr Context::getUniform( const std::string &name )
	{
		std::map<std::string, Attribute::Ptr>::iterator it = m_globalUniforms.find( name );
		if(it != m_globalUniforms.end())
			return it->second;
		return Attribute::Ptr();
	}

	void Context::renderScreen( Texture2dPtr texture )
	{
		// set texture to shader
		m_simpleTextureShaderScreen->setUniform( "texture", texture->getUniform() );
		render( m_screenQuad, m_simpleTextureShaderScreen );
	}


	void Context::render( Geometry::Ptr geo, Shader::Ptr shader, const math::Matrix44f &xform )
	{
		TransformState ts;
		getTransformState(ts);
		setModelMatrix(xform);
		render( geo, shader );
		setTransformState(ts);
	}

	void Context::renderScreen( Shader::Ptr shader )
	{
		render( m_screenQuad, shader );
	}

	void Context::render( Geometry::Ptr geo, Shader::Ptr shader )
	{
		bind(shader, geo);

		// render primitives
		geo->bindIndexBuffer();
		glDrawElements(geo->primitiveType(), geo->numPrimitives()*geo->numPrimitiveVertices(), GL_UNSIGNED_INT, 0);

		unbind( shader, geo );
	}


	void Context::bind( Shader::Ptr shader, Geometry::Ptr geo )
	{
		if(shader && shader->isOk())
		{
			// used for assigning texture units to sampler uniform attributes
			Attribute::g_nextTextureUnit = 0;

			glUseProgram(shader->m_glProgram);

			if(geo)
				// iterate all active attributes and bind attributes from geometry
				for( std::map<std::string, int>::iterator it = shader->m_activeAttributes.begin(); it != shader->m_activeAttributes.end(); ++it )
				{
					const std::string &name = it->first;
					int attrIndex = it->second;
					// getAttr and bind(with index)
					if (geo->hasAttr(name))
						geo->getAttr(name)->bindAsAttribute(attrIndex);
				}

			// iterate all active uniforms
			for( auto it = shader->m_activeUniforms.begin(), end = shader->m_activeUniforms.end(); it != end; ++it )
			{
				const std::string &name = it->first;
				int uniformIndex = it->second;
//				// if geometry has uniform attached
//				if(0)
//				{
//					// use it
//					// iterate all uniforms of the geometry
//					//for( int j=0; j<m_uniforms.size(); ++j )
//					//{
//					//	char *t1 = (char *)shader->m_activeUniformNames.m_data[i];
//					//	char *t2 = (char *)m_uniformNames.m_data[j];
//					//	if( !strcmp( (char *)shader->m_activeUniformNames.m_data[i], m_uniformNames.m_data[j] ) )
//					//		getUniform(j)->bindAsUniform( shader->m_activeUniforms.m_data[i] );
//					//}
//				}else

				// otherwise look if the shader has it
				if( shader->hasUniform(name) )
				{
					shader->getUniform(name)->bindAsUniform(uniformIndex);
				}else
				// otherwise look in the globals to see if we can take it from there
				{
					std::map<std::string, Attribute::Ptr>::iterator it = m_globalUniforms.find( name );

					if( it != m_globalUniforms.end() )
						it->second->bindAsUniform(uniformIndex);
				}
			}
		}
	}

	void Context::unbind( Shader::Ptr shader, Geometry::Ptr geo )
	{
		if(shader && shader->isOk())
		{
//			if(geo)
//				// iterate all active attributes and unbind attributes
//				for( std::map<std::string, int>::iterator it = shader->m_activeAttributes.begin(); it != shader->m_activeAttributes.end(); ++it )
//				{
//					const std::string &name = it->first;
//					int attrIndex = it->second;
//					// getAttr and bind(with index)
//					if (geo->hasAttr(name))
//						geo->getAttr(name)->unbindAsAttribute(attrIndex);
//				}


			// disable shader
			glUseProgram(0);
		}
	}


	// convinience function for updating transformState from view matrices
	void Context::setView( const math::M44f &viewToWorldTransform, const math::M44f &proj )
	{
		math::M44f view = viewToWorldTransform.inverted();
		setView( view, viewToWorldTransform, proj);
	}

	void Context::setView( const math::Matrix44f &view, const math::Matrix44f &viewInv, const math::Matrix44f &proj )
	{
		m_currentTransformState.viewMatrix = view;
		m_vmAttr->set( 0, m_currentTransformState.viewMatrix );

		m_currentTransformState.projectionMatrix = proj;
		m_pmAttr->set( 0, m_currentTransformState.projectionMatrix );

		// update camera transform
		m_currentTransformState.viewInverseMatrix = viewInv;
		m_vminvAttr->set( 0, m_currentTransformState.viewInverseMatrix );

		// updateModelViewProjection
		m_currentTransformState.modelViewProjectionMatrix = m_currentTransformState.modelMatrix * m_currentTransformState.viewMatrix * m_currentTransformState.projectionMatrix;
		m_mvpmAttr->set( 0, m_currentTransformState.modelViewProjectionMatrix );


		//update ModelViewMatrix
		m_currentTransformState.modelViewMatrix = m_currentTransformState.modelMatrix * m_currentTransformState.viewMatrix;
		m_mvmAttr->set( 0, m_currentTransformState.modelViewMatrix );

		//update ModelViewMatrixInverseTranspose
		m_currentTransformState.modelViewInverse = m_currentTransformState.modelViewMatrix.inverted();

		math::Matrix44f m = m_currentTransformState.modelViewInverse.transposed();

		// we do the transpose when we extract the orientation from 44f matrix
		m_currentTransformState.modelViewInverseTranspose.ma[0] = m.m[0][0];
		m_currentTransformState.modelViewInverseTranspose.ma[1] = m.m[0][1];
		m_currentTransformState.modelViewInverseTranspose.ma[2] = m.m[0][2];
		m_currentTransformState.modelViewInverseTranspose.ma[3] = m.m[1][0];
		m_currentTransformState.modelViewInverseTranspose.ma[4] = m.m[1][1];
		m_currentTransformState.modelViewInverseTranspose.ma[5] = m.m[1][2];
		m_currentTransformState.modelViewInverseTranspose.ma[6] = m.m[2][0];
		m_currentTransformState.modelViewInverseTranspose.ma[7] = m.m[2][1];
		m_currentTransformState.modelViewInverseTranspose.ma[8] = m.m[2][2];

		m_mvminvtAttr->set( 0, m_currentTransformState.modelViewInverseTranspose );
	}



	// used by fbos
	void Context::pushViewport( int width, int height, FBO* fbo )
	{
		ViewportState vs;
		vs.width = width;
		vs.height = height;
		vs.fbo = fbo;
		m_viewportStack.push( vs );
		glPushAttrib(GL_VIEWPORT_BIT);
		glViewport(0,0,width, height);
	}
	Context::ViewportState Context::popViewport()
	{
		m_viewportStack.pop();
		glPopAttrib();
		if( m_viewportStack.empty() )
			return Context::ViewportState();
		return m_viewportStack.top();
	}
	int Context::getViewportWidth()const
	{
		return m_viewportStack.top().width;
	}
	int Context::getViewportHeight()const
	{
		return m_viewportStack.top().height;
	}
	float Context::getViewportAspect()const
	{
		const ViewportState &vs = m_viewportStack.top();
		return float(vs.width)/float(vs.height);
	}

/*

	// sets current context
	void Context::setCurrentContext( ContextPtr context )
	{
		m_currentContext = context;
	}

	// sets time
	void Context::setTime( float time )
	{
		m_time = time;
	}

	// returns current time
	float Context::time()
	{
		return m_time;
	}
*/


	void Context::setCamera( Camera::Ptr camera )
	{
		m_camera = camera;
		if(m_camera)
			setView( camera->m_worldToView, camera->m_viewToWorld, camera->m_viewToNDC );
	}

	Camera::Ptr Context::camera()
	{
		return m_camera;
	}




	void Context::setModelMatrix( const math::Matrix44f &modelMatrix )
	{
		m_currentTransformState.modelMatrix = modelMatrix;
		m_mmAttr->set( 0, m_currentTransformState.modelMatrix );

		// updateModelViewProjection
		m_currentTransformState.modelViewProjectionMatrix = m_currentTransformState.modelMatrix * m_currentTransformState.viewMatrix * m_currentTransformState.projectionMatrix;
		m_mvpmAttr->set( 0, m_currentTransformState.modelViewProjectionMatrix );

		// update ModelViewMatrix
		m_currentTransformState.modelViewMatrix = m_currentTransformState.modelMatrix * m_currentTransformState.viewMatrix;
		m_mvmAttr->set( 0, m_currentTransformState.modelViewMatrix );

		// update ModelViewMatrixInverseTranspose
		m_currentTransformState.modelViewInverse = m_currentTransformState.modelViewMatrix.inverted();

		math::Matrix44f m = m_currentTransformState.modelViewInverse.transposed();

		// we do the transpose when we extract the orientation from 44f matrix
		m_currentTransformState.modelViewInverseTranspose.ma[0] = m.m[0][0];
		m_currentTransformState.modelViewInverseTranspose.ma[1] = m.m[0][1];
		m_currentTransformState.modelViewInverseTranspose.ma[2] = m.m[0][2];
		m_currentTransformState.modelViewInverseTranspose.ma[3] = m.m[1][0];
		m_currentTransformState.modelViewInverseTranspose.ma[4] = m.m[1][1];
		m_currentTransformState.modelViewInverseTranspose.ma[5] = m.m[1][2];
		m_currentTransformState.modelViewInverseTranspose.ma[6] = m.m[2][0];
		m_currentTransformState.modelViewInverseTranspose.ma[7] = m.m[2][1];
		m_currentTransformState.modelViewInverseTranspose.ma[8] = m.m[2][2];

		m_mvminvtAttr->set( 0,m_currentTransformState.modelViewInverseTranspose );
	}

	void Context::setViewMatrix( const math::Matrix44f &viewMatrix )
	{
		m_currentTransformState.viewMatrix = viewMatrix;
		m_currentTransformState.viewInverseMatrix = viewMatrix.inverted();

		// updateModelViewProjection
		m_currentTransformState.modelViewProjectionMatrix = m_currentTransformState.modelMatrix * m_currentTransformState.viewMatrix * m_currentTransformState.projectionMatrix;
		m_mvpmAttr->set( 0, m_currentTransformState.modelViewProjectionMatrix );

		//update ModelViewMatrix
		m_currentTransformState.modelViewMatrix = m_currentTransformState.modelMatrix * m_currentTransformState.viewMatrix;
		m_mvmAttr->set( 0, m_currentTransformState.modelViewMatrix );

		//update ModelViewMatrixInverseTranspose
		m_currentTransformState.modelViewInverse = m_currentTransformState.modelViewMatrix.inverted();

		math::Matrix44f m = m_currentTransformState.modelViewInverse.transposed();

		// we do the transpose when we extract the orientation from 44f matrix
		m_currentTransformState.modelViewInverseTranspose.ma[0] = m.m[0][0];
		m_currentTransformState.modelViewInverseTranspose.ma[1] = m.m[0][1];
		m_currentTransformState.modelViewInverseTranspose.ma[2] = m.m[0][2];
		m_currentTransformState.modelViewInverseTranspose.ma[3] = m.m[1][0];
		m_currentTransformState.modelViewInverseTranspose.ma[4] = m.m[1][1];
		m_currentTransformState.modelViewInverseTranspose.ma[5] = m.m[1][2];
		m_currentTransformState.modelViewInverseTranspose.ma[6] = m.m[2][0];
		m_currentTransformState.modelViewInverseTranspose.ma[7] = m.m[2][1];
		m_currentTransformState.modelViewInverseTranspose.ma[8] = m.m[2][2];

		m_mvminvtAttr->set( 0,m_currentTransformState. modelViewInverseTranspose );
	}
/*
	math::Vec3f Context::worldToView( const math::Vec3f &worldPos )
	{
		return math::transform( worldPos, m_currentTransformState.modelViewMatrix );
	}
	*/
	math::Matrix44f Context::getModelViewMatrix()
	{
		return m_currentTransformState.modelViewMatrix;
	}

	math::Matrix44f Context::getModelViewInverseMatrix()
	{
		return m_currentTransformState.modelViewInverse;
	}

	/*
	math::Matrix44f Context::getViewInverse()
	{
		return m_currentTransformState.viewInverseMatrix;
	}

*/

	void Context::getTransformState( TransformState &ts ) const
	{
		ts = m_currentTransformState;
	}

	void Context::setTransformState( const TransformState &ts )
	{
		m_currentTransformState = ts;
		m_mmAttr->set( 0, m_currentTransformState.modelMatrix );
		m_mvpmAttr->set( 0, m_currentTransformState.modelViewProjectionMatrix );
		m_mvmAttr->set( 0, m_currentTransformState.modelViewMatrix );
		m_vmAttr->set( 0, m_currentTransformState.viewMatrix );
		m_vminvAttr->set( 0, m_currentTransformState.viewInverseMatrix );
		m_mvminvtAttr->set( 0, m_currentTransformState.modelViewInverseTranspose );
	}

















/*

	void Context::renderScreenVFip( Texture2dPtr texture )
	{
		// set texture to shader
		m_simpleTextureShaderScreen->setUniform( "texture", texture->getUniform() );

		render( m_screenQuadVFlipped, m_simpleTextureShaderScreen );

	}









//	void TransformManager::updateModelViewProjection()
//	{
//		math::Matrix44f m = m_modelMatrix * m_viewMatrix * m_projectionMatrix;
//		m_mvpmAttr->setElement( 0, m.ma );
//	}

//	void TransformManager::updateModelViewMatrixInverseTranspose()
//	{
//		float m33f[9]; // result which will be uploaded

//		math::Matrix44f m = m_modelMatrix * m_viewMatrix;
//		m.invert();
//		m.transpose();

//		// we do the transpose when we extract the orientation from 44f matrix
//		m33f[0] = m.m[0][0];
//		m33f[1] = m.m[0][1];
//		m33f[2] = m.m[0][2];
//		m33f[3] = m.m[1][0];
//		m33f[4] = m.m[1][1];
//		m33f[5] = m.m[1][2];
//		m33f[6] = m.m[2][0];
//		m33f[7] = m.m[2][1];
//		m33f[8] = m.m[2][2];

//		m_mvminvtAttr->setElement( 0, m33f );
//	}
*/

	Texture2d::Ptr Context::getTexture2d( const std::string& name )
	{
		// check the register
		auto tex = m_2dtextures.find(name);
		if( tex != m_2dtextures.end() )
				return tex->second;
		// texture not found in register---try filesystem
		Texture2d::Ptr texture = Texture2d::load(name);
		if(texture)
		{
			addTexture2d(name,texture);
			return texture;
		}
		return Texture2d::Ptr();
	}


	void Context::addTexture2d( const std::string& name, Texture2d::Ptr texture )
	{
		m_2dtextures[name] = texture;
	}

}
