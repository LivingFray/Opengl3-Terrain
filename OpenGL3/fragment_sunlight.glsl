#version 330 core
out vec3 color;

in vec2 UV;
in vec3 normalDirection_cameraSpace;
in vec3 lightDirection_cameraSpace;
in vec3 eyeDirection_cameraSpace;
in vec3 position;

uniform sampler2D tex;
uniform vec3 lightColor;
uniform float lightPower;
uniform vec3 lightPos;

void main(){
  vec3 n = normalize(normalDirection_cameraSpace);
  vec3 l = normalize(lightDirection_cameraSpace);
  vec3 e = normalize(eyeDirection_cameraSpace);
  vec3 r = reflect(-l, n);
  vec3 matCol = texture(tex, UV).rgb;
  vec3 ambient = vec3(0,0,0) * matCol;
  vec3 specCol = vec3(0, 0, 0);
  color = ambient
        + matCol * lightColor * lightPower * clamp(dot(n, l), 0, 1)
        + specCol * lightColor * lightPower * pow(clamp(dot(e, r), 0, 1), 5);
}