// Copyright (C) 2014 - LGG EPFL
#version 330 core

// Bind color to first framebuffer
layout(location = 0) out vec3 color;

void main() {
    // depth-value according to http://www.opengl.org/wiki/Built-in_Variable_(GLSL)#Fragment_shader_inputs
    color = gl_FragCoord.z;
}
