#version 150
#extension GL_EXT_geometry_shader4 : enable

layout(points) in;
layout(triangle_strip, max_vertices = 12) out;
//layout(points, max_vertices = 12) out;
//layout(line_strip, max_vertices = 12) out;

in vec3 v_cd[];
in vec3  v_motionVec[];
out vec3 cd;
out vec2 uv;

uniform mat4        mvpm;
uniform mat4         mvm;
uniform mat4          pm;
uniform float starRadius;

void main()
{
	// setup local coordinate system

	//float r = 0.004f; // size
	//float r = 0.02f; // size
	float r = starRadius; // size
	vec3 m = v_motionVec[0];

	vec3 p = gl_PositionIn[0].xyz;
	vec3 c = v_cd[0];
	vec3 x, y, z;
	z = normalize(p);

	if( length(m) > 0.001 )
	{
		x = normalize(m);
		y = cross(z, x);
	}
	else
	{
		m = vec3(0.0);
		x = vec3(1.0, 0.0, 0.0);
		y = normalize(cross(z, x));
		x = normalize(cross(z, y));
	}
	

	x = x*r;
	y = y*r;
	z = z*r;

	gl_Position = mvpm*(vec4(p-m-x+y,1.0));
	cd = c;
	uv = vec2(-1.0,1.0);
    EmitVertex();

	gl_Position = mvpm*(vec4(p-m-x-y,1.0));
	cd = c;
	uv = vec2(-1.0,-1.0);
    EmitVertex();

	gl_Position = mvpm*(vec4(p-m+y,1.0));
	cd = c;
	uv = vec2(0.0,1.0);
	EmitVertex();


	gl_Position = mvpm*(vec4(p-m-y,1.0));
	cd = c;
	uv = vec2(0.0,-1.0);
	EmitVertex();

	gl_Position = mvpm*(vec4(p+y,1.0));
	cd = c;
	uv = vec2(0.0,1.0);
    EmitVertex();

	gl_Position = mvpm*(vec4(p-y,1.0));
	cd = c;
	uv = vec2(0.0,-1.0);
    EmitVertex();

	gl_Position = mvpm*(vec4(p+x+y,1.0));
	cd = c;
	uv = vec2(1.0,1.0);
    EmitVertex();

	gl_Position = mvpm*(vec4(p+x-y,1.0));
	cd = c;
	uv = vec2(1.0,-1.0);
    EmitVertex();


	EndPrimitive();
}