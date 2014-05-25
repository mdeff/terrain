#version 330 core

// Transformation matrices from model space to camera clip space.
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

// Vertices 3D position in model space.
in vec3 vertexPosition3DModel;

// Vertex position (mandatory output).
out gl_PerVertex {
    vec4 gl_Position;
};


void main() {

    // Model matrix transforms from model space to world space.
    // View matrix transforms from world space to camera space.
    // Projection matrix transforms from camera space to clip space (homogeneous space).
    vec4 vertexPositionWorld = model * vec4(vertexPosition3DModel, 1.0);
    vec4 vertexPositionCamera = view * vertexPositionWorld;
    gl_Position = projection * vertexPositionCamera;

}
