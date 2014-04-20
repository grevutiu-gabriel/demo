attribute vec3 P;
attribute vec3 Cd;
attribute vec3 test;

varying vec3         v_cd;
varying vec3  v_motionVec;


// used to compute motion vector
uniform mat4    mvm;
uniform mat4  mvm0R; // this matrix describes the rotation which transforms mvm*p to mvm0*p (mvm0 being the mvm transform from previous frame)


void main()
{
	v_cd = Cd;
	
	v_motionVec = (vec4(P,1.0)-mvm0R*vec4(P,1.0)).xyz;
	gl_Position =  vec4(P,1.0);


}
