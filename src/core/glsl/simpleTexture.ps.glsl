#version 330
in vec2 uv;
uniform sampler2D texture;

layout(location = 0) out vec4 frag_color;

void main()
{
	frag_color = texture2D(texture,uv);
}
