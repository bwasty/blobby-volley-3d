precision mediump float;

//varying vec3 v_normal;
varying vec3 vColor;

void main() {
	//gl_FragColor = vec4 ( 0.0, 0.0, 1.0, 1.0 );
	gl_FragColor = vec4(vColor, 1.0);
}