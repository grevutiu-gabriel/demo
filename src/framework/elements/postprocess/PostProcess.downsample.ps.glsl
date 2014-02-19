varying vec2 uv;

uniform sampler2D  input;
uniform vec2 invInputRes;

#ifdef DO_BRIGHTNESS_PASS
uniform float threshold;
#endif

void main()
{
	vec2 T = gl_FragCoord.xy;

	//get neighboring pixels
	vec4 N  = texture2D(input,vec2(T.x,T.y+1.0)*invInputRes);
	vec4 NE = texture2D(input,vec2(T.x+1.0,T.y+1.0)*invInputRes);
	vec4 E  = texture2D(input,vec2(T.x+1.0,T.y)*invInputRes); 
	vec4 C  = texture2D(input,vec2(T.x,T.y)*invInputRes);

#ifdef DO_BRIGHTNESS_PASS
	gl_FragColor = (step( threshold, (N.x+N.y+N.z)/3.0 )*N+
					step( threshold, (NE.x+NE.y+NE.z)/3.0 )*NE+
					step( threshold, (E.x+E.y+E.z)/3.0 )*E+
					step( threshold, (C.x+C.y+C.z)/3.0 )*C)*0.25;
#else
	gl_FragColor = (N+NE+E+C)*0.25;
#endif
}
