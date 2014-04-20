uniform samplerCube domeTex;
uniform float domeBrightnessScale;
varying vec3 n;

void main()
{
	gl_FragData[0] = textureCube( domeTex, n )*domeBrightnessScale;
}
