#version 330 core

// Vertices 2D position.
// First input buffer. Defined here, retrieved in C++ by glGetAttribLocation.
layout(location = 0) in vec2 vertexPosition2D;

// Vertex position (mandatory output).
out gl_PerVertex {
    vec4 gl_Position;
};

// Vertices 2D position.
out vec2 position2D;


void main() {

    position2D = vertexPosition2D;

    // Fullscreen quad has an height of 0.
    gl_Position = vec4(vertexPosition2D, 0.0, 1.0);

}
