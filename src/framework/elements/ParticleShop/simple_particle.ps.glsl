#version 330

in vec2 uv;
in float multiplier;

uniform sampler2D gradientTex;

layout(location = 0) out vec4 frag_color;

void main()
{
	//frag_color = vec4( 1.0f, 1.0f, 1.0f, 1.0 );
	//frag_color = vec4( uv.x, uv.y, 0.0f, 1.0 );
	//frag_color = vec4( texture(gradientTex, uv).xyz, 1.0 );
	frag_color = vec4( texture(gradientTex, vec2(0.5f, uv.y)).xyz*multiplier, 1.0 );
}
