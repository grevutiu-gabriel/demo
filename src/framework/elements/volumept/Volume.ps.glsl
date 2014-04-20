#version 330



// TODO:
//-investigate banding we get for many rng's
//


in vec2 uv;
layout(location = 0) out vec4 frag_color;


// primary rays
uniform sampler2D volumeFront;
uniform sampler2D volumeBack;

// volume parameters
uniform sampler3D normalizedDensity;
uniform sampler1D transferFunction;
uniform sampler2D transferFunction2;
uniform mat4      localToWorld;
uniform mat4      worldToLocal;
uniform vec3      aabb_min; // worldspace
uniform vec3      aabb_max;
uniform float     sigma_t_scale;
uniform float     shotLocalTime;


// ------------------------ MATH --------------------------------
#define M_PI 3.1415926535897932384626433832795

float mapValueToRange( float sourceRangeMin, float sourceRangeMax, float targetRangeMin, float targetRangeMax, float value )
{
	return (value-sourceRangeMin) / (sourceRangeMax - sourceRangeMin) * (targetRangeMax - targetRangeMin) + targetRangeMin;
}


// ------------------------ RANDOM NUMBER GENERATOR ------------------------------


//http://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl
vec3 gRNGState;

void initRNG()
{
	gRNGState = vec3(gl_FragCoord.xy, 123.5f);
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
	gRNGState.z+=1.0;
	return random(gRNGState);
}


// ------------------------ SAMLING FUNCTIONS ------------------------------


// uniformly sample sphere
vec3 sampleSphere( out float pdf )
{
	vec3 wi;
	float u1 = randomFloat();
	float u2 = randomFloat();
	wi.z = 1.0f - 2.0f * u1;
	float r = sqrt(max(0.0f, 1.0f - wi.z*wi.z));
	float phi = 2.0f * M_PI * u2;
	wi.x = r * cos(phi);
	wi.y = r * sin(phi);

	pdf = 1.0f / (4.0f * M_PI);
	return wi;
}

// uniformly sample hemisphere
vec3 sampleHemisphere( out float pdf )
{
	vec3 wi;
	float u1 = randomFloat();
	float u2 = randomFloat();
	wi.z = 1.0f - 2.0f * u1;
	float r = sqrt(max(0.0f, 1.0f - wi.z*wi.z));
	float phi = 2.0f * M_PI * u2;
	wi.x = r * cos(phi);
	wi.y = abs(r * sin(phi));
	pdf = 1.0f / (2.0f * M_PI);
	return wi;
}

vec4 sampleVolume( in vec3 localP )
{
	return texture(transferFunction, texture(normalizedDensity,localP).r);
	//return texture(transferFunction2, vec2(texture(normalizedDensity,localP).r, shotLocalTime));
}

//----------------------------- RAYTRACING -------------------------------------
struct Ray
{
	vec3 o;
	vec3 d;
	float tmax;
};

struct ScatterEvent
{
	vec3  p;
	vec3  albedo;
	float sigma_t;
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




// returns true if scattering event happended, false if not
// optimization: factor *stepsize out of the loop
bool sampleDistance( in Ray ray, in float stepsize, out ScatterEvent se )
{
	float odmax = -log(randomFloat())/sigma_t_scale;
	//float odmax = -log(randomFloat());
	float od = 0.0f;
	float d = randomFloat()*stepsize;
	while( od < odmax )
	{
		if( d > ray.tmax )
			return false;
		se.p = ray.o+ray.d*d;
		vec3 localP = (worldToLocal*vec4(se.p, 1)).xyz;
		vec4 volumeSample = sampleVolume(localP);
		se.albedo = volumeSample.rgb;
		se.sigma_t = volumeSample.a*sigma_t_scale;
		od += se.sigma_t*stepsize;
		d += stepsize;
	}
	return true;
}

// returns true if scattering event happended, false if not
// optimization: factor *stepsize out of the loop
bool sampleDistance( in Ray ray, in float stepsize)
{
	float odmax = -log(randomFloat())/sigma_t_scale;
	//float odmax = -log(randomFloat());
	float od = 0.0f;
	float d = randomFloat()*stepsize;
	while( od < odmax )
	{
		if( d > ray.tmax )
			return false;
		vec3 localP = (worldToLocal*vec4(ray.o+ray.d*d, 1)).xyz;
		float sigma_t = sampleVolume(localP).a*sigma_t_scale;
		od += sigma_t*stepsize;
		d += stepsize;
	}
	return true;
}


// ---------------------------- LIGHT SAMPLING -------------------------------

// directional light -------------

vec3 lightDir = vec3(-1.0f, 0.0f, 0.0f);
vec4 lightIntensityDirectional = vec4(1.0f, 1.0f, 1.0f, 100.0f);

void sampleLightDirectional( inout Ray ray, out float pdf, out vec3 Li )
{
	ray.d = lightDir;
	ray.tmax = intersectBox(ray, aabb_min, aabb_max);
	Li = lightIntensityDirectional.rgb*lightIntensityDirectional.a;
	pdf = 1.0f;
}


// point light ------------

uniform vec3 pointLightPos;
vec3 pointLightColor = vec3(1.0f, 0.32f, 0.1f);
uniform float pointLightIntensity;

void samplePointLight( inout Ray ray, out float pdf, out vec3 Li )
{
	ray.d = pointLightPos-ray.o;
	ray.tmax = length(ray.d);
	ray.d = normalize(ray.d);
	Li = (pointLightColor*pointLightIntensity)/(4.0f*M_PI*ray.tmax*ray.tmax);
	pdf = 1.0f;
}


// area light -------------

float areaLightWidth=1.0f;
float areaLightHeight=1.0f;

vec3 areaLightIntensity = vec3(1.0f, 1.0f, 1.0f)*500.0f;
uniform mat4      areaLightTransform;

void sampleAreaLight( inout Ray ray, out float pdf, out vec3 Li )
{
	float area = areaLightWidth*areaLightHeight;
	vec3 lightPos = areaLightTransform[0].xyz*(randomFloat()-0.5f)*areaLightWidth + areaLightTransform[1].xyz*(randomFloat()-0.5f)*areaLightHeight + vec3(areaLightTransform[3].x, areaLightTransform[3].y, areaLightTransform[3].z);
	ray.d = lightPos-ray.o;
	ray.tmax = length(ray.d);
	ray.d = normalize(ray.d);
	Li = areaLightIntensity/(4.0f*M_PI*ray.tmax*ray.tmax*area);
	pdf = 1.0f/area;
}


// environment light for upper hemisphere -----------
vec4 lightIntensityEnvironmentUpperHemisphere = vec4(1.0f, 1.0f, 1.0f, 5.0f);

void sampleLightEnvironmentUpperHemisphere( inout Ray ray, out float pdf, out vec3 Li )
{
	ray.d = sampleHemisphere( pdf );
	ray.tmax = intersectBox(ray, aabb_min, aabb_max);
	Li = lightIntensityEnvironmentUpperHemisphere.rgb*lightIntensityEnvironmentUpperHemisphere.a;
}

// environment light for full sphere -----------
//vec3 lightEnvironmentGradientTop = vec3(0.113f, 0.392f, 1.0f);
//vec3 lightEnvironmentGradientMiddle = vec3(0.3137f, 0.207f, 0.1568f);
//vec3 lightEnvironmentGradientBottom = vec3(0.666f, 0.333f, 0.0f);
vec3 lightEnvironmentGradientTop = vec3(30.0f/255.0f, 38.0f/255.0f, 102.0f/255.0f);
vec3 lightEnvironmentGradientMiddle = vec3(59.0f/255.0f, 31.0f/255.0f, 17.0f/255.0f);
vec3 lightEnvironmentGradientBottom = vec3(3.0f/255.0f, 80.0f/255.0f, 25.0f/255.0f);

//float lightEnvironmentIntensity = .35f;
float lightEnvironmentIntensity = 5.0f;

void sampleLightEnvironmentGradient( inout Ray ray, out float pdf, out vec3 Li )
{
	ray.d = sampleSphere( pdf );
	ray.tmax = intersectBox(ray, aabb_min, aabb_max);
	if( ray.d.y > 0.0f )
		Li = mix( lightEnvironmentGradientMiddle, lightEnvironmentGradientTop, ray.d.y )*lightEnvironmentIntensity;
	else
		Li = mix( lightEnvironmentGradientMiddle, lightEnvironmentGradientBottom, abs(ray.d.y) )*lightEnvironmentIntensity;
}

// combined light ------------
void sampleLightCombined( inout Ray ray, out float pdf, out vec3 Li )
{
	/*
	// directional +upperhemisphereenv
	float ratio = lightIntensityDirectional.a/(lightIntensityDirectional.a+(lightIntensityEnvironmentUpperHemisphere.a*2.0f*M_PI));
	float lightSample = randomFloat();
	if( lightSample < ratio )
		sampleLightDirectional( ray, pdf, Li );
	else
		sampleLightEnvironmentUpperHemisphere( ray, pdf, Li );
	pdf *= ratio;
	*/
	// point light + env
	float ratio = pointLightIntensity/(pointLightIntensity+(lightEnvironmentIntensity*4.0f*M_PI));
	float lightSample = randomFloat();
	if( lightSample < ratio )
		samplePointLight( ray, pdf, Li );
	else
		sampleLightEnvironmentGradient( ray, pdf, Li );
	pdf *= ratio;
}


// gg

//#define sampleLight sampleLightEnvironmentUpperHemisphere
//#define sampleLight sampleLightEnvironmentGradient
//#define sampleLight sampleLightDirectional
//#define sampleLight sampleLightCombined
//#define sampleLight sampleLightEnvironmentGradient
#define sampleLight samplePointLight
//#define sampleLight sampleAreaLight

void main()
{
	initRNG();

	vec4 front = texture2D( volumeFront, uv );
	vec4 back = texture2D( volumeBack, uv );

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
		vec4 st = texture(transferFunction, texture(normalizedDensity,localP).r);
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
			vec4 st2 = texture(transferFunction, texture(normalizedDensity,localP2).r);
			od2 += st2.a;
		}
		float Tlight=exp(-od2*stepsize2);

		sum.rgb += lightIntensity*Tlight*(1.0f/(4.0f*M_PI))*voldensity*st.rgb*Tray;

		Tray *= exp(-st.a*stepsize);
	}
	sum.a = Tray;
	*/

	// stochastic raymarching ER style ---
//	float stepsize = 0.0117188f;
//	float stepsize2 = 0.0117188f;
	float stepsize = 1.17188f;
	float stepsize2 = 1.17188f;
	int numSamples = 5;
	vec4 sum = vec4(0.0f);
	for(int i=0;i<numSamples;++i)
	{
		Ray ray;
		ray.o = wsStart.xyz + wsRayDir*0.0;
		ray.d = wsRayDir;
		ray.tmax = wsTotalDistance;

		float d=randomFloat()*stepsize;
		ScatterEvent se;
		if( sampleDistance(ray, stepsize, se) )
		{
			// estimate direct light ----
			Ray rayl;
			rayl.o = se.p;
			float pdf;
			vec3 Li;
			sampleLight( rayl, pdf, Li );

			if(!sampleDistance(rayl, stepsize2))
			{
				// TODO: fix to use correct phase function
				//sum.rgb += (Li*se.albedo/M_PI)/pdf; // ER st
				//sum.rgb += Li;
				//sum.rgb += se.albedo*10.0f;
				sum.rgb += (Li*se.albedo/M_PI)/pdf;
			}
		}else
		{
			// no scattering event (add transmittance sample)
			sum.a += 1.0f;
		}

	}
	sum /= numSamples;



	frag_color = vec4(sum.rgb, 1.0f-sum.a);

	frag_color = vec4(clamp(frag_color.rgb, 0.0f, 1.0f), frag_color.a);


	//frag_color = vec4(100.0f, 100.0f, 100.0f, 1.0f);
}
