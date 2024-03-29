// from: http://callumhay.blogspot.de/2010/09/gaussian-blur-shader-glsl.html
varying vec2 uv;

uniform float sigma;     // The sigma value for the gaussian function: higher value means more blur
                         // A good value for 9x9 is around 3 to 5
                         // A good value for 7x7 is around 2.5 to 4
                         // A good value for 5x5 is around 2 to 3.5
                         // ... play around with this based on what you need :)

uniform float blurSize;  // This should usually be equal to
                         // 1.0f / texture_pixel_width for a horizontal blur, and
                         // 1.0f / texture_pixel_height for a vertical blur.

uniform sampler2D input;  // Texture that will be blurred by this shader

const float pi = 3.14159265;

// The following are all mutually exclusive macros for various 
// seperable blurs of varying kernel size
#if defined(VERTICAL_BLUR_9)
const int numBlurPixelsPerSide = 4;
const vec2  blurMultiplyVec      = vec2(0.0, 1.0);
#elif defined(HORIZONTAL_BLUR_9)
const int numBlurPixelsPerSide = 4;
const vec2  blurMultiplyVec      = vec2(1.0, 0.0);
#elif defined(VERTICAL_BLUR_7)
const int numBlurPixelsPerSide = 3;
const vec2  blurMultiplyVec      = vec2(0.0, 1.0);
#elif defined(HORIZONTAL_BLUR_7)
const int numBlurPixelsPerSide = 3;
const vec2  blurMultiplyVec      = vec2(1.0, 0.0);
#elif defined(VERTICAL_BLUR_5)
const int numBlurPixelsPerSide = 2;
const vec2  blurMultiplyVec      = vec2(0.0, 1.0);
#elif defined(HORIZONTAL_BLUR_5)
const int numBlurPixelsPerSide = 2;
const vec2  blurMultiplyVec      = vec2(1.0, 0.0);
#else
// This only exists to get this shader to compile when no macros are defined
const int numBlurPixelsPerSide = 0;
const vec2  blurMultiplyVec      = vec2(0.0, 0.0);
#endif

void main()
{

	// Incremental Gaussian Coefficent Calculation (See GPU Gems 3 pp. 877 - 889)
	vec3 incrementalGaussian;
	incrementalGaussian.x = 1.0 / (sqrt(2.0 * pi) * sigma);
	incrementalGaussian.y = exp(-0.5 / (sigma * sigma));
	incrementalGaussian.z = incrementalGaussian.y * incrementalGaussian.y;

	vec4 avgValue = vec4(0.0, 0.0, 0.0, 0.0);
	float coefficientSum = 0.0;

	// Take the central sample first...
	avgValue = texture2D(input, uv)*incrementalGaussian.x;
	coefficientSum += incrementalGaussian.x;
	incrementalGaussian.xy *= incrementalGaussian.yz;

	// Go through the remaining neighbour samples (numBlurPixelsPerSide on each side of the center)
	for (int i = 1; i <= numBlurPixelsPerSide; ++i)
	{
		float weight = incrementalGaussian.x;
		avgValue += texture2D(input, uv - float(i) * blurSize * blurMultiplyVec)*weight;
		avgValue += texture2D(input, uv + float(i) * blurSize * blurMultiplyVec)*weight;
		coefficientSum += 2.0*weight;
		incrementalGaussian.xy *= incrementalGaussian.yz;


//		avgValue += texture2D(input, uv - float(i) * blurSize * blurMultiplyVec)*incrementalGaussian.x;
//		avgValue += texture2D(input, uv + float(i) * blurSize * blurMultiplyVec)*incrementalGaussian.x;
//		coefficientSum += 2.0*incrementalGaussian.x;
//		incrementalGaussian.xy *= incrementalGaussian.yz;
	}


	gl_FragColor = avgValue/ coefficientSum;
}
