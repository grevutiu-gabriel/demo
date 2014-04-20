
attribute vec3 P;
attribute vec3 N;

varying vec3 n;




uniform mat4 mvpm;

void main()
{
	n = N;
	gl_Position = mvpm * vec4(P,1.0);
}
