#version 330 core

// Transformation matrices from model space to camera clip space.
uniform mat4 modelview;
uniform mat4 projection;

// Texture 0. Defined by glActiveTexture and passed by glUniform1i.
uniform sampler1D particlesPosTex;

// Indices in the position texture.
// TODO : can be int ?
in int particlesPosTexIdx;


void main() {

    // Retrieve the 3D position of the particule in the position texture.
    vec3 vertexPosition3DModel = texelFetch(particlesPosTex, particlesPosTexIdx, 0).rgb;

    // Model matrix transforms from model space to world space.
    // View matrix transforms from world space to camera space.
    // Projection matrix transforms from camera space to clip space (homogeneous space).
    vec4 vertexPositionCamera = modelview * vec4(vertexPosition3DModel, 1.0);
    gl_Position = projection * vertexPositionCamera;

}
