#version 330
in vec3 P;
in vec2 UV;

out vec2 uv;

void main()
{
	gl_Position = vec4(P,1.0);
	uv = UV;
}
