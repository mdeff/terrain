#version 330 core

// Transformation matrices from model space to camera clip space.
uniform mat4 view;
uniform mat4 projection;

// Particules position texture.
uniform sampler1D particlesPosTex;

// Index of the current vertex defines the index in the position texture.
in int gl_VertexID;

// Vertex position (mandatory output) and point size when drawn as GL_POINTS.
out gl_PerVertex {
    vec4 gl_Position;
    float gl_PointSize;
};


void main() {

    // Retrieve the 3D position of the particule in the position texture.
    vec3 vertexPosition3DWorld = texelFetch(particlesPosTex, gl_VertexID, 0).rgb;

    // View matrix transforms from world space to camera space.
    // Projection matrix transforms from camera space to clip space (homogeneous space).
    vec4 vertexPositionCamera = view * vec4(vertexPosition3DWorld, 1.0);
    gl_Position = projection * vertexPositionCamera;

    // Particle size proportional to the inverse of the squared distance to the camera.
    gl_PointSize = 5.0 / (vertexPositionCamera.z*vertexPositionCamera.z);

}
