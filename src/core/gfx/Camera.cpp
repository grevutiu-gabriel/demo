#include "Camera.h"

/*
namespace temp
{
	core::json::Value toValue( const math::Matrix44f &matrix )
	{
		core::json::Value v = core::json::Value::createArray();
		core::json::ArrayPtr a = v.asArray();

		for( int i=0;i<16;++i )
			a->append( core::json::Value::create<float>(matrix.ma[i]) );

		return v;
	}

	math::Matrix44f toMatrix44f( core::json::Value value )
	{
		core::json::ArrayPtr a = value.asArray();
		math::Matrix44f result;
		for( int i=0;i<16;++i )
			result.ma[i] = a->get<float>(i);
		return result;
	}
}
*/

namespace base
{
	Camera::Camera()
	{
		m_znear = 0.1f;
		m_zfar  = 1000.0f;
		m_fov   = math::degToRad(45.0f);

		setRaster( 1, 1, 1.0f );

		m_viewToNDC = math::projectionMatrix( m_fov, m_aspect, m_znear, m_zfar );
		m_NDCToView = m_viewToNDC.inverted();

		m_worldToView = math::Matrix44f::Identity();
		m_viewToWorld = m_worldToView.inverted();

		m_rasterToView = m_rasterToNDC*m_NDCToView;
	}

	Camera::Camera( float fov, float aspect, float znear, float zfar ) : m_fov(fov), m_aspect(aspect), m_zfar(zfar), m_znear(znear)
	{
		setRaster( 1, 1, aspect );

		m_viewToNDC = math::projectionMatrix( m_fov, m_aspect, m_znear, m_zfar );
		m_NDCToView = m_viewToNDC.inverted();

		m_worldToView = math::Matrix44f::Identity();
		m_viewToWorld = m_worldToView.inverted();

		m_rasterToView = m_rasterToNDC*m_NDCToView;
	}

	math::Ray3f Camera::generateRay( const math::V2f &_rsP ) const
	{
		math::Vec3<float> rsP(_rsP.x, _rsP.y, 0.0f);
		math::Ray3<float> ray;

		// ray origin in view space
		ray.o = math::Vec3<float>(0.0f, 0.0f, 0.0f);
		// ray directon in view space
		ray.d = math::transform( rsP, m_rasterToView );

		// transform ray from camera space into worldspace
		ray.o = math::transform( ray.o, m_viewToWorld );
		ray.d = math::normalize( math::transform( ray.d, m_viewToWorld.getOrientation() ) );

		return ray;
	}


	void Camera::setRaster( int width, int height, float aspect )
	{
		m_width = width;
		m_height = height;

		m_aspect = aspect;

		// TODO: update projection

		float screen[4];
		screen[0] = -m_aspect;
		screen[1] = m_aspect;
		screen[2] = float(-1.0f);
		screen[3] = float(1.0f);
		m_NDCToRaster = math::M44f::TranslationMatrix(math::V3f(-screen[0], -screen[3], 0.f)) * math::M44f::ScaleMatrix(1.f / (screen[1] - screen[0]), 1.f / (screen[2] - screen[3]), 1.f) * math::M44f::ScaleMatrix( float(m_width), float(m_height), 1.0f );
		m_rasterToNDC = m_NDCToRaster.inverted();
		m_rasterToView = m_rasterToNDC*m_NDCToView;
	}

	void Camera::setPlaneDistances( float znear, float zfar )
	{
		m_znear = znear;
		m_zfar = zfar;
		setProjection( math::projectionMatrix( m_fov, m_aspect, m_znear, m_zfar ) );
	}


	void Camera::setViewToWorld( math::M44f &viewToWorld )
	{
		m_viewToWorld = viewToWorld;
		m_worldToView = m_viewToWorld.inverted();
	}

	void Camera::setProjection( math::M44f &viewToNDC )
	{
		m_viewToNDC = viewToNDC;
		m_NDCToView = m_viewToNDC.inverted();
		m_rasterToView = m_rasterToNDC*m_NDCToView;
	}

	/*
	void Camera::store( core::json::ObjectPtr o )
	{
		//  m_transform, m_znear, m_zfar, m_fov, m_aspectRatio
		o->append( "aspectRatio",core::json::Value::create<float>( m_aspect ));
		o->append( "fov", core::json::Value::create<float>( math::radToDeg(m_fov)) );
		o->append( "zfar", core::json::Value::create<float>( m_zfar ));
		o->append( "znear", core::json::Value::create<float>( m_znear ));
		o->append( "transform", temp::toValue(m_viewToWorld));
	}

	void Camera::load( core::json::ObjectPtr o )
	{
		m_aspect = o->get<float>( "aspectRatio", m_aspect );
		m_fov = math::degToRad( o->get<float>( "fov", m_fov ) );
		m_zfar = o->get<float>( "zfar", m_zfar );
		m_znear = o->get<float>( "znear", m_znear );

		// update camera ---
		setRaster( 1, 1 );

		m_viewToNDC = math::projectionMatrix( m_fov, m_aspect, m_znear, m_zfar );
		m_NDCToView = m_viewToNDC.inverted();

		setViewToWorld(temp::toMatrix44f( o->getValue("transform") ));

		m_rasterToView = m_rasterToNDC*m_NDCToView;
	}
	*/
}
