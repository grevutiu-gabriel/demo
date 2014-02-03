#version 330

#define LAMBERT
in vec3      pw; // position in worldspace
in vec3      pv; // position in eyespace
in vec3       n; // normal in eyespace
in vec2      uv; // uv coords

uniform vec3       l; // vector into the direction of an infinite distance light in worldspace

uniform mat4   vminv;
uniform mat4     mvm;
uniform mat3 mvminvt;

uniform vec3 ambient;
uniform float     ka;
uniform vec3 diffuse;
uniform float     kd;

#ifdef TEXTURE
uniform sampler2D texture;
#endif

layout(location = 0) out vec4 frag_color;


void main()
{
	vec4 result = vec4(0.0, 0.0, 0.0, 1.0);
	vec3 color = vec3(1.0, 1.0, 1.0);

	#ifdef TEXTURE
	color = texture2D( texture, uv ).xyz;
	result = vec4( color, 1.0 );
	#endif

	#ifdef LAMBERT
	vec3 N = normalize(n);
	vec3 V = normalize(pv.xyz);
	vec3 L = normalize(mvminvt*l);


	// lambert shading ==============
	vec3 a = ka*ambient;
	vec3 d = kd*max(dot(N,L),0.0)*color*diffuse;

	if( dot(N,V) > 0.0 )
		result = vec4(a, 1.0);
	else
	{
		result = vec4(a,1.0) + vec4(d, 1.0);
	}
	//result = vec4(n, 1.0);
	#endif







	frag_color = result;
}
