#version 330
#define M_PI 3.14159265

in vec3 P;
in vec2 UV;


out vec2 uv;
out float multiplier;

uniform mat4 mvpm;
uniform vec3 lpos;
uniform vec2 position;
uniform float scale;
uniform float brightness;
uniform float perpectiveCorrectionFactor;
uniform sampler1D fadeUTex;


void main()
{
	vec3 ref = vec3(1.0f, 0.0f, 0.0f);
	vec3 offset = vec3( lpos.xy*position, 0.0f );
	offset.y = -offset.y;
	// distance of lightpos on screen from screen center
	float distance = length(lpos.xy);

	// auto rotate ---
	vec3 lightDirScreen = vec3(normalize(lpos.xy), 0.0f);

	// have direction vector point away from the light
	lightDirScreen = -lightDirScreen;

	// compute rotation matrix
	vec3 c = cross(lightDirScreen, vec3(0.0, 0.0, -1.0f));
	mat3 rotation = mat3( lightDirScreen,
						  c,
						  vec3(0.0, 0.0, 1.0f));

	// scale is influenced by distance and user scale value
	float final_scale = max(dot(normalize(P), ref), 0.0f)*perpectiveCorrectionFactor*distance*scale;
	//float final_scale = 1.0f;

	// apply scaling and...
	vec3 p = P*final_scale;
	// ...rotation
	p = p*rotation;

	// now move the thing to position on screen
	p += offset;

	// done
	gl_Position = vec4(p,1.0);
	//uv = vec2(1.0, 0.5);
	uv = UV;
	multiplier = brightness*texture(fadeUTex, uv.x);
}
