#version 330 core
layout(location = 0) in vec3 vertexPosition_modelspace;
uniform mat4 lightMVP;

void main(){
	gl_Position =  lightMVP * vec4(vertexPosition_modelspace,1);
}

