uniform mat4 uMVPMatrix;
uniform vec3 eyePos;

attribute vec4 aPosition;
attribute vec3 aNormal;

varying vec3 vColor;

vec3 baseColor = vec3(0.0, 0.0, 1.0);
vec3 ambientColor = vec3(0.5, 0.5, 0.5);
vec3 lightDir = vec3(-1.0, -1.0, 0);
vec3 lightColor = vec3(1,1,1);

void main() {
	gl_Position = uMVPMatrix * aPosition;
	
	vColor = aNormal * eyePos;
}