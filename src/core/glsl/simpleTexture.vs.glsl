#version 330
in vec3 P;
in vec2 UV;

out vec2 uv;

uniform mat4 mvpm;

void main()
{
	uv = UV;
	gl_Position = mvpm * vec4(P,1.0);
}
