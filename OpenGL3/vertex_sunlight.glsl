#version 330 core

layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec3 vertexTangent;
layout(location = 4) in vec3 vertexBitangent;

uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform vec3 lightPos;
uniform mat3 MV3x3;
uniform bool useNormal;

out vec2 UV;
out vec3 normalDirection_cameraSpace;
out vec3 lightDirection_cameraSpace;
out vec3 eyeDirection_cameraSpace;
out vec3 position;
out vec3 vertexNormal_cameraSpace;
out vec3 vertexTangent_cameraSpace;
out vec3 vertexBitangent_cameraSpace;
out vec3 lightDirection_tangentSpace;
out vec3 eyeDirection_tangentSpace;

void main(){
  gl_Position = MVP * vec4(vertexPos, 1);
  UV = vertexUV;
  position = (M * vec4(vertexPos, 1)).xyz;
  vec3 vertexPosition_cameraSpace = ( V * M * vec4(vertexPos, 1)).xyz;
  eyeDirection_cameraSpace = vec3(0,0,0) - vertexPosition_cameraSpace;
  vec3 lightPosition_cameraSpace = ( V * vec4(lightPos, 0)).xyz;
  lightDirection_cameraSpace = lightPosition_cameraSpace;// + eyeDirection_cameraSpace;
  normalDirection_cameraSpace = (V * M * vec4(vertexNormal, 0)).xyz; //Doesn't handle scaling
  //Normal related stuff
  if(useNormal){
    vertexNormal_cameraSpace = MV3x3 * normalize(vertexNormal);
    vertexTangent_cameraSpace = MV3x3 * normalize(vertexTangent);
    vertexBitangent_cameraSpace = MV3x3 * normalize(vertexBitangent);
    mat3 TBN = transpose(mat3(vertexTangent_cameraSpace,vertexBitangent_cameraSpace,vertexNormal_cameraSpace));
    lightDirection_tangentSpace = TBN * lightDirection_cameraSpace;
    eyeDirection_tangentSpace = TBN * eyeDirection_cameraSpace;
  }
}