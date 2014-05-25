#version 330 core

// Transformation matrices from model space to camera clip space.
uniform mat4 view;
uniform mat4 projection;

// Vertices 3D position in world space.
layout(location = 0) in vec3 vertexPosition3DWorld;

// Vertex position (mandatory output).
out gl_PerVertex {
    vec4 gl_Position;
};

// 3D texture coordinates.
out vec3 texCoords;


void main() {

    // View matrix transforms from world space to camera space.
    // Projection matrix transforms from camera space to clip space (homogeneous space).
    gl_Position = projection * view * vec4(vertexPosition3DWorld, 1.0);

    // Position for 3D texture lookup.
    texCoords = vertexPosition3DWorld;

}
