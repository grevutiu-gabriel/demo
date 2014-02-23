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

