#version 330 core

// Transformation matrices from model space to camera clip space.
uniform mat4 modelview;
uniform mat4 projection;

// Vertices 3D position in model space.
layout(location = 0) in vec3 vertexPosition3DModel;

// Vertex position (mandatory output).
out gl_PerVertex {
    vec4 gl_Position;
};

// 3D texture coordinates.
out vec3 texCoords;


void main() {

    // Model matrix transforms from model space to world space.
    // View matrix transforms from world space to camera space.
    // Projection matrix transforms from camera space to clip space (homogeneous space).
    gl_Position = projection * modelview * vec4(vertexPosition3DModel, 1.0);

    // Position for 3D texture lookup.
    texCoords = vertexPosition3DModel;

}
