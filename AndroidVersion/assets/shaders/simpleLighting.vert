uniform mat4 uMVPMatrix;
uniform vec3 eyePos;

attribute vec4 aPosition;
attribute vec3 aNormal;

varying vec3 vColor;

vec3 baseColor = vec3(0.0, 0.0, 1.0);
vec3 ambientColor = vec3(0.3, 0.3, 0.3);
vec3 lightDir = vec3(1.0, 1.0, 0);
vec3 lightColor = vec3(1,1,1);

void main() {
	gl_Position = uMVPMatrix * aPosition;
	
	//vColor = aNormal * eyePos;
	vec3 diffuse = dot(normalize(lightDir), normalize(aNormal)) * baseColor * lightColor;
	vec3 halfWay = normalize(lightDir + eyePos);
	float specular = pow(dot(aNormal, halfWay), 60.0);
	
	vColor = ambientColor * baseColor + diffuse + specular * lightColor;
	//vColor = ambientColor * baseColor + diffuse;
	//vColor = specular * baseColor * lightColor;
	//vColor = aNormal;
}