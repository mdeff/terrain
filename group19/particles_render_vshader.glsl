#version 330 core

// Transformation matrices from model space to camera clip space.
uniform mat4 modelview;
uniform mat4 projection;

// Particules position texture.
uniform sampler1D particlesPosTex;

// Index of the current vertex defines the index in the position texture.
in int gl_VertexID;

// Vertex position.
out vec4 gl_Position;

// Size of the point when drawn as GL_POINTS.
out float gl_PointSize;


void main() {

    // Retrieve the 3D position of the particule in the position texture.
    vec3 vertexPosition3DModel = texelFetch(particlesPosTex, gl_VertexID, 0).rgb;

    // Model matrix transforms from model space to world space.
    // View matrix transforms from world space to camera space.
    // Projection matrix transforms from camera space to clip space (homogeneous space).
    vec4 vertexPositionCamera = modelview * vec4(vertexPosition3DModel, 1.0);
    gl_Position = projection * vertexPositionCamera;

    // Particle size proportional to the inverse of the squared distance to the camera.
    gl_PointSize = 5.0 / (vertexPositionCamera.z*vertexPositionCamera.z);

}
