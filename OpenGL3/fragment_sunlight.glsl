#version 330 core
out vec3 color;

in vec2 UV;
in vec3 normalDirection_cameraSpace;
in vec3 lightDirection_cameraSpace;
in vec3 eyeDirection_cameraSpace;
in vec3 position;
in vec3 vertexNormal_cameraSpace;
in vec3 vertexTangent_cameraSpace;
in vec3 vertexBitangent_cameraSpace;
in vec3 lightDirection_tangentSpace;
in vec3 eyeDirection_tangentSpace;


uniform sampler2D tex;
uniform sampler2D normal;
uniform bool useNormal;
uniform vec3 lightColor;
uniform float lightPower;
uniform vec3 lightPos;
uniform vec3 fogCol;

//TODO: Get normal from normal map
void main(){
  vec3 n;
  vec3 l;
  vec3 e;
  if(useNormal){
    n = texture(normal, UV).rgb;
    l = normalize(lightDirection_tangentSpace);
	e = normalize(eyeDirection_tangentSpace);
  }else{
    n = normalize(normalDirection_cameraSpace);
    l = normalize(lightDirection_cameraSpace);
	e = normalize(eyeDirection_cameraSpace);
  }
  vec3 r = reflect(-l, n);
  vec3 matCol = texture(tex, UV).rgb;
  vec3 ambient = vec3(0,0,0) * matCol;
  vec3 specCol = vec3(0, 0, 0);
  color = ambient
        + matCol * lightColor * lightPower * clamp(dot(n, l), 0, 1)
        + specCol * lightColor * lightPower * pow(clamp(dot(e, r), 0, 1), 5);
  //Apply fog
  float FOG_START = 250.0;
  float FOG_END = 400.0;
  float fog = length(eyeDirection_cameraSpace);
  if(fog>FOG_START){
    fog = min(fog, FOG_END);
	fog = (fog-FOG_START)/(FOG_END-FOG_START);
  }else{
    fog = 0;
  }
  color = color * (1-fog) + fogCol * fog;
}