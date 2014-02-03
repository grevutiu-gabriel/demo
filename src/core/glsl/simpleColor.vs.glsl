#version 330
in vec3 P;
in vec3 Cd;

out vec2 uv;
out vec3 cd;

uniform mat4 mvpm;

void main()
{
	cd = Cd;
	gl_Position = mvpm * vec4(P,1.0);
}
