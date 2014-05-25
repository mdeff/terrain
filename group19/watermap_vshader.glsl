#version 330 core

// Transformation matrices from world space to camera clip space.
uniform mat4 projection;
uniform mat4 view;

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

// Vertex position in light source clip space.
out vec3 reflectionCoord;

// Light and view directions.
out vec3 lightDir, viewDir;


void main() {
	
	//fixed height of water surface
	float waterSurfaceHeight = 0.018f;  
    float height = waterSurfaceHeight; 
  
    // 3D vertex position : X and Y from vertex array, Z is fixed water surface height
    vertexPosition3DWorld = vec3(vertexPosition2DWorld.xy, height);

    // View matrix transforms from world space to camera space.
    // Projection matrix transforms from camera space to clip space (homogeneous space).
    vec4 vertexPositionCamera = view * vec4(vertexPosition3DWorld, 1.0);
    gl_Position = projection * vertexPositionCamera;

    // Vertex position in light source clip space.
    //vec4 vertexPositionReflection = lightViewProjection * vec4(vertexPosition3DWorld, 1.0);
    // Reflection map texture coordinates with w division for perspective.
    //reflectionCoord = vertexPositionReflection.xyz / vertexPositionReflection.w * 0.5 + 0.5;

    reflectionCoord = vertexPositionCamera.xyz/vertexPositionCamera.z *0.5 + 0.5;

    // Light and view directions : subtraction of 2 points gives vector.
    // Camera space --> camera position at origin --> subtraction by [0,0,0].
    // No need to normalize as interpolation will not preserve vector lengths.
    lightDir = lightPositionModel - vertexPosition3DWorld;
    viewDir = vec3(vertexPositionCamera);

}
