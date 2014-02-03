#version 330
in vec3 cd;

layout(location = 0) out vec4 frag_color;

void main()
{
	frag_color = vec4( cd, 1.0 );
}
