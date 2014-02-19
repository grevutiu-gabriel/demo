varying vec4 pw;  // position in worldspace
varying vec2 uv;

uniform sampler2D input;


// GLARE =================================
#ifdef DO_GLARE
uniform sampler2D inputBlurred[ GLARE_NUM_BLUR_ITERATIONS ];
uniform float glareAmount;
#endif


// HDR ===========================
uniform float exposure;
#ifdef DO_HDR_FILMICTONEMAPPING
// filmic tonemapping parameters
uniform float A; // shoulder strength
uniform float B; // linear strength
uniform float C; // linear angle
uniform float D; // toe strength
uniform float E; // toe numerator
uniform float F; // toe denominator
// note: E/F = toe angle
uniform float linearWhitePoint; //Linear White Point Value

vec3 filmic( vec3 x )
{
	return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F)) - E/F;
}

#endif




// COLORGRADING ==========================
#ifdef DO_COLORGRADING
uniform vec3 gradeShadows;
uniform vec3 gradeMidtones;
uniform vec3 gradeHighlights;
uniform float gradeSaturation;
#endif


// VIGNETTE ===========================
#ifdef DO_VIGNETTE
uniform float vignetteStrength;
uniform float vignetteScale;
uniform float vignetteSoftness;
#endif

float linearToSRGB( float c_linear )
{
	if( c_linear <= 0.0031308 )
		return 12.92*c_linear;
	else
	{
		const float a = 0.055;
		return (1.0+a)*pow( c_linear, 1.0/2.4 ) - a;
	}
}


void main()
{

	vec4 linearColor = texture2D(input,uv);


	// GLARE =================================
	#ifdef DO_GLARE
		vec3 sumBlurred = GLARE_SUM_BLURRED;
		linearColor += vec4(sumBlurred, 1.0)*glareAmount;
	#endif



	// TONEMAPPING =================================
	vec3 gammaColor = linearColor.xyz;

	#ifdef DO_HDR_FILMICTONEMAPPING
		linearColor *= exposure;
		gammaColor = filmic(linearColor.rgb)/filmic(vec3(linearWhitePoint));
	#endif
	#ifdef DO_HDR_LINEAR_TO_SRGB
		linearColor *= exposure;
		gammaColor = vec3(linearToSRGB(linearColor.x), linearToSRGB(linearColor.y), linearToSRGB(linearColor.z));
	#endif


	// COLORGRADING ==================================
	#ifdef DO_COLORGRADING
	vec3 colorGradingInput = gammaColor;

	// saturation (http://www.francois-tarlier.com/blog/saturation-shader/)
	float saturation = gradeSaturation;   
       
	float grayscale = (colorGradingInput.r + colorGradingInput.g + colorGradingInput.g + colorGradingInput.b)/4;
	vec3 averageColor = vec3(grayscale,grayscale,grayscale);
       
	colorGradingInput = ((colorGradingInput - averageColor )*saturation)+averageColor;


	//cdl
	vec3 offset = gradeShadows*2.0 - 1.0;
	vec3 slope = vec3(1.0) - offset + gradeHighlights*2.0 - 1.0;
	vec3 power = (vec3(1.0) - gradeMidtones)*2.0;
	gammaColor = vec4(pow(colorGradingInput*slope + offset, power),1.0);


	#endif

	// VIGNETTE =========================================
	#ifdef DO_VIGNETTE
	float d = abs(length(uv-vec2(0.5)));
	gammaColor = mix( gammaColor, vec3(0.0), vec3(vignetteStrength*smoothstep( vignetteScale-vignetteSoftness, vignetteScale, d )));
	#endif

	gl_FragColor = vec4(gammaColor, linearColor.a);
}
