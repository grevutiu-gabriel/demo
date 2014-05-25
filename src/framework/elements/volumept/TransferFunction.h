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

#include "../../Object.h"


struct TransferFunction;

struct TransferFunctionNode : public Object
{
	typedef std::shared_ptr<TransferFunctionNode> Ptr;
	TransferFunctionNode( TransferFunction* tf, int index );

	void setDensity( float density );
	float getDensity()const;
	void setMappedDensity( float mappedDensity);
	float getMappedDensity()const;
	void setColor( const math::V3f& color );
	math::V3f getColor()const;

private:
	int m_index;
	TransferFunction* m_tf;
	friend struct TransferFunction;
};


struct TransferFunction
{
	typedef std::shared_ptr<TransferFunction> Ptr;
	typedef base::PiecewiseLinearFunction<math::V4f> PLF;

	TransferFunction();

	void clear();
	//void                                   setPLF( PLF plf );
	void                                   updateTexture(); // bakes pfl into texture


	TransferFunctionNode::Ptr addNode( float density, const math::V4f& value );
	TransferFunctionNode::Ptr getNode( int index );

	std::vector<TransferFunctionNode::Ptr> m_nodes;

	PLF                                    m_plf;
	int                                    m_numSamples;
	std::vector<math::V4f>                 m_samples;
	base::Texture1d::Ptr                   m_texture;
	float                                  m_st_max;
private:
	bool                                   m_isDirty;

	void makeDirty();
//	bool compareTransferFunctionNodes( TransferFunctionNode::Ptr left, TransferFunctionNode::Ptr right )
//	{
//		return left->m_density < right->m_density;
//	}
	friend struct TransferFunctionNode;
};

struct AnimatedTransferFunction
{
	typedef std::shared_ptr<AnimatedTransferFunction> Ptr;

	AnimatedTransferFunction()
	{
	}

	AnimatedTransferFunction( base::Texture2d::Ptr texture, float timeMin, float timeMax, float densityScale )
		: m_texture(texture),
		  m_timeMin(timeMin),
		  m_timeMax(timeMax),
		  m_densityScale(densityScale)
	{
	}



	float getNormalizedTime( float time )
	{
		return (time-m_timeMin)/(m_timeMax-m_timeMin);
	}

	base::Texture2d::Ptr getTexture()
	{
		return m_texture;
	}

	float getDensityScale()
	{
		return m_densityScale;
	}

private:
	float                                  m_timeMin;
	float                                  m_timeMax;
	float                                  m_densityScale;
	base::Texture2d::Ptr                   m_texture;
};

