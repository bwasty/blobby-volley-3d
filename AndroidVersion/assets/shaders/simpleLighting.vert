uniform mat4 uMVPMatrix;
attribute vec4 aPosition;
attribute vec3 a_normal;
//varying vec3 v_normal; 
   
void main() {
	gl_Position = uMVPMatrix * aPosition;
}