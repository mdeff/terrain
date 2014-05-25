#version 330 core

// Transformation matrices from world space to camera clip space.
uniform mat4 projection;
uniform mat4 view;

// Transformation matrix from world space to light clip space.
uniform mat4 lightViewProjection;

// Light source position in world space.
uniform vec3 lightPositionWorld;

// Indicate if below water level geometry should be rendered or cliped.
// Clip is 1 for normal rendering and 0 for flipped rendering to texture.
uniform float clip;

// Texture 0. Defined by glActiveTexture and passed by glUniform1i.
uniform sampler2D heightMapTex;

// Vertices 2D position in world space.
// First input buffer. Defined here, retrieved in C++ by glGetAttribLocation.
layout(location = 0) in vec2 vertexPosition2DWorld;

// Vertex position (mandatory output) and distance to clip plane.
out gl_PerVertex {
    vec4 gl_Position;
    float gl_ClipDistance[1];
};

// Vertices 3D position (after heightmap displacement) in world space.
out vec3 vertexPosition3DWorld;

// Vertex position in light source clip space.
out vec3 ShadowCoord;

// Light and view directions.
out vec3 lightDir, viewDir;


void main() {

    // World (triangle grid) coordinates are (-1,-1) x (1,1).
    // Texture (height map) coordinates are (0,0) x (1,1).
    vec2 UV = (vertexPosition2DWorld + 1.0) / 2.0;
    float height = texture2D(heightMapTex, UV).r;

    // 3D vertex position : X and Y from vertex array, Z from heightmap texture.
    vertexPosition3DWorld = vec3(vertexPosition2DWorld.xy, height);

    // View matrix transforms from world space to camera space.
    // Projection matrix transforms from camera space to clip space (homogeneous space).
    vec4 vertexPositionCamera = view * vec4(vertexPosition3DWorld.xyz, 1.0);
    gl_Position = projection * vertexPositionCamera;

    // Transform vertex position from world space to light source clip space.
    // Account for perspective by dividing by w. Map from light coordinates in
    // (-1,-1)x(1,1) to texture coordinates in (0,0)x(1,1).
    vec4 vertexPositionShadow = lightViewProjection * vec4(vertexPosition3DWorld, 1.0);
    ShadowCoord = vertexPositionShadow.xyz / vertexPositionShadow.w * 0.5 + 0.5;

    // Light and view directions : subtraction of 2 points gives vector.
    // Camera space --> camera position at origin --> subtraction by [0,0,0].
    // No need to normalize as interpolation will not preserve vector lengths.
    lightDir = lightPositionWorld - vertexPosition3DWorld;
    viewDir = vec3(vertexPositionCamera);

    // Compute clip distance. Vertex is discarded if clip distance is negative.
    // Our clip plane is simply the water level, i.e. z = 0.
    // Clip is 1 for normal rendering and 0 for flipped rendering to texture.
    gl_ClipDistance[0] = vertexPosition3DWorld.z * clip;

}
