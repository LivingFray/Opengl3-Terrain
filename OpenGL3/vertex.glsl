#version 330 core

layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;

uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform vec3 lightPos;

out vec2 UV;
out vec3 normalDirection_cameraSpace;
out vec3 lightDirection_cameraSpace;
out vec3 eyeDirection_cameraSpace;
out vec3 position;

void main(){
  gl_Position = MVP * vec4(vertexPos, 1);
  UV = vertexUV;
  position = vertexPos;
  vec3 vertexPosition_cameraSpace = ( V * M * vec4(vertexPos,1)).xyz;
  eyeDirection_cameraSpace = vec3(0,0,0) - vertexPosition_cameraSpace;
  vec3 lightPosition_cameraSpace = ( V * vec4(lightPos,1)).xyz;
  lightDirection_cameraSpace = lightPosition_cameraSpace + eyeDirection_cameraSpace;
  normalDirection_cameraSpace = (V * M * vec4(vertexPos, 0)).xyz; //Doesn't handle scaling
}