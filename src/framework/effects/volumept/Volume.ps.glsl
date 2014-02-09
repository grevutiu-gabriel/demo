#version 330
//#extension GL_EXT_texture_array : enable
//#extension GL_EXT_gpu_shader4 : enable

#define M_PI 3.1415926535897932384626433832795

// TODO:
//-investigate banding we get for many rng's
//


in vec2 uv;


layout(location = 0) out vec4 frag_color;


// primary rays
uniform sampler2D volumeFront;
uniform sampler2D volumeBack;

// volume parameters
uniform sampler3D density;
uniform sampler1D transferFunction;
uniform mat4 localToWorld;
uniform mat4 worldToLocal;
uniform float st_max;
float st_val=2.0f;
float albedo = 1.0f;




float wsStepSize = 0.1;
vec3 totalCrossSection=vec3(1.0f);
vec3 scatteringCrossSection=vec3(0.5f);
/*
uniform vec3 wsLightPos;
uniform vec3 lightColor;
uniform mat4 worldToLightProj;
// temp (uniforms)
vec3 emissionCrossSection = vec3( 0.0, 0.0, 0.0 );
*/
//#define PI 3.141593








//
// random number generator =================================
// see GPU gems 3 for details
//
/*
unsigned int z1, z2, z3, z4;

void initRNG()
{
	z1 = 128U + uint(gl_FragCoord.x);
	z2 = 128U + uint(gl_FragCoord.y);
	z3 = 128U + uint(gl_FragCoord.x + gl_FragCoord.y);
	z4 = 128U + uint(gl_FragCoord.x * gl_FragCoord.y);
}

// S1, S2, S3, and M are all constants, and z is part of the
// private per-thread generator state.
unsigned int TausStep(inout unsigned int z, int S1, int S2, int S3,
					  unsigned int M)
{
	unsigned int b=(((z << uint(S1)) ^ z) >> uint(S2));
	return z = (((z & M) << uint(S3)) ^ b);
}

// A and C are constants
unsigned int LCGStep(inout unsigned int z, unsigned int A, unsigned int C)
{
	return z=(A*z+C);
}

//float HybridTaus()
float randomFloat()
{
	// Combined period is lcm(p1,p2,p3,p4)~ 2^121
	return 2.3283064365387e-10
	* float(         // Periods
			TausStep(z1, 13, 19, 12, 4294967294) ^  // p1=2^31-1
			TausStep(z2, 2, 25, 4, 4294967288) ^    // p2=2^30-1
			TausStep(z3, 3, 11, 17, 4294967280) ^   // p3=2^28-1
			LCGStep(z4, 1664525U, 1013904223U)        // p4=2^32
					 );
}

uint simple_lcg(inout uint seed)
{
	uint new_seed = seed*uint(214013) + uint(2531011);
	seed = new_seed;
	return uint((new_seed >> 16) & uint(0x7fff));
}

float uniform_01(inout uint seed)
{
	float r = float(simple_lcg(seed));
	return r/32767.0f;
}

uint rng_state;

uint rand_lcg()
{
	// LCG values from Numerical Recipes
	rng_state = uint(1664525) * rng_state + uint(1013904223);
	return rng_state;
}


uint rand_xorshift()
{
	// Xorshift algorithm from George Marsaglia's paper
	rng_state ^= (rng_state << 13);
	rng_state ^= (rng_state >> 17);
	rng_state ^= (rng_state << 5);
	return rng_state;
}
*/



//http://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl
vec3 rng_state;

void initRNG()
{
	rng_state = vec3(gl_FragCoord.xy, 123.5f);
}

// A single iteration of Bob Jenkins' One-At-A-Time hashing algorithm.
uint hash( uint x ) {
	x += ( x << 10u );
	x ^= ( x >>  6u );
	x += ( x <<  3u );
	x ^= ( x >> 11u );
	x += ( x << 15u );
	return x;
}

// Compound versions of the hashing algorithm I whipped together.
uint hash( uvec2 v ) { return hash( v.x ^ hash(v.y)                         ); }
uint hash( uvec3 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z)             ); }
uint hash( uvec4 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z) ^ hash(v.w) ); }

// Construct a float with half-open range [0:1] using low 23 bits.
// All zeroes yields 0.0, all ones yields the next smallest representable value below 1.0.
float floatConstruct( uint m )
{
	const uint ieeeMantissa = 0x007FFFFFu; // binary32 mantissa bitmask
	const uint ieeeOne      = 0x3F800000u; // 1.0 in IEEE binary32

	m &= ieeeMantissa;                     // Keep only mantissa bits (fractional part)
	m |= ieeeOne;                          // Add fractional part to 1.0

	float  f = uintBitsToFloat( m );       // Range [1:2]
	return f - 1.0;                        // Range [0:1]
}

// Pseudo-random value in half-open range [0:1].
float random( float x ) { return floatConstruct(hash(floatBitsToUint(x))); }
float random( vec2  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec3  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec4  v ) { return floatConstruct(hash(floatBitsToUint(v))); }

float randomFloat()
{
	rng_state.z+=1.0;
	return random(rng_state);
}

// ------------------------

// uniformly sample sphere
vec3 sampleSphere()
{
	vec3 wi;
	float u1 = randomFloat();
	float u2 = randomFloat();
	wi.z = 1.0f - 2.0f * u1;
	float r = sqrt(max(0.0f, 1.0f - wi.z*wi.z));
	float phi = 2.0f * M_PI * u2;
	wi.x = r * cos(phi);
	wi.y = r * sin(phi);

	//*pdf = 1.0f / (4.0f * M_PI);
	return wi;
}


//-----------------------------------------------------------------------------
struct Ray
{
	vec3 o;
	vec3 d;
};



// Intersects ray with box ( special case: ray origin is in a box )
float intersectBox( in Ray ray /* ray origin and direction */,
					in vec3 minimum /* minimum point of a box */,
					in vec3 maximum /* maximum point of a box */ )
{
	vec3 OMIN = ( minimum - ray.o ) / (ray.d+10.0e-4); // offset to avoid division by 0
	vec3 OMAX = ( maximum - ray.o ) / (ray.d+10.0e-4);
	vec3 MAX = max ( OMAX, OMIN );
	return min ( MAX.x, min ( MAX.y, MAX.z ) );
}

float mapValueToRange( float sourceRangeMin, float sourceRangeMax, float targetRangeMin, float targetRangeMax, float value )
{
	return (value-sourceRangeMin) / (sourceRangeMax - sourceRangeMin) * (targetRangeMax - targetRangeMin) + targetRangeMin;
}

// returns true(0.0f) if scattering event happended, false(1.0f) if not
// d = (initial) distance travelled
// st = sigma_t (extinction) at scattering event
float sampleDistance( in Ray ray, in float tmax, inout float d, out float st )
{
	while( true )
	{
		float dl = -log(randomFloat())/st_max;
		d += dl;
		if( d > tmax )
			return 1.0f;
		//cumath::Vec3<T> vsP = cumath::transform( ray.getPosition(l), kt_field->m_worldToVoxel );
		vec3 localP = (worldToLocal*vec4(ray.o+ray.d*d, 1)).xyz;
		st = texture(density,localP).r;

		// TODO: transfer function
		st = texture(transferFunction, st).a;

		//st = mapValueToRange( -1000.0f, 3095.0f, 0.0f, 1.0f, st );
		if( randomFloat() < st/st_max )
		{
			return 0.0f;
		}
	}
	return 1.0f;
}

// returns true(0.0f) if scattering event happended, false(1.0f) if not
// d = (initial) distance travelled
// st = sigma_t (extinction) at scattering event
// optimization: factor *stepsize out of the loop
float sampleDistance2( in Ray ray, in float stepsize, in float tmax, inout float d, out vec4 volumeSample )
{
	float odmax = -log(randomFloat())/st_max;
	//float odmax = -log(randomFloat());
	float od = 0.0f;
	while( od < odmax )
	{
		if( d > tmax )
			return 1.0f;
		vec3 localP = (worldToLocal*vec4(ray.o+ray.d*d, 1)).xyz;
		volumeSample = texture(transferFunction, texture(density,localP).r);
		od += volumeSample.a*stepsize;
		d += stepsize;
	}
	return 0.0f;
}


void main()
{
	initRNG();

	vec4 front = texture2D( volumeFront, uv );
	vec4 back = texture2D( volumeBack, uv );

	// fetch noise value for current pixel
	vec2 viewportSize = vec2( 800, 600 );
	float frontDepth = front.w;
	float backDepth = back.w;

	float totalDistance = backDepth-frontDepth;


	if( totalDistance < 0.01f )
		discard;

	//
	vec3 vsStart = front.xyz;
	vec3 vsEnd = back.xyz;

	vec3 vsCurrent = vsStart;
	vec3 vsRayDir = normalize(vsEnd - vsStart);
	float vsTotalDistance = length(vsEnd - vsStart);

	vec4 wsStart = localToWorld*vec4(vsStart, 1);
	vec4 wsEnd = localToWorld*vec4(vsEnd, 1);
	vec4 wsCurrent = wsStart;
	vec3 wsRayDir = normalize( wsEnd.xyz - wsStart.xyz );


	float wsTotalDistance = length( wsEnd - wsStart );


	vec3 lightDir = vec3(-1.0f, 0.0f, 0.0f);
	vec3 lightIntensity = vec3(1.0f, 1.0f, 1.0f)*100.0f;

	//vec3 boxmin = localtoWorld*vec3(0.0f);
	//vec3 boxmax = localtoWorld*vec3(1.0f);
	vec3 boxmin = (localToWorld*vec4(0.0f, 0.0f, 0.0f, 1.0f)).xyz;
	vec3 boxmax = (localToWorld*vec4(1.0f)).xyz;


	int numSamples = 10;
	vec4 sum = vec4(0.0f);

	/*
	// path tracing ---
	for(int i=0;i<numSamples;++i)
	{
		Ray ray;
		ray.o = wsStart.xyz + wsRayDir*0.0;
		ray.d = wsRayDir;
		float tmax = wsTotalDistance;

		float d=0.0f;
		float st;
		if( sampleDistance(ray, tmax, d, st) == 0.0f )
		{
//			// direct light ---
//			ray.o = ray.o + ray.d*d;
//			ray.d = lightDir;
//			// TODO: transform ray into local space and tmax backwards into worldspace
//			tmax = intersectBox(ray, boxmin, boxmax);
//			d = 0.0f;
//			float st2;
//			sum.rgb += lightIntensity*sampleDistance(ray, tmax, d, st2)*(1.0f/(4.0f*M_PI))*albedo; // /pdf==st*T
		}else
		{
			// no scattering event (add transmittance sample)
			sum.a += 1.0f;
		}

	}

	sum /= numSamples;
	*/

	/*
	// raymarching test ---
	numSamples = 100;
	int numSamples2 = 100;
	Ray ray;
	ray.o = wsStart.xyz + wsRayDir*0.0f;
	ray.d = wsRayDir;
	float stepsize = wsTotalDistance/float(numSamples);
	float Tray = 1.0f;
	for(int i=0;i<numSamples;++i)
	{
		vec3 wsP = ray.o+ray.d*(float(i)*stepsize);
		vec3 localP = (worldToLocal*vec4(wsP, 1)).xyz;
		vec4 st = texture(transferFunction, texture(density,localP).r);
		float voldensity = st.a*stepsize;

		// cast secondary rays towards light
		Ray lightray;
		lightray.o = wsP;
		lightray.d = lightDir;
		// TODO: transform ray into local space and tmax backwards into worldspace
		float tmax = intersectBox(lightray, boxmin, boxmax);
		float stepsize2 = tmax/float(numSamples2);

		float od2 = 0.0f;
		for(int j=0;j<numSamples2;++j)
		{
			vec3 wsP2 = lightray.o+lightray.d*(float(j)*stepsize2);
			vec3 localP2 = (worldToLocal*vec4(wsP2, 1)).xyz;
			vec4 st2 = texture(transferFunction, texture(density,localP2).r);
			od2 += st2.a;
		}
		float Tlight=exp(-od2*stepsize2);

		sum.rgb += lightIntensity*Tlight*(1.0f/(4.0f*M_PI))*voldensity*st.rgb*Tray;

		Tray *= exp(-st.a*stepsize);
	}
	sum.a = Tray;
	*/

	///*
	// stochastic raymarching test ---
	float stepsize = 0.1f;
	float stepsize2 = 0.1f;
	numSamples = 1;
	for(int i=0;i<numSamples;++i)
	{
		Ray ray;
		ray.o = wsStart.xyz + wsRayDir*0.0;
		ray.d = wsRayDir;
		float tmax = wsTotalDistance;

		float d=randomFloat()*stepsize;
		vec4 volumeSample;
		if( sampleDistance2(ray, stepsize, tmax, d, volumeSample) == 0.0f )
		{
			// direct light ---
			ray.o = ray.o + ray.d*d;
			ray.d = lightDir;
			// TODO: transform ray into local space and tmax backwards into worldspace
			tmax = intersectBox(ray, boxmin, boxmax);
			d = randomFloat()*stepsize2;
			vec4 volumeSample2;
			sum.rgb += lightIntensity*sampleDistance2(ray, stepsize2, tmax, d, volumeSample2)*(1.0f/(4.0f*M_PI))*volumeSample.rgb; // /pdf==st*T
		}else
		{
			// no scattering event (add transmittance sample)
			sum.a += 1.0f;
		}

	}
	sum /= numSamples;
	//*/


	frag_color = vec4(sum.rgb, 1.0f-sum.a);

	//float alpha = 1.0f-sum.a;
	//vec3 backlight = vec3(1.0f);
	//frag_color = vec4(sum.rgb*alpha+backlight*(1.0f-alpha), 1.0f);
	//frag_color = vec4(sum.rgb, 1.0f);
	//frag_color = vec4(1.0f, 2.0f, 3.0f, 1.0f);
	//frag_color = vec4(front.rgb, 1.0f);

}
