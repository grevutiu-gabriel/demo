#pragma once
#include <math/Math.h>
#include <util/shared_ptr.h>

namespace base
{
	class Camera
	{
	public:

		typedef std::shared_ptr<Camera> Ptr;

		Camera();
		Camera(float fov, float aspect, float znear=0.1f, float zfar=1000.0f );

		math::Ray3f                 generateRay( const math::V2f &rsP ) const;
		void                        setRaster( int width, int height, float aspect );
		void                        setViewToWorld( math::M44f &viewToWorld );
		void                        setProjection( math::M44f &viewToNDC );

		//void                        store( core::json::ObjectPtr o );
		//void                        load( core::json::ObjectPtr o );


		math::M44f                  m_worldToView; ///< the matrix which transforms from world to camera space - the inverse of this matrix is the transform of the camera
		math::M44f                  m_viewToNDC;   // projection matrix
		math::M44f                  m_NDCToRaster;

		math::M44f                  m_rasterToNDC;
		math::M44f                  m_NDCToView;   // projection matrix
		math::M44f                  m_viewToWorld; ///< the matrix which transforms the camera from local into world space - the inverse of this matrix is the view transform

		math::M44f                  m_rasterToView;

		float                       m_znear;
		float                       m_zfar;
		float                       m_fov; // in radians
		float                       m_aspect;
		int                         m_width;
		int                         m_height;
	};
}
