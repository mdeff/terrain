#version 330 core

// First input buffer. Defined here, retrieved in C++ by glGetAttribLocation.
layout(location = 0) in vec3 position;

out vec3 position2;


void main() {

    position2 = position;
    gl_Position = vec4(position,1.0);

}
