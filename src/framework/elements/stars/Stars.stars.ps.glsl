varying vec3 cd;
varying vec2 uv;

uniform float starBrightnessScale;
uniform float starLuminanceScale;

uniform sampler1D aaTex;

void main()
{
	float finalBrightness = texture1D( aaTex, length(uv) ).x*starBrightnessScale;

	gl_FragColor = vec4(cd*starLuminanceScale*finalBrightness,1.0);
}
