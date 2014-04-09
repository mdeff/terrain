#version 330 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec2 vertexUV;

out vec2 UV;
out vec4 ShadowCoord;
out vec3 vcolor;

uniform float t;
uniform mat4 MVP;
uniform mat4 lightOffsetMVP;

void main(){

    gl_Position =  MVP * vec4(vertexPosition,1);
    ShadowCoord = lightOffsetMVP * vec4(vertexPosition,1);
    vcolor = vertexColor;
    UV = vertexUV;
}

