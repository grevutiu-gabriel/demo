#version 330
in vec3     P;
in vec3     N;
in vec2    UV;

out vec3      pv; // position in eyespace
out vec3      pw; // position in  worldspace
out vec3       n; // normal in eyespace
out vec2      uv; // uv coords

uniform mat4    mvpm;
uniform mat4     mvm;
uniform mat3 mvminvt;

void main()
{
	uv = UV;
	pw = P; // position in  worldspace
	pv = (mvm * vec4(P,1.0)).xyz; // position in eyespace
	n = mvminvt * N; // normal in eyespace
	gl_Position = mvpm * vec4(P, 1.0);
}
