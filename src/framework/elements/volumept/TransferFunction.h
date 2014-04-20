#pragma once

#include <util/shared_ptr.h>
#include <gltools/gl.h>
#include <gltools/misc.h>
#include <util/StringManip.h>
#include <util/Path.h>
#include <util/PiecewiseLinearFunction.h>
#include <gfx/Geometry.h>
#include <gfx/Shader.h>
#include <gfx/Texture.h>
#include <gfx/Image.h>
#include <gfx/Context.h>
#include <gfx/FBO.h>
#include <gfx/Field.h>


struct TransferFunction;

struct TransferFunctionNode
{
	TransferFunctionNode( TransferFunction* tf );

private:
	TransferFunction* m_tf;
};


struct TransferFunction
{
	typedef std::shared_ptr<TransferFunction> Ptr;
	typedef base::PiecewiseLinearFunction<math::V4f> PLF;

	TransferFunction();

	void                                   setPLF( PLF plf );
	void                                   updateTexture(); // bakes pfl into texture


	PLF                                    m_plf;
	int                                    m_numSamples;
	std::vector<math::V4f>                 m_samples;
	base::Texture1d::Ptr                   m_texture;
	float                                  m_st_max;
};

struct AnimatedTransferFunction
{
	typedef std::shared_ptr<AnimatedTransferFunction> Ptr;
	typedef base::PiecewiseLinearFunction<math::V4f> PLF;

	AnimatedTransferFunction();
	//AnimatedTransferFunction( const AnimatedTransferFunction& other );

	void                                   clear();
	void                                   addPLF( float time, PLF plf );
	void                                   updateTexture(); // bakes pfl into texture

	std::vector<float>                     m_time;
	std::vector<PLF>                       m_values;

	float                                  m_time_min;
	float                                  m_time_max;

	int                                    m_numSamplesTime;
	int                                    m_numSamplesDomain;
	std::vector<math::V4f>                 m_samples;
	base::Texture2d::Ptr                   m_texture;
};

