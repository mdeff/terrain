#version 330 core

// Pass-through vertex shader intended for GPGPU computations.
// This kind of computations are better suited for compute shaders. They are
// however not available with OpenGL 3.3.

// Vertices 2D position.
// First input buffer. Defined here, retrieved in C++ by glGetAttribLocation.
layout(location = 0) in vec2 vertexPosition2D;

// Vertex position (mandatory output).
out gl_PerVertex {
    vec4 gl_Position;
};


void main() {

    // Fullscreen quad has an height of 0.
    gl_Position = vec4(vertexPosition2D, 0.0, 1.0);

}
