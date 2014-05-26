#version 330 core

// Transformation matrices from world space to camera clip space.
uniform mat4 projection;
uniform mat4 view;

// Transformation matrix from world space to flipped camera view space.
uniform mat4 flippedCameraView;

// Transformation matrix from world space to light clip space.
uniform mat4 lightViewProjection;

// Light source position in model space.
uniform vec3 lightPositionModel;

// Vertices 2D position in model space.
// First input buffer. Defined here, retrieved in C++ by glGetAttribLocation.
layout(location = 0) in vec2 vertexPosition2DWorld;

// Vertex position (mandatory output).
out gl_PerVertex {
    vec4 gl_Position;
};

// Vertices 3D position (after heightmap displacement) in world space.
out vec3 vertexPosition3DWorld;

// Coordinates for flipped camera texture look-up.
// Vertex position in flipped camera clip space.
//out vec2 flippedCameraCoord;
//out vec4 vertexPositionFlippedCamera;

// Light and view directions.
out vec3 lightDir, viewDir;


void main() {

    // 3D vertex position : X and Y from vertex array, Z is fixed water surface height
    vertexPosition3DWorld = vec3(vertexPosition2DWorld.xy, 0.0);

    // View matrix transforms from world space to camera space.
    // Projection matrix transforms from camera space to clip space (homogeneous space).
    vec4 vertexPositionCamera = view * vec4(vertexPosition3DWorld, 1.0);
    gl_Position = projection * vertexPositionCamera;

    // Light and view directions : subtraction of 2 points gives vector.
    // Camera space --> camera position at origin --> subtraction by [0,0,0].
    // No need to normalize as interpolation will not preserve vector lengths.
    lightDir = lightPositionModel - vertexPosition3DWorld;
    viewDir = vec3(vertexPositionCamera);

}
